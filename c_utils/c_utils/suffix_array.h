#ifndef C_UTILS_SUFFIX_ARRAY_H
#define C_UTILS_SUFFIX_ARRAY_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    SUFFIX_ARRAY_OK = 0,
    SUFFIX_ARRAY_ERROR_INVALID_PARAMS = -1,
    SUFFIX_ARRAY_ERROR_BUILD = -2,
    SUFFIX_ARRAY_ERROR_QUERY = -3,
    SUFFIX_ARRAY_ERROR_FREE = -4,
    SUFFIX_ARRAY_ERROR_MEMORY = -5,
    SUFFIX_ARRAY_ERROR_STRING_EMPTY = -6,
    SUFFIX_ARRAY_ERROR_STRING_TOO_LONG = -7,
    SUFFIX_ARRAY_ERROR_INDEX_OUT_OF_BOUNDS = -8,
    SUFFIX_ARRAY_ERROR_LCP = -9,
    SUFFIX_ARRAY_ERROR_PATTERN_EMPTY = -10,
    SUFFIX_ARRAY_ERROR_FILE_OPEN = -11,
    SUFFIX_ARRAY_ERROR_FILE_READ = -12,
    SUFFIX_ARRAY_ERROR_FILE_WRITE = -13,
    SUFFIX_ARRAY_ERROR_ALGORITHM = -14
} suffix_array_error_t;

typedef enum {
    SUFFIX_ARRAY_ALGORITHM_DOUBLING = 0,  // 倍增法
    SUFFIX_ARRAY_ALGORITHM_SAIS = 1,      // SA-IS算法
    SUFFIX_ARRAY_ALGORITHM_RADIX = 2,     // 基数排序法
    SUFFIX_ARRAY_ALGORITHM_NAIVE = 3      // 朴素算法
} suffix_array_algorithm_t;

typedef struct {
    suffix_array_algorithm_t algorithm;    // 构建算法
    bool enable_lcp;                // 启用LCP数组
    bool enable_rank;               // 启用rank数组
    bool enable_statistics;         // 启用统计
    bool use_optimizations;         // 使用优化
    size_t max_string_length;       // 最大字符串长度
    size_t buffer_size;             // 缓冲区大小
    bool ignore_case;               // 忽略大小写
} suffix_array_config_t;

typedef struct {
    suffix_array_error_t last_error;     // 最后一次错误
    size_t builds;                       // 构建次数
    size_t queries;                      // 查询次数
    bool is_initialized;                 // 是否初始化
    size_t average_string_length;        // 平均字符串长度
    size_t build_time;                   // 构建时间（毫秒）
} suffix_array_state_t;

/**
 * @brief 后缀数组结构体
 */
typedef struct {
    int *sa;               // 后缀数组
    int *rank;             // rank数组
    int *lcp;              // LCP数组
    size_t n;              // 字符串长度
    const char *str;       // 原始字符串
    suffix_array_config_t config;  // 配置
    suffix_array_state_t state;    // 状态
} suffix_array_t;

/**
 * @brief 极简后缀数组构建 (倍增法简化版)
 * @param s 输入字符串
 * @param sa 输出后缀数组
 */
void suffix_array_build(const char *s, int *sa);

/**
 * @brief 增强版后缀数组构建
 * @param s 输入字符串
 * @param sa 输出后缀数组
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
suffix_array_error_t suffix_array_build_ex(const char *s, int *sa,
                                         const suffix_array_config_t *config,
                                         suffix_array_state_t *state);

/**
 * @brief 创建后缀数组
 * @param s 输入字符串
 * @param config 配置选项
 * @param sa 输出后缀数组结构体
 * @return 错误码
 */
suffix_array_error_t suffix_array_create(const char *s,
                                       const suffix_array_config_t *config,
                                       suffix_array_t *sa);

/**
 * @brief 释放后缀数组
 * @param sa 后缀数组结构体
 * @return 错误码
 */
suffix_array_error_t suffix_array_free(suffix_array_t *sa);

/**
 * @brief 构建LCP数组
 * @param s 输入字符串
 * @param sa 后缀数组
 * @param rank rank数组
 * @param lcp 输出LCP数组
 * @param n 字符串长度
 * @param state 状态输出
 * @return 错误码
 */
suffix_array_error_t suffix_array_build_lcp(const char *s,
                                           const int *sa,
                                           const int *rank,
                                           int *lcp,
                                           size_t n,
                                           suffix_array_state_t *state);

/**
 * @brief 构建rank数组
 * @param sa 后缀数组
 * @param rank 输出rank数组
 * @param n 字符串长度
 * @param state 状态输出
 * @return 错误码
 */
suffix_array_error_t suffix_array_build_rank(const int *sa,
                                            int *rank,
                                            size_t n,
                                            suffix_array_state_t *state);

