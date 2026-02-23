#ifndef C_UTILS_REGEX_TINY_H
#define C_UTILS_REGEX_TINY_H

#include <stdbool.h>
#include <stddef.h>

/**
 * @brief 正则表达式错误码
 */
typedef enum {
    REGEX_OK = 0,                  /**< 成功 */
    REGEX_ERROR_NULL_PTR,          /**< 空指针错误 */
    REGEX_ERROR_INVALID_ARGS,      /**< 无效参数 */
    REGEX_ERROR_INVALID_PATTERN,   /**< 无效模式 */
    REGEX_ERROR_TOO_MANY_STARS,    /**< 星号过多 */
    REGEX_ERROR_PATTERN_TOO_LONG,  /**< 模式过长 */
    REGEX_ERROR_STACK_OVERFLOW,    /**< 栈溢出 */
    REGEX_ERROR_MAX                /**< 最大错误码 */
} regex_error_t;

/**
 * @brief 正则表达式配置
 */
typedef struct {
    bool case_insensitive;         /**< 是否大小写不敏感 */
    bool dot_matches_newline;      /**< 是否点匹配换行符 */
    bool greedy;                   /**< 是否贪婪匹配 */
    bool anchored;                 /**< 是否锚定匹配 */
    size_t max_depth;              /**< 最大递归深度 */
    size_t max_matches;            /**< 最大匹配数 */
} regex_config_t;

/**
 * @brief 正则表达式匹配结果
 */
typedef struct {
    size_t start;                  /**< 匹配开始位置 */
    size_t end;                    /**< 匹配结束位置 */
    size_t length;                 /**< 匹配长度 */
    bool matched;                  /**< 是否匹配成功 */
} regex_match_result_t;

/**
 * @brief 正则表达式上下文
 */
typedef struct {
    regex_config_t config;         /**< 配置 */
    const char *pattern;           /**< 模式字符串 */
    size_t pattern_len;            /**< 模式长度 */
    const char *text;              /**< 文本字符串 */
    size_t text_len;               /**< 文本长度 */
    size_t current_depth;          /**< 当前递归深度 */
    size_t current_pos;            /**< 当前文本位置 */
} regex_ctx_t;

/**
 * @brief 获取默认配置
 * @return 默认配置
 */
regex_config_t regex_default_config(void);

/**
 * @brief 正则表达式匹配
 * @param pattern 模式字符串
 * @param text 文本字符串
 * @return 是否匹配成功
 */
bool regex_match(const char *pattern, const char *text);

/**
 * @brief 正则表达式匹配（带错误处理）
 * @param pattern 模式字符串
 * @param text 文本字符串
 * @param config 配置选项
 * @param error 错误码输出
 * @return 是否匹配成功
 */
bool regex_match_ex(const char *pattern, const char *text, const regex_config_t *config, regex_error_t *error);

/**
 * @brief 正则表达式匹配（带结果）
 * @param pattern 模式字符串
 * @param text 文本字符串
 * @param result 匹配结果输出
 * @param config 配置选项
 * @param error 错误码输出
 * @return 是否匹配成功
 */
bool regex_match_result(const char *pattern, const char *text, regex_match_result_t *result, const regex_config_t *config, regex_error_t *error);

/**
 * @brief 正则表达式搜索
 * @param pattern 模式字符串
 * @param text 文本字符串
 * @param start_pos 起始位置
 * @param result 匹配结果输出
 * @param config 配置选项
 * @param error 错误码输出
 * @return 是否找到匹配
 */
bool regex_search(const char *pattern, const char *text, size_t start_pos, regex_match_result_t *result, const regex_config_t *config, regex_error_t *error);

/**
 * @brief 初始化正则表达式上下文
 * @param ctx 上下文
 * @param pattern 模式字符串
 * @param text 文本字符串
 * @param config 配置选项
 * @param error 错误码输出
 * @return 是否成功
 */
bool regex_init(regex_ctx_t *ctx, const char *pattern, const char *text, const regex_config_t *config, regex_error_t *error);

/**
 * @brief 使用上下文进行匹配
 * @param ctx 上下文
 * @param result 匹配结果输出
 * @param error 错误码输出
 * @return 是否匹配成功
 */
bool regex_match_ctx(regex_ctx_t *ctx, regex_match_result_t *result, regex_error_t *error);

/**
 * @brief 验证正则表达式模式
 * @param pattern 模式字符串
 * @param error 错误码输出
 * @return 是否有效
 */
bool regex_validate_pattern(const char *pattern, regex_error_t *error);

/**
 * @brief 转义正则表达式特殊字符
 * @param input 输入字符串
 * @param output 输出缓冲区
 * @param output_size 输出缓冲区大小
 * @param error 错误码输出
 * @return 输出长度
 */
size_t regex_escape(const char *input, char *output, size_t output_size, regex_error_t *error);

/**
 * @brief 获取错误信息
 * @param error 错误码
 * @return 错误信息字符串
 */
const char* regex_error_string(regex_error_t error);

#endif // C_UTILS_REGEX_TINY_H
