/*
  File for 'bear' task implementation.
*/

#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/malloc.h"
#include "threads/thread.h"
#include "threads/synch.h"
#include "devices/timer.h"
/*

static void init(unsigned int pot_size UNUSED)
{
    // Not implemented.
}

static void bear(void* arg UNUSED)
{
    msg("bear created.");

    // Not implemented.
}

static void bee(void* arg UNUSED)
{
    msg("bee %d created.", (int) arg);

    // Not implemented.
} */

struct pot {
    struct lock lck;
    struct condition full;
    struct condition wait;
    int honey;
    int cap;
    int collected;
    bool bear_eating;
} global_pot;

void init(int cap);
void collect_honey(void);
void add_honey(void);
void bear(void *arg);
void bee(void *arg);
void test_bear(unsigned int num_bees, unsigned int pot_size);

void init(int cap) {
    lock_init(&global_pot.lck);
    cond_init(&global_pot.full);
    cond_init(&global_pot.wait);
    global_pot.honey = 0;
    global_pot.cap = cap;
    global_pot.collected = 0;
    global_pot.bear_eating = false;
}
void collect_honey(void) {
    timer_sleep(10);
    lock_acquire(&global_pot.lck);
    global_pot.collected++;
    printf("Пчела собрала мёд: %d\n", global_pot.collected);
    lock_release(&global_pot.lck);
}

void add_honey(void) {
    lock_acquire(&global_pot.lck);
    while (global_pot.honey == global_pot.cap) {
        cond_wait(&global_pot.full, &global_pot.lck);
    }
    if (!global_pot.bear_eating && global_pot.collected > 0) {
        int can_add = global_pot.cap - global_pot.honey;
        int to_add = (global_pot.collected <= can_add) ? global_pot.collected : can_add;
        global_pot.honey += to_add;
        global_pot.collected -= to_add;
        printf("Пчела добавила мёд: %d/%d\n", global_pot.honey, global_pot.cap);
    }
    if (global_pot.honey == global_pot.cap) {
        global_pot.bear_eating = true;
        printf("Медведь ест мёд...\n");
        cond_signal(&global_pot.full, &global_pot.lck);
    }
    lock_release(&global_pot.lck);
}

/* Медведь ест мёд */
void bear(void *arg) {
    (void)arg;
    while (true) {
        lock_acquire(&global_pot.lck);
        while (global_pot.honey < global_pot.cap) {
            printf("Медведь спит...\n");
            cond_wait(&global_pot.full, &global_pot.lck);
        }
        printf("Медведь съел мёд и снова заснул.\n");
        global_pot.honey = 0;
        global_pot.bear_eating = false;
        cond_broadcast(&global_pot.wait, &global_pot.lck);
        lock_release(&global_pot.lck);
    }
}

void bee(void *arg) {
    (void)arg;
    while (true) {
        collect_honey();
        add_honey();
    }
}
void test_bear(unsigned int num_bees, unsigned int pot_size)
{
  unsigned int i;
  init(pot_size);

  thread_create("bear", PRI_DEFAULT, &bear, NULL);

  for(i = 0; i < num_bees; i++)
  {
    char name[32];
    snprintf(name, sizeof(name), "bee_%d", i + 1);
    thread_create(name, PRI_DEFAULT, &bee, (void*) (i+1) );
  }

  timer_msleep(5000);
  pass();    
}

