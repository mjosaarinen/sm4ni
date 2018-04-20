// testmain.c
// 2018-04-20  Markku-Juhani O. Saarinen <mjos@iki.fi>

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

// reference implementation
#include "sm4_ref.h"

// AES-NI / SSE3 implementation, encrypt 4 blocks at once
void sm4_encrypt4(const uint32_t rk[32], void *src, const void *dst);

// high-precision time

static double clk_now()
{
    struct timespec ts;

    // You may onsider CLOCK_MONOTONIC and CLOCK_MONOTONIC_RAW here too
    if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts) != 0) {
        perror("clock_gettime()");
        exit(-1);
    }
    return ((double) ts.tv_sec) + 1E-9 * ((double) ts.tv_nsec);
}

// test speed and validity

int main(int argc, char **argv)
{
    int i, e;
    double clk1, clk2, n;

    // test vectors from the standard

    const uint8_t key[16] = {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
        0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10
    };

    const uint8_t ref[16] = {
        0x68, 0x1E, 0xDF, 0x34, 0xD2, 0x06, 0x96, 0x5E,
        0x86, 0xB3, 0xE9, 0x4F, 0x53, 0x6E, 0x42, 0x46
    };

    uint8_t buf1[64], buf2[64];
    uint32_t rk[32];

    memset(buf1, 0x55, sizeof(buf1));
    memset(buf2, 0xAA, sizeof(buf2));

    // Test reference implementation with a test vector.

    sm4_key_schedule(key, rk);
    sm4_encrypt(rk, key, buf1);
    if (memcmp(buf1, ref, 16) != 0) {
        fprintf(stderr, "sm4_encrypt() test failed.\n");
        return -1;
    }
    sm4_decrypt(rk, buf1, buf2);
    if (memcmp(buf2, key, 16) != 0) {
        fprintf(stderr, "sm4_decrypt() test failed.\n");
        return -1;
    }

    // Test the SM4NI four-block version against reference implementation.

    for (i = 0; i < 64; i++)
        buf1[i] = i;

    // encrypt 4 blocks at once to buf2
    sm4_encrypt4(rk, buf1, buf2);

    // individual blocks in place (with ref algoritm)
    sm4_encrypt(rk, buf1, buf1);
    sm4_encrypt(rk, buf1 + 16, buf1 + 16);
    sm4_encrypt(rk, buf1 + 32, buf1 + 32);
    sm4_encrypt(rk, buf1 + 48, buf1 + 48);

    if (memcmp(buf1, buf2, 64) != 0) {
                fprintf(stderr, "sm4_encrypt4() test failed.\n");
        return -1;
    }

    // bench reference implementation

    n = 0;
    e = 16;
    clk1 = clk_now();
    do {

        for (i = 0; i < e; i++) {
            sm4_encrypt(rk, buf1, buf1);
            sm4_encrypt(rk, buf1 + 16, buf1 + 16);
            sm4_encrypt(rk, buf1 + 32, buf1 + 32);
            sm4_encrypt(rk, buf1 + 48, buf1 + 48);
        }
        n += 64 * e;
        e <<= 1;
        clk2 = clk_now() - clk1;
    } while (clk2 < 2.0);

    printf("SM4 reference %10.3f MB/s\n", (n / 1E6) / ((double) clk2));

    // bench reference implementation

    n = 0;
    e = 16;
    clk1 = clk_now();
    do {

        for (i = 0; i < e; i++) {
            sm4_encrypt4(rk, buf1, buf1);
        }
        n += 64 * e;
        e <<= 1;
        clk2 = clk_now() - clk1;
    } while (clk2 < 2.0);

    printf("Vector SM4NI  %10.3f MB/s\n", (n / 1E6) / ((double) clk2));


    return 0;
}


