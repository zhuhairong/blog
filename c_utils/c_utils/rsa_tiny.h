#ifndef C_UTILS_RSA_TINY_H
#define C_UTILS_RSA_TINY_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * @brief RSA 错误码
 */
typedef enum {
    RSA_OK = 0,                  /**< 成功 */
    RSA_ERROR_NULL_PTR,          /**< 空指针错误 */
    RSA_ERROR_INVALID_ARGS,      /**< 无效参数 */
    RSA_ERROR_MODULUS_TOO_SMALL,  /**< 模数过小 */
    RSA_ERROR_EXPONENT_TOO_LARGE, /**< 指数过大 */
    RSA_ERROR_BASE_TOO_LARGE,     /**< 底数过大 */
    RSA_ERROR_CALCULATION_FAILED, /**< 计算失败 */
    RSA_ERROR_OVERFLOW,          /**< 溢出错误 */
    RSA_ERROR_MAX                /**< 最大错误码 */
} rsa_error_t;

/**
 * @brief RSA 配置
 */
typedef struct {
    bool use_optimized;           /**< 是否使用优化算法 */
    bool check_inputs;            /**< 是否检查输入参数 */
    bool use_montgomery;          /**< 是否使用蒙哥马利算法 */
    size_t max_bits;              /**< 最大位数 */
    size_t max_iterations;        /**< 最大迭代次数 */
} rsa_config_t;

/**
 * @brief RSA 密钥结构
 */
typedef struct {
    uint64_t n;                   /**< 模数 */
    uint64_t e;                   /**< 公钥指数 */
    uint64_t d;                   /**< 私钥指数 */
    uint64_t p;                   /**< 素数因子 p */
    uint64_t q;                   /**< 素数因子 q */
} rsa_key_t;

/**
 * @brief 获取默认配置
 * @return 默认配置
 */
rsa_config_t rsa_default_config(void);

/**
 * @brief RSA 模幂运算: (base^exp) % mod
 * @param base 底数
 * @param exp 指数
 * @param mod 模数
 * @return 计算结果
 */
uint64_t rsa_tiny_modpow(uint64_t base, uint64_t exp, uint64_t mod);

/**
 * @brief RSA 模幂运算（带错误处理）
 * @param base 底数
 * @param exp 指数
 * @param mod 模数
 * @param config 配置选项
 * @param error 错误码输出
 * @return 计算结果，失败返回 0
 */
uint64_t rsa_tiny_modpow_ex(uint64_t base, uint64_t exp, uint64_t mod, const rsa_config_t *config, rsa_error_t *error);

/**
 * @brief 快速模幂运算（二进制方法）
 * @param base 底数
 * @param exp 指数
 * @param mod 模数
 * @param error 错误码输出
 * @return 计算结果
 */
uint64_t rsa_tiny_modpow_binary(uint64_t base, uint64_t exp, uint64_t mod, rsa_error_t *error);

/**
 * @brief 快速模幂运算（蒙哥马利方法）
 * @param base 底数
 * @param exp 指数
 * @param mod 模数
 * @param error 错误码输出
 * @return 计算结果
 */
uint64_t rsa_tiny_modpow_montgomery(uint64_t base, uint64_t exp, uint64_t mod, rsa_error_t *error);

/**
 * @brief RSA 加密
 * @param key RSA 密钥
 * @param plaintext 明文
 * @param error 错误码输出
 * @return 密文
 */
uint64_t rsa_tiny_encrypt(const rsa_key_t *key, uint64_t plaintext, rsa_error_t *error);

/**
 * @brief RSA 解密
 * @param key RSA 密钥
 * @param ciphertext 密文
 * @param error 错误码输出
 * @return 明文
 */
uint64_t rsa_tiny_decrypt(const rsa_key_t *key, uint64_t ciphertext, rsa_error_t *error);

/**
 * @brief 生成 RSA 密钥对
 * @param key RSA 密钥输出
 * @param bits 密钥位数
 * @param error 错误码输出
 * @return 是否成功
 */
bool rsa_tiny_generate_key(rsa_key_t *key, size_t bits, rsa_error_t *error);

/**
 * @brief 验证 RSA 密钥对
 * @param key RSA 密钥
 * @param error 错误码输出
 * @return 是否有效
 */
bool rsa_tiny_validate_key(const rsa_key_t *key, rsa_error_t *error);

/**
 * @brief 计算最大明文大小
 * @param mod 模数
 * @return 最大明文大小
 */
uint64_t rsa_tiny_max_plaintext(uint64_t mod);

/**
 * @brief 检查参数是否有效
 * @param base 底数
 * @param exp 指数
 * @param mod 模数
 * @param error 错误码输出
 * @return 是否有效
 */
bool rsa_tiny_validate_params(uint64_t base, uint64_t exp, uint64_t mod, rsa_error_t *error);

/**
 * @brief 获取错误信息
 * @param error 错误码
 * @return 错误信息字符串
 */
const char* rsa_error_string(rsa_error_t error);

#endif // C_UTILS_RSA_TINY_H
