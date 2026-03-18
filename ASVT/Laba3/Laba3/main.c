#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <stdint.h>
#include <stdbool.h>

#define RINGBUF_SIZE 32

#define SEG_PORT    PORTC
#define SEG_DDR     DDRC

#define DIG_PORT    PORTA
#define DIG_DDR     DDRA

#define BTN_INC_PIN     PD0
#define BTN_DEC_PIN     PD1
#define BTN_MODE_PIN    PD2
#define BTN_NEXT_PIN    PD3

#define BTN_PIN_REG     PIND
#define BTN_PORT_REG    PORTD
#define BTN_DDR_REG     DDRD

typedef enum
{
    EV_NONE = 0,
    EV_TICK_1S,
    EV_MODE_TOGGLE,
    EV_NEXT_FIELD_OR_FORMAT,
    EV_INC,
    EV_DEC
} event;

typedef struct
{
    volatile event buffer[RINGBUF_SIZE];
    volatile uint8_t head:5;
    volatile uint8_t tail:5;
} RingBuffer;

static inline void RB_Init(RingBuffer *rb)
{
    rb->head = 0;
    rb->tail = 0;
}

static bool RB_Push(RingBuffer *rb, event e)
{
    bool ok = false;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        uint8_t next = (uint8_t)((rb->head + 1u) % RINGBUF_SIZE);
        if (next != rb->tail)
        {
            rb->buffer[rb->head] = e;
            rb->head = next;
            ok = true;
        }
    }

    return ok;
}

static bool RB_Pop(RingBuffer *rb, event *e)
{
    bool ok = false;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        if (rb->tail != rb->head)
        {
            *e = rb->buffer[rb->tail];
            rb->tail = (uint8_t)((rb->tail + 1u) % RINGBUF_SIZE);
            ok = true;
        }
    }

    return ok;
}




typedef enum
{
    MODE_DISPLAY = 0,
    MODE_SET
} ClockMode;

typedef enum
{
    VIEW_HH_MM = 0,
    VIEW_MM_SS
} DisplayFormat;

typedef enum
{
    FIELD_HH = 0,
    FIELD_MM,
    FIELD_SS
} SetField;

typedef struct
{
    uint8_t hh;
    uint8_t mm;
    uint8_t ss;
} ClockTime;

static uint8_t Dec2_FromStr(const char *s)
{
	return (uint8_t)((s[0] - '0') * 10u + (s[1] - '0'));
}

static ClockTime ClockTime_FromBuildTime(void)
{
	ClockTime t;

	t.hh = Dec2_FromStr(&__TIME__[0]);
	t.mm = Dec2_FromStr(&__TIME__[3]);
	t.ss = Dec2_FromStr(&__TIME__[6]);

	return t;
}

static volatile RingBuffer g_queue;

static volatile ClockMode g_mode = MODE_DISPLAY;
static volatile DisplayFormat g_format = VIEW_HH_MM;
static volatile SetField g_set_field = FIELD_HH;
static volatile ClockTime g_time = {0, 0, 0};

static volatile uint8_t g_blink_visible = 1;
static volatile uint16_t g_blink_ms = 0;

static volatile uint8_t g_disp_digits[4] = {0, 0, 0, 0};
static volatile uint8_t g_disp_dp[4]     = {0, 0, 1, 0};
static volatile uint8_t g_scan_pos = 0;

static const uint8_t seg_lut[10] =
{
    0b00111111,
    0b00000110,
    0b01011011,
    0b01001111,
    0b01100110,
    0b01101101,
    0b01111101,
    0b00000111,
    0b01111111,
    0b01101111
};

#define SEG_BLANK 0x00
#define DISP_BLANK 10

static void Clock_Normalize(ClockTime *t)
{
    if (t->ss >= 60) t->ss = 0;
    if (t->mm >= 60) t->mm = 0;
    if (t->hh >= 24) t->hh = 0;
}

