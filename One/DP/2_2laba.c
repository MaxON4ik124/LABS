#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int max(int x, int y) // возвращает максималку
{
    if(x > y)
        return x;
    return y;
}
int power10(int r)
{
    int powers[10] = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000};
    return powers[r]; // возвращает 10^r
}
int intlen(int val)  // возвращает длинну числа
{
    int len = 0;
    while(val > 0)
    {
        len++;
        val /= 10;
    }
    return len;
}
int main()
{
    char expression[100];
    scanf("%s", expression);
    int len = strlen(expression);
    int g = 0; // локальный счетчик разрядов в числе
    int fnum = 0; // 0 - первое число, 1 - второе
    int frac = 1; // 0 - целая часть, 1 - дробная
    int mantissa1 = 0; // мантисса первого числа
    int mantissa2 = 0; // мантисса второго числа
    int exponent1 = 0; // точность первого числа
    int exponent2 = 0; // точность второго числа
    int mantissa; // суммарная мантисса
    int exponent; // максимальная точность
    for(int i = len-1; i > -1; i--) // проходимся с конца числа к началу
    {
        if(expression[i] == '.') // Если поймали точку, активируем флаг дробной части
        {
            frac = 0;
            continue;
        }
        if(expression[i] == '+') // Если поймали плюс, активируем флаг первого числа
        {
            frac = 1;
            fnum = 1;
            g = 0;
            continue;
        }
        else
        {
            if(fnum == 0)
                mantissa2 += power10(g) * (expression[i] - '0');  // Заполняем 2-ю мантиссу
            if(fnum == 1)
                mantissa1 += power10(g) * (expression[i] - '0'); // Заполняем 1-ю мантиссу
            if(frac == 1 && fnum == 1)
                exponent1++; // считаем точность первого числа
            if(frac == 1 && fnum == 0)
                exponent2++; // считаем точность второго числа
            g++;
        }
    }
    exponent = max(exponent1, exponent2);
    mantissa1 *= power10(exponent - exponent1); 
    mantissa2 *= power10(exponent - exponent2);
    // Подгоняем мантиссы под одну точность (Одна мантиссса не изметися, вторая умножится на 10 ^ (разница между точностями))
    mantissa = mantissa1 + mantissa2;
    int whole = mantissa / power10(exponent); // целая часть
    int fractional = mantissa % power10(exponent); // дробная часть
    char fillzero[100] = {'0'};

    
    if(fractional > 0)
    {
        fillzero[exponent - intlen(fractional)] = '\0'; // заполняем нулями дробную часть
        while(fractional % 10 == 0)
            fractional /= 10;  // убираем нули в дробной части
        printf("%d.%s%d", whole, fillzero, fractional);
        return 0;
    }
    else
    {
        printf("%d", whole);
        return 0;
    }
}