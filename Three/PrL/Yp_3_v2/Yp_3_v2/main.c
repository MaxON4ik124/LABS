#include <stdio.h>
#include "sha1.h"
#include "CPU.h"
#include "GPU.h"
#include <stdint.h>

int main()
{
    FILE* mainPass = fopen("mainPass.txt", "w");
    uint8_t hash[20];
    char password[512];
    char salt[512];
    printf("Enter password: ");
    scanf("%s", password);
    printf("Enter any salt: ");
    scanf("%s", salt);
    sha1_hash((uint8_t*)password, salt, hash);
    for (int i = 0; i < 20; i++)
        fprintf(mainPass, "%02x", hash[i]);
    fclose(mainPass);
    DictBruteGPU(password);
    DictBruteCPU(salt);
    return 0;
}