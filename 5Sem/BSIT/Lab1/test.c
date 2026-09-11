#include <stdio.h>
#include <stdlib.h>

typedef struct 
{
    char* data1;
    char* data2;
    int data3;
} test;

void fill_data(test* ex)
{
    ex->data1 = "Some data";
    ex->data2 = "Secret";
    ex->data3 = 134;
}
int main()
{
    // test* example = malloc(sizeof(test));
    // example->data1 = "Secret data";
    // example->data2 = "Public data";
    // example->data3 = 143;
    // printf("%s\n%s\n%d\n", example->data1, example->data2, example->data3);
    // fill_data(example);
    // printf("%s\n%s\n%d\n", example->data1, example->data2, example->data3);
    // free(example);

    int num = 1024 * 1024 * 1024;
    int r = num / 0x40000000;
    printf("%d", r);
    return 0;
}
