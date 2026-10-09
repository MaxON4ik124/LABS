#include <stdio.h>
#include <stdlib.h>

int main() {
    char cVarl;
    char* str = (char*)malloc(256 * sizeof(char));
    scanf("%255s", str);
    char* local_140 = (char*)&str;
    do
    {
        cVarl = *local_140;
        local_140 = local_140 + 1;
    } while (cVarl != '\0');
    size_t length = local_140 - ((char*)str - 1);;
    printf("%zu\n", length);
    free(str);
    return 0;
}