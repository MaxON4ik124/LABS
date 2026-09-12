/*
strcpy()
*/
#include <stdio.h>

void strcopy(char *s1, char *s2)
{
    while(*s1 != '\0')
    {
        *s2 = *s1;
        *s2++;
        *s1++;
    }
    *s2++ = '\0';
}
int main()
{
    char s1[20] = "sdfsdfsdf";
    char s2[20];
    strcopy(s1, s2);
    printf("%s", s2);
}