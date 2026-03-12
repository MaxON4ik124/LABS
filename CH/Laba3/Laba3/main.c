/*
 * Clock for EasyAVR v7 + ATmega32 (8 MHz)
 * ---------------------------------------
 * Real EasyAVR v7 onboard 4-digit display wiring:
 *   SEGMENTS: PORTC (PC0=a, PC1=b, PC2=c, PC3=d, PC4=e, PC5=f, PC6=g, PC7=dp)
 *   DIGITS:   PORTA (PA0..PA3 via SW8.1..SW8.4)
 *
 * Buttons:
 *   PD2 / INT0 : toggle display/setup mode
 *   PD3 / INT1 : display mode -> switch HH.MM / MM.SS
 *                setup mode   -> select HH -> MM -> SS -> HH
 *   PD0        : increment selected value
 *   PD1        : decrement selected value
 *
 * Features:
 * - Timer0 CTC, 1 ms tick (TCCR0 / TCNT0 / OCR0)
 * - 4-digit multiplexing
 * - blink selected field at 2 Hz in setup mode
 * - auto-repeat:
 *     press        -> immediate +/-1
 *     hold > 2 s   -> every 200 ms
 *     hold > 4 s   -> every 100 ms
 * - initial time: 00:00:00
 */

#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <stdint.h>
#include <stdbool.h>

/* ============================================================
 * Hardware configuration
 * ============================================================ */

/* EasyAVR v7 onboard 4-digit display */
#define SEG_PORT   PORTC
#define SEG_DDR    DDRC

#define DIG_PORT   PORTA
#define DIG_DDR    DDRA
#define DIG_MASK   0x0F

/* Buttons */
#define BTN_PIN    PIND
#define BTN_PORT   PORTD
#define BTN_DDR    DDRD

#define BTN_INC    PD0
#define BTN_DEC    PD1
#define BTN_INT0   PD2
#define BTN_INT1   PD3

/* For EasyAVR v7 onboard 7-seg this should stay 0 */
#define COMMON_ANODE 0

/* ============================================================
 * Application types
 * ============================================================ */

typedef enum {
    MODE_DISPLAY = 0,
    MODE_SETUP   = 1
} app_mode_t;

typedef enum {
    VIEW_HHMM = 0,
    VIEW_MMSS = 1
} view_mode_t;

typedef enum {
    SEL_HH = 0,
    SEL_MM = 1,
    SEL_SS = 2
} setup_sel_t;

typedef enum {
    EVT_NONE = 0,
    EVT_TOGGLE_MODE = 1,
    EVT_SWITCH_VIEW_OR_FIELD = 2,
    EVT_INC_STEP = 3,
    EVT_DEC_STEP  = 4
} event_t;

/* ============================================================
 * Ring buffer for events
 * ============================================================ */

#define EVT_QUEUE_SIZE 16

static volatile event_t evt_queue[EVT_QUEUE_SIZE];
static volatile uint8_t evt_head = 0;
static volatile uint8_t evt_tail = 0;

static inline void push_event_isr(event_t e)
{
    uint8_t next = (uint8_t)((evt_head + 1u) % EVT_QUEUE_SIZE);
    if (next != evt_tail) {
        evt_queue[evt_head] = e;
        evt_head = next;
    }
}

static bool pop_event(event_t *e)
{
    bool ok = false;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        if (evt_tail != evt_head) {
            *e = evt_queue[evt_tail];
            evt_tail = (uint8_t)((evt_tail + 1u) % EVT_QUEUE_SIZE);
            ok = true;
        }
    }

    return ok;
}

/* ============================================================
 * Global state
 * ============================================================ */

static volatile uint32_t g_ms = 0;

/* Here we store already ENCODED segment patterns */
static volatile uint8_t g_disp[4] = {0, 0, 0, 0};

/* Current time */
static volatile uint8_t g_hours   = 0;
static volatile uint8_t g_minutes = 0;
static volatile uint8_t g_seconds = 0;

/* UI state */
static volatile app_mode_t  g_mode     = MODE_DISPLAY;
static volatile view_mode_t g_view     = VIEW_HHMM;
static volatile setup_sel_t g_selected = SEL_HH;

/* Blink 2 Hz => toggle every 250 ms */
static volatile uint8_t g_blink_visible = 1;

/* Debounce timestamps for INT0 / INT1 */
static volatile uint32_t g_last_int0_ms = 0;
static volatile uint32_t g_last_int1_ms = 0;

/* ============================================================
 * Button state machine for PD0 / PD1
 * ============================================================ */

