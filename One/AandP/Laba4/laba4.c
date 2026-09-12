#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#define MAX_SIZE 10000000
#define SIZE 10000 // Дефолтный размер массива
#define SCATTER 10000 // Дефолтный расброс чисел (от 0 до SCATTER)
unsigned long long comp = 0;
unsigned long long mov = 0;
int arr[MAX_SIZE] = {0};
int arr_from_file[MAX_SIZE] = {0};
int len = SIZE;
int scatter = SCATTER;
FILE* input; // Входной файл
void swap(int *x, int *y) // базовый свап
{
    int temp = *x;
    *x = *y;
    *y = temp;
}
void fill_from_file() // Здесь заполняем массив данными из файла
{
    int a;
    int i = 0;
    while(!feof(input))
    {
        fscanf(input, "%d ", &a);
        arr_from_file[i] = a;
        i++;
    }
    len = i;
}
void fill_arr_from_file() // Здесь идет откат массива до значений из файла
{
    for(int i = 0;i < len;i++)
        arr[i] = arr_from_file[i];
}
void random_fill(int* arr) // заполняем массив случайными числами
{
    srand(time(0));
    for(int i = 0;i < len;i++)
        arr[i] = (rand() % scatter) + 1;
}
void print_arr(int* arr) // выводим массив через пробел и ставим \n в конце
{
    for(int i = 0;i < len;i++)
        printf("%d ", arr[i]);
    printf("\n");
}

// BUBBLESORT (Тут пузырьковая сортировка)

void Bubble_sort(int arr[SIZE], int len)
{
    for(int i = 0; i < len - 1;i++)
    {
        for(int j = 0; j < len - 1;j++)
        {
            comp++;
            if(arr[j] > arr[j+1])
            {
                swap(&arr[j], &arr[j+1]);
                mov++;
            }
        }
    }
}

// QUICKSORT (тут быстрая сортировка)

void Quick_sort(int* arr, int left, int right)
{
    int supporter;
    int index;
    int left_border = left;
    int right_border = right;
    supporter = arr[left];
    while(left < right)
    {
        while((arr[right] > supporter) && (left < right))
        {
            right--;
            comp++;
        }
        if(left != right)
        {
            arr[left] = arr[right];
            left++;
            mov++;
        }
        while((arr[left] < supporter) && (left < right))
        {
            left++;
            comp++;
        }
        if(left != right)
        {
            arr[right] = arr[left];
            right--;
            mov++;
        }
    }
    arr[left] = supporter;
    index = left;
    left = left_border;
    right = right_border;
    if(left < index)
        Quick_sort(arr, left, index - 1);
    if(right > index)
        Quick_sort(arr, index + 1, right);
}

// HEAPSORT (Тут вся пирамидальная сортировка)

void siftDown(int* arr, int root, int bottom)
{
    int maxChild;
    int done = 0;
    while((2*root <= bottom) && (!done))
    {
        if(2*root == bottom)
        {
            maxChild = 2*root;
            comp++;
        }
        else if(arr[2*root] > arr[2*root + 1])
        {
            maxChild = 2*root;
            comp++;
        }
        else
        {
            maxChild = 2*root+1;
            comp++;
        }
        if(arr[root] < arr[maxChild])
        {
            mov++;
            swap(&arr[root], &arr[maxChild]);
            root = maxChild;
        }
        else
            done = 1;
    }
}

void Heap_sort(int* arr, int len) 
{
    for(int i = len/2; i >= 0;i--)
        siftDown(arr, i, len - 2);
    for(int i = len - 1; i >= 1;i--)
    {
        mov++;
        swap(&arr[0], &arr[i]);
        siftDown(arr, 0, i-1);
    }
}


int main()
{
    int phase = 1;
    char op;
    printf("Enter method of filling array (f - from file, r - random numbers): ");
    scanf("%c", &op);
    if(op == 'f')
    {
        char filename[1000];
        printf("Enter fullname of file (example: input.txt): ");
        scanf("%s", filename);
        input = fopen(filename, "r");
        fill_from_file();
    }
    else if(op == 'r')
    {
        int l, s;
        printf("Enter count of elements of array (default = 10000, if using default count enter 0): ");
        scanf("%d", &l);
        if(l != 0)
            len = l;
        printf("Enter scatter of elements of array (default = 10000, if using default count enter 0): ");
        scanf("%d", &s);
        if(s != 0)
            scatter = s;
    }
    printf("STATS OF SORTING ARRAYS: \n");
    printf("COUNT OF ARRAY ELEMENTS: %d. SCATTER: %d.\n", len, scatter);
    comp = 0;
    mov = 0;

    printf("CURRENT SORTING ALGORITHM: QUICK SORT.\n");
    printf("UNSORTED ARRAY: ");
    if(op == 'r')
        random_fill(arr);
    else
        fill_arr_from_file();
    print_arr(arr);
    printf("START OF SORTING!\n");
    clock_t start = clock();
    Quick_sort(arr, 0, len-1);
    clock_t end = clock();
    printf("DONE!\n");
    printf("SORTED ARRAY: ");
    print_arr(arr);
    double timer = (double)(end - start)/CLOCKS_PER_SEC;
    printf("TIME: %lf; COMPARES: %llu; MOVES: %llu.\n", timer, comp, mov);
    comp = 0;
    mov = 0;

    printf("CURRENT SORTING ALGORITHM: HEAP SORT.\n");
    printf("UNSORTED ARRAY: ");
    if(op == 'r')
        random_fill(arr);
    else
        fill_arr_from_file();
    print_arr(arr);
    printf("START OF SORTING!\n");
    start = clock();
    Heap_sort(arr, len);
    end = clock();
    printf("DONE!\n");
    printf("SORTED ARRAY: ");
    print_arr(arr);
    timer = (double)(end - start)/CLOCKS_PER_SEC;
    printf("TIME: %lf; COMPARES: %llu; MOVES: %llu.\n", timer, comp, mov);
    comp = 0;
    mov = 0;
    printf("CURRENT SORTING ALGORITHM: BUBBLE SORT.\n");
    printf("UNSORTED ARRAY: ");
    if(op == 'r')
        random_fill(arr);
    else
        fill_arr_from_file();
    print_arr(arr);
    printf("START OF SORTING!\n");
    start = clock();
    Bubble_sort(arr, len);
    end = clock();
    printf("DONE!\n");
    printf("SORTED ARRAY: ");
    print_arr(arr);
    timer = (double)(end - start)/CLOCKS_PER_SEC;
    printf("TIME: %lf; COMPARES: %llu; MOVES: %llu.\n", timer, comp, mov);
    comp = 0;
    mov = 0;
}