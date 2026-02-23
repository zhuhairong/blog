#ifndef C_UTILS_MANACHER_H
#define C_UTILS_MANACHER_H

#include <stddef.h>
#include <stdbool.h>

// Manacher 错误码
typedef enum {
    MANACHER_OK = 0,
    MANACHER_INVALID_INPUT = -1,
    MANACHER_MEMORY_ERROR = -2,
    MANACHER_BUFFER_TOO_SMALL = -3,
    MANACHER_EMPTY_STRING = -4,
    MANACHER_STRING_TOO_LONG = -5
} manacher_error_t;

// Manacher 配置
typedef struct {
    bool case_sensitive;
    bool find_all;
    bool include_single_char;
    size_t max_string_length;
    bool enable_stats;
} manacher_config_t;

// Manacher 统计信息
typedef struct {
    size_t longest_length;
    size_t total_palindromes;
    size_t start_position;
    double processing_time;
} manacher_stats_t;

// Manacher 算法，返回最长回文子串的长度
int manacher_longest_palindrome(const char *s);

// 带错误处理的最长回文子串查找
manacher_error_t manacher_longest_palindrome_ex(const char *s, size_t s_len, 
                                              size_t *length, size_t *start, 
                                              const manacher_config_t *config);

// 查找所有回文子串
size_t manacher_find_all(const char *s, size_t s_len, 
                        size_t **positions, size_t **lengths, 
                        size_t max_results, manacher_error_t *error);

// 检查字符串是否为回文
bool manacher_is_palindrome(const char *s, size_t s_len, bool case_sensitive);

// 获取回文子串
typedef struct {
    size_t start;
    size_t length;
} manacher_palindrome_t;

// 查找所有回文子串（结构化返回）
size_t manacher_find_all_struct(const char *s, size_t s_len, 
                               manacher_palindrome_t *results, 
                               size_t max_results, manacher_error_t *error);

// 获取默认配置
void manacher_get_default_config(manacher_config_t *config);

#endif // C_UTILS_MANACHER_H
