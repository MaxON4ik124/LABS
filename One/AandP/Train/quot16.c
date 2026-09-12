/*
Дается строка, нужно вывести сколько раз подряд шла данная буква 
Например ввод абс
Вывод а1б1с1
Ввод аабсссб
Вывод а2б1с3б1
*/























#include <stdio.h>

void f(char *str, char *res)
{
    int fr = 1;
    int i = 0;
    int j = 0;
    while(str[i] != '\0')
    {
        if(str[i] == str[i+1])
            fr++;
        else
        {
            res[j] = str[i];
            j++;
            if(fr < 10)
            {
                res[j] = fr + '0';
                j++;
            }
            else
            {
                int l = 0;
                int fr1 = fr;
                while(fr1 > 0)
                {
                    l++;
                    fr1 /= 10;
                }
                int l1 = l;
                while(l > 0)
                {
                    res[j+l-1] = (fr % 10) + '0';
                    fr /= 10;
                    l--;
                }
                j += l1;
            }
            fr = 1;
        }
        i++;
    }
    res[j++] = '\0';
}
int main()
{
    char str[20] = "aabccccccccccccb";
    char res[10];
    f(str, res);
    printf("%s", res);
}