typedef struct {
    uint8_t pin_mask;

    uint8_t stable_pressed;
    uint8_t raw_prev;
    uint8_t debounce_cnt_ms;

    uint32_t press_start_ms;
    uint32_t next_repeat_ms;

    event_t step_event;
} button_state_t;

static volatile button_state_t g_btn_inc = {
    .pin_mask = (1u << BTN_INC),
    .stable_pressed = 0,
    .raw_prev = 0,
    .debounce_cnt_ms = 0,
    .press_start_ms = 0,
    .next_repeat_ms = 0,
    .step_event = EVT_INC_STEP
};

static volatile button_state_t g_btn_dec = {
    .pin_mask = (1u << BTN_DEC),
    .stable_pressed = 0,
    .raw_prev = 0,
    .debounce_cnt_ms = 0,
    .press_start_ms = 0,
    .next_repeat_ms = 0,
    .step_event = EVT_DEC_STEP
};

/* ============================================================
 * 7-segment encoding
 * bit0=a bit1=b bit2=c bit3=d bit4=e bit5=f bit6=g bit7=dp
 * ============================================================ */

static const uint8_t seg_digits[10] = {
    0b00111111, /* 0 */
    0b00000110, /* 1 */
    0b01011011, /* 2 */
    0b01001111, /* 3 */
    0b01100110, /* 4 */
    0b01101101, /* 5 */
    0b01111101, /* 6 */
    0b00000111, /* 7 */
    0b01111111, /* 8 */
    0b01101111  /* 9 */
};

#define SEG_DP    0x80
#define SEG_BLANK 0x00

static inline uint8_t seg_encode_digit(uint8_t d)
{
    if (d < 10u) return seg_digits[d];
    return SEG_BLANK;
}

/* EasyAVR v7 uses direct mapping on PORTC */
static inline uint8_t seg_hw_prepare(uint8_t logical_pattern)
{
#if COMMON_ANODE
    return (uint8_t)~logical_pattern;
#else
    return logical_pattern;
#endif
}

/*
 * Physical digit order on board is usually:
 * leftmost  -> PA3
 * next      -> PA2
 * next      -> PA1
 * rightmost -> PA0
 *
 * If you get mirrored output 4321 instead of 1234,
 * swap only the masks in digit_on().
 */
static inline void digits_all_off(void)
{
    DIG_PORT &= (uint8_t)~DIG_MASK;
}

static inline void digit_on(uint8_t idx)
{
    uint8_t mask = 0;

    switch (idx) {
        case 0: mask = (1u << PA3); break; /* leftmost  */
        case 1: mask = (1u << PA2); break;
        case 2: mask = (1u << PA1); break;
        case 3: mask = (1u << PA0); break; /* rightmost */
        default: mask = 0; break;
    }

    DIG_PORT = (DIG_PORT & (uint8_t)~DIG_MASK) | mask;
}

/* ============================================================
 * JTAG disable for PORTC normal I/O
 * ============================================================ */

static void jtag_disable(void)
{
#ifdef JTD
    MCUCSR |= (1u << JTD);
    MCUCSR |= (1u << JTD);
#endif
}

/* ============================================================
 * Time helpers
 * ============================================================ */

static void time_tick_1s(void)
{
    g_seconds++;
    if (g_seconds >= 60u) {
        g_seconds = 0;
        g_minutes++;
        if (g_minutes >= 60u) {
            g_minutes = 0;
            g_hours++;
            if (g_hours >= 24u) {
                g_hours = 0;
            }
        }
    }
}

static void inc_selected_value(void)
{
    switch (g_selected) {
        case SEL_HH:
            g_hours = (uint8_t)((g_hours + 1u) % 24u);
            break;
        case SEL_MM:
            g_minutes = (uint8_t)((g_minutes + 1u) % 60u);
            break;
        case SEL_SS:
            g_seconds = (uint8_t)((g_seconds + 1u) % 60u);
            break;
    }
}

static void dec_selected_value(void)
{
    switch (g_selected) {
        case SEL_HH:
            g_hours = (g_hours == 0u) ? 23u : (uint8_t)(g_hours - 1u);
            break;
        case SEL_MM:
            g_minutes = (g_minutes == 0u) ? 59u : (uint8_t)(g_minutes - 1u);
            break;
        case SEL_SS:
            g_seconds = (g_seconds == 0u) ? 59u : (uint8_t)(g_seconds - 1u);
            break;
    }
}

/* ============================================================
 * Display preparation
 * ============================================================ */

