#ifndef C_UTILS_TRIE_H
#define C_UTILS_TRIE_H

#include <stdbool.h>
#include <stddef.h>

// Trie 错误码
typedef enum {
    TRIE_OK = 0,
    TRIE_INVALID_PARAMS = -1,
    TRIE_MEMORY_ERROR = -2,
    TRIE_KEY_NOT_FOUND = -3,
    TRIE_EMPTY_KEY = -4,
    TRIE_DUPLICATE_KEY = -5
} trie_error_t;

// Trie 配置选项
typedef struct {
    bool case_sensitive;     // 是否大小写敏感
    bool allow_duplicates;    // 是否允许重复键
    size_t max_depth;         // 最大深度
    size_t max_children;      // 最大子节点数 (0 表示无限制)
    void (*value_free)(void*); // 值释放函数
} trie_config_t;

// Trie 节点
typedef struct trie_node_s {
    struct trie_node_s *children[256];
    bool is_end;
    void *value;
    size_t child_count;
} trie_node_t;

// Trie 结构体
typedef struct {
    trie_node_t *root;
    size_t size;
    trie_config_t config;
    trie_error_t last_error;
    size_t memory_usage;
} trie_t;

// Trie 遍历回调函数类型
typedef bool (*trie_traverse_cb)(const char *key, void *value, void *user_data);

// 创建 Trie
// 返回 NULL 表示内存分配失败
trie_t* trie_create(void);

// 使用配置创建 Trie
// 返回 NULL 表示内存分配失败
trie_t* trie_create_with_config(const trie_config_t *config);

// 销毁 Trie
void trie_free(trie_t *t);

// 插入键值对
// 返回 TRIE_OK 表示成功，其他值表示错误
trie_error_t trie_insert(trie_t *t, const char *key, void *value);

// 获取键对应的值
// 返回 NULL 表示键不存在或发生错误 (使用 trie_get_last_error 检查)
void* trie_get(trie_t *t, const char *key);

// 删除键
// 返回 true 表示成功，false 表示失败
bool trie_remove(trie_t *t, const char *key);

// 检查键是否存在
// 返回 true 表示存在，false 表示不存在
bool trie_contains(trie_t *t, const char *key);

// 获取前缀匹配的所有键值对
// 返回匹配的数量，结果存储在 keys 和 values 中
// keys 和 values 应该由调用者分配足够的空间
size_t trie_prefix_search(trie_t *t, const char *prefix, char **keys, void **values, size_t max_results);

// 遍历 Trie
// 使用回调函数处理每个键值对
// 如果回调返回 false，遍历会提前终止
void trie_traverse(trie_t *t, trie_traverse_cb cb, void *user_data);

// 获取 Trie 大小
size_t trie_size(trie_t *t);

// 获取内存使用情况
size_t trie_memory_usage(trie_t *t);

// 获取最后一次错误
// 返回 TRIE_OK 表示没有错误
trie_error_t trie_get_last_error(trie_t *t);

// 获取错误信息
const char* trie_strerror(trie_error_t error);

// 清空 Trie
void trie_clear(trie_t *t);

#endif // C_UTILS_TRIE_H
