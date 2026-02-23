#ifndef C_UTILS_OTP_H
#define C_UTILS_OTP_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * @brief OTP 错误码
 */
typedef enum {
    OTP_OK = 0,                  /**< 成功 */
    OTP_ERROR_NULL_PTR,          /**< 空指针错误 */
    OTP_ERROR_INVALID_ARGS,      /**< 无效参数 */
    OTP_ERROR_SECRET_TOO_SHORT,  /**< 密钥太短 */
    OTP_ERROR_HMAC_FAILED,       /**< HMAC 计算失败 */
    OTP_ERROR_INVALID_ALGORITHM, /**< 无效算法 */
    OTP_ERROR_MAX                /**< 最大错误码 */
} otp_error_t;

/**
 * @brief OTP 算法类型
 */
typedef enum {
    OTP_ALGORITHM_SHA1 = 0,      /**< SHA-1 */
    OTP_ALGORITHM_SHA256,        /**< SHA-256 */
    OTP_ALGORITHM_SHA512,        /**< SHA-512 */
    OTP_ALGORITHM_MAX            /**< 最大算法类型 */
} otp_algorithm_t;

/**
 * @brief TOTP 配置
 */
typedef struct {
    otp_algorithm_t algorithm;   /**< 哈希算法 */
    uint32_t digits;             /**< 验证码位数 (6-8) */
    uint32_t time_step;          /**< 时间步长 (秒) */
    uint64_t epoch;              /**< 起始时间戳 (Unix 时间) */
} otp_totp_config_t;

/**
 * @brief HOTP 配置
 */
typedef struct {
    otp_algorithm_t algorithm;   /**< 哈希算法 */
    uint32_t digits;             /**< 验证码位数 (6-8) */
} otp_hotp_config_t;

/**
 * @brief OTP URI 配置
 */
typedef struct {
    const char *type;            /**< 类型 ("totp" 或 "hotp") */
    const char *label;           /**< 标签 (如 "user@example.com") */
    const char *issuer;          /**< 发行者 */
    const uint8_t *secret;       /**< 密钥 */
    size_t secret_len;           /**< 密钥长度 */
    uint64_t counter;            /**< HOTP 计数器 */
    otp_totp_config_t totp_config; /**< TOTP 配置 */
} otp_uri_config_t;

/**
 * @brief 获取默认 TOTP 配置
 * @return 默认配置
 */
otp_totp_config_t otp_default_totp_config(void);

/**
 * @brief 获取默认 HOTP 配置
 * @return 默认配置
 */
otp_hotp_config_t otp_default_hotp_config(void);

/**
 * @brief 生成 6 位 TOTP 验证码
 * @param secret 密钥
 * @param secret_len 密钥长度
 * @param time_sec 当前时间戳 (秒)
 * @return 6 位验证码
 */
uint32_t otp_generate_totp(const uint8_t *secret, size_t secret_len, uint64_t time_sec);

/**
 * @brief 使用自定义配置生成 TOTP 验证码
 * @param secret 密钥
 * @param secret_len 密钥长度
 * @param time_sec 当前时间戳 (秒)
 * @param config 配置选项
 * @param error 错误码输出
 * @return 验证码，失败返回 0
 */
uint32_t otp_generate_totp_ex(const uint8_t *secret, size_t secret_len, uint64_t time_sec, const otp_totp_config_t *config, otp_error_t *error);

/**
 * @brief 生成 HOTP 验证码
 * @param secret 密钥
 * @param secret_len 密钥长度
 * @param counter 计数器
 * @param digits 验证码位数
 * @param error 错误码输出
 * @return 验证码，失败返回 0
 */
uint32_t otp_generate_hotp(const uint8_t *secret, size_t secret_len, uint64_t counter, uint32_t digits, otp_error_t *error);

/**
 * @brief 使用自定义配置生成 HOTP 验证码
 * @param secret 密钥
 * @param secret_len 密钥长度
 * @param counter 计数器
 * @param config 配置选项
 * @param error 错误码输出
 * @return 验证码，失败返回 0
 */
