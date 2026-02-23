#ifndef C_UTILS_HMAC_H
#define C_UTILS_HMAC_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// HMAC 错误码
typedef enum {
    HMAC_OK = 0,
    HMAC_ERROR_INVALID_PARAM,
    HMAC_ERROR_INVALID_ALGORITHM,
    HMAC_ERROR_MEMORY_ALLOC,
    HMAC_ERROR_BUFFER_TOO_SMALL,
    HMAC_ERROR_UNSUPPORTED_ALGORITHM
} hmac_error_t;

// HMAC 算法
typedef enum {
    HMAC_ALGO_SHA1,
    HMAC_ALGO_SHA224,
    HMAC_ALGO_SHA256,
    HMAC_ALGO_SHA384,
    HMAC_ALGO_SHA512,
    HMAC_ALGO_MD5,
    HMAC_ALGO_CUSTOM
} hmac_algorithm_t;

// HMAC 上下文
typedef struct {
    hmac_algorithm_t algo;
    size_t digest_size;
    uint8_t *key;
    size_t key_len;
    void *ctx;
    bool initialized;
    bool has_error;
    hmac_error_t error;
    char error_msg[256];
} hmac_ctx_t;

// HMAC 配置
typedef struct {
    hmac_algorithm_t algo;
    size_t digest_size;
    bool use_secure_compare;
} hmac_config_t;

// 默认 HMAC 配置
// algo: HMAC 算法
// 返回: 默认配置
hmac_config_t hmac_default_config(hmac_algorithm_t algo);

// 初始化 HMAC 上下文
// ctx: 上下文
// key: 密钥
// key_len: 密钥长度
// config: 配置（可为 NULL 使用默认配置）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool hmac_init(hmac_ctx_t *ctx, const uint8_t *key, size_t key_len, const hmac_config_t *config, hmac_error_t *error);

// 更新 HMAC 计算
// ctx: 上下文
// data: 数据
// data_len: 数据长度
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool hmac_update(hmac_ctx_t *ctx, const uint8_t *data, size_t data_len, hmac_error_t *error);

// 获取最终 HMAC 值
// ctx: 上下文
// digest: 输出缓冲区
// digest_size: 输出缓冲区大小
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool hmac_final(hmac_ctx_t *ctx, uint8_t *digest, size_t digest_size, hmac_error_t *error);

// 一次性 HMAC 计算
// algo: HMAC 算法
// key: 密钥
// key_len: 密钥长度
// data: 数据
// data_len: 数据长度
// digest: 输出缓冲区
// digest_size: 输出缓冲区大小
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool hmac_compute(hmac_algorithm_t algo, const uint8_t *key, size_t key_len, const uint8_t *data, size_t data_len, uint8_t *digest, size_t digest_size, hmac_error_t *error);

// HMAC-SHA1
// key: 密钥
// key_len: 密钥长度
// data: 数据
// data_len: 数据长度
// digest: 输出缓冲区（至少 20 字节）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool hmac_sha1(const uint8_t *key, size_t key_len, const uint8_t *data, size_t data_len, uint8_t digest[20], hmac_error_t *error);

// HMAC-SHA256
// key: 密钥
// key_len: 密钥长度
// data: 数据
// data_len: 数据长度
// digest: 输出缓冲区（至少 32 字节）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool hmac_sha256(const uint8_t *key, size_t key_len, const uint8_t *data, size_t data_len, uint8_t digest[32], hmac_error_t *error);

// 释放 HMAC 上下文
// ctx: 上下文
void hmac_free(hmac_ctx_t *ctx);

// 获取算法摘要大小
// algo: HMAC 算法
// 返回: 摘要大小（字节），失败返回 0
size_t hmac_get_digest_size(hmac_algorithm_t algo);

// 安全比较两个 HMAC 值
// digest1: 第一个摘要
// digest2: 第二个摘要
// size: 摘要大小
// 返回: 相等返回 true，否则返回 false
bool hmac_secure_compare(const uint8_t *digest1, const uint8_t *digest2, size_t size);

// 获取错误信息
// error: 错误码
// 返回: 错误信息字符串
const char* hmac_strerror(hmac_error_t error);

#endif // C_UTILS_HMAC_H
