/*
quicksort
*/
#include <stdio.h>
void swap(int *x, int *y)
{
    int temp = *x;
    *x = *y;
    *y = temp;
}
void quicksort(int *arr, int beg, int end)
{
    int pivot = arr[end];
    int i = beg;
    int j = beg-1;
    while(i <= end)
    {
        if(arr[i] > pivot)
            i++;
        else if(arr[i] <= pivot)
        {
            j++;
            swap(&arr[i], &arr[j]);
            i++;
        }
    }
    if(beg < end)
    {
        quicksort(arr, 0, j-1);
        quicksort(arr, j+1, i);
    }
}
int main()
{
    int arr[10] = {3, 12, 54, 1, 23, 54, 55, 5, 12, 2};
    quicksort(arr, 0, 9);
    for(int i = 0;i < 10;i++)
        printf("%d ", arr[i]);
}