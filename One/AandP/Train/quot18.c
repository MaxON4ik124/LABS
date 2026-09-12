/*
bubblesort()
*/
#include <stdio.h>
void swap(int *x, int *y)
{
    int temp = *x;
    *x = *y;
    *y = temp;
}

void bubblesort(int *arr, int len)
{
    for(int i = 0;i < len-1;i++)
    {
        for(int j = 0;j < len-1;j++)
        {
            if(arr[j] > arr[j+1])
                swap(&arr[j], &arr[j+1]);
        }
    }
}
int main()
{
    int arr[10] = {45, 234, 233, 44, 13, 2, 1, 22, 2, 2};
    bubblesort(arr, 10);
    for(int i = 0;i < 10;i++)
        printf("%d ", arr[i]);
}