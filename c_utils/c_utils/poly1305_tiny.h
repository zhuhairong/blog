#ifndef C_UTILS_POLY1305_TINY_H
#define C_UTILS_POLY1305_TINY_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * @brief Poly1305 错误码
 */
typedef enum {
    POLY1305_OK = 0,                  /**< 成功 */
    POLY1305_ERROR_NULL_PTR,          /**< 空指针错误 */
    POLY1305_ERROR_INVALID_KEY_SIZE,  /**< 无效密钥大小 */
    POLY1305_ERROR_INVALID_MSG_SIZE,  /**< 无效消息大小 */
    POLY1305_ERROR_INVALID_MAC_SIZE,  /**< 无效 MAC 大小 */
    POLY1305_ERROR_MAX                /**< 最大错误码 */
} poly1305_error_t;

/**
 * @brief Poly1305 配置
 */
typedef struct {
    size_t key_size;                  /**< 密钥大小 (必须为 32 字节) */
    size_t mac_size;                  /**< MAC 大小 (通常为 16 字节) */
    bool verify_only;                 /**< 是否仅验证模式 */
} poly1305_config_t;

/**
 * @brief Poly1305 上下文
 */
typedef struct {
    uint32_t r[5];                    /**< 密钥的 r 部分 */
    uint32_t s[4];                    /**< 密钥的 s 部分 */
    uint32_t h[5];                    /**< 哈希状态 */
    uint8_t buffer[16];               /**< 消息缓冲区 */
    size_t buffer_pos;                /**< 缓冲区位置 */
    poly1305_config_t config;         /**< 配置 */
} poly1305_ctx_t;

/**
 * @brief Poly1305 密钥大小
 */
#define POLY1305_KEY_SIZE 32

/**
 * @brief Poly1305 MAC 大小
 */
#define POLY1305_MAC_SIZE 16

/**
 * @brief 获取默认配置
 * @return 默认配置
 */
poly1305_config_t poly1305_default_config(void);

/**
 * @brief 计算 Poly1305 MAC
 * @param key 密钥 (32 字节)
 * @param msg 消息
 * @param len 消息长度
 * @param mac MAC 输出 (16 字节)
 */
void poly1305_tiny(const uint8_t *key, const uint8_t *msg, size_t len, uint8_t *mac);

/**
 * @brief 计算 Poly1305 MAC（带错误处理）
 * @param key 密钥
 * @param key_len 密钥长度
 * @param msg 消息
 * @param msg_len 消息长度
 * @param mac MAC 输出
 * @param mac_len MAC 长度
 * @param error 错误码输出
 * @return 是否成功
 */
bool poly1305_tiny_ex(const uint8_t *key, size_t key_len, const uint8_t *msg, size_t msg_len, uint8_t *mac, size_t mac_len, poly1305_error_t *error);

/**
 * @brief 初始化 Poly1305 上下文
 * @param ctx 上下文
 * @param key 密钥
 * @param key_len 密钥长度
 * @param error 错误码输出
 * @return 是否成功
 */
bool poly1305_init(poly1305_ctx_t *ctx, const uint8_t *key, size_t key_len, poly1305_error_t *error);

/**
 * @brief 使用配置初始化 Poly1305 上下文
 * @param ctx 上下文
 * @param key 密钥
 * @param key_len 密钥长度
 * @param config 配置选项
 * @param error 错误码输出
 * @return 是否成功
 */
bool poly1305_init_ex(poly1305_ctx_t *ctx, const uint8_t *key, size_t key_len, const poly1305_config_t *config, poly1305_error_t *error);

/**
 * @brief 更新 Poly1305 上下文
 * @param ctx 上下文
 * @param msg 消息
 * @param msg_len 消息长度
 * @param error 错误码输出
 * @return 是否成功
 */
bool poly1305_update(poly1305_ctx_t *ctx, const uint8_t *msg, size_t msg_len, poly1305_error_t *error);

/**
 * @brief 完成 Poly1305 计算并获取 MAC
 * @param ctx 上下文
 * @param mac MAC 输出
 * @param mac_len MAC 长度
 * @param error 错误码输出
 * @return 是否成功
 */
bool poly1305_final(poly1305_ctx_t *ctx, uint8_t *mac, size_t mac_len, poly1305_error_t *error);

/**
 * @brief 验证 Poly1305 MAC
 * @param key 密钥
 * @param key_len 密钥长度
 * @param msg 消息
 * @param msg_len 消息长度
 * @param expected_mac 期望的 MAC
 * @param mac_len MAC 长度
 * @param error 错误码输出
 * @return 是否验证通过
 */
bool poly1305_verify(const uint8_t *key, size_t key_len, const uint8_t *msg, size_t msg_len, const uint8_t *expected_mac, size_t mac_len, poly1305_error_t *error);

/**
 * @brief 重置 Poly1305 上下文
 * @param ctx 上下文
 * @param error 错误码输出
 * @return 是否成功
 */
bool poly1305_reset(poly1305_ctx_t *ctx, poly1305_error_t *error);

/**
 * @brief 获取错误信息
 * @param error 错误码
 * @return 错误信息字符串
 */
const char* poly1305_error_string(poly1305_error_t error);

#endif // C_UTILS_POLY1305_TINY_H
