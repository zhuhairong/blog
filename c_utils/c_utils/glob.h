#ifndef C_UTILS_GLOB_H
#define C_UTILS_GLOB_H

#include <stdbool.h>
#include <stddef.h>

// Glob 错误码
typedef enum {
    GLOB_OK = 0,
    GLOB_ERROR_INVALID_PARAM,
    GLOB_ERROR_INVALID_PATTERN,
    GLOB_ERROR_MEMORY_ALLOC,
    GLOB_ERROR_BUFFER_TOO_SMALL
} glob_error_t;

// Glob 标志
typedef enum {
    GLOB_FLAG_NONE = 0,
    GLOB_FLAG_CASE_INSENSITIVE = (1 << 0),
    GLOB_FLAG_PATHNAME = (1 << 1),
    GLOB_FLAG_NOESCAPE = (1 << 2),
    GLOB_FLAG_PERIOD = (1 << 3),
    GLOB_FLAG_MARK = (1 << 4),
    GLOB_FLAG_ONLYDIR = (1 << 5),
    GLOB_FLAG_NOCHECK = (1 << 6),
    GLOB_FLAG_APPEND = (1 << 7)
} glob_flags_t;

// Glob 结果
typedef struct {
    char **gl_pathv;
    size_t gl_pathc;
    size_t gl_offs;
    bool has_error;
    glob_error_t error;
    char error_msg[256];
} glob_t;

// Glob 配置
typedef struct {
    glob_flags_t flags;
    size_t max_matches;
    bool recursive;
    bool follow_symlinks;
} glob_config_t;

// 默认 Glob 配置
// 返回: 默认配置
glob_config_t glob_default_config(void);

// 字符串匹配
// pattern: 模式字符串
// text: 文本字符串
// flags: 标志
// error: 错误码（输出参数，可为 NULL）
// 返回: 匹配返回 true，否则返回 false
bool glob_match(const char *pattern, const char *text, glob_flags_t flags, glob_error_t *error);

// 文件系统匹配
// pattern: 模式字符串
// config: 配置（可为 NULL 使用默认配置）
// result: 结果（输出参数）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool glob(const char *pattern, const glob_config_t *config, glob_t *result, glob_error_t *error);

// 释放 Glob 结果
// result: 结果
void glob_free(glob_t *result);

// 批量匹配
// patterns: 模式数组
// text: 文本字符串
// count: 模式数量
// flags: 标志
// matches: 匹配结果数组（输出参数）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool glob_match_batch(const char **patterns, const char *text, size_t count, glob_flags_t flags, bool *matches, glob_error_t *error);

// 检查模式是否有效
// pattern: 模式字符串
// flags: 标志
// error: 错误码（输出参数，可为 NULL）
// 返回: 有效返回 true，无效返回 false
bool glob_validate_pattern(const char *pattern, glob_flags_t flags, glob_error_t *error);

// 获取错误信息
// error: 错误码
// 返回: 错误信息字符串
const char* glob_strerror(glob_error_t error);

#endif // C_UTILS_GLOB_H
