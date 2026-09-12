#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#define ROTLEFT(a,b) (((a) << (b)) | ((a) >> (32-(b))))
typedef struct {
    uint8_t data[64];
    uint32_t datalen;
    uint64_t bitlen;
    uint32_t state[5];
} SHA1_CTX;
__device__ void gpu_sha1_transform(SHA1_CTX *ctx, const uint8_t data[]);
__device__ void gpu_sha1_init(SHA1_CTX *ctx);
__device__ void gpu_sha1_update(SHA1_CTX *ctx, const uint8_t data[], size_t len);
__device__ void gpu_sha1_final(SHA1_CTX *ctx, uint8_t hash[]);
__device__ void gpu_sha1_hash(const uint8_t *password, const uint8_t *salt, uint8_t hash[]);
