    
/*
  File for 'hair' task implementation.
*/

/*#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/malloc.h"
#include "threads/thread.h"
#include "threads/synch.h"
#include "devices/timer.h"


// Монитор парикмахерской
static struct lock barber_lock;       // Защита общих ресурсов
static struct condition barber_cond; // Условие для парикмахера (ожидание клиента)
static struct condition client_cond; // Условие для клиентов (ожидание своей очереди)
static int clients_waiting = 0;       // Количество ожидающих клиентов
static bool barber_busy = false;      // Статус парикмахера

static void init(void) // инициализируем мониторы для состояний клиента и барбера, а также устанавливаем занятость барбера и кол-во людей в очереди
{
    lock_init(&barber_lock);
    cond_init(&barber_cond);
    cond_init(&client_cond);
    clients_waiting = 0;
    barber_busy = false;

}

static void hairdresser(void* arg UNUSED) // функция поведения барбера
{
    msg("hairdresser created.");

    while (true) {
        lock_acquire(&barber_lock);

        // барбер спит, если в очереди нет людей
        while (clients_waiting == 0) {
            msg("Barber sleeping\n");
            cond_wait(&barber_cond, &barber_lock);
        }

        clients_waiting--;
        barber_busy = true;
        lock_release(&barber_lock);

        timer_sleep(10);

        lock_acquire(&barber_lock);
        barber_busy = false;
        cond_signal(&client_cond, &barber_lock); // сигнал, что барбер свободен, можно стричь следующего клиента  
        lock_release(&barber_lock);

}
}

static void client(void* arg UNUSED)  // функция поведения клиента
{
    msg("client %d created.", (int) arg);

    lock_acquire(&barber_lock);
    clients_waiting++;

    if(!barber_busy) cond_signal(&barber_cond, &barber_lock);
// клиент ждет, если барбер занят 
    while (barber_busy) cond_wait(&client_cond, &barber_lock);
    
    // Симуляция стрижки
    msg("Barber awake and now start working");

    
    msg("Client %d is getting a haircut\n", (int)arg);
    lock_release(&barber_lock);
    timer_sleep(10);
}

void test_hair(unsigned int num_clients, unsigned int interval)
{
  unsigned int i;
  init();

  thread_create("hairdresser", PRI_DEFAULT, &hairdresser, NULL);

  for(i = 0; i < num_clients; i++)
  {
    char name[32];
    timer_sleep(interval);
    snprintf(name, sizeof(name), "client_%d", i + 1);
    thread_create(name, PRI_DEFAULT, &client, (void*) (i+1) );
  }

  timer_msleep(5000);
  pass();
}
*/



#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/malloc.h"
#include "threads/thread.h"
#include "threads/synch.h"
#include "devices/timer.h"

// Монитор парикмахерской
static struct lock barber_lock;       // Мьютекс для защиты общих переменных
static struct condition barber_cv;    // Условие, на котором парикмахер ждёт появления клиента
static struct condition customer_cv;  // Условие, на котором клиент ждёт приглашения сесть в кресло
static int waiting_customers = 0;       // Количество ожидающих клиентов

// Инициализация монитора
static void init(void) {
    lock_init(&barber_lock);
    cond_init(&barber_cv);
    cond_init(&customer_cv);
    waiting_customers = 0;
}

// Функция поведения парикмахера (барбера)
static void hairdresser(void* arg UNUSED) {
    msg("Starting hairdresser.");

    while (true) {
        lock_acquire(&barber_lock);
        // Если нет клиентов, парикмахер засыпает
        while (waiting_customers == 0) {
            msg("hairdresser sleep.");
            cond_wait(&barber_cv, &barber_lock);
        }
        // Есть хотя бы один ожидающий клиент – приглашаем его
        waiting_customers--;
        cond_signal(&customer_cv, &barber_lock);
        lock_release(&barber_lock);

        // Симуляция стрижки (только парикмахер выполняет timer_sleep)
        msg("hairdresser start making hair.");
        timer_sleep(10);
        msg("Hairdresser end.");
    }
}   

// Функция поведения клиента
static void client(void* arg) {
    int id = (int)arg;
    msg("Client %d has come.", id);

    lock_acquire(&barber_lock);
    // Клиент заходит в приёмную – увеличиваем счётчик ожидающих
    waiting_customers++;
    // Если парикмахер спит, сигнализируем ему о наличии клиента
    cond_signal(&barber_cv, &barber_lock);
    // Ждем, пока парикмахер пригласит нас сесть в кресло
    cond_wait(&customer_cv, &barber_lock);
    lock_release(&barber_lock);

    // Клиент получает стрижку (симуляция стрижки уже выполнена парикмахером)
    msg("CLient %d getting haircut.", id);
}

void test_hair(unsigned int num_clients, unsigned int interval) {
    unsigned int i;
    init();

    thread_create("hairdresser", PRI_DEFAULT, &hairdresser, NULL);

    for (i = 0; i < num_clients; i++) {
        timer_sleep(interval);
        char name[32];
        snprintf(name, sizeof(name), "client_%d", i + 1);
        thread_create(name, PRI_DEFAULT, &client, (void*)(i + 1));
    }

    timer_msleep(5000);
    pass();
}