static void Clock_Tick1s(ClockTime *t)
{
    t->ss++;
    if (t->ss >= 60)
    {
        t->ss = 0;
        t->mm++;
        if (t->mm >= 60)
        {
            t->mm = 0;
            t->hh++;
            if (t->hh >= 24)
            {
                t->hh = 0;
            }
        }
    }
}

static void Clock_IncField(ClockTime *t, SetField field)
{
    switch (field)
    {
        case FIELD_HH: t->hh = (uint8_t)((t->hh + 1u) % 24u); break;
        case FIELD_MM: t->mm = (uint8_t)((t->mm + 1u) % 60u); break;
        case FIELD_SS: t->ss = (uint8_t)((t->ss + 1u) % 60u); break;
        default: break;
    }
}

static void Clock_DecField(ClockTime *t, SetField field)
{
    switch (field)
    {
        case FIELD_HH: t->hh = (t->hh == 0) ? 23 : (uint8_t)(t->hh - 1u); break;
        case FIELD_MM: t->mm = (t->mm == 0) ? 59 : (uint8_t)(t->mm - 1u); break;
        case FIELD_SS: t->ss = (t->ss == 0) ? 59 : (uint8_t)(t->ss - 1u); break;
        default: break;
    }
}

static void Display_UpdateBuffer(void)
{
    uint8_t hh, mm, ss;
    uint8_t blank_hh = 0;
    uint8_t blank_mm = 0;
    uint8_t blank_ss = 0;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        hh = g_time.hh;
        mm = g_time.mm;
        ss = g_time.ss;

        if ((g_mode == MODE_SET) && (g_blink_visible == 0))
        {
            switch (g_set_field)
            {
                case FIELD_HH: blank_hh = 1; break;
                case FIELD_MM: blank_mm = 1; break;
                case FIELD_SS: blank_ss = 1; break;
                default: break;
            }
        }
    }

    g_disp_dp[0] = 0;
    g_disp_dp[1] = 0;
    g_disp_dp[2] = 1;
    g_disp_dp[3] = 0;

    if (g_format == VIEW_HH_MM)
    {
        if (blank_hh)
        {
            g_disp_digits[3] = DISP_BLANK;
            g_disp_digits[2] = DISP_BLANK;
            g_disp_digits[1] = mm / 10;
            g_disp_digits[0] = mm % 10;
        }
        else if (blank_mm)
        {
            g_disp_digits[3] = hh / 10;
            g_disp_digits[2] = hh % 10;
            g_disp_digits[1] = DISP_BLANK;
            g_disp_digits[0] = DISP_BLANK;
        }
        else
        {
            g_disp_digits[3] = hh / 10;
            g_disp_digits[2] = hh % 10;
            g_disp_digits[1] = mm / 10;
            g_disp_digits[0] = mm % 10;
        }
    }
    else
    {
        if (blank_mm)
        {
            g_disp_digits[3] = DISP_BLANK;
            g_disp_digits[2] = DISP_BLANK;
            g_disp_digits[1] = ss / 10;
            g_disp_digits[0] = ss % 10;
        }
        else if (blank_ss)
        {
            g_disp_digits[3] = mm / 10;
            g_disp_digits[2] = mm % 10;
            g_disp_digits[1] = DISP_BLANK;
            g_disp_digits[0] = DISP_BLANK;
        }
        else
        {
            g_disp_digits[3] = mm / 10;
            g_disp_digits[2] = mm % 10;
            g_disp_digits[1] = ss / 10;
            g_disp_digits[0] = ss % 10;
        }
    }
}

static uint8_t Display_EncodeDigit(uint8_t d, uint8_t dp)
{
    uint8_t code = (d <= 9) ? seg_lut[d] : SEG_BLANK;
    if (dp) code |= (1u << 7);
    return code;
}

static void Display_AllDigitsOff(void)
{
    DIG_PORT &= (uint8_t)~0x0F;
}

