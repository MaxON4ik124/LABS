/* File for 'narrow_bridge' task implementation.  
   SPbSTU, IBKS, 2017 */

#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/thread.h"
#include "threads/synch.h"
#include "narrow-bridge.h"
#include "devices/timer.h"

// Глобальные переменные для управления мостом
unsigned int cars_on_bridge = 0;
unsigned int cars_crossed = 0;
unsigned int waiting_normal_left = 0, waiting_normal_right = 0;
unsigned int waiting_emergency_left = 0, waiting_emergency_right = 0;
int normal_left_counter,
    normal_right_counter,
    emergency_left_counter,
    emergency_right_counter;

enum bridge bridge_direction;

// Семафоры для контроля потока машин
struct semaphore sem_normal_left, sem_normal_right;
struct semaphore sem_emergency_left, sem_emergency_right;

// Инициализация моста и семафоров
void narrow_bridge_init(void) {
  sema_init(&sem_normal_left, 0);
  sema_init(&sem_normal_right, 0);
  sema_init(&sem_emergency_left, 0);
  sema_init(&sem_emergency_right, 0);
  bridge_direction = neutral;
  cars_on_bridge = 0;
  
  normal_left_counter = normal_right_counter = emergency_left_counter = emergency_right_counter = 0;
  waiting_normal_left = waiting_normal_right = waiting_emergency_left = waiting_emergency_right = 0;
}


void arrive_from_left(enum car_priority prio)
{
  /* Если мост занят автомобилями, движущимися в противоположном направлении, 
     или если на мосту уже 2 автомобиля, или если очередь уже достаточно длинная,
     автомобиль должен ждать */
  if ((bridge_direction != left && bridge_direction != neutral) ||
      cars_on_bridge == 2 ||
      waiting_emergency_left >= 2 ||
      ((waiting_normal_left + waiting_emergency_left) >= 2 && prio == car_normal)) {
      
      if (prio == car_normal) {
        waiting_normal_left++;
        sema_down(&sem_normal_left);
        waiting_normal_left--;
      } else { // аварийный автомобиль
        waiting_emergency_left++;
        sema_down(&sem_emergency_left);
        waiting_emergency_left--;
      }
  }
  
  /* Дополнительная проверка на наличие автомобилей с правой стороны */
  if (prio == car_emergency) {
      if (!(cars_on_bridge < 2 && (normal_right_counter + emergency_right_counter == 0))) {
        waiting_emergency_left++;
        sema_down(&sem_emergency_left);
        waiting_emergency_left--;
      }
      emergency_left_counter++;
      cars_on_bridge++;
  } else { // обычный автомобиль
      if (!(cars_on_bridge < 2 && (normal_right_counter + emergency_right_counter == 0))) {
        waiting_normal_left++;
        sema_down(&sem_normal_left);
        waiting_normal_left--;
      }
      normal_left_counter++;
      cars_on_bridge++;
  }
  
  /* Устанавливаем активное направление для моста */
  bridge_direction = left;
}

void arrive_from_right(enum car_priority prio)
{
  if ((bridge_direction != right && bridge_direction != neutral) ||
      cars_on_bridge == 2 ||
      waiting_emergency_right >= 2 ||
      ((waiting_normal_right + waiting_emergency_right) >= 2 && prio == car_normal)) {
      
      if (prio == car_normal) {
        waiting_normal_right++;
        sema_down(&sem_normal_right);
        waiting_normal_right--;
      } else {
        waiting_emergency_right++;
        sema_down(&sem_emergency_right);
        waiting_emergency_right--;
      }
  }
  
  if (prio == car_emergency) {
      if (!(cars_on_bridge < 2 && (normal_left_counter + emergency_left_counter == 0))) {
        waiting_emergency_right++;
        sema_down(&sem_emergency_right);
        waiting_emergency_right--;
      }
      emergency_right_counter++;
      cars_on_bridge++;
  } else {
      if (!(cars_on_bridge < 2 && (normal_left_counter + emergency_left_counter == 0))) {
        waiting_normal_right++;
        sema_down(&sem_normal_right);
        waiting_normal_right--;
      }
      normal_right_counter++;
      cars_on_bridge++;
  }
  
  bridge_direction = right;
}


void movingleft(int dir) { // 1 - left, 2 - right
    if (waiting_emergency_right >= 2) {
      sema_up(&sem_emergency_right);
      sema_up(&sem_emergency_right);
    } else if (waiting_emergency_right == 1) {
      sema_up(&sem_emergency_right);
      sema_up(&sem_normal_right);
    } else if (waiting_emergency_left >= 2) {
      sema_up(&sem_emergency_left);
      sema_up(&sem_emergency_left);
    } else if (waiting_emergency_left == 1) {
      sema_up(&sem_emergency_left);
      sema_up(&sem_normal_left);
    } else if (waiting_normal_right != 0) {
      sema_up(&sem_normal_right);
      sema_up(&sem_normal_right);
    } else if (waiting_normal_left != 0) {
      sema_up(&sem_normal_left);
      sema_up(&sem_normal_left);
    }
}

void movingright(void){
    if (waiting_emergency_left >= 2) {
      sema_up(&sem_emergency_left);
      sema_up(&sem_emergency_left);
    } else if (waiting_emergency_left == 1) {
      sema_up(&sem_emergency_left);
      sema_up(&sem_normal_left);
    } else if (waiting_emergency_right >= 2) {
      sema_up(&sem_emergency_right);
      sema_up(&sem_emergency_right);
    } else if (waiting_emergency_right == 1) {
      sema_up(&sem_emergency_right);
      sema_up(&sem_normal_right);
    } else if (waiting_normal_left != 0) {
      sema_up(&sem_normal_left);
      sema_up(&sem_normal_left);
    } else if (waiting_normal_right != 0) {
      sema_up(&sem_normal_right);
      sema_up(&sem_normal_right);
    }
  }


void arrive_bridge(enum car_priority prio UNUSED, enum car_direction dir UNUSED)
{
  if (dir == dir_left) {
    arrive_from_left(prio);
  } else if (dir == dir_right) {
    arrive_from_right(prio);
  }
}

void exit_from_side(int *emergency_counter, int *normal_counter,
                           void (*choose_next)(void))
{
    int removed = 0;
    /* Удаляем до двух автомобилей */
    while (removed < 2 && (*emergency_counter > 0 || *normal_counter > 0)) {
        if (*emergency_counter > 0) {
            (*emergency_counter)--;
        } else if (*normal_counter > 0) {
            (*normal_counter)--;
        }
        if (cars_on_bridge > 0) {
            cars_on_bridge--;
        }
        removed++;
    }
    choose_next();
}

/* Улучшенная функция exit_bridge, которая использует exit_from_side для уменьшения дублирования кода */
void exit_bridge(enum car_priority prio, enum car_direction dir)
{
    /* Если на мосту нет автомобилей, выходим */
    if (cars_on_bridge == 0)
        return;

    /* Выбираем сторону по активному направлению */
    if (bridge_direction == left) {
        exit_from_side(&emergency_left_counter, &normal_left_counter, movingleft);
    } else if (bridge_direction == right) {
        exit_from_side(&emergency_right_counter, &normal_right_counter, movingright);
    }
}
