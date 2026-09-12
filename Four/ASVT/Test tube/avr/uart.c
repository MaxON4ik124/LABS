#include "uart.h"
#include "config.h"
#include "ringbuf.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

static ringbuf_t g_rx_rb;
static ringbuf_t g_tx_rb;
static uint8_t g_rx_storage[UART_RX_BUF_SIZE];
static uint8_t g_tx_storage[UART_TX_BUF_SIZE];

static uint16_t uart_calc_ubrr(void)
{
    return (uint16_t)((F_CPU / (8UL * UART_BAUD)) - 1UL);
}

void uart_init(void)
{
    const uint16_t ubrr = uart_calc_ubrr();

    ringbuf_init(&g_rx_rb, g_rx_storage, (uint8_t)UART_RX_BUF_SIZE);
    ringbuf_init(&g_tx_rb, g_tx_storage, (uint8_t)UART_TX_BUF_SIZE);

    UCSRA = _BV(U2X);
    UBRRH = (uint8_t)(ubrr >> 8);
    UBRRL = (uint8_t)(ubrr & 0xFFu);

    UCSRB = _BV(RXEN) | _BV(TXEN) | _BV(RXCIE);
    UCSRC = _BV(URSEL) | _BV(UCSZ1) | _BV(UCSZ0); /* 8N1 */
}

bool uart_read_byte(uint8_t *byte_out)
{
    bool ok;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        ok = ringbuf_pop(&g_rx_rb, byte_out);
    }
    return ok;
}

void uart_write_byte(uint8_t byte_value)
{
    bool pushed = false;

    while (!pushed)
    {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
            pushed = ringbuf_push(&g_tx_rb, byte_value);
            if (pushed)
            {
                UCSRB |= _BV(UDRIE);
            }
        }
    }
}

void uart_write_block(const uint8_t *data, uint16_t size)
{
    uint16_t i;
    for (i = 0u; i < size; ++i)
    {
        uart_write_byte(data[i]);
    }
}

ISR(USART_RXC_vect)
{
    const uint8_t value = UDR;
    (void)ringbuf_push_isr(&g_rx_rb, value);
}

ISR(USART_UDRE_vect)
{
    uint8_t value;
    if (ringbuf_pop_isr(&g_tx_rb, &value))
    {
        UDR = value;
    }
    else
    {
        UCSRB &= (uint8_t)~_BV(UDRIE);
    }
}
