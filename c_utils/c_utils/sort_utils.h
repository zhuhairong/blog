#ifndef C_UTILS_SORT_UTILS_H
#define C_UTILS_SORT_UTILS_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    SORT_UTILS_OK = 0,
    SORT_UTILS_ERROR_INVALID_PARAMS = -1,
    SORT_UTILS_ERROR_QUICKSORT = -2,
    SORT_UTILS_ERROR_BINARY_SEARCH = -3,
    SORT_UTILS_ERROR_MEMORY = -4,
    SORT_UTILS_ERROR_COMPARATOR_NULL = -5,
    SORT_UTILS_ERROR_SIZE_ZERO = -6,
    SORT_UTILS_ERROR_ELEMENT_SIZE_ZERO = -7,
    SORT_UTILS_ERROR_NOT_FOUND = -8,
    SORT_UTILS_ERROR_INDEX_OUT_OF_BOUNDS = -9,
    SORT_UTILS_ERROR_FILE_OPEN = -10,
    SORT_UTILS_ERROR_FILE_READ = -11,
    SORT_UTILS_ERROR_FILE_WRITE = -12
} sort_utils_error_t;

typedef struct {
    bool enable_statistics;        // 启用统计
    bool enable_optimizations;     // 启用优化
    size_t max_memory;             // 最大内存使用
    size_t buffer_size;            // 缓冲区大小
    size_t search_threshold;       // 搜索阈值
    size_t sort_threshold;         // 排序阈值
} sort_utils_config_t;

typedef struct {
    sort_utils_error_t last_error; // 最后一次错误
    size_t comparisons;            // 比较次数
    size_t swaps;                  // 交换次数
    size_t searches;               // 搜索次数
    size_t sorts;                  // 排序次数
    bool is_initialized;           // 是否初始化
    uint64_t time_taken;           // 耗时（微秒）
} sort_utils_state_t;

/**
 * @brief 快速排序 (通用)
 * @param base 数据基址
 * @param nmemb 元素数量
 * @param size 元素大小
 * @param compar 比较函数
 */
