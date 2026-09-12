#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
int getlen(long value)
{
    int len = 0;
    while (value > 0)
    {
        len++;
        value /= 10;
    }
    return len;
}

int main()
{
    char ticket[100000];
    char* tick;
    scanf("%s", &ticket);
    if (ticket[0] == '-')
        tick = ticket + 1;
    else
        tick = ticket;
    long itick = atoi(tick);
    int tlen = strlen(tick);
    int itlen = getlen(itick);
    if (tlen % 2 == 0)
    {
        int mid = tlen / 2;
        int left = 0;
        int right = 0;
        for (int i = tlen - itlen; i < tlen;i++)
        {
            if (i < mid)
            {
                left += tick[i] - '0';
            }
            else
                right += tick[i] - '0';
        }
        if (left == right)
            printf("1");
        else
            printf("0");
    }
    else
        printf("0");

}