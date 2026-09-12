/*
15. Билет 5. Функция, выдаёт частное и остаток от деления x на y, нельзя
пользоваться / и %
*/

#include <stdio.h>

void divide(int x, int y)
{
    int p = 0;
    int q = 0;
    while(x >= y)
    {
        x -= y;
        p++;
    }
    q = x;
    printf("%d %d\n", p, q);
}
int main()
{
    divide(20, 5);
}