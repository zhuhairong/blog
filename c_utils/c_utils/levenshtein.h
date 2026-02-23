#ifndef C_UTILS_LEVENSHTEIN_H
#define C_UTILS_LEVENSHTEIN_H

#include <stddef.h>
#include <stdbool.h>

// Levenshtein 错误码
typedef enum {
    LEVENSHTEIN_OK = 0,
    LEVENSHTEIN_INVALID_INPUT = -1,
    LEVENSHTEIN_MEMORY_ERROR = -2,
    LEVENSHTEIN_BUFFER_TOO_SMALL = -3
} levenshtein_error_t;

// Levenshtein 配置
typedef struct {
    bool case_sensitive;
    bool use_optimized;
    bool include_substitution;
    bool include_insertion;
    bool include_deletion;
    size_t max_distance;
} levenshtein_config_t;

// 计算 Levenshtein 编辑距离
size_t levenshtein_distance(const char *s1, const char *s2);

// 带错误处理的编辑距离计算
levenshtein_error_t levenshtein_distance_ex(const char *s1, size_t s1_len, const char *s2, size_t s2_len, 
                                           size_t *distance, const levenshtein_config_t *config);

// 计算字符串相似度（0.0-1.0）
double levenshtein_similarity(const char *s1, const char *s2);

// 带配置的相似度计算
double levenshtein_similarity_ex(const char *s1, size_t s1_len, const char *s2, size_t s2_len, 
                                const levenshtein_config_t *config, levenshtein_error_t *error);

// 优化版本的编辑距离计算（使用线性空间）
size_t levenshtein_distance_optimized(const char *s1, const char *s2);

// 批量计算编辑距离
size_t levenshtein_distance_batch(const char *target, const char **strings, size_t count, 
                                 size_t *distances);

// 获取默认配置
void levenshtein_get_default_config(levenshtein_config_t *config);

#endif // C_UTILS_LEVENSHTEIN_H
