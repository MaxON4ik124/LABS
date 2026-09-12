/*
1 день сдачи АиПа 
Вставить в центр строки(массива) цифру
*/

#include <stdio.h>
#define MAX_LEN 100
int f(char *str, char n)
{
    int len = 0;
    while(str[len] != '\0') len++;
    int mid = len / 2;
    for(int i = len-1;i >= mid;i--)
        str[i+1] = str[i];
    str[mid] = n;
}

int main()
{
    char string[100];
    char c;
    scanf("%s %c", string, &c);
    f(string, c);
    printf("%s", string);
}

// Сложность O(l)
// Инвариант str[i] = str[i] if(i < mid) && str[i+1] = str[i] if(i > mid) && str[i] = n if(i == mid);