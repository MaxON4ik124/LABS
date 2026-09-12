
/* File for 'narrow_bridge' task implementation.  
   SPbSTU, IBKS, 2017 */
#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/thread.h"
#include "threads/synch.h"
#include "narrow-bridge.h"

#define DIR_COUNT 2
#define PRIO_COUNT 2

int cars_on;
int dir_now;
int total_cars;
struct semaphore sem[DIR_COUNT * PRIO_COUNT];
int car_count[DIR_COUNT * PRIO_COUNT];
int switch_flag;
int other_dir;
struct lock bridge_lock;

void wake_up(void);
void initialize_semaphores(void);
int get_index(int dir, int prio);

int get_index(int dir, int prio) {
    return dir * PRIO_COUNT + prio;
}

void initialize_semaphores(void) {
    lock_init(&bridge_lock);
    for (int i = 0; i < DIR_COUNT * PRIO_COUNT; i++) {
        sema_init(&sem[i], 0);
        car_count[i] = 0;
    }
}

void wake_up(void) {
    int high_prio_index = get_index(dir_now, 1);
    int low_prio_index = get_index(dir_now, 0);

    if (car_count[high_prio_index] >= 2) {
        sema_up(&sem[high_prio_index]);
        sema_up(&sem[high_prio_index]);
        cars_on = 2;
    } else if (car_count[high_prio_index] == 1 && car_count[low_prio_index] > 0) {
        sema_up(&sem[low_prio_index]);
        sema_up(&sem[high_prio_index]);
        cars_on = 2;
    } else if (car_count[high_prio_index] == 0 && car_count[low_prio_index] >= 2) {
        sema_up(&sem[low_prio_index]);
        sema_up(&sem[low_prio_index]);
        cars_on = 2;
    } else if (car_count[high_prio_index] == 1) {
        sema_up(&sem[high_prio_index]);
        cars_on++;
    } else if (car_count[low_prio_index] == 1) {
        sema_up(&sem[low_prio_index]);
        cars_on++;
    } 

}

void narrow_bridge_init(void) {
    total_cars = 0;
    cars_on = 0;
    switch_flag = 0;
    initialize_semaphores();
}

void arrive_bridge(enum car_priority prio, enum car_direction dir) {
    if (dir < 0 || dir >= DIR_COUNT || prio < 0 || prio >= PRIO_COUNT) return;
    
    int index = get_index(dir, prio);
    lock_acquire(&bridge_lock);
    car_count[index]++;
    total_cars++;
    
    if (total_cars == 1 && cars_on == 0) {
        dir_now = dir;
    }
    
    if ((total_cars <= 2) && (cars_on < 2) && (dir == dir_now)) {
        cars_on++;
        sema_up(&sem[index]);
    }
    lock_release(&bridge_lock);
    sema_down(&sem[index]);
}

void exit_bridge(enum car_priority prio, enum car_direction dir) {
    if (dir < 0 || dir >= DIR_COUNT || prio < 0 || prio >= PRIO_COUNT) return;
    
    int index = get_index(dir, prio);
    lock_acquire(&bridge_lock);
    
    cars_on = (cars_on > 0) ? cars_on - 1 : 0;
    total_cars = (total_cars > 0) ? total_cars - 1 : 0;
    car_count[index] = (car_count[index] > 0) ? car_count[index] - 1 : 0;
    other_dir = (dir_now == 0) ? 1 : 0;
    
    if (cars_on == 0 && total_cars > 0) {
        if (car_count[get_index(other_dir, 0)] == 0 && car_count[get_index(other_dir, 1)] == 0) {
            switch_flag = 1;
        }
        if (car_count[get_index(dir_now, 1)] > 0 && car_count[get_index(other_dir, 1)] == 0) {
            switch_flag = 1;
        }
        if (!switch_flag) {
            dir_now = other_dir;
        }
        wake_up();
        switch_flag = 0;
    }
    lock_release(&bridge_lock);
}
