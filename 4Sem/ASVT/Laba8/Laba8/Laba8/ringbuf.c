#include "ringbuf.h"

static uint8_t next_index(const ringbuf_t *rb, uint8_t index)
{
    return (uint8_t)((index + 1u) & rb->mask);
}

void ringbuf_init(ringbuf_t *rb, uint8_t *storage, uint8_t size)
{
    rb->head = 0u;
    rb->tail = 0u;
    rb->mask = (uint8_t)(size - 1u);
    rb->data = storage;
    rb->is_full = 0u;
}
uint8_t ringbuf_count(const ringbuf_t *rb)
{
    return (uint8_t)((rb->head - rb->tail) & rb->mask);
}

bool ringbuf_push(ringbuf_t *rb, uint8_t value)
{
    uint8_t next = next_index(rb, rb->head);
    if (rb->is_full) return false;
    rb->is_full = next + 1 == rb->tail ? 1 : 0;
    rb->data[rb->head] = value;
    rb->head = next;
    return true;
}

bool ringbuf_pop(ringbuf_t *rb, uint8_t *value)
{
    if (rb->head == rb->tail) return false;
    rb->is_full = 0;
    *value = rb->data[rb->tail];
    rb->tail = next_index(rb, rb->tail);
    return true;
}

bool ringbuf_push_isr(ringbuf_t *rb, uint8_t value)
{
    return ringbuf_push(rb, value);
}

bool ringbuf_pop_isr(ringbuf_t *rb, uint8_t *value)
{
    return ringbuf_pop(rb, value);
}
