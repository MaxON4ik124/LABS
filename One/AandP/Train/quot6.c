#include <stdio.h>
/*
1. Размерность типа данных int (в битах)
*/
int getsize()
{
    int maxint = -1;
    while(maxint < 0) maxint--;
    maxint++;
    long long b = 2;
    int size = 2;
    do
    {
        size++;
        b *= 2;
    }
    while(b < maxint);
    return size;
}
int main()
{
    printf("%d", getsize());
    // getsize();
}
