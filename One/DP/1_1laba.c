#include <stdio.h>
#include <math.h>
void main()
{

    unsigned int stop = 0;
    stop--;
    int res = stop / 71585;
    printf("71585");
    for(int i = 2; i <= res; i++)
    {
        printf(",%u", i*71585);
    }
}