static void display_store_patterns(uint8_t p0, uint8_t p1, uint8_t p2, uint8_t p3)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        g_disp[0] = p0;
        g_disp[1] = p1;
        g_disp[2] = p2;
        g_disp[3] = p3;
    }
}

static view_mode_t get_effective_view_for_setup(void)
{
    if (g_selected == SEL_HH) return VIEW_HHMM;
    if (g_selected == SEL_SS) return VIEW_MMSS;
    return g_view;
}

static void refresh_display_buffer(void)
{
    uint8_t hh, mm, ss;
    app_mode_t mode;
    view_mode_t view;
    setup_sel_t selected;
    uint8_t blink_visible;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        hh = g_hours;
        mm = g_minutes;
        ss = g_seconds;
        mode = g_mode;
        view = g_view;
        selected = g_selected;
        blink_visible = g_blink_visible;
    }

    if (mode == MODE_SETUP) {
        view = get_effective_view_for_setup();
    }

    {
        uint8_t d0, d1, d2, d3;
        uint8_t p0, p1, p2, p3;

        if (view == VIEW_HHMM) {
            d0 = (uint8_t)(hh / 10u);
            d1 = (uint8_t)(hh % 10u);
            d2 = (uint8_t)(mm / 10u);
            d3 = (uint8_t)(mm % 10u);

            if (mode == MODE_SETUP && !blink_visible) {
                if (selected == SEL_HH) {
                    d0 = 255u;
                    d1 = 255u;
                } else if (selected == SEL_MM) {
                    d2 = 255u;
                    d3 = 255u;
                }
            }
        } else {
            d0 = (uint8_t)(mm / 10u);
            d1 = (uint8_t)(mm % 10u);
            d2 = (uint8_t)(ss / 10u);
            d3 = (uint8_t)(ss % 10u);

            if (mode == MODE_SETUP && !blink_visible) {
                if (selected == SEL_MM) {
                    d0 = 255u;
                    d1 = 255u;
                } else if (selected == SEL_SS) {
                    d2 = 255u;
                    d3 = 255u;
                }
            }
        }

        p0 = (d0 < 10u) ? seg_encode_digit(d0) : SEG_BLANK;
        p1 = (d1 < 10u) ? (uint8_t)(seg_encode_digit(d1) | SEG_DP) : SEG_BLANK;
        p2 = (d2 < 10u) ? seg_encode_digit(d2) : SEG_BLANK;
        p3 = (d3 < 10u) ? (uint8_t)(seg_encode_digit(d3) | SEG_DP) : SEG_BLANK;

        display_store_patterns(p0, p1, p2, p3);
    }
}

/* ============================================================
 * Button handling (called every 1 ms from timer ISR)
 * ============================================================ */

#define DEBOUNCE_MS 20

static void button_scan_1ms(volatile button_state_t *b, uint8_t pin_snapshot, uint32_t now_ms)
{
    uint8_t raw_pressed = ((pin_snapshot & b->pin_mask) == 0u) ? 1u : 0u;

    if (raw_pressed != b->raw_prev) {
        b->raw_prev = raw_pressed;
        b->debounce_cnt_ms = 0;
    } else {
        if (b->debounce_cnt_ms < DEBOUNCE_MS) {
            b->debounce_cnt_ms++;
            if (b->debounce_cnt_ms == DEBOUNCE_MS) {
                if (b->stable_pressed != raw_pressed) {
                    b->stable_pressed = raw_pressed;

                    if (raw_pressed) {
                        b->press_start_ms = now_ms;
                        b->next_repeat_ms = now_ms + 2000u;
                        push_event_isr(b->step_event);
                    }
                }
            }
        }
    }

    if (b->stable_pressed) {
        uint32_t held = now_ms - b->press_start_ms;

        if (held >= 4000u) {
            while ((int32_t)(now_ms - b->next_repeat_ms) >= 0) {
                push_event_isr(b->step_event);
                b->next_repeat_ms += 100u;
            }
        } else if (held >= 2000u) {
            while ((int32_t)(now_ms - b->next_repeat_ms) >= 0) {
                push_event_isr(b->step_event);
                b->next_repeat_ms += 200u;
            }
        }
    }
}

/* ============================================================
 * Initialization
 * ============================================================ */

static void gpio_init(void)
{
    /* Segments */
    SEG_DDR  = 0xFF;
    SEG_PORT = seg_hw_prepare(SEG_BLANK);

    /* Digits */
    DIG_DDR  |= DIG_MASK;
    digits_all_off();

    /* Buttons PD0..PD3 as input with pull-ups */
    BTN_DDR  &= (uint8_t)~((1u << BTN_INC) | (1u << BTN_DEC) | (1u << BTN_INT0) | (1u << BTN_INT1));
    BTN_PORT |= (1u << BTN_INC) | (1u << BTN_DEC) | (1u << BTN_INT0) | (1u << BTN_INT1);
}

