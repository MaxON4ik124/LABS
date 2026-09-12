#ifndef SLEEP_H
#define SLEEP_H

#include "threads/thread.h"
#include "list.h"
#include <stdint.h>
#include <stdbool.h>

void sleeping_list_init(void);

bool wake_time_less(const struct list_elem *, const struct list_elem *, void *aux);

void add_sleeping_thread(struct thread *, int64_t);

void wake_up_threads(int64_t current_tick);

#endif
