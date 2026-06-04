#include "hmac.h"
#include "sha1.h"
#include "sha256_tiny.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

hmac_config_t hmac_default_config(hmac_algorithm_t algo) {
    hmac_config_t config = {
        .algo = algo,
        .digest_size = hmac_get_digest_size(algo),
        .use_secure_compare = true
    };
    return config;
}

bool hmac_init(hmac_ctx_t *ctx, const uint8_t *key, size_t key_len, const hmac_config_t *config, hmac_error_t *error) {
    if (!ctx || !key) {
        if (error) *error = HMAC_ERROR_INVALID_PARAM;
        return false;
    }

    memset(ctx, 0, sizeof(hmac_ctx_t));

    if (config) {
        ctx->algo = config->algo;
        ctx->digest_size = config->digest_size;
    } else {
        ctx->algo = HMAC_ALGO_SHA1;
        ctx->digest_size = 20;
    }

    ctx->key = malloc(key_len);
    if (!ctx->key) {
        if (error) *error = HMAC_ERROR_MEMORY_ALLOC;
        return false;
    }

    memcpy(ctx->key, key, key_len);
    ctx->key_len = key_len;
    ctx->initialized = true;

    if (error) *error = HMAC_OK;
    return true;
}

bool hmac_update(hmac_ctx_t *ctx, const uint8_t *data, size_t data_len, hmac_error_t *error) {
    if (!ctx || !data || !ctx->initialized) {
        if (error) *error = HMAC_ERROR_INVALID_PARAM;
        return false;
    }

    /* Accumulate data into internal buffer.
     * For incremental updates we buffer everything, then process in hmac_final().
     * The ctx->ctx void* is reused to hold accumulated data. */
    if (!ctx->ctx) {
        /* First update: allocate a simple dynamic buffer */
        ctx->ctx = malloc(sizeof(uint8_t*) + sizeof(size_t));
        if (!ctx->ctx) {
            if (error) *error = HMAC_ERROR_MEMORY_ALLOC;
            return false;
        }
        *(uint8_t**)ctx->ctx = NULL;
        *((size_t*)ctx->ctx + 1) = 0;
    }

    uint8_t **buf_ptr = (uint8_t**)ctx->ctx;
    size_t *buf_len = (size_t*)((uint8_t**)ctx->ctx + 1);

    uint8_t *new_buf = realloc(*buf_ptr, *buf_len + data_len);
    if (!new_buf) {
        if (error) *error = HMAC_ERROR_MEMORY_ALLOC;
        return false;
    }
    memcpy(new_buf + *buf_len, data, data_len);
    *buf_ptr = new_buf;
    *buf_len += data_len;

    if (error) *error = HMAC_OK;
    return true;
}

bool hmac_final(hmac_ctx_t *ctx, uint8_t *digest, size_t digest_size, hmac_error_t *error) {
    if (!ctx || !digest || !ctx->initialized) {
        if (error) *error = HMAC_ERROR_INVALID_PARAM;
        return false;
    }

    if (digest_size < ctx->digest_size) {
        if (error) *error = HMAC_ERROR_BUFFER_TOO_SMALL;
        return false;
    }

    /* Get accumulated data if any */
    uint8_t *acc_data = NULL;
    size_t acc_len = 0;
    if (ctx->ctx) {
        acc_data = *(uint8_t**)ctx->ctx;
        acc_len = *((size_t*)((uint8_t**)ctx->ctx + 1));
    }

    bool result = false;
    switch (ctx->algo) {
        case HMAC_ALGO_SHA1: {
            uint8_t tmp[20];
            result = hmac_sha1(ctx->key, ctx->key_len,
                               acc_data ? acc_data : (const uint8_t*)"", acc_len,
                               tmp, error);
            if (result) memcpy(digest, tmp, 20);
            break;
        }
        case HMAC_ALGO_SHA256: {
            uint8_t tmp[32];
            result = hmac_sha256(ctx->key, ctx->key_len,
                                 acc_data ? acc_data : (const uint8_t*)"", acc_len,
                                 tmp, error);
            if (result) memcpy(digest, tmp, 32);
            break;
        }
        default:
            if (error) *error = HMAC_ERROR_UNSUPPORTED_ALGORITHM;
            result = false;
            break;
    }

    return result;
}

bool hmac_compute(hmac_algorithm_t algo, const uint8_t *key, size_t key_len,
                  const uint8_t *data, size_t data_len,
                  uint8_t *digest, size_t digest_size, hmac_error_t *error) {
    if (!key || !data || !digest) {
        if (error) *error = HMAC_ERROR_INVALID_PARAM;
        return false;
    }

    hmac_ctx_t ctx;
    hmac_config_t config = hmac_default_config(algo);

    if (!hmac_init(&ctx, key, key_len, &config, error)) return false;
    if (!hmac_update(&ctx, data, data_len, error)) {
        hmac_free(&ctx);
        return false;
    }
    if (!hmac_final(&ctx, digest, digest_size, error)) {
        hmac_free(&ctx);
        return false;
    }

    hmac_free(&ctx);
    return true;
}

