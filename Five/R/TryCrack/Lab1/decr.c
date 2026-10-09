#include <stdio.h>

int isPrime(int num) {
    if (num <= 3) return 1;
    for (int i = 2; i * i <= num; i++) {
        if (num % i == 0) return 0;
    }
    return 1;
}

int main()
{
    char data[] = "\x57\x20\x3\x1\x5c\x2e\x4a\x62\x9\x4f\x27";
    char key[] = "!]$m0MK90g0GJ@zQn@t/CX0fJPQmCND[QldCR!0Jg]g&0J3BQlSDQndCQ0JTQng==";
    int i = 0;
    int keyind = 0;
    for (i = 0; i < sizeof(data) - 1; i++) {
        while (isPrime(keyind)) {
            keyind++;
        }
        data[i] ^= key[keyind];
        keyind++;
        if (keyind >= sizeof(key) - 1) {
            keyind = 0;
        }
    }
    printf("Decrypted data: %s\n", data);
}