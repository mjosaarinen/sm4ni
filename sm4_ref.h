// sm4_ref.h
// 2018-04-20  Markku-Juhani O. Saarinen <mjos@iki.fi>

#ifndef SM4_REF_H
#define SM4_REF_H

#define SM4_BLOCK_SIZE    16
#define SM4_KEY_SCHEDULE  32

#include <stdint.h>

// reference implementation based on the internet draft

void sm4_key_schedule(const uint8_t key[], uint32_t rk[]);

void sm4_encrypt(const uint32_t rk[SM4_KEY_SCHEDULE],
    const uint8_t *plaintext, uint8_t *ciphertext);

void sm4_decrypt(const uint32_t rk[SM4_KEY_SCHEDULE],
    const uint8_t *ciphertext, uint8_t *plaintext);

#endif

