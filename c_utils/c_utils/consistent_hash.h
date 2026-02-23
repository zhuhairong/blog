#ifndef C_UTILS_CONSISTENT_HASH_H
#define C_UTILS_CONSISTENT_HASH_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// 一致性哈希错误码
typedef enum {
    CH_OK = 0,
    CH_ERROR_INVALID_NODE,
    CH_ERROR_INVALID_REPLICAS,
    CH_ERROR_INVALID_KEY,
    CH_ERROR_MEMORY_ALLOC,
    CH_ERROR_NODE_NOT_FOUND
} ch_error_t;

// 哈希函数类型
typedef uint32_t (*ch_hash_func_t)(const void *data, size_t len);

// 一致性哈希条目
typedef struct {
    uint32_t hash;
    int node_id;
} ch_entry_t;

// 一致性哈希类型
typedef struct {
    ch_entry_t *ring;
    size_t count;
    size_t capacity;
    ch_hash_func_t hash_func;
} consistent_hash_t;

// 初始化一致性哈希
// ch: 一致性哈希结构
// hash_func: 自定义哈希函数（可为 NULL 使用默认哈希函数）
// 返回: 成功返回 CH_OK，失败返回错误码
ch_error_t ch_init(consistent_hash_t *ch, ch_hash_func_t hash_func);

// 添加节点
// ch: 一致性哈希结构
// node_id: 节点 ID
// replicas: 副本数量
// 返回: 成功返回 CH_OK，失败返回错误码
ch_error_t ch_add_node(consistent_hash_t *ch, int node_id, int replicas);

// 删除节点
// ch: 一致性哈希结构
// node_id: 节点 ID
// 返回: 成功返回 CH_OK，失败返回错误码
ch_error_t ch_remove_node(consistent_hash_t *ch, int node_id);

// 获取节点
// ch: 一致性哈希结构
// key: 键
// key_len: 键长度
// 返回: 成功返回节点 ID，失败返回 -1
int ch_get_node(const consistent_hash_t *ch, const void *key, size_t key_len);

// 获取节点（字符串键）
// ch: 一致性哈希结构
// key: 字符串键
// 返回: 成功返回节点 ID，失败返回 -1
int ch_get_node_str(const consistent_hash_t *ch, const char *key);

// 获取节点数量
// ch: 一致性哈希结构
// 返回: 节点数量
size_t ch_get_node_count(const consistent_hash_t *ch);

// 获取副本数量
// ch: 一致性哈希结构
// node_id: 节点 ID
// 返回: 副本数量，节点不存在返回 0
size_t ch_get_replica_count(const consistent_hash_t *ch, int node_id);

// 检查节点是否存在
// ch: 一致性哈希结构
// node_id: 节点 ID
// 返回: 存在返回 true，不存在返回 false
bool ch_node_exists(const consistent_hash_t *ch, int node_id);

// 释放一致性哈希
// ch: 一致性哈希结构
void ch_free(consistent_hash_t *ch);

// 获取错误信息
// error: 错误码
// 返回: 错误信息字符串
const char* ch_strerror(ch_error_t error);

#endif // C_UTILS_CONSISTENT_HASH_H
