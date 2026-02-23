#ifndef C_UTILS_SORT_MERGE_H
#define C_UTILS_SORT_MERGE_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    MERGE_OK = 0,
    MERGE_ERROR_INVALID_PARAMS = -1,
    MERGE_ERROR_CREATE = -2,
    MERGE_ERROR_SORT = -3,
    MERGE_ERROR_MERGE = -4,
    MERGE_ERROR_MEMORY = -5,
    MERGE_ERROR_SIZE_ZERO = -6,
    MERGE_ERROR_ELEMENT_SIZE_ZERO = -7,
    MERGE_ERROR_COMPARATOR_NULL = -8,
    MERGE_ERROR_FILE_OPEN = -9,
    MERGE_ERROR_FILE_READ = -10,
    MERGE_ERROR_FILE_WRITE = -11,
    MERGE_ERROR_TIMEOUT = -12
} merge_error_t;

typedef struct {
    bool stable;                   // 稳定排序
    bool in_place;                 // 原地排序
    bool parallel;                 // 并行排序
    bool enable_statistics;        // 启用统计
    size_t threshold_small;        // 小数据集阈值
    size_t threshold_parallel;     // 并行排序阈值
    size_t max_memory;             // 最大内存使用
    size_t buffer_size;            // 缓冲区大小
} merge_config_t;

typedef struct {
    merge_error_t last_error;      // 最后一次错误
    size_t comparisons;            // 比较次数
    size_t swaps;                  // 交换次数
    size_t memory_used;            // 内存使用
    uint64_t time_taken;           // 耗时（微秒）
    bool is_sorted;                // 是否已排序
    size_t elements_sorted;        // 排序元素数
    size_t merges_performed;       // 执行的合并次数
} merge_state_t;

/**
 * @brief 归并排序
 * @param arr 整数数组
 * @param n 数组大小
 */
void sort_merge(int *arr, size_t n);

/**
 * @brief 增强版归并排序
 * @param arr 数组
 * @param n 数组大小
 * @param element_size 元素大小
 * @param compar 比较函数
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
merge_error_t sort_merge_ex(void *arr, size_t n, size_t element_size,
                          int (*compar)(const void *, const void *),
                          const merge_config_t *config,
                          merge_state_t *state);

/**
 * @brief 并行归并排序
 * @param arr 数组
 * @param n 数组大小
 * @param element_size 元素大小
 * @param compar 比较函数
 * @param num_threads 线程数
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
merge_error_t sort_merge_parallel(void *arr, size_t n, size_t element_size,
                                 int (*compar)(const void *, const void *),
                                 size_t num_threads,
                                 const merge_config_t *config,
                                 merge_state_t *state);

/**
 * @brief 原地归并排序
 * @param arr 数组
 * @param n 数组大小
 * @param element_size 元素大小
 * @param compar 比较函数
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
merge_error_t sort_merge_inplace(void *arr, size_t n, size_t element_size,
                                int (*compar)(const void *, const void *),
                                const merge_config_t *config,
                                merge_state_t *state);

/**
 * @brief 合并两个已排序的数组
 * @param arr1 第一个数组
 * @param n1 第一个数组大小
 * @param arr2 第二个数组
 * @param n2 第二个数组大小
 * @param result 结果数组
 * @param element_size 元素大小
 * @param compar 比较函数
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
merge_error_t merge_arrays(const void *arr1, size_t n1,
                          const void *arr2, size_t n2,
                          void *result, size_t element_size,
                          int (*compar)(const void *, const void *),
                          const merge_config_t *config,
                          merge_state_t *state);

/**
 * @brief 合并排序的内部合并操作
 * @param arr 数组
 * @param left 左边界
 * @param mid 中间边界
 * @param right 右边界
 * @param element_size 元素大小
 * @param compar 比较函数
 * @param temp 临时缓冲区
 * @param temp_size 临时缓冲区大小
 * @param state 状态输出
 * @return 错误码
 */
merge_error_t merge_operation(void *arr, size_t left, size_t mid, size_t right,
                             size_t element_size,
                             int (*compar)(const void *, const void *),
                             void *temp, size_t temp_size,
                             merge_state_t *state);

/**
 * @brief 排序文件中的数据（使用归并排序）
 * @param input_file 输入文件
 * @param output_file 输出文件
 * @param element_size 元素大小
 * @param compar 比较函数
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
merge_error_t merge_sort_file(const char *input_file, const char *output_file,
                             size_t element_size,
                             int (*compar)(const void *, const void *),
                             const merge_config_t *config,
                             merge_state_t *state);

/**
 * @brief 检查数组是否已排序
 * @param arr 数组
 * @param n 数组大小
 * @param element_size 元素大小
 * @param compar 比较函数
 * @param is_sorted 输出结果
 * @return 错误码
 */
merge_error_t merge_is_sorted(const void *arr, size_t n, size_t element_size,
                             int (*compar)(const void *, const void *),
                             bool *is_sorted);

/**
 * @brief 获取归并排序统计信息
 * @param state 状态结构
 * @return 错误码
 */
merge_error_t merge_get_statistics(merge_state_t *state);

/**
 * @brief 重置归并排序状态
 * @param state 状态结构
 */
void merge_reset_state(merge_state_t *state);

/**
 * @brief 获取最后一次错误信息
 * @param state 状态
 * @return 错误描述字符串
 */
const char* merge_strerror(const merge_state_t *state);

/**
 * @brief 初始化配置为默认值
 * @param config 配置结构
 */
void merge_config_init(merge_config_t *config);

/**
 * @brief 初始化状态为默认值
 * @param state 状态结构
 */
void merge_state_init(merge_state_t *state);

#endif // C_UTILS_SORT_MERGE_H
