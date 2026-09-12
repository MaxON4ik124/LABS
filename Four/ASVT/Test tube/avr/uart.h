#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdbool.h>

void uart_init(void);
bool uart_read_byte(uint8_t *byte_out);
void uart_write_byte(uint8_t byte_value);
void uart_write_block(const uint8_t *data, uint16_t size);

#endif /* UART_H */
