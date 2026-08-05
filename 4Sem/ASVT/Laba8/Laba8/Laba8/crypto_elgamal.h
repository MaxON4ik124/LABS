#ifndef CRYPTO_ELGAMAL_H
#define CRYPTO_ELGAMAL_H

#include <stdint.h>
#include <stdbool.h>

uint16_t crypto_mod_pow_u16(uint16_t base, uint16_t exp, uint16_t mod);
uint16_t crypto_encrypt_byte(uint8_t plain, uint16_t shared_secret, uint16_t mod_p);
bool crypto_decrypt_word(uint16_t cipher, uint16_t shared_secret, uint16_t mod_p, uint8_t *plain_out);

#endif /* CRYPTO_ELGAMAL_H */
