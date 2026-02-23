#ifndef C_UTILS_SORT_HEAP_H
#define C_UTILS_SORT_HEAP_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    HEAP_OK = 0,
    HEAP_ERROR_INVALID_PARAMS = -1,
    HEAP_ERROR_CREATE = -2,
    HEAP_ERROR_INSERT = -3,
    HEAP_ERROR_DELETE = -4,
    HEAP_ERROR_POP = -5,
    HEAP_ERROR_PEEK = -6,
    HEAP_ERROR_SORT = -7,
    HEAP_ERROR_MEMORY = -8,
    HEAP_ERROR_EMPTY = -9,
    HEAP_ERROR_FULL = -10,
    HEAP_ERROR_INDEX_OUT_OF_BOUNDS = -11,
    HEAP_ERROR_COMPARATOR_NULL = -12
} heap_error_t;

typedef enum {
    HEAP_TYPE_MAX = 0,  // 最大堆
    HEAP_TYPE_MIN = 1   // 最小堆
} heap_type_t;

typedef struct {
    heap_type_t type;            // 堆类型
    bool use_malloc;             // 使用malloc分配内存
    bool enable_resize;          // 启用自动调整大小
    bool enable_statistics;      // 启用统计
    size_t initial_capacity;     // 初始容量
    size_t max_capacity;         // 最大容量（0表示无限制）
    float resize_factor;         // 调整大小因子
} heap_config_t;

typedef struct {
    heap_error_t last_error;     // 最后一次错误
    size_t size;                 // 当前大小
    size_t capacity;             // 容量
    size_t operations;           // 操作计数
    size_t insertions;           // 插入计数
    size_t deletions;            // 删除计数
    bool is_initialized;         // 是否初始化
    heap_type_t type;            // 堆类型
    uint64_t time_taken;         // 耗时（微秒）
} heap_state_t;

/**
 * @brief 堆结构体
 */
typedef struct {
    void *data;                  // 数据数组
    size_t size;                 // 当前大小
    size_t capacity;             // 容量
    size_t element_size;         // 元素大小
    heap_type_t type;            // 堆类型
    int (*compar)(const void *, const void *); // 比较函数
    heap_state_t *state;         // 状态
} heap_t;

/**
 * @brief 堆排序
 * @param arr 整数数组
 * @param n 数组大小
 */
void sort_heap(int *arr, size_t n);

/**
 * @brief 增强版堆排序
 * @param arr 数组
 * @param n 数组大小
 * @param element_size 元素大小
 * @param type 堆类型
 * @param compar 比较函数
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
heap_error_t sort_heap_ex(void *arr, size_t n, size_t element_size,
                         heap_type_t type,
                         int (*compar)(const void *, const void *),
                         const heap_config_t *config,
                         heap_state_t *state);

/**
 * @brief 创建堆
 * @param capacity 初始容量
 * @param element_size 元素大小
 * @param type 堆类型
 * @param compar 比较函数
 * @param config 配置选项
 * @param state 状态输出
 * @return 堆指针，失败返回NULL
 */
heap_t* heap_create(size_t capacity, size_t element_size,
                   heap_type_t type,
                   int (*compar)(const void *, const void *),
                   const heap_config_t *config,
                   heap_state_t *state);

/**
 * @brief 销毁堆
 * @param heap 堆指针
 * @param state 状态输出
 * @return 错误码
 */
heap_error_t heap_destroy(heap_t *heap, heap_state_t *state);

/**
 * @brief 向堆中插入元素
 * @param heap 堆指针
 * @param element 元素指针
 * @return 错误码
 */
heap_error_t heap_insert(heap_t *heap, const void *element);

/**
 * @brief 从堆中删除元素
 * @param heap 堆指针
 * @param index 元素索引
 * @return 错误码
 */
heap_error_t heap_delete(heap_t *heap, size_t index);

/**
 * @brief 弹出堆顶元素
 * @param heap 堆指针
 * @param element 输出元素指针
 * @return 错误码
 */
