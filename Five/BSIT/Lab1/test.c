#include <stdio.h>
#include <string.h>
int main()
{
    char* data = "SECRET";
    char* key = "gfg3";
    char enc[12];
    for(int i = 0;i < strlen(data);i++)
        enc[i] = data[i] ^ key[i % strlen(key)];
    enc[11] = '\0';
    printf("%s\n", enc);
    for(int i = 0;i < strlen(data);i++)
        enc[i] = enc[i] ^ key[i % strlen(key)];
    printf("%s", enc);
}