uint32_t otp_generate_hotp_ex(const uint8_t *secret, size_t secret_len, uint64_t counter, const otp_hotp_config_t *config, otp_error_t *error);

/**
 * @brief 验证 TOTP 验证码
 * @param secret 密钥
 * @param secret_len 密钥长度
 * @param time_sec 当前时间戳 (秒)
 * @param code 要验证的验证码
 * @param window 时间窗口 (前后允许的时间步数)
 * @param error 错误码输出
 * @return 是否验证通过
 */
bool otp_verify_totp(const uint8_t *secret, size_t secret_len, uint64_t time_sec, uint32_t code, uint32_t window, otp_error_t *error);

/**
 * @brief 使用自定义配置验证 TOTP 验证码
 * @param secret 密钥
 * @param secret_len 密钥长度
 * @param time_sec 当前时间戳 (秒)
 * @param code 要验证的验证码
 * @param window 时间窗口 (前后允许的时间步数)
 * @param config 配置选项
 * @param error 错误码输出
 * @return 是否验证通过
 */
bool otp_verify_totp_ex(const uint8_t *secret, size_t secret_len, uint64_t time_sec, uint32_t code, uint32_t window, const otp_totp_config_t *config, otp_error_t *error);

/**
 * @brief 验证 HOTP 验证码
 * @param secret 密钥
 * @param secret_len 密钥长度
 * @param counter 计数器
 * @param code 要验证的验证码
 * @param digits 验证码位数
 * @param error 错误码输出
 * @return 是否验证通过
 */
bool otp_verify_hotp(const uint8_t *secret, size_t secret_len, uint64_t counter, uint32_t code, uint32_t digits, otp_error_t *error);

/**
 * @brief 使用自定义配置验证 HOTP 验证码
 * @param secret 密钥
 * @param secret_len 密钥长度
 * @param counter 计数器
 * @param code 要验证的验证码
 * @param config 配置选项
 * @param error 错误码输出
 * @return 是否验证通过
 */
bool otp_verify_hotp_ex(const uint8_t *secret, size_t secret_len, uint64_t counter, uint32_t code, const otp_hotp_config_t *config, otp_error_t *error);

/**
 * @brief 生成 OTP URI (用于二维码生成)
 * @param config URI 配置
 * @param uri_buffer URI 缓冲区
 * @param buffer_size 缓冲区大小
 * @param error 错误码输出
 * @return URI 长度，失败返回 0
 */
size_t otp_generate_uri(const otp_uri_config_t *config, char *uri_buffer, size_t buffer_size, otp_error_t *error);

/**
 * @brief 生成随机 OTP 密钥
 * @param secret_buffer 密钥缓冲区
 * @param buffer_size 缓冲区大小
 * @param error 错误码输出
 * @return 生成的密钥长度，失败返回 0
 */
size_t otp_generate_secret(uint8_t *secret_buffer, size_t buffer_size, otp_error_t *error);

/**
 * @brief 将二进制密钥转换为 Base32 字符串
 * @param secret 二进制密钥
 * @param secret_len 密钥长度
 * @param base32_buffer Base32 缓冲区
 * @param buffer_size 缓冲区大小
 * @param error 错误码输出
 * @return Base32 字符串长度，失败返回 0
 */
size_t otp_secret_to_base32(const uint8_t *secret, size_t secret_len, char *base32_buffer, size_t buffer_size, otp_error_t *error);

/**
 * @brief 将 Base32 字符串转换为二进制密钥
 * @param base32_str Base32 字符串
 * @param secret_buffer 密钥缓冲区
 * @param buffer_size 缓冲区大小
 * @param error 错误码输出
 * @return 二进制密钥长度，失败返回 0
 */
size_t otp_base32_to_secret(const char *base32_str, uint8_t *secret_buffer, size_t buffer_size, otp_error_t *error);

/**
 * @brief 获取错误信息
 * @param error 错误码
 * @return 错误信息字符串
 */
const char* otp_error_string(otp_error_t error);

#endif // C_UTILS_OTP_H
