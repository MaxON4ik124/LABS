/*
Написать функцию, принимающую на вход 2 строки.
В функции удваиваются те маленькие английские буквы в строке 1, 
которые есть в строке 2. И удаляются те заглавные буквы в строке 1, 
что в строке 2.
*/
#include <stdio.h>
#define MAX_LEN 100
void f(char *str1, char *str2, char *res)
{
    int len1 = 0;
    int len2 = 0;
    int len = 0;
    int flag = 1;
    while(str1[len1] != '\0')
    {
        len2 = 0;
        flag = 1;
        if('a' <= str1[len1] && str1[len1] <= 'z')
        {
            while(str2[len2] != '\0')
            {
                if(('a' <= str2[len2] <= 'z') && (str1[len1] == str2[len2]))
                {
                    flag = 2;
                    break;
                }
                len2++;
            }
        }
        else if('A' <= str1[len1] && str1[len1] <= 'Z')
        {
            while(str2[len2] != '\0')
            {
                if(('A' <= str2[len2] && str2[len2] <= 'Z') && (str1[len1] == str2[len2]))
                {
                    flag = 0;
                    break;
                }
                len2++;
            }
        }
        if(flag == 1)
        {
            res[len] = str1[len1];
            len++;
        }
        if(flag == 2)
        {
            for(int i = 0;i < 2;i++)
            {
                res[len] = str1[len1];
                len++;
            }
        }
        res[len] = '\0';
        len1++;
    }
}
int main()
{
    char str1[MAX_LEN];
    char str2[MAX_LEN];
    char res[2*MAX_LEN];
    scanf("%s", str1);
    scanf("%s", str2);
    f(str1, str2, res);
    printf("%s", res);
}
// сложность O(l^2)
// инвариант if(x in ('a', 'z') && x in str2, то x удвоится) if(x in ('A', 'Z') && x in str2, то x удалится) 