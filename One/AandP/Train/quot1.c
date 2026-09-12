/*
Задача на склейку строк и вставку единицы
*/
#include <stdio.h>
int glue(char *str1, char *str2, char *res)
{
    int len1 = 0;
    int len2 = 0;
    while(str1[len1] != '\0')
    {
        res[len1] = str1[len1];
        len1++;
    }
    res[len1++] = '1';
    while(str2[len2] != '\0')
    {
        res[len1+len2] = str2[len2];
        len2++;
    }
}
int main()
{
    char str1[100];
    scanf("%s", str1);
    char str2[100];
    scanf("%s", str2);
    char res[200];
    glue(str1, str2, res);
    printf("%s", res);
}

// O(l) = l1 + l2
// Инвариант: res[i] == str1[i] && res[len1 - 1] == '1' && res[len1 + len2 - 1] == str2[len2 - 1] 