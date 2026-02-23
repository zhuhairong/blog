#ifndef C_UTILS_HEAP_H
#define C_UTILS_HEAP_H

#include <stddef.h>
#include <stdbool.h>

// 堆错误码
typedef enum {
    HEAP_OK = 0,
    HEAP_ERROR_INVALID_PARAM,
    HEAP_ERROR_MEMORY_ALLOC,
    HEAP_ERROR_EMPTY_HEAP,
    HEAP_ERROR_FULL_HEAP,
    HEAP_ERROR_ITEM_NOT_FOUND
} heap_error_t;

// 堆类型
typedef enum {
    HEAP_TYPE_MIN,
    HEAP_TYPE_MAX,
    HEAP_TYPE_CUSTOM
} heap_type_t;

// 堆
typedef struct heap_s heap_t;

// 比较函数类型
typedef int (*heap_compar_t)(const void *, const void *);

// 堆配置
typedef struct {
    heap_type_t type;
    heap_compar_t compar;
    size_t item_size;
    size_t capacity;
    bool auto_resize;
} heap_config_t;

// 默认堆配置
// type: 堆类型
// item_size: 元素大小
// compar: 自定义比较函数（仅当 type 为 HEAP_TYPE_CUSTOM 时有效）
// 返回: 默认配置
heap_config_t heap_default_config(heap_type_t type, size_t item_size, heap_compar_t compar);

// 创建堆
// config: 配置
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回堆指针，失败返回 NULL
heap_t* heap_create(const heap_config_t *config, heap_error_t *error);

// 释放堆
// h: 堆
void heap_free(heap_t *h);

// 属性
size_t heap_size(const heap_t *h);
bool   heap_is_empty(const heap_t *h);
bool   heap_is_full(const heap_t *h);
size_t heap_capacity(const heap_t *h);

// 操作
bool   heap_push(heap_t *h, const void *item, heap_error_t *error);
bool   heap_pop(heap_t *h, void *out_item, heap_error_t *error);
void*  heap_peek(const heap_t *h, heap_error_t *error);
bool   heap_remove(heap_t *h, const void *item, heap_error_t *error);
bool   heap_contains(const heap_t *h, const void *item, heap_error_t *error);
bool   heap_clear(heap_t *h, heap_error_t *error);

// 批量操作
bool   heap_push_batch(heap_t *h, const void *items, size_t count, heap_error_t *error);
bool   heap_pop_batch(heap_t *h, void *out_items, size_t count, size_t *popped, heap_error_t *error);

// 堆排序
// items: 元素数组
// count: 元素数量
// item_size: 元素大小
// config: 堆配置
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool   heap_sort(void *items, size_t count, size_t item_size, const heap_config_t *config, heap_error_t *error);

// 调整堆容量
// h: 堆
// new_capacity: 新容量
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool   heap_reserve(heap_t *h, size_t new_capacity, heap_error_t *error);

// 检查堆是否有错误
// h: 堆
// error: 错误码（输出参数，可为 NULL）
// error_msg: 错误信息（输出参数，可为 NULL）
// 返回: 有错误返回 true，无错误返回 false
bool   heap_has_error(const heap_t *h, heap_error_t *error, const char **error_msg);

// 获取错误信息
// error: 错误码
// 返回: 错误信息字符串
const char* heap_strerror(heap_error_t error);

#endif // C_UTILS_HEAP_H
