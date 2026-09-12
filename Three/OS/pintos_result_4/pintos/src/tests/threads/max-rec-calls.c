/*
  File for 'max-rec-calls' task implementation.
*/

#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/malloc.h"
#include "threads/thread.h"
#include "threads/init.h"


// !!! help !!! dop 2, laba 1
static void recursion(int old, int now){
	old++;
	printf("%d\n", old);
	recursion(old, now++);
//return;
}

// !!! help !!! dop 2, laba 1 - end






void test_max_rec_calls(void) 
{

	// !!! help !!! dop 2, laba 1
	int old = 0;
	int now = 0;	
	recursion(old, now);



	// !!! help !!! dop 2, laba 1 - end
  
//msg("Not implemented.");
}
