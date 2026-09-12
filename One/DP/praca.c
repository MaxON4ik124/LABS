#include <stdio.h>
int intlen(int x)
{
    int len = 0;
    while(x > 0)
    {
        len++;
        x /= 10;
    }
    return len;
}
int max(int x, int y)
{
    return (x > y) ? x : y;
}
int main()
{
    char str[51];

    int i;
    int num1 = 0; 
    int num2 = 0; 
    int dec1 = 0; 
    int dec2 = 0; 
    int sign = 1;

    scanf("%s", str);

    for (i = 0; str[i] != '-'; i++)
    {
        if (str[i] == '.')
        {
            dec1 = 1;
            continue;
        }
        num1 = num1 * 10 + (str[i] - '0');
        if (dec1) dec1 *= 10;
    }

    for (i++; str[i] != '\0'; i++)
    {
        if (str[i] == '.')
        {
            dec2 = 1;
            continue;
        }
        num2 = num2 * 10 + (str[i] - '0');
        if (dec2) dec2 *= 10;
    }

    while (dec1 < dec2)
    {
        dec1 *= 10;
        num1 *= 10;
    }

    while (dec2 < dec1)
    {
        dec2 *= 10;
        num2 *= 10;
    }

    if (num1 < num2)
    {
        sign = -1;
        num2 = num2 - num1;
    }
    else
    {
        num2 = num1 - num2;
    }

    while (num2 % 10 == 0 && dec1 > 1)
    {
        num2 /= 10;
        dec1 /= 10;
    }

    if (sign == -1)
    {
        printf("-");
    }

    if (num2 == 0)
    {
        printf("0\n");
    }
    else
    {
        printf("%d.%d\n", num2 / dec1, num2 % dec1);
    }

    return 0;
}
