#ifndef C_UTILS_KMP_H
#define C_UTILS_KMP_H

#include <stddef.h>
#include <stdbool.h>

// KMP 错误码
typedef enum {
    KMP_OK = 0,
    KMP_INVALID_INPUT = -1,
    KMP_PATTERN_TOO_SHORT = -2,
    KMP_TEXT_TOO_SHORT = -3,
    KMP_MEMORY_ERROR = -4,
    KMP_BUFFER_TOO_SMALL = -5
} kmp_error_t;

// KMP 配置
typedef struct {
    bool case_sensitive;
    bool find_all;
    size_t max_matches;
} kmp_config_t;

// 返回第一个匹配位置的索引，未找到返回 -1
int kmp_search(const char *text, const char *pattern);

// 带错误处理的 KMP 搜索
int kmp_search_ex(const char *text, size_t text_len, const char *pattern, size_t pattern_len, 
                  kmp_error_t *error);

// 查找所有匹配位置
size_t kmp_search_all(const char *text, size_t text_len, const char *pattern, size_t pattern_len, 
                      size_t *matches, size_t max_matches, kmp_error_t *error);

// 带配置的 KMP 搜索
int kmp_search_with_config(const char *text, size_t text_len, const char *pattern, size_t pattern_len, 
                          const kmp_config_t *config, kmp_error_t *error);

// 构建部分匹配表
int kmp_build_lps(const char *pattern, size_t pattern_len, int *lps, kmp_error_t *error);

// 获取默认配置
void kmp_get_default_config(kmp_config_t *config);

#endif // C_UTILS_KMP_H
