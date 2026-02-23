#ifndef C_UTILS_SHA1_H
#define C_UTILS_SHA1_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint32_t state[5];
    uint32_t count[2];
    uint8_t  buffer[64];
} sha1_ctx_t;

typedef enum {
    SHA1_OK = 0,
    SHA1_ERROR_INVALID_PARAMS = -1,
    SHA1_ERROR_CONTEXT_NULL = -2,
    SHA1_ERROR_DATA_NULL = -3,
    SHA1_ERROR_DIGEST_NULL = -4,
    SHA1_ERROR_BUFFER_TOO_SMALL = -5,
    SHA1_ERROR_FILE_OPEN = -6,
    SHA1_ERROR_FILE_READ = -7,
    SHA1_ERROR_MEMORY = -8
} sha1_error_t;

typedef struct {
    bool enable_file_operations;  // 启用文件操作
    bool enable_hex_output;       // 启用十六进制输出
    bool enable_string_output;     // 启用字符串输出
    size_t max_input_size;         // 最大输入大小（0表示无限制）
} sha1_config_t;

typedef struct {
    sha1_error_t last_error;     // 最后一次错误
    size_t total_processed;      // 总处理字节数
    bool is_initialized;         // 是否初始化
    bool is_finalized;           // 是否已完成
} sha1_state_t;

/**
 * @brief 初始化SHA1上下文
 * @param ctx 上下文指针
 */
void sha1_init(sha1_ctx_t *ctx);

/**
 * @brief 增强版SHA1上下文初始化
 * @param ctx 上下文指针
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
sha1_error_t sha1_init_ex(sha1_ctx_t *ctx, const sha1_config_t *config, sha1_state_t *state);

/**
 * @brief 更新SHA1上下文
 * @param ctx 上下文指针
 * @param data 输入数据
 * @param len 数据长度
 */
void sha1_update(sha1_ctx_t *ctx, const uint8_t *data, size_t len);

/**
 * @brief 增强版SHA1上下文更新
 * @param ctx 上下文指针
 * @param data 输入数据
 * @param len 数据长度
 * @param state 状态输出
 * @return 错误码
 */
sha1_error_t sha1_update_ex(sha1_ctx_t *ctx, const uint8_t *data, size_t len, sha1_state_t *state);

/**
 * @brief 完成SHA1计算
 * @param ctx 上下文指针
 * @param digest 输出摘要（20字节）
 */
void sha1_final(sha1_ctx_t *ctx, uint8_t digest[20]);

/**
 * @brief 增强版SHA1计算完成
 * @param ctx 上下文指针
 * @param digest 输出摘要（20字节）
 * @param state 状态输出
 * @return 错误码
 */
sha1_error_t sha1_final_ex(sha1_ctx_t *ctx, uint8_t digest[20], sha1_state_t *state);

/**
 * @brief 一次性计算SHA1摘要
 * @param data 输入数据
 * @param len 数据长度
 * @param digest 输出摘要（20字节）
 * @return 错误码
 */
sha1_error_t sha1_hash(const uint8_t *data, size_t len, uint8_t digest[20]);

/**
 * @brief 计算字符串的SHA1摘要
 * @param str 输入字符串
 * @param digest 输出摘要（20字节）
 * @return 错误码
 */
sha1_error_t sha1_hash_string(const char *str, uint8_t digest[20]);

/**
 * @brief 计算文件的SHA1摘要
 * @param filename 文件名
 * @param digest 输出摘要（20字节）
 * @param state 状态输出
 * @return 错误码
 */
sha1_error_t sha1_hash_file(const char *filename, uint8_t digest[20], sha1_state_t *state);

/**
 * @brief 将SHA1摘要转换为十六进制字符串
 * @param digest 摘要数据（20字节）
 * @param hex_str 输出字符串（至少41字节）
 * @return 错误码
 */
sha1_error_t sha1_to_hex(const uint8_t digest[20], char *hex_str);

/**
 * @brief 比较两个SHA1摘要是否相等
 * @param digest1 第一个摘要（20字节）
 * @param digest2 第二个摘要（20字节）
 * @param result 输出比较结果
 * @return 错误码
 */
sha1_error_t sha1_compare(const uint8_t digest1[20], const uint8_t digest2[20], bool *result);

/**
 * @brief 重置SHA1上下文
 * @param ctx 上下文指针
 * @param state 状态输出
 * @return 错误码
 */
sha1_error_t sha1_reset(sha1_ctx_t *ctx, sha1_state_t *state);

/**
 * @brief 获取SHA1摘要大小
 * @return 摘要大小（字节）
 */
size_t sha1_digest_size(void);

/**
 * @brief 获取SHA1块大小
 * @return 块大小（字节）
 */
size_t sha1_block_size(void);

/**
 * @brief 获取最后一次错误信息
 * @param state 状态
 * @return 错误描述字符串
 */
const char* sha1_strerror(const sha1_state_t *state);

/**
 * @brief 初始化配置为默认值
 * @param config 配置结构
 */
void sha1_config_init(sha1_config_t *config);

/**
 * @brief 初始化状态为默认值
 * @param state 状态结构
 */
void sha1_state_init(sha1_state_t *state);

#endif // C_UTILS_SHA1_H
