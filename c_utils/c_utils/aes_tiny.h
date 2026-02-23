#ifndef C_UTILS_AES_TINY_H
#define C_UTILS_AES_TINY_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// AES tiny 错误码
typedef enum {
    AES_TINY_OK = 0,
    AES_TINY_INVALID_PARAMS = -1,
    AES_TINY_INVALID_KEY_SIZE = -2,
    AES_TINY_INVALID_BLOCK_SIZE = -3,
    AES_TINY_MEMORY_ERROR = -4,
    AES_TINY_ENCRYPT_ERROR = -5,
    AES_TINY_DECRYPT_ERROR = -6,
    AES_TINY_UNSUPPORTED_MODE = -7
} aes_tiny_error_t;

// AES 模式
typedef enum {
    AES_TINY_MODE_ECB,
    AES_TINY_MODE_CBC,
    AES_TINY_MODE_CFB,
    AES_TINY_MODE_OFB
} aes_tiny_mode_t;

// AES 密钥大小
typedef enum {
    AES_TINY_KEY_SIZE_128 = 16,
    AES_TINY_KEY_SIZE_192 = 24,
    AES_TINY_KEY_SIZE_256 = 32
} aes_tiny_key_size_t;

// AES tiny 配置选项
typedef struct {
    bool use_padding;           // 是否使用填充
    bool check_key_size;        // 是否检查密钥大小
    bool check_block_size;      // 是否检查块大小
    aes_tiny_mode_t mode;       // AES 模式
    aes_tiny_key_size_t key_size; // 密钥大小
} aes_tiny_config_t;

// AES tiny 上下文
typedef struct {
    aes_tiny_config_t config;
    uint8_t round_keys[240];     // 轮密钥
    int rounds;                  // 轮数
    aes_tiny_error_t last_error;
    size_t encrypt_count;
    size_t decrypt_count;
} aes_tiny_ctx_t;

// 创建 AES tiny 上下文
// 返回 AES_TINY_OK 表示成功，其他值表示错误
aes_tiny_error_t aes_tiny_create(aes_tiny_ctx_t** ctx, const aes_tiny_config_t* config, const uint8_t* key, size_t key_len);

// 销毁 AES tiny 上下文
void aes_tiny_destroy(aes_tiny_ctx_t* ctx);

// AES 加密（单个块）
// 返回 AES_TINY_OK 表示成功，其他值表示错误
aes_tiny_error_t aes_tiny_encrypt_block(aes_tiny_ctx_t* ctx, const uint8_t *in, uint8_t *out);

// AES 解密（单个块）
// 返回 AES_TINY_OK 表示成功，其他值表示错误
aes_tiny_error_t aes_tiny_decrypt_block(aes_tiny_ctx_t* ctx, const uint8_t *in, uint8_t *out);

// AES 加密（多个块）
// 返回 AES_TINY_OK 表示成功，其他值表示错误
aes_tiny_error_t aes_tiny_encrypt(aes_tiny_ctx_t* ctx, const uint8_t *in, size_t in_len, uint8_t *out, size_t *out_len, const uint8_t* iv);

// AES 解密（多个块）
// 返回 AES_TINY_OK 表示成功，其他值表示错误
aes_tiny_error_t aes_tiny_decrypt(aes_tiny_ctx_t* ctx, const uint8_t *in, size_t in_len, uint8_t *out, size_t *out_len, const uint8_t* iv);

// 批量加密
// 返回 AES_TINY_OK 表示成功，其他值表示错误
aes_tiny_error_t aes_tiny_encrypt_batch(aes_tiny_ctx_t* ctx, const uint8_t** in, const size_t* in_lens, size_t count, uint8_t** out, size_t* out_lens, const uint8_t* iv);

// 传统加密函数（向后兼容）
void aes_tiny_encrypt_simple(const uint8_t *in, const uint8_t *key, uint8_t *out);

// 获取最后一次错误
aes_tiny_error_t aes_tiny_get_last_error(aes_tiny_ctx_t* ctx);

// 获取错误信息
const char* aes_tiny_strerror(aes_tiny_error_t error);

#endif // C_UTILS_AES_TINY_H
