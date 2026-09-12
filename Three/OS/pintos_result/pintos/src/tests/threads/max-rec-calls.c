/*
  File for 'max-rec-calls' task implementation.
*/

#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/malloc.h"
#include "threads/thread.h"

static int max = 0;

static void reccallsbase(int depth) {
    // Печатаем текущую глубину вызовов.
    max = depth;
    printf("Recursion depth: %d\n", depth);
    reccallsbase(depth + 1);
}

void test_max_rec_calls(void) {
    printf("Starting max-rec-calls test...\n");
    reccallsbase(1);
    printf("Max recursion depth: %d\n", max);

}
