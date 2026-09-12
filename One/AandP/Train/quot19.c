/*
Поиск подстроки в строке
*/

#include <stdio.h>
int substr(char* s1, char* s2)
{
    int l1 = 0;
    while(s1[l1] != '\0') l1++;
    int l2 = 0;
    while(s2[l2] != '\0') l2++;
    int i = 0;
    int j = 0;
    while(i < l1-l2)
    {
        j = 0;
        if(s1[i] == s2[0])
        {
            while(j < l2)
            {
                if(s1[i+j] != s2[j])
                {
                    i += j;
                    break;
                }
                j++;
            }
        }
        if(j == l2)
            return 1;
        i++;
    }
    return 0;
}

int main()
{
    char s1[1000];
    char s2[1000];
    scanf("%s", s1);
    scanf("%s", s2);
    printf("%d", substr(s1, s2));
}