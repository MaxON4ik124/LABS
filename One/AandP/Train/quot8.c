/*
Палиндром
*/
#include <stdio.h>

int pal(long x)
{
    long source = x;
    long reversed = 0;
    while(source > 0)
    {
        reversed = reversed * 10 + source % 10;
        source /= 10;
        // printf("%ld %ld\n", source, reversed);
    }
    if(reversed == x)
        return 1;
    return 0;
}
int main()
{
    printf("%d", pal(123321));
}