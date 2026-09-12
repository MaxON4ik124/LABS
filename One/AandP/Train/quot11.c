/*
strcat()
*/
#include <stdio.h>


void strchat(char *s1, char *s2)
{
    while(*s1 != '\0')
        *s1++;
    while(*s2 != '\0')
    {
        *s1 = *s2;
        *s1++;
        *s2++;
    }
    *s2++ = '\0';
}
int main()
{
    char s1[30] = "Hello ";
    char s2[10] = "world\"";
    strchat(s1, s2);
    printf("%s", s1);
}