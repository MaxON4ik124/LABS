#include "threads/thread.h"
#include "list.h"
#include "threads/interrupt.h"
#include "threads/synch.h"
#include <stdint.h>
#include <stdbool.h>

static struct list sleeping_list;

void sleeping_list_init(void) {
  list_init(&sleeping_list);
}

bool wake_time_less (const struct list_elem *a, const struct list_elem *b, void *aux UNUSED) {
  struct thread *t_a = list_entry(a, struct thread, sleep_elem);
  struct thread *t_b = list_entry(b, struct thread, sleep_elem);

  return t_a->wake_time < t_b->wake_time;
}

// Добавляем и блокируем
void add_sleeping_thread(struct thread *t, int64_t wake_time) {
  // Для соблюдение атомарности - запрещаем прерывания
  enum intr_level old_level = intr_disable();

  t->wake_time = wake_time;
  list_insert_ordered(&sleeping_list, &t->sleep_elem, wake_time_less, NULL);

  thread_block();

  intr_set_level(old_level);
}

// Вызываем каждый тик таймера
void wake_up_threads(int64_t current_tick) {
  // Удаляем и разблокируем все потоки, которые должны проснуться
  while (!list_empty(&sleeping_list)) {
    struct list_elem *e = list_front(&sleeping_list);
    struct thread *t = list_entry(e, struct thread, sleep_elem);

    // Нет смысла проверять дальше, потому что они тоже спят
    if (t->wake_time > current_tick)
      break;

    list_remove(&t->sleep_elem);
    thread_unblock(t);
  }
}
