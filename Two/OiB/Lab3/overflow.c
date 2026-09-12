#include <stdio.h>
int main()
{
    char buf[16];
    printf("Enter message: ");
    gets(buf);
    printf("\nMessage: %s\n", buf);
}