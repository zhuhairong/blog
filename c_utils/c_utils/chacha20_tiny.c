#include "chacha20_tiny.h"
#include <string.h>

#define ROTL32(v, n) (((v) << (n)) | ((v) >> (32 - (n))))

#define QR(a, b, c, d) ( \
    a += b, d ^= a, d = ROTL32(d, 16), \
    c += d, b ^= c, b = ROTL32(b, 12), \
    a += b, d ^= a, d = ROTL32(d,  8), \
    c += d, b ^= c, b = ROTL32(b,  7))

static void chacha20_block(const uint32_t state[16], uint8_t out[64]) {
    uint32_t x[16];
    memcpy(x, state, sizeof(x));

    /* 20 rounds = 10 double rounds */
    for (int i = 0; i < 10; i++) {
        /* Column rounds */
        QR(x[0], x[4], x[8],  x[12]);
        QR(x[1], x[5], x[9],  x[13]);
        QR(x[2], x[6], x[10], x[14]);
        QR(x[3], x[7], x[11], x[15]);
        /* Diagonal rounds */
        QR(x[0], x[5], x[10], x[15]);
        QR(x[1], x[6], x[11], x[12]);
        QR(x[2], x[7], x[8],  x[13]);
        QR(x[3], x[4], x[9],  x[14]);
    }

    /* Add original state and serialize (little-endian) */
    for (int i = 0; i < 16; i++) {
        x[i] += state[i];
        out[i * 4 + 0] = (uint8_t)(x[i] & 0xff);
        out[i * 4 + 1] = (uint8_t)((x[i] >> 8) & 0xff);
        out[i * 4 + 2] = (uint8_t)((x[i] >> 16) & 0xff);
        out[i * 4 + 3] = (uint8_t)((x[i] >> 24) & 0xff);
    }
}

void chacha20_tiny(const uint8_t *key, const uint8_t *nonce, uint32_t counter, uint8_t *out, size_t len) {
    if (len == 0) return;

    uint32_t state[16];
    state[0] = 0x61707865; state[1] = 0x3320646e; state[2] = 0x79622d32; state[3] = 0x6b206574;
    memcpy(&state[4], key, 32);
    state[12] = counter;
    memcpy(&state[13], nonce, 12);

    uint8_t block[64];
    size_t offset = 0;

    while (offset < len) {
        chacha20_block(state, block);

        size_t to_copy = len - offset;
        if (to_copy > 64) to_copy = 64;
        memcpy(out + offset, block, to_copy);

        offset += to_copy;
        state[12]++;  /* increment counter for next block */
    }
}
