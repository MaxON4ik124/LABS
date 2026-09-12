/*
strcmp()
*/

#include <stdio.h>

int strcmp(char *s1, char *s2)
{
    int i = 0;
    while(*s1 != '\0')
    {
        if(*s1 > *s2) return 1;
        if(*s1 < *s2) return -1;
        *s1++;
        *s2++;
    }
    return 0;
}
int main()
{
    char s1[10] = "ABCdefghi";
    char s2[10] = "ABCdefghi";
    printf("%d", strcmp(s1, s2));
}