static void Display_EnableDigit(uint8_t pos)
{
    DIG_PORT = (DIG_PORT & (uint8_t)~0x0F) | (1u << pos);
}

static void Display_ScanStep(void)
{
    uint8_t d = g_disp_digits[g_scan_pos];
    uint8_t code;

    Display_AllDigitsOff();

    code = Display_EncodeDigit(d, g_disp_dp[g_scan_pos]);
    SEG_PORT = code;
    Display_EnableDigit(g_scan_pos);

    g_scan_pos++;
    if (g_scan_pos >= 4)
    {
        g_scan_pos = 0;
    }
}

typedef struct
{
    uint8_t stable_pressed;
    uint8_t raw_prev;
    uint8_t debounce_cnt;
    uint16_t hold_ms;
    uint16_t repeat_ms;
} ButtonState;

static volatile ButtonState g_btn_inc = {0, 0, 0, 0, 0};
static volatile ButtonState g_btn_dec = {0, 0, 0, 0, 0};

static void Button_ProcessOne(volatile ButtonState *b, uint8_t raw_pressed, event ev)
{
    if (raw_pressed == b->raw_prev)
    {
        if (b->debounce_cnt < 3) b->debounce_cnt++;
    }
    else
    {
        b->debounce_cnt = 0;
        b->raw_prev = raw_pressed;
    }

    if (b->debounce_cnt >= 3)
    {
        if (b->stable_pressed != raw_pressed)
        {
            b->stable_pressed = raw_pressed;

            if (raw_pressed)
            {
                b->hold_ms = 0;
                b->repeat_ms = 0;
                RB_Push((RingBuffer *)&g_queue, ev);
            }
            else
            {
                b->hold_ms = 0;
                b->repeat_ms = 0;
            }
        }
    }

    if (b->stable_pressed)
    {
        b->hold_ms += 10;

        if (b->hold_ms >= 2000)
        {
            b->repeat_ms += 10;

            if (b->hold_ms < 4000)
            {
                if (b->repeat_ms >= 200)
                {
                    b->repeat_ms = 0;
                    RB_Push((RingBuffer *)&g_queue, ev);
                }
            }
            else
            {
                if (b->repeat_ms >= 100)
                {
                    b->repeat_ms = 0;
                    RB_Push((RingBuffer *)&g_queue, ev);
                }
            }
        }
    }
}

static void Buttons_Poll10ms(void)
{
    uint8_t pin = BTN_PIN_REG;
    uint8_t inc_pressed = ((pin & (1u << BTN_INC_PIN)) != 0u) ? 1u : 0u;
    uint8_t dec_pressed = ((pin & (1u << BTN_DEC_PIN)) != 0u) ? 1u : 0u;

    Button_ProcessOne(&g_btn_inc, inc_pressed, EV_INC);
    Button_ProcessOne(&g_btn_dec, dec_pressed, EV_DEC);
}

static void App_HandleEvent(event e)
{
    switch (e)
    {
        case EV_TICK_1S:
            if (g_mode == MODE_DISPLAY)
            {
                ClockTime tmp;

                ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
                {
                    tmp = g_time;
                }

                Clock_Tick1s(&tmp);

                ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
                {
                    g_time = tmp;
                }
            }
            break;

        case EV_MODE_TOGGLE:
            if (g_mode == MODE_DISPLAY)
            {
                g_mode = MODE_SET;
                g_set_field = FIELD_HH;
                g_blink_visible = 1;
                g_blink_ms = 0;
            }
            else
            {
                g_mode = MODE_DISPLAY;
                g_blink_visible = 1;
            }
            break;

        case EV_NEXT_FIELD_OR_FORMAT:
            if (g_mode == MODE_DISPLAY)
            {
                g_format = (g_format == VIEW_HH_MM) ? VIEW_MM_SS : VIEW_HH_MM;
            }
            else
            {
                g_set_field = (g_set_field == FIELD_SS) ? FIELD_HH : (SetField)(g_set_field + 1);
                g_blink_visible = 1;
                g_blink_ms = 0;
            }
            break;

        case EV_INC:
            if (g_mode == MODE_SET)
            {
                ClockTime tmp;
                ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { tmp = g_time; }
                Clock_IncField(&tmp, g_set_field);
                Clock_Normalize(&tmp);
                ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { g_time = tmp; }
                g_blink_visible = 1;
                g_blink_ms = 0;
            }
            break;

        case EV_DEC:
            if (g_mode == MODE_SET)
            {
                ClockTime tmp;
                ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { tmp = g_time; }
                Clock_DecField(&tmp, g_set_field);
                Clock_Normalize(&tmp);
                ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { g_time = tmp; }
                g_blink_visible = 1;
                g_blink_ms = 0;
            }
            break;

        default:
            break;
    }
}

