#include "sha256_tiny.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define CH(x,y,z)  (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define ROTR(x,n)  (((x) >> (n)) | ((x) << (32-(n))))
#define SIG0(x)    (ROTR(x,2) ^ ROTR(x,13) ^ ROTR(x,22))
#define SIG1(x)    (ROTR(x,6) ^ ROTR(x,11) ^ ROTR(x,25))
#define sig0(x)    (ROTR(x,7) ^ ROTR(x,18) ^ ((x) >> 3))
#define sig1(x)    (ROTR(x,17) ^ ROTR(x,19) ^ ((x) >> 10))

static const uint32_t K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

/* Internal SHA-256 context for multi-block processing */
typedef struct {
    uint32_t h[8];
    uint64_t total_bits;
    uint8_t  buf[64];
    size_t   buf_len;
} sha256_internal_ctx;

static void sha256_init_internal(sha256_internal_ctx *ctx) {
    ctx->h[0] = 0x6a09e667;
    ctx->h[1] = 0xbb67ae85;
    ctx->h[2] = 0x3c6ef372;
    ctx->h[3] = 0xa54ff53a;
    ctx->h[4] = 0x510e527f;
    ctx->h[5] = 0x9b05688c;
    ctx->h[6] = 0x1f83d9ab;
    ctx->h[7] = 0x5be0cd19;
    ctx->total_bits = 0;
    ctx->buf_len = 0;
}

/* SHA-256 block compression */
static void sha256_compress_block(uint32_t h[8], const uint8_t block[64]) {
    uint32_t w[64];
    int i;

    /* Prepare message schedule */
    for (i = 0; i < 16; i++) {
        w[i] = ((uint32_t)block[i*4] << 24) |
               ((uint32_t)block[i*4+1] << 16) |
               ((uint32_t)block[i*4+2] << 8) |
               ((uint32_t)block[i*4+3]);
    }
    for (i = 16; i < 64; i++) {
        w[i] = sig1(w[i-2]) + w[i-7] + sig0(w[i-15]) + w[i-16];
    }

    /* Initialize working variables */
    uint32_t a = h[0], b = h[1], c = h[2], d = h[3];
    uint32_t e = h[4], f = h[5], g = h[6], hh = h[7];

    /* 64 rounds */
    for (i = 0; i < 64; i++) {
        uint32_t t1 = hh + SIG1(e) + CH(e, f, g) + K[i] + w[i];
        uint32_t t2 = SIG0(a) + MAJ(a, b, c);
        hh = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }

    h[0] += a; h[1] += b; h[2] += c; h[3] += d;
    h[4] += e; h[5] += f; h[6] += g; h[7] += hh;
}

/* Feed data bytes into the internal context, compressing full blocks */
static void sha256_update_internal(sha256_internal_ctx *ctx, const uint8_t *data, size_t len) {
    ctx->total_bits += (uint64_t)len * 8;

    if (ctx->buf_len > 0) {
        size_t space = 64 - ctx->buf_len;
        size_t copy = len < space ? len : space;
        memcpy(ctx->buf + ctx->buf_len, data, copy);
        ctx->buf_len += copy;
        data += copy;
        len -= copy;

        if (ctx->buf_len == 64) {
            sha256_compress_block(ctx->h, ctx->buf);
            ctx->buf_len = 0;
        }
    }

    while (len >= 64) {
        sha256_compress_block(ctx->h, data);
        data += 64;
        len -= 64;
    }

    if (len > 0) {
        memcpy(ctx->buf, data, len);
        ctx->buf_len = len;
    }
}

/* Finalize: apply padding and produce digest */
static void sha256_final_internal(sha256_internal_ctx *ctx, uint8_t digest[32]) {
    /* Append 0x80 byte */
    ctx->buf[ctx->buf_len++] = 0x80;

    /* If there isn't room for 8-byte length, pad and compress */
    if (ctx->buf_len > 56) {
        memset(ctx->buf + ctx->buf_len, 0, 64 - ctx->buf_len);
        sha256_compress_block(ctx->h, ctx->buf);
        ctx->buf_len = 0;
    }

    /* Pad to 56 bytes (448 bits) */
    memset(ctx->buf + ctx->buf_len, 0, 56 - ctx->buf_len);

    /* Append total length in bits as big-endian 64-bit */
    uint64_t bits = ctx->total_bits;
    for (int i = 7; i >= 0; i--) {
        ctx->buf[56 + i] = (uint8_t)(bits & 0xff);
        bits >>= 8;
    }

    sha256_compress_block(ctx->h, ctx->buf);

    /* Convert hash state to big-endian byte array */
    for (int i = 0; i < 8; i++) {
        digest[i*4]     = (uint8_t)(ctx->h[i] >> 24);
        digest[i*4 + 1] = (uint8_t)(ctx->h[i] >> 16);
        digest[i*4 + 2] = (uint8_t)(ctx->h[i] >> 8);
        digest[i*4 + 3] = (uint8_t)(ctx->h[i]);
    }
}

/* ========== Public API ========== */

void sha256_tiny(const uint8_t *data, size_t len, uint8_t *digest) {
    sha256_internal_ctx ctx;
    sha256_init_internal(&ctx);
    if (data && len > 0) {
        sha256_update_internal(&ctx, data, len);
    }
    sha256_final_internal(&ctx, digest);
}