static void exti_init(void)
{
    /* INT0: falling edge */
    MCUCR |= (1u << ISC01);
    MCUCR &= (uint8_t)~(1u << ISC00);

    /* INT1: falling edge */
    MCUCR |= (1u << ISC11);
    MCUCR &= (uint8_t)~(1u << ISC10);

    /* Clear pending flags */
    GIFR |= (1u << INTF0) | (1u << INTF1);

    /* Enable INT0, INT1 */
    GICR |= (1u << INT0) | (1u << INT1);
}

static void timer0_init(void)
{
    /*
     * Timer0 CTC, 1 ms tick
     * F_CPU = 8 MHz
     * prescaler = 64
     * 8,000,000 / 64 = 125,000 Hz
     * OCR0 = 124 => 125 counts => 1 ms
     */
    TCCR0 = 0;
    TCNT0 = 0;
    OCR0  = 124;

    TCCR0 |= (1u << WGM01);              /* CTC */
    TCCR0 |= (1u << CS01) | (1u << CS00);/* prescaler 64 */

    TIMSK |= (1u << OCIE0);
}

/* ============================================================
 * ISRs
 * ============================================================ */

ISR(INT0_vect)
{
    uint32_t now = g_ms;
    if ((now - g_last_int0_ms) >= 50u) {
        g_last_int0_ms = now;
        push_event_isr(EVT_TOGGLE_MODE);
    }
}

ISR(INT1_vect)
{
    uint32_t now = g_ms;
    if ((now - g_last_int1_ms) >= 50u) {
        g_last_int1_ms = now;
        push_event_isr(EVT_SWITCH_VIEW_OR_FIELD);
    }
}

ISR(TIMER0_COMP_vect)
{
    static uint16_t ms_div_1s = 0;
    static uint8_t mux_digit = 0;

    g_ms++;

    /* blink state for setup mode */
    g_blink_visible = (((g_ms / 250u) & 1u) == 0u) ? 1u : 0u;

    /* time base */
    ms_div_1s++;
    if (ms_div_1s >= 1000u) {
        ms_div_1s = 0;
        time_tick_1s();
    }

    /* scan PD0 / PD1 */
    {
        uint8_t pins = BTN_PIN;
        button_scan_1ms(&g_btn_inc, pins, g_ms);
        button_scan_1ms(&g_btn_dec, pins, g_ms);
    }

    /* multiplex display */
    digits_all_off();
    SEG_PORT = seg_hw_prepare(g_disp[mux_digit]);
    digit_on(mux_digit);

    mux_digit++;
    if (mux_digit >= 4u) {
        mux_digit = 0u;
    }
}

/* ============================================================
 * Event processing
 * ============================================================ */
static void process_event(event_t e)
{
	switch (e) {
		case EVT_TOGGLE_MODE:
		//g_seconds = (g_seconds == 0) ? 55 : 0;
		display_store_patterns(
		seg_encode_digit(1),
		seg_encode_digit(4) | SEG_DP,
		seg_encode_digit(8),
		seg_encode_digit(8) | SEG_DP);
		break;

		case EVT_SWITCH_VIEW_OR_FIELD:
		//g_minutes = (g_minutes == 0) ? 44 : 0;
		display_store_patterns(
				seg_encode_digit(1),
				seg_encode_digit(3) | SEG_DP,
				seg_encode_digit(3),
				seg_encode_digit(7) | SEG_DP);
		break;

		case EVT_INC_STEP:
		g_hours = (uint8_t)((g_hours + 1u) % 24u);
		break;

		case EVT_DEC_STEP:
		g_hours = (g_hours == 0u) ? 23u : (uint8_t)(g_hours - 1u);
		break;

		default:
		break;
	}
}

/* ============================================================
 * Main
 * ============================================================ */

int main(void)
{
    jtag_disable();
    gpio_init();
    exti_init();
    timer0_init();

    refresh_display_buffer();
    sei();

    while (1) {
	    event_t e;

	    while (pop_event(&e)) {
		    process_event(e);
	    }

	    refresh_display_buffer();
    }
	//g_disp[0] = seg_encode_digit(1);
	//g_disp[1] = seg_encode_digit(4);
	//g_disp[2] = seg_encode_digit(8);
	//g_disp[3] = seg_encode_digit(8);
}