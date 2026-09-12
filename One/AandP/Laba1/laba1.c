#include <stdio.h>
#include <math.h>
#include <stdlib.h>
void main()
{
    {
        unsigned short maxus = 1;
        while (maxus > 0)
            maxus = maxus << 1;
        printf("Unsigned short max: %hu \n", maxus-1);
    }
    { 
        unsigned int maxui = 1;
        while (maxui > 0)
            maxui = maxui << 1;
        printf("Unsigned int max: %u \n", maxui-1);
    }
    {   
        unsigned long maxul = 1;
        while (maxul > 0)
            maxul = maxul << 1;
        printf("Unsigned long max: %lu \n", maxul-1);
    }
    {
        short maxs = -1;
        while (maxs < 0)
            maxs--;
        printf("Short max: %hd \n", maxs);
    }
    {
        int maxi = -1;
        while (maxi < 0)
            maxi--;
        printf("Int max: %d \n", maxi);
    }
    {
        long maxl = -1;
        while (maxl < 0)
            maxl--;
        printf("Long max: %ld \n", maxl);
    }
    {
        char maxc = -1;
        while (maxc < 0)
            maxc--;
        printf("Char max: %d \n", maxc);
    }
    {
        unsigned char maxuc = 0;
        while (maxuc <= 0)
            maxuc--;
        printf("Unsigned char max: %d \n", maxuc);
    }
    {
        unsigned char minuc = 1;
        while (minuc > 0)
            minuc++;
        printf("Unsigned char min: %d \n", minuc);
    }
    {
        char minc = 1;
        while (minc > 0)
            minc++;
        printf("Char min: %d \n", minc);
    }
    {
        unsigned short minus = 1;
        while ((minus >> 1) < 0)
            minus = minus >> 1;
        printf("Unsigned short min: %hu \n", minus >> 1);
    }
    { 
        unsigned int minui = 1;
        while (minui < 0)
            minui = minui >> 1;
        printf("Unsigned int min: %u \n", minui >> 1);
    }
    {   
        unsigned long minul = 1;
        while (minul < 0)
            minul = minul >> 1;
        printf("Unsigned long min: %lu \n", minul >> 1);
    }
    {
        short mins = -1;
        while (mins < 0)
            mins--;
        printf("Short min: %hd \n", mins+1);
    }
    {
        int mini = -1;
        while (mini < 0)
            mini--;
        printf("Int min: %d \n", mini+1);
    }
    {
        long minl = -1;
        while (minl < 0)
            minl--;
        printf("Long min: %ld \n", minl+1);
    }
    getchar();
}