#ifndef C_UTILS_PBKDF2_H
#define C_UTILS_PBKDF2_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief PBKDF2 错误码
 */
typedef enum {
    PBKDF2_OK = 0,                  /**< 成功 */
    PBKDF2_ERROR_NULL_PTR,          /**< 空指针错误 */
    PBKDF2_ERROR_INVALID_ARGS,      /**< 无效参数 */
    PBKDF2_ERROR_ITERATIONS_TOO_LOW, /**< 迭代次数过低 */
    PBKDF2_ERROR_OUTPUT_TOO_SHORT,  /**< 输出长度过短 */
    PBKDF2_ERROR_HMAC_FAILED,       /**< HMAC 计算失败 */
    PBKDF2_ERROR_UNSUPPORTED_ALGORITHM, /**< 不支持的算法 */
    PBKDF2_ERROR_MAX                /**< 最大错误码 */
} pbkdf2_error_t;

/**
 * @brief PBKDF2 哈希算法类型
 */
typedef enum {
    PBKDF2_ALGORITHM_SHA1 = 0,      /**< SHA-1 */
    PBKDF2_ALGORITHM_SHA256,        /**< SHA-256 */
    PBKDF2_ALGORITHM_SHA512,        /**< SHA-512 */
    PBKDF2_ALGORITHM_MAX            /**< 最大算法类型 */
} pbkdf2_algorithm_t;

/**
 * @brief PBKDF2 配置
 */
typedef struct {
    pbkdf2_algorithm_t algorithm;   /**< 哈希算法 */
    uint32_t iterations;            /**< 迭代次数 */
    size_t salt_len;                /**< 盐长度 */
    size_t output_len;              /**< 输出长度 */
} pbkdf2_config_t;

/**
 * @brief 获取默认配置
 * @return 默认配置
 */
pbkdf2_config_t pbkdf2_default_config(void);

/**
 * @brief PBKDF2-SHA1 密钥派生
 * @param pass 密码
 * @param salt 盐值
 * @param salt_len 盐值长度
 * @param iterations 迭代次数
 * @param out 输出缓冲区
 * @param out_len 输出长度
 */
void pbkdf2_sha1(const char *pass, const uint8_t *salt, size_t salt_len, int iterations, uint8_t *out, size_t out_len);

/**
 * @brief PBKDF2 密钥派生（使用自定义算法）
 * @param pass 密码
 * @param pass_len 密码长度
 * @param salt 盐值
 * @param salt_len 盐值长度
 * @param iterations 迭代次数
 * @param algorithm 哈希算法
 * @param out 输出缓冲区
 * @param out_len 输出长度
 * @param error 错误码输出
 * @return 是否成功
 */
bool pbkdf2(const uint8_t *pass, size_t pass_len, const uint8_t *salt, size_t salt_len, uint32_t iterations, pbkdf2_algorithm_t algorithm, uint8_t *out, size_t out_len, pbkdf2_error_t *error);

/**
 * @brief PBKDF2 密钥派生（使用配置）
 * @param pass 密码
 * @param pass_len 密码长度
 * @param salt 盐值
 * @param config 配置选项
 * @param out 输出缓冲区
 * @param error 错误码输出
 * @return 是否成功
 */
bool pbkdf2_ex(const uint8_t *pass, size_t pass_len, const uint8_t *salt, const pbkdf2_config_t *config, uint8_t *out, pbkdf2_error_t *error);

/**
 * @brief PBKDF2-SHA256 密钥派生
 * @param pass 密码
 * @param pass_len 密码长度
 * @param salt 盐值
 * @param salt_len 盐值长度
 * @param iterations 迭代次数
 * @param out 输出缓冲区
 * @param out_len 输出长度
 * @param error 错误码输出
 * @return 是否成功
 */
bool pbkdf2_sha256(const uint8_t *pass, size_t pass_len, const uint8_t *salt, size_t salt_len, uint32_t iterations, uint8_t *out, size_t out_len, pbkdf2_error_t *error);

/**
 * @brief PBKDF2-SHA512 密钥派生
 * @param pass 密码
 * @param pass_len 密码长度
 * @param salt 盐值
 * @param salt_len 盐值长度
 * @param iterations 迭代次数
 * @param out 输出缓冲区
 * @param out_len 输出长度
 * @param error 错误码输出
 * @return 是否成功
 */
bool pbkdf2_sha512(const uint8_t *pass, size_t pass_len, const uint8_t *salt, size_t salt_len, uint32_t iterations, uint8_t *out, size_t out_len, pbkdf2_error_t *error);

/**
 * @brief 生成随机盐值
 * @param salt 盐值缓冲区
 * @param salt_len 盐值长度
 * @param error 错误码输出
 * @return 是否成功
 */
bool pbkdf2_generate_salt(uint8_t *salt, size_t salt_len, pbkdf2_error_t *error);

/**
 * @brief 验证 PBKDF2 派生的密钥
 * @param pass 密码
 * @param pass_len 密码长度
 * @param salt 盐值
 * @param salt_len 盐值长度
 * @param iterations 迭代次数
 * @param algorithm 哈希算法
 * @param expected 期望的派生密钥
 * @param expected_len 期望的派生密钥长度
 * @param error 错误码输出
 * @return 是否验证通过
 */
bool pbkdf2_verify(const uint8_t *pass, size_t pass_len, const uint8_t *salt, size_t salt_len, uint32_t iterations, pbkdf2_algorithm_t algorithm, const uint8_t *expected, size_t expected_len, pbkdf2_error_t *error);

/**
 * @brief 获取推荐的迭代次数
 * @param algorithm 哈希算法
 * @param security_level 安全级别 (1-5, 1=最低, 5=最高)
 * @return 推荐的迭代次数
 */
uint32_t pbkdf2_get_recommended_iterations(pbkdf2_algorithm_t algorithm, uint8_t security_level);

/**
 * @brief 获取算法的输出长度
 * @param algorithm 哈希算法
 * @return 算法的输出长度（字节）
 */
size_t pbkdf2_get_algorithm_output_length(pbkdf2_algorithm_t algorithm);

/**
 * @brief 获取错误信息
 * @param error 错误码
 * @return 错误信息字符串
 */
const char* pbkdf2_error_string(pbkdf2_error_t error);

#endif // C_UTILS_PBKDF2_H
