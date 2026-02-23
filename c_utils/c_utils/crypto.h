#ifndef C_UTILS_CRYPTO_H
#define C_UTILS_CRYPTO_H

#include <stddef.h>
#include <stdint.h>
#include "md5.h"

// SHA-256 上下文类型 (与 crypto.c 中的内部结构匹配)
typedef struct {
    uint32_t state[8];
    uint64_t count;
    uint8_t  buffer[64];
} sha256_ctx_t;

void sha256_init(sha256_ctx_t *ctx);
void sha256_update(sha256_ctx_t *ctx, const uint8_t *data, size_t len);
void sha256_final(sha256_ctx_t *ctx, uint8_t digest[32]);

// 实用工具：将字节数组转为十六进制字符串 (out 至少需要 len*2 + 1)
void crypto_to_hex(const uint8_t *data, size_t len, char *out);

#endif // C_UTILS_CRYPTO_H
