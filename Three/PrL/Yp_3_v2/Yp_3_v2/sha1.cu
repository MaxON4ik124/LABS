#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "sha1.cuh"

// Константы SHA-1 в макросах для быстрого доступа
#define K0 0x5A827999
#define K1 0x6ED9EBA1
#define K2 0x8F1BBCDC
#define K3 0xCA62C1D6

__device__ int gpu_strlen(const char* str)
{
    int len = 0;
    while(str[len] != '\0')
        len++;
    return len;
}

// Оптимизированная трансформация с разверткой циклов
__device__ void gpu_sha1_transform(SHA1_CTX *ctx, const uint8_t data[]) 
{
    // Используем регистры вместо массива в local memory
    uint32_t w0, w1, w2, w3, w4, w5, w6, w7, w8, w9, w10, w11, w12, w13, w14, w15;
    uint32_t a, b, c, d, e, temp;
    
    // Инициализация первых 16 слов (развернуто)
    w0  = (data[0] << 24)  | (data[1] << 16)  | (data[2] << 8)   | data[3];
    w1  = (data[4] << 24)  | (data[5] << 16)  | (data[6] << 8)   | data[7];
    w2  = (data[8] << 24)  | (data[9] << 16)  | (data[10] << 8)  | data[11];
    w3  = (data[12] << 24) | (data[13] << 16) | (data[14] << 8)  | data[15];
    w4  = (data[16] << 24) | (data[17] << 16) | (data[18] << 8)  | data[19];
    w5  = (data[20] << 24) | (data[21] << 16) | (data[22] << 8)  | data[23];
    w6  = (data[24] << 24) | (data[25] << 16) | (data[26] << 8)  | data[27];
    w7  = (data[28] << 24) | (data[29] << 16) | (data[30] << 8)  | data[31];
    w8  = (data[32] << 24) | (data[33] << 16) | (data[34] << 8)  | data[35];
    w9  = (data[36] << 24) | (data[37] << 16) | (data[38] << 8)  | data[39];
    w10 = (data[40] << 24) | (data[41] << 16) | (data[42] << 8)  | data[43];
    w11 = (data[44] << 24) | (data[45] << 16) | (data[46] << 8)  | data[47];
    w12 = (data[48] << 24) | (data[49] << 16) | (data[50] << 8)  | data[51];
    w13 = (data[52] << 24) | (data[53] << 16) | (data[54] << 8)  | data[55];
    w14 = (data[56] << 24) | (data[57] << 16) | (data[58] << 8)  | data[59];
    w15 = (data[60] << 24) | (data[61] << 16) | (data[62] << 8)  | data[63];
    
    a = ctx->state[0];
    b = ctx->state[1];
    c = ctx->state[2];
    d = ctx->state[3];
    e = ctx->state[4];
    
    // Раунды 0-19: f = (b & c) | (~b & d), k = K0
    #define ROUND_0_19(a, b, c, d, e, w) \
        temp = ROTLEFT(a, 5) + ((b & c) | (~b & d)) + e + K0 + w; \
        e = d; d = c; c = ROTLEFT(b, 30); b = a; a = temp;
    
    ROUND_0_19(a, b, c, d, e, w0);
    ROUND_0_19(a, b, c, d, e, w1);
    ROUND_0_19(a, b, c, d, e, w2);
    ROUND_0_19(a, b, c, d, e, w3);
    ROUND_0_19(a, b, c, d, e, w4);
    ROUND_0_19(a, b, c, d, e, w5);
    ROUND_0_19(a, b, c, d, e, w6);
    ROUND_0_19(a, b, c, d, e, w7);
    ROUND_0_19(a, b, c, d, e, w8);
    ROUND_0_19(a, b, c, d, e, w9);
    ROUND_0_19(a, b, c, d, e, w10);
    ROUND_0_19(a, b, c, d, e, w11);
    ROUND_0_19(a, b, c, d, e, w12);
    ROUND_0_19(a, b, c, d, e, w13);
    ROUND_0_19(a, b, c, d, e, w14);
    ROUND_0_19(a, b, c, d, e, w15);
    
    // Расширение w и раунды 16-19
    w0  = ROTLEFT(w13 ^ w8  ^ w2  ^ w0, 1);  ROUND_0_19(a, b, c, d, e, w0);
    w1  = ROTLEFT(w14 ^ w9  ^ w3  ^ w1, 1);  ROUND_0_19(a, b, c, d, e, w1);
    w2  = ROTLEFT(w15 ^ w10 ^ w4  ^ w2, 1);  ROUND_0_19(a, b, c, d, e, w2);
    w3  = ROTLEFT(w0  ^ w11 ^ w5  ^ w3, 1);  ROUND_0_19(a, b, c, d, e, w3);
    
    #undef ROUND_0_19
    
    // Раунды 20-39: f = b ^ c ^ d, k = K1
    #define ROUND_20_39(a, b, c, d, e, w) \
        temp = ROTLEFT(a, 5) + (b ^ c ^ d) + e + K1 + w; \
        e = d; d = c; c = ROTLEFT(b, 30); b = a; a = temp;
    
    w4  = ROTLEFT(w1  ^ w12 ^ w6  ^ w4, 1);  ROUND_20_39(a, b, c, d, e, w4);
    w5  = ROTLEFT(w2  ^ w13 ^ w7  ^ w5, 1);  ROUND_20_39(a, b, c, d, e, w5);
    w6  = ROTLEFT(w3  ^ w14 ^ w8  ^ w6, 1);  ROUND_20_39(a, b, c, d, e, w6);
    w7  = ROTLEFT(w4  ^ w15 ^ w9  ^ w7, 1);  ROUND_20_39(a, b, c, d, e, w7);
    w8  = ROTLEFT(w5  ^ w0  ^ w10 ^ w8, 1);  ROUND_20_39(a, b, c, d, e, w8);
    w9  = ROTLEFT(w6  ^ w1  ^ w11 ^ w9, 1);  ROUND_20_39(a, b, c, d, e, w9);
    w10 = ROTLEFT(w7  ^ w2  ^ w12 ^ w10, 1); ROUND_20_39(a, b, c, d, e, w10);
    w11 = ROTLEFT(w8  ^ w3  ^ w13 ^ w11, 1); ROUND_20_39(a, b, c, d, e, w11);
    w12 = ROTLEFT(w9  ^ w4  ^ w14 ^ w12, 1); ROUND_20_39(a, b, c, d, e, w12);
    w13 = ROTLEFT(w10 ^ w5  ^ w15 ^ w13, 1); ROUND_20_39(a, b, c, d, e, w13);
    w14 = ROTLEFT(w11 ^ w6  ^ w0  ^ w14, 1); ROUND_20_39(a, b, c, d, e, w14);
    w15 = ROTLEFT(w12 ^ w7  ^ w1  ^ w15, 1); ROUND_20_39(a, b, c, d, e, w15);
    w0  = ROTLEFT(w13 ^ w8  ^ w2  ^ w0, 1);  ROUND_20_39(a, b, c, d, e, w0);
    w1  = ROTLEFT(w14 ^ w9  ^ w3  ^ w1, 1);  ROUND_20_39(a, b, c, d, e, w1);
    w2  = ROTLEFT(w15 ^ w10 ^ w4  ^ w2, 1);  ROUND_20_39(a, b, c, d, e, w2);
    w3  = ROTLEFT(w0  ^ w11 ^ w5  ^ w3, 1);  ROUND_20_39(a, b, c, d, e, w3);
    w4  = ROTLEFT(w1  ^ w12 ^ w6  ^ w4, 1);  ROUND_20_39(a, b, c, d, e, w4);
    w5  = ROTLEFT(w2  ^ w13 ^ w7  ^ w5, 1);  ROUND_20_39(a, b, c, d, e, w5);
    w6  = ROTLEFT(w3  ^ w14 ^ w8  ^ w6, 1);  ROUND_20_39(a, b, c, d, e, w6);
    w7  = ROTLEFT(w4  ^ w15 ^ w9  ^ w7, 1);  ROUND_20_39(a, b, c, d, e, w7);
    
    #undef ROUND_20_39
    
    // Раунды 40-59: f = (b & c) | (b & d) | (c & d), k = K2
    #define ROUND_40_59(a, b, c, d, e, w) \
        temp = ROTLEFT(a, 5) + ((b & c) | (b & d) | (c & d)) + e + K2 + w; \
        e = d; d = c; c = ROTLEFT(b, 30); b = a; a = temp;
    
    w8  = ROTLEFT(w5  ^ w0  ^ w10 ^ w8, 1);  ROUND_40_59(a, b, c, d, e, w8);
    w9  = ROTLEFT(w6  ^ w1  ^ w11 ^ w9, 1);  ROUND_40_59(a, b, c, d, e, w9);
    w10 = ROTLEFT(w7  ^ w2  ^ w12 ^ w10, 1); ROUND_40_59(a, b, c, d, e, w10);
    w11 = ROTLEFT(w8  ^ w3  ^ w13 ^ w11, 1); ROUND_40_59(a, b, c, d, e, w11);
    w12 = ROTLEFT(w9  ^ w4  ^ w14 ^ w12, 1); ROUND_40_59(a, b, c, d, e, w12);
    w13 = ROTLEFT(w10 ^ w5  ^ w15 ^ w13, 1); ROUND_40_59(a, b, c, d, e, w13);
    w14 = ROTLEFT(w11 ^ w6  ^ w0  ^ w14, 1); ROUND_40_59(a, b, c, d, e, w14);
    w15 = ROTLEFT(w12 ^ w7  ^ w1  ^ w15, 1); ROUND_40_59(a, b, c, d, e, w15);
    w0  = ROTLEFT(w13 ^ w8  ^ w2  ^ w0, 1);  ROUND_40_59(a, b, c, d, e, w0);
    w1  = ROTLEFT(w14 ^ w9  ^ w3  ^ w1, 1);  ROUND_40_59(a, b, c, d, e, w1);
    w2  = ROTLEFT(w15 ^ w10 ^ w4  ^ w2, 1);  ROUND_40_59(a, b, c, d, e, w2);
    w3  = ROTLEFT(w0  ^ w11 ^ w5  ^ w3, 1);  ROUND_40_59(a, b, c, d, e, w3);
    w4  = ROTLEFT(w1  ^ w12 ^ w6  ^ w4, 1);  ROUND_40_59(a, b, c, d, e, w4);
    w5  = ROTLEFT(w2  ^ w13 ^ w7  ^ w5, 1);  ROUND_40_59(a, b, c, d, e, w5);
    w6  = ROTLEFT(w3  ^ w14 ^ w8  ^ w6, 1);  ROUND_40_59(a, b, c, d, e, w6);
    w7  = ROTLEFT(w4  ^ w15 ^ w9  ^ w7, 1);  ROUND_40_59(a, b, c, d, e, w7);
    w8  = ROTLEFT(w5  ^ w0  ^ w10 ^ w8, 1);  ROUND_40_59(a, b, c, d, e, w8);
    w9  = ROTLEFT(w6  ^ w1  ^ w11 ^ w9, 1);  ROUND_40_59(a, b, c, d, e, w9);
    w10 = ROTLEFT(w7  ^ w2  ^ w12 ^ w10, 1); ROUND_40_59(a, b, c, d, e, w10);
    w11 = ROTLEFT(w8  ^ w3  ^ w13 ^ w11, 1); ROUND_40_59(a, b, c, d, e, w11);
    
    #undef ROUND_40_59
    
    // Раунды 60-79: f = b ^ c ^ d, k = K3
    #define ROUND_60_79(a, b, c, d, e, w) \
        temp = ROTLEFT(a, 5) + (b ^ c ^ d) + e + K3 + w; \
        e = d; d = c; c = ROTLEFT(b, 30); b = a; a = temp;
    
    w12 = ROTLEFT(w9  ^ w4  ^ w14 ^ w12, 1); ROUND_60_79(a, b, c, d, e, w12);
    w13 = ROTLEFT(w10 ^ w5  ^ w15 ^ w13, 1); ROUND_60_79(a, b, c, d, e, w13);
    w14 = ROTLEFT(w11 ^ w6  ^ w0  ^ w14, 1); ROUND_60_79(a, b, c, d, e, w14);
    w15 = ROTLEFT(w12 ^ w7  ^ w1  ^ w15, 1); ROUND_60_79(a, b, c, d, e, w15);
    w0  = ROTLEFT(w13 ^ w8  ^ w2  ^ w0, 1);  ROUND_60_79(a, b, c, d, e, w0);
    w1  = ROTLEFT(w14 ^ w9  ^ w3  ^ w1, 1);  ROUND_60_79(a, b, c, d, e, w1);
    w2  = ROTLEFT(w15 ^ w10 ^ w4  ^ w2, 1);  ROUND_60_79(a, b, c, d, e, w2);
    w3  = ROTLEFT(w0  ^ w11 ^ w5  ^ w3, 1);  ROUND_60_79(a, b, c, d, e, w3);
    w4  = ROTLEFT(w1  ^ w12 ^ w6  ^ w4, 1);  ROUND_60_79(a, b, c, d, e, w4);
    w5  = ROTLEFT(w2  ^ w13 ^ w7  ^ w5, 1);  ROUND_60_79(a, b, c, d, e, w5);
    w6  = ROTLEFT(w3  ^ w14 ^ w8  ^ w6, 1);  ROUND_60_79(a, b, c, d, e, w6);
    w7  = ROTLEFT(w4  ^ w15 ^ w9  ^ w7, 1);  ROUND_60_79(a, b, c, d, e, w7);
    w8  = ROTLEFT(w5  ^ w0  ^ w10 ^ w8, 1);  ROUND_60_79(a, b, c, d, e, w8);
    w9  = ROTLEFT(w6  ^ w1  ^ w11 ^ w9, 1);  ROUND_60_79(a, b, c, d, e, w9);
    w10 = ROTLEFT(w7  ^ w2  ^ w12 ^ w10, 1); ROUND_60_79(a, b, c, d, e, w10);
    w11 = ROTLEFT(w8  ^ w3  ^ w13 ^ w11, 1); ROUND_60_79(a, b, c, d, e, w11);
    w12 = ROTLEFT(w9  ^ w4  ^ w14 ^ w12, 1); ROUND_60_79(a, b, c, d, e, w12);
    w13 = ROTLEFT(w10 ^ w5  ^ w15 ^ w13, 1); ROUND_60_79(a, b, c, d, e, w13);
    w14 = ROTLEFT(w11 ^ w6  ^ w0  ^ w14, 1); ROUND_60_79(a, b, c, d, e, w14);
    w15 = ROTLEFT(w12 ^ w7  ^ w1  ^ w15, 1); ROUND_60_79(a, b, c, d, e, w15);
    
    #undef ROUND_60_79
    
    ctx->state[0] += a;
    ctx->state[1] += b;
    ctx->state[2] += c;
    ctx->state[3] += d;
    ctx->state[4] += e;
}

