#include "crypto_elgamal.h"

uint16_t crypto_mod_pow_u16(uint16_t base, uint16_t exp, uint16_t mod)
{
    uint32_t result = 1u;
    uint32_t cur = (uint32_t)(base % mod);

    while (exp != 0u)
    {
        if ((exp & 1u) != 0u)
        {
            result = (result * cur) % mod;
        }
        cur = (cur * cur) % mod;
        exp >>= 1u;
    }

    return (uint16_t)result;
}

uint16_t crypto_encrypt_byte(uint8_t plain, uint16_t shared_secret, uint16_t mod_p)
{
    return (uint16_t)(((uint32_t)plain + (uint32_t)(shared_secret % mod_p)) % mod_p);
}

bool crypto_decrypt_word(uint16_t cipher, uint16_t shared_secret, uint16_t mod_p, uint8_t *plain_out)
{
    uint16_t value;

    value = (uint16_t)((cipher + mod_p - (shared_secret % mod_p)) % mod_p);
    if (value > 255u)
    {
        return false;
    }

    *plain_out = (uint8_t)value;
    return true;
}
