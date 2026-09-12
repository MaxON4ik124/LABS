#include <stdio.h>
char upper(char c)
{
    if(c >= 'a' && c <= 'z')
        return c - ('b' - 'B');
    return c;
}
int main()
{
    char str[1000];
    int n;
    int strln = 1;
    scanf("%d\n", &n);
    fgets(str, 1000, stdin);
    for(int i = 0;i < 1000;i++)
    {
        if(str[i] == ' ')
        {
            strln = 1;
        }
        else
        {
            if(strln == n)
            {
                str[i] = upper(str[i]);
            }
            strln++;
        }
    }
    printf("%s", str);
}