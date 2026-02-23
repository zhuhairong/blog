#ifndef C_UTILS_RABIN_KARP_H
#define C_UTILS_RABIN_KARP_H

#include <stddef.h>
#include <stdbool.h>

/**
 * @brief Rabin-Karp 错误码
 */
typedef enum {
    RABIN_KARP_OK = 0,                  /**< 成功 */
    RABIN_KARP_ERROR_NULL_PTR,          /**< 空指针错误 */
    RABIN_KARP_ERROR_INVALID_ARGS,      /**< 无效参数 */
    RABIN_KARP_ERROR_PATTERN_TOO_LONG,  /**< 模式串过长 */
    RABIN_KARP_ERROR_TEXT_TOO_SHORT,    /**< 文本过短 */
    RABIN_KARP_ERROR_HASH_OVERFLOW,     /**< 哈希溢出 */
    RABIN_KARP_ERROR_MAX                /**< 最大错误码 */
} rabin_karp_error_t;

/**
 * @brief Rabin-Karp 配置
 */
typedef struct {
    bool case_insensitive;             /**< 是否大小写不敏感 */
    bool binary_mode;                  /**< 是否二进制模式 */
    size_t radix;                      /**< 哈希基数 */
    size_t modulus;                    /**< 哈希模数 */
    size_t max_matches;                /**< 最大匹配数 (0 表示无限制) */
} rabin_karp_config_t;

/**
 * @brief Rabin-Karp 匹配结果
 */
typedef struct {
    size_t *positions;                 /**< 匹配位置数组 */
    size_t count;                      /**< 匹配数量 */
    size_t capacity;                   /**< 结果容量 */
    bool overflow;                     /**< 是否溢出 */
} rabin_karp_result_t;

/**
 * @brief Rabin-Karp 上下文
 */
typedef struct {
    rabin_karp_config_t config;         /**< 配置 */
    size_t pattern_len;                /**< 模式串长度 */
    size_t text_len;                   /**< 文本长度 */
    size_t hash_pattern;               /**< 模式串哈希值 */
    size_t hash_text;                  /**< 当前文本窗口哈希值 */
    size_t radix_power;                /**< 基数的 (pattern_len-1) 次方 */
} rabin_karp_ctx_t;

/**
 * @brief 获取默认配置
 * @return 默认配置
 */
rabin_karp_config_t rabin_karp_default_config(void);

/**
 * @brief Rabin-Karp 字符串搜索
 * @param txt 文本字符串
 * @param pat 模式字符串
 * @return 第一个匹配位置的索引，未找到返回 -1
 */
int rabin_karp_search(const char *txt, const char *pat);

/**
 * @brief Rabin-Karp 字符串搜索（带错误处理）
 * @param txt 文本字符串
 * @param txt_len 文本长度
 * @param pat 模式字符串
 * @param pat_len 模式长度
 * @param config 配置选项
 * @param error 错误码输出
 * @return 第一个匹配位置的索引，未找到返回 -1
 */
size_t rabin_karp_search_ex(const char *txt, size_t txt_len, const char *pat, size_t pat_len, const rabin_karp_config_t *config, rabin_karp_error_t *error);

/**
 * @brief 查找所有匹配位置
 * @param txt 文本字符串
 * @param txt_len 文本长度
 * @param pat 模式字符串
 * @param pat_len 模式长度
 * @param result 匹配结果输出
 * @param config 配置选项
 * @param error 错误码输出
 * @return 是否成功
 */
bool rabin_karp_find_all(const char *txt, size_t txt_len, const char *pat, size_t pat_len, rabin_karp_result_t *result, const rabin_karp_config_t *config, rabin_karp_error_t *error);

/**
 * @brief 初始化 Rabin-Karp 上下文
 * @param ctx 上下文
 * @param pat 模式字符串
 * @param pat_len 模式长度
 * @param config 配置选项
 * @param error 错误码输出
 * @return 是否成功
 */
bool rabin_karp_init(rabin_karp_ctx_t *ctx, const char *pat, size_t pat_len, const rabin_karp_config_t *config, rabin_karp_error_t *error);

/**
 * @brief 使用上下文搜索
 * @param ctx 上下文
 * @param txt 文本字符串
 * @param txt_len 文本长度
 * @param start_pos 起始位置
 * @param error 错误码输出
 * @return 匹配位置的索引，未找到返回 -1
 */
size_t rabin_karp_search_with_ctx(rabin_karp_ctx_t *ctx, const char *txt, size_t txt_len, size_t start_pos, rabin_karp_error_t *error);

/**
 * @brief 释放匹配结果
 * @param result 匹配结果
 */
void rabin_karp_free_result(rabin_karp_result_t *result);

/**
 * @brief 重置匹配结果
 * @param result 匹配结果
 * @param error 错误码输出
 * @return 是否成功
 */
bool rabin_karp_reset_result(rabin_karp_result_t *result, rabin_karp_error_t *error);

/**
 * @brief 计算字符串的哈希值
 * @param str 字符串
 * @param len 长度
 * @param radix 基数
 * @param modulus 模数
 * @param error 错误码输出
 * @return 哈希值
 */
size_t rabin_karp_hash(const char *str, size_t len, size_t radix, size_t modulus, rabin_karp_error_t *error);

/**
 * @brief 获取错误信息
 * @param error 错误码
 * @return 错误信息字符串
 */
const char* rabin_karp_error_string(rabin_karp_error_t error);

#endif // C_UTILS_RABIN_KARP_H