void sort_utils_quicksort(void *base, size_t nmemb, size_t size, 
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
sort_utils_error_t sort_utils_quicksort_ex(void *base, size_t nmemb, size_t size, 
                    int (*compar)(const void *, const void *),
                    const sort_utils_config_t *config,
                    sort_utils_state_t *state);

/**
 * @brief 二分查找 (返回第一个匹配项的索引，未找到返回 -1)
 * @param key 查找键
 * @param base 数据基址
 * @param nmemb 元素数量
 * @param size 元素大小
 * @param compar 比较函数
 * @return 索引，未找到返回 -1
 */
int  sort_binary_search(const void *key, const void *base, size_t nmemb, size_t size,
                        int (*compar)(const void *, const void *));

/**
 * @brief 增强版二分查找
 * @param key 查找键
 * @param base 数据基址
 * @param nmemb 元素数量
 * @param size 元素大小
 * @param compar 比较函数
 * @param index 输出索引
 * @param found 输出是否找到
 * @param state 状态输出
 * @return 错误码
 */
sort_utils_error_t sort_binary_search_ex(const void *key, const void *base, size_t nmemb, size_t size,
                        int (*compar)(const void *, const void *),
                        size_t *index, bool *found,
                        sort_utils_state_t *state);

/**
 * @brief 线性查找
 * @param key 查找键
 * @param base 数据基址
 * @param nmemb 元素数量
 * @param size 元素大小
 * @param compar 比较函数
 * @param index 输出索引
 * @param found 输出是否找到
 * @param state 状态输出
 * @return 错误码
 */
sort_utils_error_t sort_linear_search(const void *key, const void *base, size_t nmemb, size_t size,
                        int (*compar)(const void *, const void *),
                        size_t *index, bool *found,
                        sort_utils_state_t *state);

/**
 * @brief 插值查找
 * @param key 查找键
 * @param base 数据基址
 * @param nmemb 元素数量
 * @param size 元素大小
 * @param compar 比较函数
 * @param index 输出索引
 * @param found 输出是否找到
 * @param state 状态输出
 * @return 错误码
 */
sort_utils_error_t sort_interpolation_search(const void *key, const void *base, size_t nmemb, size_t size,
                        int (*compar)(const void *, const void *),
                        size_t *index, bool *found,
                        sort_utils_state_t *state);

/**
 * @brief 斐波那契查找
 * @param key 查找键
 * @param base 数据基址
 * @param nmemb 元素数量
 * @param size 元素大小
 * @param compar 比较函数
 * @param index 输出索引
 * @param found 输出是否找到
 * @param state 状态输出
 * @return 错误码
 */
sort_utils_error_t sort_fibonacci_search(const void *key, const void *base, size_t nmemb, size_t size,
                        int (*compar)(const void *, const void *),
                        size_t *index, bool *found,
                        sort_utils_state_t *state);

/**
 * @brief 检查数组是否已排序
 * @param base 数据基址
 * @param nmemb 元素数量
 * @param size 元素大小
 * @param compar 比较函数
 * @param is_sorted 输出结果
 * @param state 状态输出
 * @return 错误码
 */
sort_utils_error_t sort_is_sorted(const void *base, size_t nmemb, size_t size,
                        int (*compar)(const void *, const void *),
                        bool *is_sorted,
                        sort_utils_state_t *state);

/**
 * @brief 交换两个元素
 * @param a 第一个元素
 * @param b 第二个元素
 * @param size 元素大小
 * @param state 状态输出
 * @return 错误码
 */
sort_utils_error_t sort_swap(void *a, void *b, size_t size,
                        sort_utils_state_t *state);

/**
 * @brief 复制元素
 * @param dest 目标位置
 * @param src 源位置
 * @param size 元素大小
 * @param count 元素数量
 * @param state 状态输出
 * @return 错误码
 */
sort_utils_error_t sort_copy(void *dest, const void *src, size_t size, size_t count,
                        sort_utils_state_t *state);

/**
 * @brief 填充数组
 * @param base 数据基址
 * @param value 填充值
 * @param size 元素大小
 * @param count 元素数量
 * @param state 状态输出
 * @return 错误码
 */
sort_utils_error_t sort_fill(void *base, const void *value, size_t size, size_t count,
                        sort_utils_state_t *state);

/**
 * @brief 反转数组
 * @param base 数据基址
 * @param nmemb 元素数量
 * @param size 元素大小
 * @param state 状态输出
 * @return 错误码
 */
sort_utils_error_t sort_reverse(void *base, size_t nmemb, size_t size,
                        sort_utils_state_t *state);

/**
 * @brief 旋转数组
 * @param base 数据基址
 * @param nmemb 元素数量
 * @param size 元素大小
 * @param shift 旋转位移
 * @param state 状态输出
 * @return 错误码
 */
sort_utils_error_t sort_rotate(void *base, size_t nmemb, size_t size, size_t shift,
                        sort_utils_state_t *state);

/**
 * @brief 查找最小值
 * @param base 数据基址
 * @param nmemb 元素数量
 * @param size 元素大小
 * @param compar 比较函数
 * @param index 输出索引
 * @param state 状态输出
 * @return 错误码
 */
sort_utils_error_t sort_find_min(const void *base, size_t nmemb, size_t size,
                        int (*compar)(const void *, const void *),
                        size_t *index,
                        sort_utils_state_t *state);

/**
 * @brief 查找最大值
 * @param base 数据基址
 * @param nmemb 元素数量
 * @param size 元素大小
 * @param compar 比较函数
 * @param index 输出索引
 * @param state 状态输出
 * @return 错误码
 */
sort_utils_error_t sort_find_max(const void *base, size_t nmemb, size_t size,
                        int (*compar)(const void *, const void *),
                        size_t *index,
                        sort_utils_state_t *state);

/**
 * @brief 查找第k小元素
 * @param base 数据基址
 * @param nmemb 元素数量
 * @param size 元素大小
 * @param k 第k小
 * @param compar 比较函数
 * @param element 输出元素
 * @param state 状态输出
 * @return 错误码
 */
sort_utils_error_t sort_find_kth_smallest(void *base, size_t nmemb, size_t size, size_t k,
                        int (*compar)(const void *, const void *),
                        void *element,
                        sort_utils_state_t *state);

/**
 * @brief 排序文件中的数据
 * @param input_file 输入文件
 * @param output_file 输出文件
 * @param nmemb 元素数量
 * @param size 元素大小
 * @param compar 比较函数
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
sort_utils_error_t sort_sort_file(const char *input_file, const char *output_file,
                        size_t nmemb, size_t size,
                        int (*compar)(const void *, const void *),
                        const sort_utils_config_t *config,
                        sort_utils_state_t *state);

/**
 * @brief 比较两个元素
 * @param a 第一个元素
 * @param b 第二个元素
 * @param size 元素大小
 * @param compar 比较函数
 * @param result 输出比较结果
 * @param state 状态输出
 * @return 错误码
 */
sort_utils_error_t sort_compare(const void *a, const void *b, size_t size,
                        int (*compar)(const void *, const void *),
                        int *result,
                        sort_utils_state_t *state);

/**
 * @brief 获取排序工具统计信息
 * @param state 状态结构
 * @return 错误码
 */
sort_utils_error_t sort_get_statistics(sort_utils_state_t *state);

/**
 * @brief 重置排序工具状态
 * @param state 状态结构
 */
void sort_reset_state(sort_utils_state_t *state);

/**
 * @brief 获取最后一次错误信息
 * @param state 状态
 * @return 错误描述字符串
 */
const char* sort_utils_strerror(const sort_utils_state_t *state);

/**
 * @brief 初始化配置为默认值
 * @param config 配置结构
 */
void sort_utils_config_init(sort_utils_config_t *config);

/**
 * @brief 初始化状态为默认值
 * @param state 状态结构
 */
void sort_utils_state_init(sort_utils_state_t *state);

#endif // C_UTILS_SORT_UTILS_H