/**
 * @brief 二分查找模式
 * @param s 输入字符串
 * @param sa 后缀数组
 * @param n 字符串长度
 * @param pattern 模式字符串
 * @param start 输出起始位置
 * @param end 输出结束位置
 * @param state 状态输出
 * @return 错误码
 */
suffix_array_error_t suffix_array_search(const char *s,
                                        const int *sa,
                                        size_t n,
                                        const char *pattern,
                                        size_t *start,
                                        size_t *end,
                                        suffix_array_state_t *state);

/**
 * @brief 获取最长公共前缀
 * @param s 输入字符串
 * @param sa 后缀数组
 * @param lcp LCP数组
 * @param n 字符串长度
 * @param i 第一个后缀索引
 * @param j 第二个后缀索引
 * @param lcp_length 输出LCP长度
 * @param state 状态输出
 * @return 错误码
 */
suffix_array_error_t suffix_array_get_lcp(const char *s,
                                         const int *sa,
                                         const int *lcp,
                                         size_t n,
                                         size_t i,
                                         size_t j,
                                         size_t *lcp_length,
                                         suffix_array_state_t *state);

/**
 * @brief 查找最长重复子串
 * @param s 输入字符串
 * @param sa 后缀数组
 * @param lcp LCP数组
 * @param n 字符串长度
 * @param start 输出起始位置
 * @param length 输出长度
 * @param state 状态输出
 * @return 错误码
 */
suffix_array_error_t suffix_array_find_longest_repeated_substring(const char *s,
                                                                 const int *sa,
                                                                 const int *lcp,
                                                                 size_t n,
                                                                 size_t *start,
                                                                 size_t *length,
                                                                 suffix_array_state_t *state);

/**
 * @brief 查找最长公共子串
 * @param s1 第一个字符串
 * @param s2 第二个字符串
 * @param config 配置选项
 * @param start1 输出第一个字符串中的起始位置
 * @param start2 输出第二个字符串中的起始位置
 * @param length 输出长度
 * @param state 状态输出
 * @return 错误码
 */
suffix_array_error_t suffix_array_find_longest_common_substring(const char *s1,
                                                              const char *s2,
                                                              const suffix_array_config_t *config,
                                                              size_t *start1,
                                                              size_t *start2,
                                                              size_t *length,
                                                              suffix_array_state_t *state);

/**
 * @brief 从文件构建后缀数组
 * @param filename 文件名
 * @param config 配置选项
 * @param sa 输出后缀数组结构体
 * @param state 状态输出
 * @return 错误码
 */
suffix_array_error_t suffix_array_build_from_file(const char *filename,
                                                const suffix_array_config_t *config,
                                                suffix_array_t *sa,
                                                suffix_array_state_t *state);

/**
 * @brief 保存后缀数组到文件
 * @param sa 后缀数组结构体
 * @param filename 文件名
 * @param state 状态输出
 * @return 错误码
 */
suffix_array_error_t suffix_array_save_to_file(const suffix_array_t *sa,
                                             const char *filename,
                                             suffix_array_state_t *state);

/**
 * @brief 验证后缀数组
 * @param s 输入字符串
 * @param sa 后缀数组
 * @param n 字符串长度
 * @param valid 输出验证结果
 * @param state 状态输出
 * @return 错误码
 */
suffix_array_error_t suffix_array_validate(const char *s,
                                          const int *sa,
                                          size_t n,
                                          bool *valid,
                                          suffix_array_state_t *state);

/**
 * @brief 获取后缀数组信息
 * @param sa 后缀数组结构体
 * @param size 输出大小
 * @param has_lcp 输出是否有LCP数组
 * @param has_rank 输出是否有rank数组
 * @return 错误码
 */
suffix_array_error_t suffix_array_get_info(const suffix_array_t *sa,
                                          size_t *size,
                                          bool *has_lcp,
                                          bool *has_rank);

/**
 * @brief 获取后缀数组统计信息
 * @param state 状态结构
 * @return 错误码
 */
suffix_array_error_t suffix_array_get_statistics(suffix_array_state_t *state);

/**
 * @brief 重置后缀数组状态
 * @param state 状态结构
 */
void suffix_array_reset_state(suffix_array_state_t *state);

/**
 * @brief 获取最后一次错误信息
 * @param state 状态
 * @return 错误描述字符串
 */
const char* suffix_array_strerror(const suffix_array_state_t *state);

/**
 * @brief 初始化配置为默认值
 * @param config 配置结构
 */
void suffix_array_config_init(suffix_array_config_t *config);

/**
 * @brief 初始化状态为默认值
 * @param state 状态结构
 */
void suffix_array_state_init(suffix_array_state_t *state);

#endif // C_UTILS_SUFFIX_ARRAY_H
