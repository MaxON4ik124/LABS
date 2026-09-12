#ifndef RINGBUF_H
#define RINGBUF_H

#include <stdint.h>
#include <stdbool.h>


typedef struct
{
    volatile uint8_t head;
    volatile uint8_t tail;
    uint8_t mask;
    uint8_t *data;
    uint8_t is_full: 1;
} ringbuf_t;

void ringbuf_init(ringbuf_t *rb, uint8_t *storage, uint8_t size);
uint8_t ringbuf_count(const ringbuf_t *rb);

bool ringbuf_push(ringbuf_t *rb, uint8_t value);
bool ringbuf_pop(ringbuf_t *rb, uint8_t *value);

bool ringbuf_push_isr(ringbuf_t *rb, uint8_t value);
bool ringbuf_pop_isr(ringbuf_t *rb, uint8_t *value);

#endif /* RINGBUF_H */