bool hmac_sha1(const uint8_t *key, size_t key_len, const uint8_t *data, size_t data_len,
               uint8_t digest[20], hmac_error_t *error) {
    if (!key || !data || !digest) {
        if (error) *error = HMAC_ERROR_INVALID_PARAM;
        return false;
    }

    sha1_ctx_t ctx;
    uint8_t k_ipad[64], k_opad[64], tk[20];
    const uint8_t *k = key;
    size_t klen = key_len;

    /* If key is longer than block size, hash it first */
    if (key_len > 64) {
        sha1_init(&ctx);
        sha1_update(&ctx, key, key_len);
        sha1_final(&ctx, tk);
        k = tk;
        klen = 20;
    }

    memset(k_ipad, 0, 64);
    memset(k_opad, 0, 64);
    memcpy(k_ipad, k, klen);
    memcpy(k_opad, k, klen);

    for (int i = 0; i < 64; i++) {
        k_ipad[i] ^= 0x36;
        k_opad[i] ^= 0x5c;
    }

    /* Inner hash: H(k_ipad || data) */
    sha1_init(&ctx);
    sha1_update(&ctx, k_ipad, 64);
    sha1_update(&ctx, data, data_len);
    sha1_final(&ctx, digest);

    /* Outer hash: H(k_opad || inner_hash) */
    sha1_init(&ctx);
    sha1_update(&ctx, k_opad, 64);
    sha1_update(&ctx, digest, 20);
    sha1_final(&ctx, digest);

    if (error) *error = HMAC_OK;
    return true;
}

bool hmac_sha256(const uint8_t *key, size_t key_len, const uint8_t *data, size_t data_len,
                 uint8_t digest[32], hmac_error_t *error) {
    if (!key || !data || !digest) {
        if (error) *error = HMAC_ERROR_INVALID_PARAM;
        return false;
    }

    uint8_t k_ipad[64], k_opad[64];
    const uint8_t *k = key;
    size_t klen = key_len;
    uint8_t tk[32];

    /* If key is longer than block size (64 for SHA-256), hash it first */
    if (key_len > 64) {
        sha256_tiny(key, key_len, tk);
        k = tk;
        klen = 32;
    }

    memset(k_ipad, 0, 64);
    memset(k_opad, 0, 64);
    memcpy(k_ipad, k, klen);
    memcpy(k_opad, k, klen);

    for (int i = 0; i < 64; i++) {
        k_ipad[i] ^= 0x36;
        k_opad[i] ^= 0x5c;
    }

    /* Inner hash: H(k_ipad || data) — allocate temp buffer */
    size_t inner_len = 64 + data_len;
    uint8_t *inner_buf = malloc(inner_len);
    if (!inner_buf) {
        if (error) *error = HMAC_ERROR_MEMORY_ALLOC;
        return false;
    }
    memcpy(inner_buf, k_ipad, 64);
    memcpy(inner_buf + 64, data, data_len);
    sha256_tiny(inner_buf, inner_len, digest);
    free(inner_buf);

    /* Outer hash: H(k_opad || inner_hash) */
    uint8_t outer_buf[64 + 32];
    memcpy(outer_buf, k_opad, 64);
    memcpy(outer_buf + 64, digest, 32);
    sha256_tiny(outer_buf, 64 + 32, digest);

    if (error) *error = HMAC_OK;
    return true;
}

void hmac_free(hmac_ctx_t *ctx) {
    if (!ctx) return;
    free(ctx->key);
    ctx->key = NULL;
    if (ctx->ctx) {
        uint8_t *buf = *(uint8_t**)ctx->ctx;
        free(buf);
        free(ctx->ctx);
        ctx->ctx = NULL;
    }
    ctx->initialized = false;
}

size_t hmac_get_digest_size(hmac_algorithm_t algo) {
    switch (algo) {
        case HMAC_ALGO_SHA1:   return 20;
        case HMAC_ALGO_SHA224: return 28;
        case HMAC_ALGO_SHA256: return 32;
        case HMAC_ALGO_SHA384: return 48;
        case HMAC_ALGO_SHA512: return 64;
        case HMAC_ALGO_MD5:    return 16;
        default:               return 0;
    }
}

bool hmac_secure_compare(const uint8_t *a, const uint8_t *b, size_t len) {
    if (!a || !b) return false;
    uint8_t result = 0;
    for (size_t i = 0; i < len; i++) {
        result |= a[i] ^ b[i];
    }
    return result == 0;
}

const char* hmac_strerror(hmac_error_t error) {
    switch (error) {
        case HMAC_OK:                       return "Success";
        case HMAC_ERROR_INVALID_PARAM:      return "Invalid parameter";
        case HMAC_ERROR_INVALID_ALGORITHM:  return "Invalid algorithm";
        case HMAC_ERROR_MEMORY_ALLOC:       return "Memory allocation failed";
        case HMAC_ERROR_BUFFER_TOO_SMALL:   return "Buffer too small";
        case HMAC_ERROR_UNSUPPORTED_ALGORITHM: return "Unsupported algorithm";
        default:                            return "Unknown error";
    }
}