static void GPIO_Init(void)
{
    SEG_DDR = 0xFF;
    SEG_PORT = 0x00;

    DIG_DDR |= 0x0F;
    DIG_PORT &= (uint8_t)~0x0F;

    BTN_DDR_REG &= (uint8_t)~((1u << BTN_INC_PIN) |
                              (1u << BTN_DEC_PIN) |
                              (1u << BTN_MODE_PIN) |
                              (1u << BTN_NEXT_PIN));

    BTN_PORT_REG &= (uint8_t)~((1u << BTN_INC_PIN) |
                               (1u << BTN_DEC_PIN) |
                               (1u << BTN_MODE_PIN) |
                               (1u << BTN_NEXT_PIN));
}

static void Timer0_Init_1ms(void)
{
    TCCR0 = (1u << WGM01) | (1u << CS01) | (1u << CS00);
    OCR0 = 124;
    TIMSK |= (1u << OCIE0);
}

static void Timer1_Init_1Hz(void)
{
    TCCR1A = 0x00;
    TCCR1B = (1u << WGM12) | (1u << CS12);
    OCR1A = 31249;
    TIMSK |= (1u << OCIE1A);
}

static void ExtInterrupts_Init(void)
{
    MCUCR |= (1u << ISC01);
    MCUCR &= (uint8_t)~(1u << ISC00);

    MCUCR |= (1u << ISC11);
    MCUCR &= (uint8_t)~(1u << ISC10);

    GICR |= (1u << INT0) | (1u << INT1);
}

ISR(TIMER0_COMP_vect)
{
    static uint8_t cnt10ms = 0;

    Display_ScanStep();

    if (g_mode == MODE_SET)
    {
        g_blink_ms++;
        if (g_blink_ms >= 250)
        {
            g_blink_ms = 0;
            g_blink_visible ^= 1u;
        }
    }
    else
    {
        g_blink_visible = 1;
        g_blink_ms = 0;
    }

    cnt10ms++;
    if (cnt10ms >= 10)
    {
        cnt10ms = 0;
        Buttons_Poll10ms();
    }
}

ISR(TIMER1_COMPA_vect)
{
    RB_Push((RingBuffer *)&g_queue, EV_TICK_1S);
}

ISR(INT0_vect)
{
    RB_Push((RingBuffer *)&g_queue, EV_MODE_TOGGLE);
}

ISR(INT1_vect)
{
    RB_Push((RingBuffer *)&g_queue, EV_NEXT_FIELD_OR_FORMAT);
}

int main(void)
{
    event e;
	ClockTime start_time;
    RB_Init((RingBuffer *)&g_queue);
    GPIO_Init();
    Timer0_Init_1ms();
    Timer1_Init_1Hz();
    ExtInterrupts_Init();
	start_time = ClockTime_FromBuildTime();
    Display_UpdateBuffer();
	
    sei();
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		g_time = start_time;
	}

    while (1)
    {
        if (RB_Pop((RingBuffer *)&g_queue, &e))
        {
            App_HandleEvent(e);
        }

        Display_UpdateBuffer();
    }
}