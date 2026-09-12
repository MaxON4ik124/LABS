/*
  File for 'birds' task implementation.
*/

#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/malloc.h"
#include "threads/thread.h"
#include "threads/synch.h"
#include "devices/timer.h"

struct condition cond_mother;

struct lock lock_dish;

int active_dish;
int max_dish;

static void init(unsigned int dish_size) // dish_size == F
{
    cond_init(&cond_mother);
    lock_init(&lock_dish);
    active_dish = dish_size;
    max_dish = dish_size;
}

static void bird(void* arg) // mother
{
    msg("bird created.");
    lock_acquire(&lock_dish);

    while(1){
      cond_wait(&cond_mother, &lock_dish);
      active_dish = max_dish;
      msg("mother gave food                ticks:%4llu", (unsigned long long) timer_ticks ());
    }
}

static void chick(void* arg) // children
{
    msg("chick %4d created.", (int) arg);
    lock_acquire(&lock_dish);
    active_dish--;
    msg("chick %4d ate and go to sleep    ticks:%4llu", (int) arg, (unsigned long long) timer_ticks ()); //
    timer_sleep(2);
    msg("chick %4d woke up                ticks:%4llu", (int) arg, (unsigned long long) timer_ticks ()); //
    if (active_dish == 0){
      msg("chick %4d called mother", (int) arg);
      cond_signal(&cond_mother, &lock_dish);
      lock_release(&lock_dish);
    } else {
      lock_release(&lock_dish);
    }
}


void test_birds(unsigned int num_chicks, unsigned int dish_size)
{
  unsigned int i;
  init(dish_size);

  thread_create("bird", PRI_DEFAULT, &bird, NULL);

  for(i = 0; i < num_chicks; i++)
  {
    char name[32];
    snprintf(name, sizeof(name), "chick_%d", i + 1);
    thread_create(name, PRI_DEFAULT, &chick, (void*) (i+1) );
  }

  timer_msleep(5000);
  pass();
}
