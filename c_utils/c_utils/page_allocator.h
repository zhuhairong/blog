#ifndef C_UTILS_PAGE_ALLOCATOR_H
#define C_UTILS_PAGE_ALLOCATOR_H

#include <stddef.h>
#include <stdbool.h>

/**
 * @brief 页分配器错误码
 */
typedef enum {
    PAGE_ALLOC_OK = 0,                 /**< 成功 */
    PAGE_ALLOC_ERROR_NULL_PTR,         /**< 空指针错误 */
    PAGE_ALLOC_ERROR_INVALID_SIZE,     /**< 无效大小 */
    PAGE_ALLOC_ERROR_ALLOCATION_FAILED, /**< 分配失败 */
    PAGE_ALLOC_ERROR_INVALID_ALIGNMENT, /**< 无效对齐要求 */
    PAGE_ALLOC_ERROR_MAX               /**< 最大错误码 */
} page_alloc_error_t;

/**
 * @brief 页分配器配置
 */
typedef struct {
    size_t alignment;                  /**< 对齐要求 (0 表示系统页大小) */
    bool zero_initialize;              /**< 是否零初始化内存 */
    size_t min_size;                   /**< 最小分配大小 */
    size_t max_size;                   /**< 最大分配大小 (0 表示无限制) */
} page_alloc_config_t;

/**
 * @brief 页分配器统计信息
 */
typedef struct {
    size_t total_allocated;            /**< 总分配内存大小 */
    size_t current_allocated;          /**< 当前分配内存大小 */
    size_t allocation_count;           /**< 分配次数 */
    size_t free_count;                 /**< 释放次数 */
    size_t page_size;                  /**< 系统页大小 */
} page_alloc_stats_t;

/**
 * @brief 获取默认配置
 * @return 默认配置
 */
page_alloc_config_t page_alloc_default_config(void);

/**
 * @brief 获取系统页大小
 * @return 系统页大小
 */
size_t page_alloc_get_page_size(void);

/**
 * @brief 页对齐分配内存
 * @param size 分配大小
 * @return 分配的内存指针，失败返回 NULL
 */
void* page_alloc(size_t size);

/**
 * @brief 使用自定义配置页对齐分配内存
 * @param size 分配大小
 * @param config 配置选项
 * @param error 错误码输出
 * @return 分配的内存指针，失败返回 NULL
 */
void* page_alloc_ex(size_t size, const page_alloc_config_t *config, page_alloc_error_t *error);

/**
 * @brief 释放页对齐分配的内存
 * @param ptr 内存指针
 */
void page_free(void *ptr);

/**
 * @brief 释放页对齐分配的内存（带错误码）
 * @param ptr 内存指针
 * @param error 错误码输出
 * @return 是否成功
 */
bool page_free_ex(void *ptr, page_alloc_error_t *error);

/**
 * @brief 分配多个页对齐内存块
 * @param count 块数量
 * @param size 每个块大小
 * @param ptrs 指针数组输出
 * @param error 错误码输出
 * @return 是否成功
 */
bool page_alloc_array(size_t count, size_t size, void **ptrs, page_alloc_error_t *error);

/**
 * @brief 释放多个页对齐内存块
 * @param count 块数量
 * @param ptrs 指针数组
 * @param error 错误码输出
 * @return 是否成功
 */
bool page_free_array(size_t count, void **ptrs, page_alloc_error_t *error);

/**
 * @brief 重新分配页对齐内存
 * @param ptr 旧内存指针
 * @param old_size 旧大小
 * @param new_size 新大小
 * @param error 错误码输出
 * @return 新内存指针，失败返回 NULL
 */
void* page_realloc(void *ptr, size_t old_size, size_t new_size, page_alloc_error_t *error);

/**
 * @brief 检查内存是否页对齐
 * @param ptr 内存指针
 * @param alignment 对齐要求 (0 表示系统页大小)
 * @return 是否页对齐
 */
bool page_is_aligned(const void *ptr, size_t alignment);

/**
 * @brief 获取页分配器统计信息
 * @param stats 统计信息输出
 * @param error 错误码输出
 * @return 是否成功
 */
bool page_alloc_get_stats(page_alloc_stats_t *stats, page_alloc_error_t *error);

/**
 * @brief 重置页分配器统计信息
 * @param error 错误码输出
 * @return 是否成功
 */
bool page_alloc_reset_stats(page_alloc_error_t *error);

/**
 * @brief 获取错误信息
 * @param error 错误码
 * @return 错误信息字符串
 */
const char* page_alloc_error_string(page_alloc_error_t error);

#endif // C_UTILS_PAGE_ALLOCATOR_H
