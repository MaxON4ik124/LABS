#include <stdio.h>
#include <string.h>
#include <stdlib.h>
void to_base(long long val, int base)
{
    int isminus = 0;
    int isplus = 0;
    if(val < 0)
        isminus = 1;
    else
        isplus = 1;
    val = llabs(val);
    char alp[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char res1[1000];
    char res2[1000] = "";
    
    int i = 0;


    while (val > 0)
    {
        res1[i] = alp[val % base];
        val /= base;
        i++;
    }
    res1[i++] = '\0';
    int len = strlen(res1);
    if(isminus == 1)
    {
        res2[0] = '-';
        len++;        
    }
    
    for(int i = isminus;i < len;i++)
        res2[len-i-isplus] = res1[i-isminus];
    printf("%s", res2);
    
}
void display(long long value, char type[], int base)
{
    printf("%s;", type);
    to_base(value, 16); 
    printf("=");
    to_base(value, base);
    printf(";%d\n", base);
}
int main()
{
    short val1 = -0x472C;
    int val2 = 0x3B60CCBA;
    unsigned int val3 = 0x26D5A180;
    long val4 = -0x7D70E002;
    unsigned long val5 = 0xF7BDFB00;
    display(val1, "short", 22);
    display(val2, "int", 35);
    display(val3, "unsigned int", 30);
    display(val4, "long",  5);
    display(val5, "unsigned long", 21);
}