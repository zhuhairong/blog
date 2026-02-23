#ifndef C_UTILS_SORT_H
#define C_UTILS_SORT_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    SORT_OK = 0,
    SORT_ERROR_INVALID_PARAMS = -1,
    SORT_ERROR_COMPARATOR_NULL = -2,
    SORT_ERROR_MEMORY = -3,
    SORT_ERROR_SIZE_ZERO = -4,
    SORT_ERROR_ELEMENT_SIZE_ZERO = -5,
    SORT_ERROR_ALGORITHM_NOT_SUPPORTED = -6,
    SORT_ERROR_FILE_OPEN = -7,
    SORT_ERROR_FILE_READ = -8,
    SORT_ERROR_FILE_WRITE = -9,
    SORT_ERROR_TIMEOUT = -10
} sort_error_t;

typedef enum {
    SORT_ALGORITHM_QUICK = 0,
    SORT_ALGORITHM_MERGE = 1,
    SORT_ALGORITHM_HEAP = 2,
    SORT_ALGORITHM_INSERTION = 3,
    SORT_ALGORITHM_BUBBLE = 4,
    SORT_ALGORITHM_SELECTION = 5,
    SORT_ALGORITHM_SHELL = 6,
    SORT_ALGORITHM_COUNTING = 7,
    SORT_ALGORITHM_BUCKET = 8,
    SORT_ALGORITHM_RADIX = 9,
    SORT_ALGORITHM_AUTO = 10
} sort_algorithm_t;

typedef struct {
    sort_algorithm_t algorithm;    // 排序算法
    bool stable;                   // 稳定排序
    bool parallel;                 // 并行排序
    bool in_place;                 // 原地排序
    size_t max_memory;             // 最大内存使用
    size_t threshold_small;        // 小数据集阈值
    size_t threshold_parallel;     // 并行排序阈值
    uint32_t random_seed;          // 随机种子
} sort_config_t;

typedef struct {
    sort_error_t last_error;       // 最后一次错误
    size_t comparisons;            // 比较次数
    size_t swaps;                  // 交换次数
    size_t memory_used;            // 内存使用
    uint64_t time_taken;           // 耗时（微秒）
    bool is_sorted;                // 是否已排序
    size_t elements_sorted;        // 排序元素数
    sort_algorithm_t algorithm_used; // 实际使用的算法
} sort_state_t;

/**
 * @brief 通用快速排序
 * @param base 数据基址
 * @param nmemb 元素数量
 * @param size 元素大小
 * @param compar 比较函数
 */
void sort_quicksort(void *base, size_t nmemb, size_t size,
                   int (*compar)(const void *, const void *));

/**
 * @brief 增强版快速排序
 * @param base 数据基址
 * @param nmemb 元素数量
 * @param size 元素大小
 * @param compar 比较函数
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
sort_error_t sort_quicksort_ex(void *base, size_t nmemb, size_t size,
                              int (*compar)(const void *, const void *),
                              const sort_config_t *config, sort_state_t *state);

/**
 * @brief 归并排序
 * @param base 数据基址
 * @param nmemb 元素数量
 * @param size 元素大小
 * @param compar 比较函数
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
sort_error_t sort_mergesort(void *base, size_t nmemb, size_t size,
                           int (*compar)(const void *, const void *),
                           const sort_config_t *config, sort_state_t *state);

/**
 * @brief 堆排序
 * @param base 数据基址
 * @param nmemb 元素数量
 * @param size 元素大小
 * @param compar 比较函数
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
sort_error_t sort_heapsort(void *base, size_t nmemb, size_t size,
                          int (*compar)(const void *, const void *),
                          const sort_config_t *config, sort_state_t *state);

/**
 * @brief 插入排序
 * @param base 数据基址
 * @param nmemb 元素数量
 * @param size 元素大小
 * @param compar 比较函数
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
sort_error_t sort_insertion(void *base, size_t nmemb, size_t size,
                           int (*compar)(const void *, const void *),
                           const sort_config_t *config, sort_state_t *state);

/**
 * @brief 希尔排序
 * @param base 数据基址
 * @param nmemb 元素数量
 * @param size 元素大小
 * @param compar 比较函数
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
sort_error_t sort_shell(void *base, size_t nmemb, size_t size,
                       int (*compar)(const void *, const void *),
                       const sort_config_t *config, sort_state_t *state);

/**
 * @brief 计数排序（适用于整数）
 * @param base 数据基址
 * @param nmemb 元素数量
 * @param min_val 最小值
 * @param max_val 最大值
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
sort_error_t sort_counting(int *base, size_t nmemb, int min_val, int max_val,
                          const sort_config_t *config, sort_state_t *state);

/**
 * @brief 基数排序（适用于整数）
 * @param base 数据基址
 * @param nmemb 元素数量
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
sort_error_t sort_radix(int *base, size_t nmemb,
                       const sort_config_t *config, sort_state_t *state);

/**
 * @brief 自动选择排序算法
 * @param base 数据基址
 * @param nmemb 元素数量
 * @param size 元素大小
 * @param compar 比较函数
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
sort_error_t sort_auto(void *base, size_t nmemb, size_t size,
                      int (*compar)(const void *, const void *),
                      const sort_config_t *config, sort_state_t *state);

/**
 * @brief 检查数组是否已排序
 * @param base 数据基址
 * @param nmemb 元素数量
 * @param size 元素大小
 * @param compar 比较函数
 * @param is_sorted 输出结果
 * @return 错误码
 */
sort_error_t sort_is_sorted(void *base, size_t nmemb, size_t size,
                          int (*compar)(const void *, const void *),
                          bool *is_sorted);

/**
 * @brief 部分排序（只排序前k个元素）
 * @param base 数据基址
 * @param nmemb 元素数量
 * @param size 元素大小
 * @param k 要排序的元素数
 * @param compar 比较函数
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
sort_error_t sort_partial(void *base, size_t nmemb, size_t size, size_t k,
                         int (*compar)(const void *, const void *),
                         const sort_config_t *config, sort_state_t *state);

/**
 * @brief 并行排序
 * @param base 数据基址
 * @param nmemb 元素数量
 * @param size 元素大小
 * @param compar 比较函数
 * @param num_threads 线程数
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
sort_error_t sort_parallel(void *base, size_t nmemb, size_t size,
                          int (*compar)(const void *, const void *),
                          size_t num_threads,
                          const sort_config_t *config, sort_state_t *state);

/**
 * @brief 排序文件中的数据
 * @param input_file 输入文件
 * @param output_file 输出文件
 * @param element_size 元素大小
 * @param compar 比较函数
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
sort_error_t sort_file(const char *input_file, const char *output_file,
                      size_t element_size,
                      int (*compar)(const void *, const void *),
                      const sort_config_t *config, sort_state_t *state);

/**
 * @brief 获取排序统计信息
 * @param state 状态结构
 * @return 错误码
 */
sort_error_t sort_get_statistics(sort_state_t *state);

/**
 * @brief 重置排序状态
 * @param state 状态结构
 */
void sort_reset_state(sort_state_t *state);

/**
 * @brief 获取最后一次错误信息
 * @param state 状态
 * @return 错误描述字符串
 */
const char* sort_strerror(const sort_state_t *state);

/**
 * @brief 初始化配置为默认值
 * @param config 配置结构
 */
void sort_config_init(sort_config_t *config);

/**
 * @brief 初始化状态为默认值
 * @param state 状态结构
 */
void sort_state_init(sort_state_t *state);

#endif // C_UTILS_SORT_H
