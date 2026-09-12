#ifndef SHA1_H
#define SHA1_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
// #include "sha1.c"

#define ROTLEFT(a,b) (((a) << (b)) | ((a) >> (32-(b))))

typedef struct {
    uint8_t data[64];
    uint32_t datalen;
    unsigned long long bitlen;
    uint32_t state[5];
} SHA1_CTX;

void sha1_transform(SHA1_CTX *ctx, const uint8_t data[]);

void sha1_init(SHA1_CTX *ctx);

void sha1_update(SHA1_CTX *ctx, const uint8_t data[], size_t len);

void sha1_final(SHA1_CTX *ctx, uint8_t hash[]);

void sha1_hash(const uint8_t* password, const uint8_t *salt, uint8_t hash[]);
#endif /*SHA1_H*/