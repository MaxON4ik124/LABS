/*
11.Есть одномерный массив целых чисел и нужно построить функцию,
получающую на вход вещественное число x и возвращающую индекс
элемента, который ближе всего к этому числу.
*/
#include <stdio.h>
#include <math.h>

int get_closest(int *arr, float x, int n)
{
    float minrange = 10000000.0;
    int closest = 0;
    for(int i = 0;i < n;i++)
    {
        if(fabs(arr[i] - x) < minrange)
        {
            minrange = fabs(arr[i] - x);
            closest = i;
        }
    }
    return closest;
}

int main()
{
    int arr[10] = {12, 23, 4, 21, 22, 3, 11, 23, 23, 10};
    int x = get_closest(arr, 3.14, 10);
    printf("%d", x);
}