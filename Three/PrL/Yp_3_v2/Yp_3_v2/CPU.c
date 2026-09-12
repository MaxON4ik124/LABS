#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdint.h>
#include "sha1.h"
#include <string.h>

// void generateSalt(char salt[])
// {
//     const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
//     int a = rand() % 8;
//     for (int i = 0; i < a; i++) {
//         salt[i] = charset[rand() % (sizeof(charset) - 1)];
//     }
//     salt[a] = '\0';
    
// }
void DictBruteCPU(char salt[])
{
    char localSalt[512];
    strcpy(localSalt, salt);
    srand(time(NULL));
    clock_t start;
    double cpu_time_used;
    start = clock();
    int f = 0;
    FILE* passwords = fopen("passwds1.txt", "r");
    FILE* mainPass = fopen("mainPass.txt", "r");
    uint8_t mainHash[20];
    char hexHash[41];
    fgets(hexHash, sizeof(hexHash), mainPass);
    for (int i = 0; i < 20; i++)
        sscanf(&hexHash[i * 2], "%2hhx", &mainHash[i]);
    printf("Target hash: ");
    for (int i = 0; i < 20; i++)
        printf("%02x", mainHash[i]);
    // printf(" %s\n", localSalt);
    printf("\nStarting dictionary brute-force on CPU...\n");
    while (feof(passwords) == 0)
    {
        char buffer[512];
        if (fgets(buffer, 512, passwords) != NULL)
        {
            buffer[strcspn(buffer, "\n")] = '\0';
            uint8_t hash[20];
            sha1_hash((uint8_t*)buffer, salt, hash);
            if(memcmp(hash, mainHash, 20) == 0)
            {
                f = 1;
                printf("\nCollision found: %s\n", buffer);
                // printf("Used salt: %s\n", salt);
                printf("Hash: ");
                for (int i = 0; i < 20; i++)
                    printf("%02x", hash[i]);
                printf("\n");
                break;
            }
        }
    }
    fclose(mainPass);
    fclose(passwords);
    if(f == 0)
        printf("Password not found in the dictionary.\n");
    cpu_time_used = ((double)(clock() - start)) / CLOCKS_PER_SEC;
    printf("CPU Time used: %.3f s.\n", cpu_time_used);
}