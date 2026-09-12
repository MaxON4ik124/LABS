#include <stdio.h>
#include <string.h>
int min(int a, int b)
{
    return (a < b) ? a : b;
}
void substr(char *str, char *val, int start, int end)
{
    int x = 0;
    for(int h = start; h < end; h++)
    {
        val[x] = str[h];
        x++;
    }
}
int compare(char *a, char *b)
{
    for(int i = 0; i < strlen(a); i++)
    {
        if(a[i] != b[i])
            return 0;
    }
    return 1;
}


void getres(char *str1, char *str2, char *res)
{
    int ln1 = strlen(str1);
    int ln2 = strlen(str2);
    char resstr[200];
    for(int i = 0; i < ln1; i++) resstr[i] = str1[i];
    for(int i = ln1; i < ln1+ln2; i++) resstr[i] = str2[i-ln1];
    int len = ln1 + ln2;
    int reach = min(ln1, ln2);
    int overlap = 0;
    int fixed = 0;
    for(int i = reach; i > -1; i--)
    {
        char p1[100] = "";
        char p2[100] = "";
        substr(str1, p1, ln1-i, ln1);
        substr(str2, p2, 0, i);
        fixed = compare(p1, p2);
        if(fixed == 1)
        {
            overlap = strlen(p1);
            int g = 0;
            for(int k = 0;k < len;k++)
            {
                if(k < ln1 || k >= ln1+overlap)
                {
                    res[g] = resstr[k];
                    g++;
                }
            }
            break;
        }
    }
}
int main()
{
    int n;
    scanf("%d", &n);
    // char results[10000][100];
    for(int i = 0; i < n; i++)
    {
        char results[100] = "\0";
        char str1[50];
        char str2[50];
        scanf("%s %s", str1, str2);
        getres(str1, str2, results);
        printf("%s\n", results);
    }

}
