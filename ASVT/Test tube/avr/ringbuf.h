#ifndef RINGBUF_H
#define RINGBUF_H

#include <stdint.h>
#include <stdbool.h>

/*
 * Generic single-producer/single-consumer circular buffer.
 * Size must be a power of two; mask = size - 1.
 */
typedef struct
{
    volatile uint8_t head;
    volatile uint8_t tail;
    uint8_t mask;
    uint8_t *data;
    uint8_t is_full;
} ringbuf_t;

void ringbuf_init(ringbuf_t *rb, uint8_t *storage, uint8_t size);
bool ringbuf_is_empty(const ringbuf_t *rb);
bool ringbuf_is_full(const ringbuf_t *rb);
uint8_t ringbuf_count(const ringbuf_t *rb);

bool ringbuf_push(ringbuf_t *rb, uint8_t value);
bool ringbuf_pop(ringbuf_t *rb, uint8_t *value);

/* ISR helpers: identical logic, but named separately for clarity in listings. */
bool ringbuf_push_isr(ringbuf_t *rb, uint8_t value);
bool ringbuf_pop_isr(ringbuf_t *rb, uint8_t *value);

#endif /* RINGBUF_H */
