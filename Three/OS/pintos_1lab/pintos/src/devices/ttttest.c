#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//#include "threads/thread.h"

typedef struct LinkedList {
  struct thread *t;
  struct LinkedList *next;
} LinkedList;

struct thread {
  char name[16];
};

LinkedList* add_thread(LinkedList *top, struct thread *t) {
  LinkedList *node = (LinkedList *)malloc(sizeof(LinkedList));
  node->t = t;
  node->next = NULL;

  if (top == NULL){
    top = node;

    return top;
  }

  top->next = node;
  top = node;

  return top;
}

void print_blocked_threads(const LinkedList *top) {
  LinkedList *tmp = (LinkedList *)malloc(sizeof(LinkedList));
  tmp = top;

  unsigned char i = 0;
  while (tmp != NULL) {
    printf("Thread %d, name: %s\n", i++, top->t->name);

    tmp = tmp->next;
  }

  printf("End of the LinkedList of threads\n");
}

int main(void) {
  LinkedList *lst = NULL;
  
  struct thread *t;
  strcpy(t->name, "thread 1");  

  struct thread *t2;
  strcpy(t2->name, "thread 2");  

  struct thread *t3;
  strcpy(t3->name, "thread 3");  
  
  lst = add_thread(lst, t);
  lst = add_thread(lst, t2);
  lst = add_thread(lst, t3);

 print_blocked_threads(lst);

 return 0;
}
