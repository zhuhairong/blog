#ifndef C_UTILS_LRU_CACHE_H
#define C_UTILS_LRU_CACHE_H

#include <stddef.h>
#include <stdbool.h>

// LRU 缓存错误码
typedef enum {
    LRU_CACHE_OK = 0,
    LRU_CACHE_INVALID_INPUT = -1,
    LRU_CACHE_MEMORY_ERROR = -2,
    LRU_CACHE_CAPACITY_ERROR = -3,
    LRU_CACHE_KEY_NOT_FOUND = -4,
    LRU_CACHE_FULL = -5,
    LRU_CACHE_KEY_TOO_LONG = -6,
    LRU_CACHE_VALUE_ERROR = -7
} lru_cache_error_t;

// LRU 缓存配置
typedef struct {
    size_t capacity;
    size_t max_key_length;
    bool thread_safe;
    bool enable_stats;
    bool copy_keys;
    bool copy_values;
    void (*value_free)(void*);
} lru_cache_config_t;

// LRU 缓存统计信息
typedef struct {
    size_t hits;
    size_t misses;
    size_t puts;
    size_t evictions;
    size_t current_size;
    size_t capacity;
    double hit_rate;
} lru_cache_stats_t;

typedef struct lru_cache_s lru_cache_t;

// 创建 LRU 缓存
lru_cache_t* lru_cache_create(size_t capacity);
void         lru_cache_free(lru_cache_t *cache);

// 存取
void  lru_cache_put(lru_cache_t *cache, const char *key, void *value);
void* lru_cache_get(lru_cache_t *cache, const char *key);

// 带错误处理的创建
lru_cache_t* lru_cache_create_ex(const lru_cache_config_t *config, lru_cache_error_t *error);

// 带错误处理的存取
lru_cache_error_t lru_cache_put_ex(lru_cache_t *cache, const char *key, void *value, size_t value_size);
lru_cache_error_t lru_cache_get_ex(lru_cache_t *cache, const char *key, void **value);

// 删除
lru_cache_error_t lru_cache_remove(lru_cache_t *cache, const char *key);

// 检查键是否存在
bool lru_cache_contains(lru_cache_t *cache, const char *key);

// 清空缓存
lru_cache_error_t lru_cache_clear(lru_cache_t *cache);

// 获取缓存大小
size_t lru_cache_size(const lru_cache_t *cache);

// 获取缓存容量
size_t lru_cache_capacity(const lru_cache_t *cache);

// 设置缓存容量
lru_cache_error_t lru_cache_set_capacity(lru_cache_t *cache, size_t capacity);

// 获取统计信息
lru_cache_error_t lru_cache_get_stats(const lru_cache_t *cache, lru_cache_stats_t *stats);

// 重置统计信息
lru_cache_error_t lru_cache_reset_stats(lru_cache_t *cache);

// 获取默认配置
void lru_cache_get_default_config(lru_cache_config_t *config);

#endif // C_UTILS_LRU_CACHE_H
