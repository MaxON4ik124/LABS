#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    // Входные аргументы (пример командной строки)
    char* args[] = {"echo", "arg1", "arg2"};
    int argc = 3;

    // ---------------------------
    // 1. Буфер под строки (эмуляция стека)
    char stack_buf[128];
    char *sp = stack_buf + sizeof(stack_buf); // стек растёт вниз

    // Массив указателей (эмуляция esp)
    void **esp = malloc(32 * sizeof(void*));
    void **stack_top = esp;

    // ---------------------------
    // 2. Копируем строки на "стек"
    int cnt = argc - 1;
    while (cnt >= 0) {
        int len = strlen(args[cnt]) + 1;
        sp -= len;                 // сдвигаем указатель вниз
        memcpy(sp, args[cnt], len); // копируем строку в стек
        *stack_top = sp;            // кладём адрес строки в esp
        printf("| ESP (string): %s |\n", (char*)*stack_top);
        stack_top++;
        cnt--;
    }

    // ---------------------------
    // 3. Кладём указатели на строки (argv)
    cnt = argc - 1;
    while (cnt >= 0) {
        *stack_top = &esp[cnt];     // указатель на указатель на строку
        printf("| ESP (ptr to string): %s |\n", *((char**)*stack_top));
        stack_top++;
        cnt--;
    }

    // ---------------------------
    // 4. Кладём argc на стек
    *stack_top = (void*)(uintptr_t)argc;
    printf("| ESP (argc): %d |\n", (int)(uintptr_t)*stack_top);
    stack_top++;

    // ---------------------------
    // 5. Нули (например для выравнивания)
    memset(stack_top, 0, 8);
    printf("| ESP (NULL sentinel): %p |\n", *stack_top);

    free(esp);
    return 0;
}
