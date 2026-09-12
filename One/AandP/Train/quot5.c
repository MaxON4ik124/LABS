/*
Есть две строки, например str1=“hello” str2=“world” , 
если буква из первой строки равна какой-то из второй, 
то эта буква в первой строке не должна повтортся, 
вывод будет типо helo
*/

#include <stdio.h>

void f(char *str1, char *str2, char *res)
{
    int haveDouble = 0;
    int i = 0;
    int j = 0;
    int l = 0;
    while(str1[i] != '\0')
    {
        j = 0;
        haveDouble = 0;
        while(str2[j] != '\0')
        {
            if(str1[i] == str2[j])
            {
                haveDouble = 1;
                break;
            }
            j++;
        }
        if(haveDouble == 0)
        {
            res[l] = str1[i];
            l++;
        }
        if(haveDouble == 1)
        {
            int k = 0;
            while(str1[k] != '\0')
            {
                if(str1[i] == str1[k] && i == k)
                {
                    res[l] = str1[i];
                    l++;
                    break;
                }
                else if(str1[i] == str1[k] && i != k)
                    break;
                k++;
            }
        }
        i++;
    }
}
void main()
{
    char str1[100];
    scanf("%s", str1);
    char str2[100];
    scanf("%s", str2);
    char res[100];
    f(str1, str2, res);
    printf("%s", res);
}

// O(l) = l^2
// Инвариант: (if x in s2, то count(x) == 1)