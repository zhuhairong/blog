#ifndef C_UTILS_GLOB_MATCH_H
#define C_UTILS_GLOB_MATCH_H

#include <stdbool.h>
#include <stddef.h>

// 通配符匹配错误码
typedef enum {
    GLOB_MATCH_OK = 0,
    GLOB_MATCH_ERROR_INVALID_PARAM,
    GLOB_MATCH_ERROR_INVALID_PATTERN,
    GLOB_MATCH_ERROR_MEMORY_ALLOC
} glob_match_error_t;

// 通配符匹配标志
typedef enum {
    GLOB_MATCH_FLAG_NONE = 0,
    GLOB_MATCH_FLAG_CASE_INSENSITIVE = (1 << 0),
    GLOB_MATCH_FLAG_NOESCAPE = (1 << 1),
    GLOB_MATCH_FLAG_DOT_MATCHES_SLASH = (1 << 2)
} glob_match_flags_t;

// 通配符匹配配置
typedef struct {
    glob_match_flags_t flags;
    char wildcard_single;
    char wildcard_any;
    char escape_char;
} glob_match_config_t;

// 默认通配符匹配配置
// 返回: 默认配置
glob_match_config_t glob_match_default_config(void);

// 简单通配符匹配
// pattern: 模式字符串
// text: 文本字符串
// flags: 标志
// error: 错误码（输出参数，可为 NULL）
// 返回: 匹配返回 true，否则返回 false
bool glob_match_simple(const char *pattern, const char *text, glob_match_flags_t flags, glob_match_error_t *error);

// 配置化通配符匹配
// pattern: 模式字符串
// text: 文本字符串
// config: 配置（可为 NULL 使用默认配置）
// error: 错误码（输出参数，可为 NULL）
// 返回: 匹配返回 true，否则返回 false
bool glob_match_config(const char *pattern, const char *text, const glob_match_config_t *config, glob_match_error_t *error);

// 批量通配符匹配
// patterns: 模式数组
// text: 文本字符串
// count: 模式数量
// flags: 标志
// matches: 匹配结果数组（输出参数）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool glob_match_batch(const char **patterns, const char *text, size_t count, glob_match_flags_t flags, bool *matches, glob_match_error_t *error);

// 检查模式是否有效
// pattern: 模式字符串
// config: 配置（可为 NULL 使用默认配置）
// error: 错误码（输出参数，可为 NULL）
// 返回: 有效返回 true，无效返回 false
bool glob_match_validate_pattern(const char *pattern, const glob_match_config_t *config, glob_match_error_t *error);

// 获取错误信息
// error: 错误码
// 返回: 错误信息字符串
const char* glob_match_strerror(glob_match_error_t error);

#endif // C_UTILS_GLOB_MATCH_H
