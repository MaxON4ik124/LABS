/*
На вход: строка из цифр.
Вывод: строка, содержащая в себе самую длинную строго возрастающую 
последовательность цифр
*/

#include <stdio.h>

void f(char *inp, char *out)
{
    int i = 1;
    int j = 1;
    int jmax = 1;
    char buf[100];
    char past = inp[0];
    buf[0] = past;
    while(inp[i] != '\0')
    {
        if(inp[i] > past)
        {
            buf[j] = inp[i];
            j++;
        }
        else
        {
            if(j > jmax)
            {
                for(int k = 0;k < j;k++)
                    out[k] = buf[k];
                jmax = j;
            }
            buf[1] = '\0';
            buf[0] = inp[i];
            j = 1;
        }
        past = inp[i];
        i++;
    }
    out[jmax] = '\0';
}
int main()
{
    char str[100];
    char res[100];
    scanf("%s", str);
    f(str, res);
    printf("%s", res);
}

// Сложность O(l)
// Инвариант: res[i] < res[i+1]