heap_error_t heap_pop(heap_t *heap, void *element);

/**
 * @brief 查看堆顶元素
 * @param heap 堆指针
 * @param element 输出元素指针
 * @return 错误码
 */
heap_error_t heap_peek(const heap_t *heap, void *element);

/**
 * @brief 检查堆是否为空
 * @param heap 堆指针
 * @param is_empty 输出结果
 * @return 错误码
 */
heap_error_t heap_is_empty(const heap_t *heap, bool *is_empty);

/**
 * @brief 检查堆是否已满
 * @param heap 堆指针
 * @param is_full 输出结果
 * @return 错误码
 */
heap_error_t heap_is_full(const heap_t *heap, bool *is_full);

/**
 * @brief 获取堆大小
 * @param heap 堆指针
 * @return 堆大小
 */
size_t heap_size(const heap_t *heap);

/**
 * @brief 获取堆容量
 * @param heap 堆指针
 * @return 堆容量
 */
size_t heap_capacity(const heap_t *heap);

/**
 * @brief 调整堆大小
 * @param heap 堆指针
 * @param new_capacity 新容量
 * @return 错误码
 */
heap_error_t heap_resize(heap_t *heap, size_t new_capacity);

/**
 * @brief 构建堆
 * @param arr 数组
 * @param n 数组大小
 * @param element_size 元素大小
 * @param type 堆类型
 * @param compar 比较函数
 * @param state 状态输出
 * @return 错误码
 */
heap_error_t heap_build(void *arr, size_t n, size_t element_size,
                       heap_type_t type,
                       int (*compar)(const void *, const void *),
                       heap_state_t *state);

/**
 * @brief 堆化指定节点
 * @param arr 数组
 * @param n 数组大小
 * @param element_size 元素大小
 * @param index 节点索引
 * @param type 堆类型
 * @param compar 比较函数
 * @param state 状态输出
 * @return 错误码
 */
heap_error_t heap_heapify(void *arr, size_t n, size_t element_size,
                         size_t index,
                         heap_type_t type,
                         int (*compar)(const void *, const void *),
                         heap_state_t *state);

/**
 * @brief 合并两个堆
 * @param heap1 第一个堆
 * @param heap2 第二个堆
 * @param result 结果堆
 * @return 错误码
 */
heap_error_t heap_merge(heap_t *heap1, heap_t *heap2, heap_t *result);

/**
 * @brief 获取堆中的最小值
 * @param heap 堆指针
 * @param element 输出元素指针
 * @return 错误码
 */
heap_error_t heap_get_min(const heap_t *heap, void *element);

/**
 * @brief 获取堆中的最大值
 * @param heap 堆指针
 * @param element 输出元素指针
 * @return 错误码
 */
heap_error_t heap_get_max(const heap_t *heap, void *element);

/**
 * @brief 清空堆
 * @param heap 堆指针
 * @return 错误码
 */
heap_error_t heap_clear(heap_t *heap);

/**
 * @brief 验证堆属性
 * @param heap 堆指针
 * @param is_valid 输出结果
 * @return 错误码
 */
heap_error_t heap_validate(const heap_t *heap, bool *is_valid);

/**
 * @brief 获取堆统计信息
 * @param heap 堆指针
 * @param state 状态输出
 * @return 错误码
 */
heap_error_t heap_get_statistics(const heap_t *heap, heap_state_t *state);

/**
 * @brief 获取最后一次错误信息
 * @param state 状态
 * @return 错误描述字符串
 */
const char* heap_strerror(const heap_state_t *state);

/**
 * @brief 初始化配置为默认值
 * @param config 配置结构
 */
void heap_config_init(heap_config_t *config);

/**
 * @brief 初始化状态为默认值
 * @param state 状态结构
 */
void heap_state_init(heap_state_t *state);

#endif // C_UTILS_SORT_HEAP_H
