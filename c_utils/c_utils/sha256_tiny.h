#ifndef C_UTILS_SHA256_TINY_H
#define C_UTILS_SHA256_TINY_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef enum {
    SHA256_TINY_OK = 0,
    SHA256_TINY_ERROR_INVALID_PARAMS = -1,
    SHA256_TINY_ERROR_DATA_NULL = -2,
    SHA256_TINY_ERROR_DIGEST_NULL = -3,
    SHA256_TINY_ERROR_BUFFER_TOO_SMALL = -4,
    SHA256_TINY_ERROR_FILE_OPEN = -5,
    SHA256_TINY_ERROR_FILE_READ = -6,
    SHA256_TINY_ERROR_MEMORY = -7
} sha256_tiny_error_t;

typedef struct {
    bool enable_file_operations;  // 启用文件操作
    bool enable_hex_output;       // 启用十六进制输出
    bool enable_string_output;     // 启用字符串输出
    size_t max_input_size;         // 最大输入大小（0表示无限制）
} sha256_tiny_config_t;

typedef struct {
    sha256_tiny_error_t last_error;     // 最后一次错误
    size_t total_processed;          // 总处理字节数
    bool is_initialized;             // 是否初始化
} sha256_tiny_state_t;

/**
 * @brief 计算SHA-256哈希值
 * @param data 输入数据
 * @param len 数据长度
 * @param digest 输出摘要（32字节）
 */
void sha256_tiny(const uint8_t *data, size_t len, uint8_t *digest);

/**
 * @brief 增强版SHA-256哈希计算
 * @param data 输入数据
 * @param len 数据长度
 * @param digest 输出摘要（32字节）
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
sha256_tiny_error_t sha256_tiny_ex(const uint8_t *data, size_t len, uint8_t *digest, const sha256_tiny_config_t *config, sha256_tiny_state_t *state);

/**
 * @brief 计算字符串的SHA-256哈希值
 * @param str 输入字符串
 * @param digest 输出摘要（32字节）
 * @return 错误码
 */
sha256_tiny_error_t sha256_tiny_hash_string(const char *str, uint8_t *digest);

/**
 * @brief 计算文件的SHA-256哈希值
 * @param filename 文件名
 * @param digest 输出摘要（32字节）
 * @param state 状态输出
 * @return 错误码
 */
sha256_tiny_error_t sha256_tiny_hash_file(const char *filename, uint8_t *digest, sha256_tiny_state_t *state);

/**
 * @brief 将SHA-256摘要转换为十六进制字符串
 * @param digest 摘要数据（32字节）
 * @param hex_str 输出字符串（至少65字节）
 * @return 错误码
 */
sha256_tiny_error_t sha256_tiny_to_hex(const uint8_t *digest, char *hex_str);

/**
 * @brief 比较两个SHA-256摘要是否相等
 * @param digest1 第一个摘要（32字节）
 * @param digest2 第二个摘要（32字节）
 * @param result 输出比较结果
 * @return 错误码
 */
sha256_tiny_error_t sha256_tiny_compare(const uint8_t *digest1, const uint8_t *digest2, bool *result);

/**
 * @brief 获取SHA-256摘要大小
 * @return 摘要大小（字节）
 */
size_t sha256_tiny_digest_size(void);

/**
 * @brief 获取最后一次错误信息
 * @param state 状态
 * @return 错误描述字符串
 */
const char* sha256_tiny_strerror(const sha256_tiny_state_t *state);

/**
 * @brief 初始化配置为默认值
 * @param config 配置结构
 */
void sha256_tiny_config_init(sha256_tiny_config_t *config);

/**
 * @brief 初始化状态为默认值
 * @param state 状态结构
 */
void sha256_tiny_state_init(sha256_tiny_state_t *state);

#endif // C_UTILS_SHA256_TINY_H
