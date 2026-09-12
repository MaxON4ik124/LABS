/* tests/threads/test-new-alg.c
 *
 * Тест нового алгоритма SJF, где effective_priority вычисляется как:
 *      effective_priority = base_priority - cpu_burst
 *
 * Параметры процессов:
 *   Proc0: cpu_burst = 2, base_priority = 63  → effective_priority = 63 - 2 = 61
 *   Proc1: cpu_burst = 3, base_priority = 23  → effective_priority = 23 - 3 = 20
 *   Proc2: cpu_burst = 1, base_priority = 63  → effective_priority = 63 - 1 = 62 (самый высокий)
 *   Proc3: cpu_burst = 5, base_priority = 27  → effective_priority = 27 - 5 = 22
 *
 * Ожидаемый порядок выполнения: сначала Proc2, затем Proc0, затем Proc3 и Proc1.
 */

#include "tests/threads/tests.h"
#include "threads/thread.h"
#include "threads/synch.h"
#include "devices/timer.h"
#include <stdio.h>

#define NUM_PROCS 4

/* Глобальный счётчик завершённых процессов и блокировка для его защиты */
static int finished_count = 0;
static struct lock finish_lock;

/* Структура с параметрами процесса */
struct proc_info {
  const char *name;      /* Имя процесса */
  int cpu_burst;         /* Значение cpu_burst */
  int base_priority;     /* Базовый приоритет */
};

/* Функция, исполняемая каждым тестовым процессом */
static void
process_function (void *aux) {
  struct proc_info *info = (struct proc_info *) aux;
  int burst = info->cpu_burst;
  int tick = 0;
  while (tick < burst) {
    printf("Process %s: tick %d (cpu_burst=%d)\n",
           thread_name(), tick, burst);
    tick++;
    timer_sleep(1);  /* Имитируем работу: ожидание 1 тик */
  }
  printf("Process %s: finished\n", thread_name());

  lock_acquire(&finish_lock);
  finished_count++;
  lock_release(&finish_lock);
  thread_exit();
}

/* Основная функция теста */
void
test_new_alg (void) {
  /* Параметры создаваемых процессов */
  struct proc_info procs[NUM_PROCS] = {
    { "Proc0", 2, 63 },
    { "Proc1", 3, 23 },
    { "Proc2", 1, 63 },
    { "Proc3", 5, 27 }
  };

  printf("(test-new-alg) begin\n");

  /* Переключаем режим планирования в SJF */
  current_scheduler_mode = SCHED_SJF;

  lock_init(&finish_lock);
  int i; 
  /* Устанавливаем максимальный приоритет для главного потока, чтобы потоки не стартовали раньше создания */
  thread_set_priority(PRI_MAX);

  for (i = 0; i < NUM_PROCS; i++) {
    thread_create_with_burst(procs[i].name, procs[i].base_priority,
                             process_function, &procs[i],
                             procs[i].cpu_burst);
  }

  /* Понижаем приоритет главного потока, чтобы тестовые процессы могли выполняться */
  thread_set_priority(PRI_MIN);

  /* Ждем, пока все тестовые процессы завершатся */
  while (finished_count < NUM_PROCS)
    timer_sleep(1);

  printf("(test-new-alg) end\n");
}