sha256_tiny_error_t sha256_tiny_ex(const uint8_t *data, size_t len, uint8_t *digest,
                                    const sha256_tiny_config_t *config, sha256_tiny_state_t *state) {
    if (!digest) {
        if (state) {
            state->last_error = SHA256_TINY_ERROR_DIGEST_NULL;
        }
        return SHA256_TINY_ERROR_DIGEST_NULL;
    }
    if (!data && len > 0) {
        if (state) {
            state->last_error = SHA256_TINY_ERROR_DATA_NULL;
        }
        return SHA256_TINY_ERROR_DATA_NULL;
    }
    if (config && config->max_input_size > 0 && len > config->max_input_size) {
        if (state) {
            state->last_error = SHA256_TINY_ERROR_BUFFER_TOO_SMALL;
        }
        return SHA256_TINY_ERROR_BUFFER_TOO_SMALL;
    }

    sha256_tiny(data, len, digest);

    if (state) {
        state->last_error = SHA256_TINY_OK;
        state->total_processed = len;
        state->is_initialized = true;
    }
    return SHA256_TINY_OK;
}

sha256_tiny_error_t sha256_tiny_hash_string(const char *str, uint8_t *digest) {
    if (!str) return SHA256_TINY_ERROR_DATA_NULL;
    if (!digest) return SHA256_TINY_ERROR_DIGEST_NULL;
    sha256_tiny((const uint8_t *)str, strlen(str), digest);
    return SHA256_TINY_OK;
}

sha256_tiny_error_t sha256_tiny_hash_file(const char *filename, uint8_t *digest, sha256_tiny_state_t *state) {
    if (!filename) {
        if (state) state->last_error = SHA256_TINY_ERROR_FILE_OPEN;
        return SHA256_TINY_ERROR_FILE_OPEN;
    }
    if (!digest) {
        if (state) state->last_error = SHA256_TINY_ERROR_DIGEST_NULL;
        return SHA256_TINY_ERROR_DIGEST_NULL;
    }

    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        if (state) state->last_error = SHA256_TINY_ERROR_FILE_OPEN;
        return SHA256_TINY_ERROR_FILE_OPEN;
    }

    sha256_internal_ctx ctx;
    sha256_init_internal(&ctx);

    uint8_t buf[4096];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), fp)) > 0) {
        sha256_update_internal(&ctx, buf, n);
    }

    if (ferror(fp)) {
        fclose(fp);
        if (state) state->last_error = SHA256_TINY_ERROR_FILE_READ;
        return SHA256_TINY_ERROR_FILE_READ;
    }

    fclose(fp);
    sha256_final_internal(&ctx, digest);

    if (state) {
        state->last_error = SHA256_TINY_OK;
        state->total_processed = ctx.total_bits / 8;
        state->is_initialized = true;
    }
    return SHA256_TINY_OK;
}

sha256_tiny_error_t sha256_tiny_to_hex(const uint8_t *digest, char *hex_str) {
    if (!digest) return SHA256_TINY_ERROR_DIGEST_NULL;
    if (!hex_str) return SHA256_TINY_ERROR_BUFFER_TOO_SMALL;

    for (int i = 0; i < 32; i++) {
        snprintf(hex_str + i * 2, 3, "%02x", digest[i]);
    }
    hex_str[64] = '\0';
    return SHA256_TINY_OK;
}

sha256_tiny_error_t sha256_tiny_compare(const uint8_t *digest1, const uint8_t *digest2, bool *result) {
    if (!digest1 || !digest2) return SHA256_TINY_ERROR_INVALID_PARAMS;
    if (!result) return SHA256_TINY_ERROR_INVALID_PARAMS;
    *result = (memcmp(digest1, digest2, 32) == 0);
    return SHA256_TINY_OK;
}

size_t sha256_tiny_digest_size(void) {
    return 32;
}

const char* sha256_tiny_strerror(const sha256_tiny_state_t *state) {
    if (!state) return "Null state";
    switch (state->last_error) {
        case SHA256_TINY_OK:                     return "Success";
        case SHA256_TINY_ERROR_INVALID_PARAMS:   return "Invalid parameters";
        case SHA256_TINY_ERROR_DATA_NULL:        return "Data is null";
        case SHA256_TINY_ERROR_DIGEST_NULL:      return "Digest buffer is null";
        case SHA256_TINY_ERROR_BUFFER_TOO_SMALL: return "Buffer too small";
        case SHA256_TINY_ERROR_FILE_OPEN:        return "File open error";
        case SHA256_TINY_ERROR_FILE_READ:        return "File read error";
        case SHA256_TINY_ERROR_MEMORY:           return "Memory allocation error";
        default:                                  return "Unknown error";
    }
}

void sha256_tiny_config_init(sha256_tiny_config_t *config) {
    if (!config) return;
    config->enable_file_operations = true;
    config->enable_hex_output = true;
    config->enable_string_output = true;
    config->max_input_size = 0; /* unlimited */
}

void sha256_tiny_state_init(sha256_tiny_state_t *state) {
    if (!state) return;
    state->last_error = SHA256_TINY_OK;
    state->total_processed = 0;
    state->is_initialized = false;
}
