#include <stdio.h>
/*
Задача на поиск самой длинной строго возрастающей последовательности
*/
void f(char *str, char *res)
{
    int lmax = 1;
    int l = 1;
    int i = 1;
    while(str[i] != '\0')
    {
        if(str[i] > str[i-1]) l++;
        else
        {
            if(l > lmax)
            {
                for(int j = 0;j < l;j++)
                    res[j] = str[i-l+j];
                lmax = l;
                res[l] = '\0';
            }
            l = 1;
        }
        i++;
    }
    if(l > lmax)
    {
        for(int j = 0;j < l;j++)
            res[j] = str[i-l+j];
        lmax = l;
        res[l] = '\0';
    }
}


int main(){
    char str[100];
    scanf("%s", str);
    char res[100];
    f(str, res);
    printf("%s", res);
}