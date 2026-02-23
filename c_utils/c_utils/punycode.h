#ifndef C_UTILS_PUNYCODE_H
#define C_UTILS_PUNYCODE_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Punycode 错误码
 */
typedef enum {
    PUNYCODE_OK = 0,                  /**< 成功 */
    PUNYCODE_ERROR_NULL_PTR,          /**< 空指针错误 */
    PUNYCODE_ERROR_INVALID_ARGS,      /**< 无效参数 */
    PUNYCODE_ERROR_BUFFER_TOO_SMALL,  /**< 缓冲区太小 */
    PUNYCODE_ERROR_INVALID_INPUT,     /**< 无效输入 */
    PUNYCODE_ERROR_OVERFLOW,          /**< 溢出错误 */
    PUNYCODE_ERROR_MAX                /**< 最大错误码 */
} punycode_error_t;

/**
 * @brief Punycode 配置
 */
typedef struct {
    bool strict;                    /**< 是否严格模式 */
    bool idna_compatible;           /**< 是否兼容 IDNA 标准 */
    size_t max_input_len;           /**< 最大输入长度 */
    size_t max_output_len;          /**< 最大输出长度 */
} punycode_config_t;

/**
 * @brief 获取默认配置
 * @return 默认配置
 */
punycode_config_t punycode_default_config(void);

/**
 * @brief Punycode 编码
 * @param input Unicode 码点数组
 * @param input_len 输入长度
 * @param output 输出缓冲区
 * @param output_len 输出长度（输入/输出）
 * @return 是否成功
 */
bool punycode_encode(const uint32_t *input, size_t input_len, char *output, size_t *output_len);

/**
 * @brief Punycode 编码（带错误处理）
 * @param input Unicode 码点数组
 * @param input_len 输入长度
 * @param output 输出缓冲区
 * @param output_len 输出长度（输入/输出）
 * @param config 配置选项
 * @param error 错误码输出
 * @return 是否成功
 */
bool punycode_encode_ex(const uint32_t *input, size_t input_len, char *output, size_t *output_len, const punycode_config_t *config, punycode_error_t *error);

/**
 * @brief Punycode 解码
 * @param input Punycode 编码字符串
 * @param input_len 输入长度
 * @param output Unicode 码点数组
 * @param output_len 输出长度（输入/输出）
 * @param error 错误码输出
 * @return 是否成功
 */
bool punycode_decode(const char *input, size_t input_len, uint32_t *output, size_t *output_len, punycode_error_t *error);

/**
 * @brief Punycode 解码（带配置）
 * @param input Punycode 编码字符串
 * @param input_len 输入长度
 * @param output Unicode 码点数组
 * @param output_len 输出长度（输入/输出）
 * @param config 配置选项
 * @param error 错误码输出
 * @return 是否成功
 */
bool punycode_decode_ex(const char *input, size_t input_len, uint32_t *output, size_t *output_len, const punycode_config_t *config, punycode_error_t *error);

/**
 * @brief 编码域名（IDNA 兼容）
 * @param domain 域名
 * @param output 输出缓冲区
 * @param output_len 输出长度（输入/输出）
 * @param error 错误码输出
 * @return 是否成功
 */
bool punycode_encode_domain(const char *domain, char *output, size_t *output_len, punycode_error_t *error);

/**
 * @brief 解码域名（IDNA 兼容）
 * @param domain Punycode 编码域名
 * @param output 输出缓冲区
 * @param output_len 输出长度（输入/输出）
 * @param error 错误码输出
 * @return 是否成功
 */
bool punycode_decode_domain(const char *domain, char *output, size_t *output_len, punycode_error_t *error);

/**
 * @brief 检查字符串是否为 Punycode 编码
 * @param input 输入字符串
 * @param input_len 输入长度
 * @return 是否为 Punycode 编码
 */
bool punycode_is_encoded(const char *input, size_t input_len);

/**
 * @brief 获取错误信息
 * @param error 错误码
 * @return 错误信息字符串
 */
const char* punycode_error_string(punycode_error_t error);

#endif // C_UTILS_PUNYCODE_H
