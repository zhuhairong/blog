#ifndef C_UTILS_KV_STORE_H
#define C_UTILS_KV_STORE_H

#include <stdbool.h>
#include <stddef.h>

// KV 存储错误码
typedef enum {
    KV_OK = 0,
    KV_INVALID_INPUT = -1,
    KV_FILE_ERROR = -2,
    KV_MEMORY_ERROR = -3,
    KV_KEY_NOT_FOUND = -4,
    KV_PARSE_ERROR = -5,
    KV_WRITE_ERROR = -6,
    KV_READ_ERROR = -7,
    KV_BUFFER_TOO_SMALL = -8
} kv_error_t;

// KV 存储配置
typedef struct {
    bool enable_compression;
    bool enable_encryption;
    bool enable_backup;
    size_t max_key_length;
    size_t max_value_length;
    size_t max_entries;
} kv_config_t;

// KV 存储条目
typedef struct {
    char *key;
    char *value;
} kv_entry_t;

// 设置值并保存到文件
bool kv_save(const char *filename, const char *key, const char *value);

// 从文件读取值 (调用者需 free 返回值)
char* kv_load(const char *filename, const char *key);

// 带错误处理的保存
kv_error_t kv_save_ex(const char *filename, const char *key, const char *value, size_t value_len);

// 带错误处理的加载
kv_error_t kv_load_ex(const char *filename, const char *key, char *buffer, size_t buffer_size, size_t *value_len);

// 删除键值对
kv_error_t kv_delete(const char *filename, const char *key);

// 检查键是否存在
bool kv_exists(const char *filename, const char *key);

// 获取所有键值对 (调用者需 free 返回的数组和其中的字符串)
size_t kv_get_all(const char *filename, kv_entry_t **entries, kv_error_t *error);

// 批量保存键值对
kv_error_t kv_save_batch(const char *filename, const kv_entry_t *entries, size_t count);

// 清空所有键值对
kv_error_t kv_clear(const char *filename);

// 获取存储统计信息
kv_error_t kv_get_stats(const char *filename, size_t *entry_count, size_t *file_size);

// 带配置的初始化
kv_error_t kv_init_with_config(const char *filename, const kv_config_t *config);

// 获取默认配置
void kv_get_default_config(kv_config_t *config);

// 释放键值对数组
void kv_free_entries(kv_entry_t *entries, size_t count);

#endif // C_UTILS_KV_STORE_H
