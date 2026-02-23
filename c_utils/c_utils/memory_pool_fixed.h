#ifndef C_UTILS_MEMORY_POOL_FIXED_H
#define C_UTILS_MEMORY_POOL_FIXED_H

#include <stddef.h>
#include <stdbool.h>

/**
 * @brief 内存池错误码
 */
typedef enum {
    MEM_POOL_FIXED_OK = 0,              /**< 成功 */
    MEM_POOL_FIXED_ERROR_NULL_PTR,      /**< 空指针错误 */
    MEM_POOL_FIXED_ERROR_INVALID_SIZE,  /**< 无效大小 */
    MEM_POOL_FIXED_ERROR_OUT_OF_MEMORY, /**< 内存不足 */
    MEM_POOL_FIXED_ERROR_INVALID_BLOCK, /**< 无效块 */
    MEM_POOL_FIXED_ERROR_NOT_INITIALIZED, /**< 未初始化 */
    MEM_POOL_FIXED_ERROR_MAX            /**< 最大错误码 */
} mem_pool_fixed_error_t;

/**
 * @brief 内存池配置
 */
typedef struct {
    bool thread_safe;            /**< 是否线程安全 */
    bool zero_initialize;        /**< 是否零初始化内存 */
    size_t alignment;            /**< 内存对齐要求 (0 表示默认对齐) */
} mem_pool_fixed_config_t;

/**
 * @brief 内存池状态
 */
typedef struct {
    size_t total_blocks;         /**< 总块数 */
    size_t used_blocks;          /**< 已使用块数 */
    size_t free_blocks;          /**< 空闲块数 */
    size_t block_size;           /**< 块大小 */
    size_t total_memory;         /**< 总内存大小 */
} mem_pool_fixed_stats_t;

/**
 * @brief 固定大小内存池结构
 */
typedef struct {
    void *pool;                  /**< 内存池基地址 */
    void *free_list;             /**< 空闲块链表 */
    size_t block_size;           /**< 块大小 */
    size_t num_blocks;           /**< 总块数 */
    size_t used_blocks;          /**< 已使用块数 */
    mem_pool_fixed_config_t config; /**< 配置 */
#if defined(_WIN32)
    void *mutex;                 /**< 线程同步互斥量 */
#else
    void *mutex;                 /**< 线程同步互斥量 */
#endif
} mem_pool_fixed_t;

/**
 * @brief 获取默认配置
 * @return 默认配置
 */
mem_pool_fixed_config_t mem_pool_fixed_default_config(void);

/**
 * @brief 初始化内存池
 * @param mp 内存池指针
 * @param block_size 块大小
 * @param num_blocks 块数量
 * @return 错误码
 */
mem_pool_fixed_error_t mem_pool_fixed_init(mem_pool_fixed_t *mp, size_t block_size, size_t num_blocks);

/**
 * @brief 使用自定义配置初始化内存池
 * @param mp 内存池指针
 * @param block_size 块大小
 * @param num_blocks 块数量
 * @param config 配置选项
 * @return 错误码
 */
mem_pool_fixed_error_t mem_pool_fixed_init_ex(mem_pool_fixed_t *mp, size_t block_size, size_t num_blocks, const mem_pool_fixed_config_t *config);

/**
 * @brief 从内存池分配内存
 * @param mp 内存池指针
 * @return 分配的内存指针，失败返回 NULL
 */
void* mem_pool_fixed_alloc(mem_pool_fixed_t *mp);

/**
 * @brief 从内存池分配内存（带错误码）
 * @param mp 内存池指针
 * @param error 错误码输出
 * @return 分配的内存指针，失败返回 NULL
 */
void* mem_pool_fixed_alloc_ex(mem_pool_fixed_t *mp, mem_pool_fixed_error_t *error);

/**
 * @brief 释放内存到内存池
 * @param mp 内存池指针
 * @param ptr 要释放的内存指针
 */
void mem_pool_fixed_free(mem_pool_fixed_t *mp, void *ptr);

/**
 * @brief 释放内存到内存池（带错误码）
 * @param mp 内存池指针
 * @param ptr 要释放的内存指针
 * @return 错误码
 */
mem_pool_fixed_error_t mem_pool_fixed_free_ex(mem_pool_fixed_t *mp, void *ptr);

/**
 * @brief 销毁内存池
 * @param mp 内存池指针
 */
void mem_pool_fixed_destroy(mem_pool_fixed_t *mp);

/**
 * @brief 销毁内存池（带错误码）
 * @param mp 内存池指针
 * @return 错误码
 */
mem_pool_fixed_error_t mem_pool_fixed_destroy_ex(mem_pool_fixed_t *mp);

/**
 * @brief 获取内存池状态
 * @param mp 内存池指针
 * @param stats 状态输出
 * @return 错误码
 */
mem_pool_fixed_error_t mem_pool_fixed_get_stats(const mem_pool_fixed_t *mp, mem_pool_fixed_stats_t *stats);

/**
 * @brief 检查内存指针是否属于内存池
 * @param mp 内存池指针
 * @param ptr 内存指针
 * @return 是否属于内存池
 */
bool mem_pool_fixed_contains(const mem_pool_fixed_t *mp, const void *ptr);

/**
 * @brief 重置内存池（释放所有分配的内存）
 * @param mp 内存池指针
 * @return 错误码
 */
mem_pool_fixed_error_t mem_pool_fixed_reset(mem_pool_fixed_t *mp);

/**
 * @brief 获取错误信息
 * @param error 错误码
 * @return 错误信息字符串
 */
const char* mem_pool_fixed_error_string(mem_pool_fixed_error_t error);

#endif // C_UTILS_MEMORY_POOL_FIXED_H