__device__ void gpu_sha1_init(SHA1_CTX *ctx) {
    ctx->datalen = 0;
    ctx->bitlen = 0;
    ctx->state[0] = 0x67452301;
    ctx->state[1] = 0xEFCDAB89;
    ctx->state[2] = 0x98BADCFE;
    ctx->state[3] = 0x10325476;
    ctx->state[4] = 0xC3D2E1F0;
}

__device__ void gpu_sha1_update(SHA1_CTX *ctx, const uint8_t data[], size_t len) {
    for (size_t i = 0; i < len; ++i) {
        ctx->data[ctx->datalen] = data[i];
        ctx->datalen++;
        if (ctx->datalen == 64) {
            gpu_sha1_transform(ctx, ctx->data);
            ctx->bitlen += 512;
            ctx->datalen = 0;
        }
    }
}

__device__ void gpu_sha1_final(SHA1_CTX *ctx, uint8_t hash[]) {
    uint32_t i = ctx->datalen;

    if (ctx->datalen < 56) {
        ctx->data[i++] = 0x80;
        #pragma unroll
        while (i < 56)
            ctx->data[i++] = 0x00;
    } else {
        ctx->data[i++] = 0x80;
        #pragma unroll
        while (i < 64)
            ctx->data[i++] = 0x00;
        gpu_sha1_transform(ctx, ctx->data);
        
        #pragma unroll
        for(i = 0; i < 56; i++)
            ctx->data[i] = 0;
    }

    ctx->bitlen += ctx->datalen * 8;
    ctx->data[63] = ctx->bitlen;
    ctx->data[62] = ctx->bitlen >> 8;
    ctx->data[61] = ctx->bitlen >> 16;
    ctx->data[60] = ctx->bitlen >> 24;
    ctx->data[59] = ctx->bitlen >> 32;
    ctx->data[58] = ctx->bitlen >> 40;
    ctx->data[57] = ctx->bitlen >> 48;
    ctx->data[56] = ctx->bitlen >> 56;
    gpu_sha1_transform(ctx, ctx->data);

    // Оптимизированный вывод (развернут)
    #pragma unroll
    for (i = 0; i < 4; ++i) {
        hash[i]      = (ctx->state[0] >> (24 - i * 8)) & 0xff;
        hash[i + 4]  = (ctx->state[1] >> (24 - i * 8)) & 0xff;
        hash[i + 8]  = (ctx->state[2] >> (24 - i * 8)) & 0xff;
        hash[i + 12] = (ctx->state[3] >> (24 - i * 8)) & 0xff;
        hash[i + 16] = (ctx->state[4] >> (24 - i * 8)) & 0xff;
    }
}

// Оптимизированная версия с предвычислением длины
__device__ void gpu_sha1_hash(const uint8_t *password, const uint8_t *salt, uint8_t hash[])
{
    SHA1_CTX ctx;
    gpu_sha1_init(&ctx);
    
    // Вычисляем длину один раз
    int pass_len = gpu_strlen((char*)password);
    int salt_len = gpu_strlen((char*)salt);
    
    gpu_sha1_update(&ctx, password, pass_len);
    // gpu_sha1_update(&ctx, salt, salt_len);
    gpu_sha1_final(&ctx, hash);
}
