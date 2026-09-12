#include <stdio.h>
struct test
{
    int a;
    int b;
    struct test* next;
};
int main()
{
    // int* a;
    // int* save = a;
    // int a1 = 10;
    // int a2 = 34;
    // int a3 = 123;
    // *a = a1;
    // *a++;
    // *a = a2;
    // *a++;
    // *a = a3;
    // a = save;
    // printf("%d %d %d", *a, *(a+1), *(a+2));

    struct test* tester;
    struct test* save = tester;
    tester->a = 0;
    tester->b = 13;
    for(int i = 1;i < 3;i++)
    {
        *tester++;
        // tester = tester->next;
        tester->a = i;
        tester->b = 5*i;
    }
    tester = save;
    for(int i = 0;i < 3;i++)
    {
        printf("%d %d\n", tester->a, tester->b);
        *tester++;
    }
    
}