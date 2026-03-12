#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <stdint.h>
#include <stdbool.h>

#define RINGBUF_SIZE 32
// Делаем макросы (PORTC - выбор ячейки дисплея)
#define SEG_PORT   PORTC
#define SEG_DDR    DDRC
#define SEG_DP    0x80
#define SEG_BLANK 0x00
// PORTA - выбор самого числа
#define DIG_PORT   PORTA
#define DIG_DDR    DDRA
#define DIG_MASK   0x0F
// PORTD - порт управления
#define BTN_PIN    PIND
#define BTN_PORT   PORTD
#define BTN_DDR    DDRD

#define BTN_INC    PD0
#define BTN_DEC    PD1
#define BTN_INT0   PD2
#define BTN_INT1   PD3

// Числа на дисплее
static uint8_t seg_digits[10] = 
{
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111 // 9
};

// Enum для режима, отображения, типа времени
typedef enum
{
    MODE_VIEW,
    MODE_SETUP
} clock_mode;
typedef enum
{
    VIEW_MM_SS,
    VIEW_HH_MM
} clock_view_mode;
typedef enum
{
    SETUP_HH,
    SETUP_MM,
    SETUP_SS
} clock_setup_mode;
typedef enum
{
    EVENT_IDLE,
    EVENT_EDIT_VIEW,
    EVENT_TOGGLE_VIEW,
    EVENT_INC_STEP,
    EVENT_DEC_STEP
} event;


// Кольцевой буфер


typedef struct
{
    volatile event buffer[RINGBUF_SIZE];
    volatile uint8_t head;
    volatile uint8_t tail;
} RingBuffer;
static void RB_Push(RingBuffer *ring_buffer, event e)
{
    uint8_t next = (ring_buffer->head + 1u) % RINGBUF_SIZE;
    if(next != rb->tail)
    {
        rb->buffer[rb->head] = e;
        rb->head = next
    }
}
static void RB_Pop(RingBuffer* ring_buffer, event *e)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        if(ring_buffer->tail != ring_buffer->head)
        {
            *e = ring_buffer->buffer[ring_buffer->tail]
            ring_buffer->tail = (ring_buffer->tail + 1) % RINGBUF_SIZE;
        }
    }
}
