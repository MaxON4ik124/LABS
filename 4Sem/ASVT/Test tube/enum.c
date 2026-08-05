#define F_CPU 8000000UL

#include <stdio.h>
// #include <avr/io.h>
// #include <avr/interrupt.h>
// #include <util/atomic.h>
#include <stdint.h>
#include <stdbool.h>


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
    MODE_VIEW = 0,
    MODE_SETUP = 1
} clock_mode;
typedef enum
{
    VIEW_MM_SS = 0,
    VIEW_HH_MM = 1
} clock_view_mode;
typedef enum
{
    SETUP_HH = 0,
    SETUP_MM = 1,
    SETUP_SS = 2
} clock_setup_mode;
typedef enum
{
    EVENT_IDLE,
    EVENT_EDIT_VIEW,
    EVENT_TOGGLE_VIEW,
    EVENT_INC_STEP,
    EVENT_DEC_STEP
} event;
int main()
{
    event curevent = EVENT_DEC_STEP;
    printf("%d", curevent);
}