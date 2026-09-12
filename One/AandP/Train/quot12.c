/*
сумма цифр
*/
#include <stdio.h>

int digcnt(long x)
{
    int cnt = 0;
    while(1)
    {
        while(x > 0)
        {
            cnt += x % 10;
            x /= 10;
        }
        if(cnt > 10)
        {
            x = cnt;
            cnt = 0;
        }
        else break;
    }
    return cnt;
}

int main()
{
    long x = 123312;
    printf("%d", digcnt(x));
}