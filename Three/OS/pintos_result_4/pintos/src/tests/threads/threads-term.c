/*
  File for 'threads-term' task implementation.
*/

#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/malloc.h"
#include "threads/thread.h"
#include "devices/timer.h"



void loop(void);



void test_threads_term(void){
	char thread_name[16];
	int count = 0;
	tid_t t_1;
	tid_t t_3;
	while(count!=5){
		
		snprintf(thread_name, sizeof thread_name, "thread_%d", ++count);
		
		if(count == 1){
			t_1 = thread_create(thread_name, PRI_DEFAULT, loop, NULL);
		}
		else if(count == 3){
			t_3 = thread_create(thread_name, PRI_DEFAULT, loop, NULL);		
		}
		else{
			thread_create(thread_name, PRI_DEFAULT, loop, NULL);
		}
		
	}
	

	timer_msleep(4000);
	//printf("hello!!!");
	thread_terminate(t_1);
	thread_terminate(t_3);
	//printf("hello!!!");
	timer_msleep(4000);
	return;

//msg("Not implemented.");
}

void loop(void){
	int64_t time;	
	while(1){
		
		printf("%s\n", thread_current()->name);
		time=timer_ticks();
		while(timer_elapsed(time)<100){
		}
	
			
	}
		
}


