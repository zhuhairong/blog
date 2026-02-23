#ifndef C_UTILS_CHACHA20_TINY_H
#define C_UTILS_CHACHA20_TINY_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// ChaCha20 上下文
typedef struct {
    uint32_t state[16];
    uint32_t counter;
    uint8_t  nonce[12];
    uint8_t  buffer[64];
    size_t   buffer_pos;
} chacha20_context_t;

// ChaCha20 错误码
typedef enum {
    CHACHA20_OK = 0,
    CHACHA20_ERROR_INVALID_KEY,
    CHACHA20_ERROR_INVALID_NONCE,
    CHACHA20_ERROR_INVALID_COUNTER,
    CHACHA20_ERROR_INVALID_LENGTH
} chacha20_error_t;

// 初始化 ChaCha20 上下文
// ctx: 上下文指针
// key: 密钥（必须是 32 字节）
// nonce: 随机数（必须是 12 字节）
// counter: 初始计数器值
// 返回: 成功返回 CHACHA20_OK，失败返回错误码
chacha20_error_t chacha20_init(chacha20_context_t *ctx, const uint8_t *key, const uint8_t *nonce, uint32_t counter);

// 更新 ChaCha20 加密/解密
// ctx: 上下文指针
// in: 输入数据
// out: 输出数据
// len: 数据长度
// 返回: 成功返回 CHACHA20_OK，失败返回错误码
chacha20_error_t chacha20_update(chacha20_context_t *ctx, const uint8_t *in, uint8_t *out, size_t len);

// 一次性 ChaCha20 加密/解密
// key: 密钥（必须是 32 字节）
// nonce: 随机数（必须是 12 字节）
// counter: 初始计数器值
// in: 输入数据
// out: 输出数据
// len: 数据长度
// 返回: 成功返回 CHACHA20_OK，失败返回错误码
chacha20_error_t chacha20_encrypt(const uint8_t *key, const uint8_t *nonce, uint32_t counter, const uint8_t *in, uint8_t *out, size_t len);

// 获取错误信息
// error: 错误码
// 返回: 错误信息字符串
const char* chacha20_strerror(chacha20_error_t error);

// 验证测试向量
// 返回: 验证通过返回 true，失败返回 false
bool chacha20_validate_test_vectors(void);

#endif // C_UTILS_CHACHA20_TINY_H
