/*
strlen()
*/

#include <stdio.h>

int strlen(char *s)
{
    int l = 0;
    while(*s++ != '\0') l++;
    return l;
}
int main()
{
    char s[30] = "ADFDFD";
    printf("%d", strlen(s));
}