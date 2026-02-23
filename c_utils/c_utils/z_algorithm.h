#ifndef C_UTILS_Z_ALGORITHM_H
#define C_UTILS_Z_ALGORITHM_H

#include <stddef.h>
#include <stdbool.h>

// Z 算法错误码
typedef enum {
    Z_ALGORITHM_OK = 0,
    Z_ALGORITHM_INVALID_PARAMS = -1,
    Z_ALGORITHM_MEMORY_ERROR = -2,
    Z_ALGORITHM_BUFFER_TOO_SMALL = -3,
    Z_ALGORITHM_STRING_TOO_LONG = -4,
    Z_ALGORITHM_MATCH_NOT_FOUND = -5
} z_algorithm_error_t;

// Z 算法配置选项
typedef struct {
    bool use_optimization;      // 是否使用优化
    bool check_bounds;          // 是否检查边界
    bool allow_empty_string;    // 是否允许空字符串
    size_t max_string_length;   // 最大字符串长度
    size_t max_pattern_length;  // 最大模式长度
} z_algorithm_config_t;

// Z 算法上下文
typedef struct {
    z_algorithm_config_t config;
    z_algorithm_error_t last_error;
    size_t compute_count;
    size_t match_count;
} z_algorithm_ctx_t;

// 创建 Z 算法上下文
z_algorithm_error_t z_algorithm_create(z_algorithm_ctx_t** ctx, const z_algorithm_config_t* config);

// 销毁 Z 算法上下文
void z_algorithm_destroy(z_algorithm_ctx_t* ctx);

// 计算 Z 数组
// 返回 Z_ALGORITHM_OK 表示成功，其他值表示错误
z_algorithm_error_t z_algorithm_compute_safe(z_algorithm_ctx_t* ctx, const char *s, size_t s_len, int *z, size_t z_size);

// 使用 Z 算法进行字符串匹配
// 返回 Z_ALGORITHM_OK 表示成功，Z_ALGORITHM_MATCH_NOT_FOUND 表示未找到匹配
// matches 数组将存储匹配位置，match_count 是匹配数量
z_algorithm_error_t z_algorithm_match(z_algorithm_ctx_t* ctx, const char *text, size_t text_len, const char *pattern, size_t pattern_len, size_t *matches, size_t max_matches, size_t *match_count);

// 批量计算 Z 数组
// 返回 Z_ALGORITHM_OK 表示成功，其他值表示错误
z_algorithm_error_t z_algorithm_compute_batch(z_algorithm_ctx_t* ctx, const char** strings, size_t* lengths, size_t count, int** z_arrays, size_t* z_sizes);

// 计算最长公共前缀
// 返回最长公共前缀的长度
size_t z_algorithm_lcp(const char *s1, const char *s2);

// 查找所有模式匹配
// 返回匹配的数量
z_algorithm_error_t z_algorithm_find_all_matches(z_algorithm_ctx_t* ctx, const char *text, size_t text_len, const char *pattern, size_t pattern_len, size_t** matches, size_t* match_count);

// 传统计算 Z 数组函数（向后兼容）
void z_algorithm_compute(const char *s, int *z);

// 获取最后一次错误
z_algorithm_error_t z_algorithm_get_last_error(z_algorithm_ctx_t* ctx);

// 获取错误信息
const char* z_algorithm_strerror(z_algorithm_error_t error);

#endif // C_UTILS_Z_ALGORITHM_H
