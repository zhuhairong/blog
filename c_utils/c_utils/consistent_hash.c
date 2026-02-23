#include "consistent_hash.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// 默认哈希函数
static uint32_t default_hash(const void *data, size_t len) {
    const uint8_t *bytes = (const uint8_t*)data;
    uint32_t h = 0;
    for (size_t i = 0; i < len; i++) {
        h = h * 31 + bytes[i];
    }
    return h;
}

// 字符串哈希函数（用于内部节点哈希计算）
static uint32_t hash_string(const char *key) {
    uint32_t h = 0;
    while (*key) h = h * 31 + (uint8_t)*key++;
    return h;
}

static int compare(const void *a, const void *b) {
    const ch_entry_t *ea = (const ch_entry_t*)a;
    const ch_entry_t *eb = (const ch_entry_t*)b;
    if (ea->hash < eb->hash) return -1;
    if (ea->hash > eb->hash) return 1;
    return 0;
}

// 初始化一致性哈希
ch_error_t ch_init(consistent_hash_t *ch, ch_hash_func_t hash_func) {
    if (!ch) return CH_ERROR_INVALID_NODE;
    
    ch->ring = NULL;
    ch->count = 0;
    ch->capacity = 0;
    ch->hash_func = hash_func ? hash_func : default_hash;
    
    return CH_OK;
}

// 添加节点
ch_error_t ch_add_node(consistent_hash_t *ch, int node_id, int replicas) {
    if (!ch) return CH_ERROR_INVALID_NODE;
    if (replicas <= 0) return CH_ERROR_INVALID_REPLICAS;
    
    size_t new_capacity = ch->capacity + replicas;
    ch_entry_t *new_ring = realloc(ch->ring, new_capacity * sizeof(ch_entry_t));
    if (!new_ring) return CH_ERROR_MEMORY_ALLOC;
    
    ch->ring = new_ring;
    ch->capacity = new_capacity;
    
    for (int i = 0; i < replicas; i++) {
        char buf[64];
        snprintf(buf, sizeof(buf), "node-%d-rep-%d", node_id, i);
        ch->ring[ch->count].hash = hash_string(buf);
        ch->ring[ch->count].node_id = node_id;
        ch->count++;
    }
    
    qsort(ch->ring, ch->count, sizeof(ch_entry_t), compare);
    return CH_OK;
}

// 删除节点
ch_error_t ch_remove_node(consistent_hash_t *ch, int node_id) {
    if (!ch) return CH_ERROR_INVALID_NODE;
    
    size_t new_count = 0;
    for (size_t i = 0; i < ch->count; i++) {
        if (ch->ring[i].node_id != node_id) {
            ch->ring[new_count++] = ch->ring[i];
        }
    }
    
    if (new_count == ch->count) {
        return CH_ERROR_NODE_NOT_FOUND;
    }
    
    ch->count = new_count;
    ch->capacity = new_count;
    
    if (new_count > 0) {
        ch_entry_t *new_ring = realloc(ch->ring, new_count * sizeof(ch_entry_t));
        if (new_ring) {
            ch->ring = new_ring;
        }
    } else {
        free(ch->ring);
        ch->ring = NULL;
    }
    
    return CH_OK;
}

// 获取节点
int ch_get_node(const consistent_hash_t *ch, const void *key, size_t key_len) {
    if (!ch || !key || ch->count == 0) return -1;
    
    uint32_t h = ch->hash_func(key, key_len);
    
    // 二分查找
    size_t left = 0, right = ch->count;
    while (left < right) {
        size_t mid = left + (right - left) / 2;
        if (ch->ring[mid].hash < h) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    
    if (left < ch->count) {
        return ch->ring[left].node_id;
    }
    // 回绕到第一个节点
    return ch->ring[0].node_id;
}

// 获取节点（字符串键）
int ch_get_node_str(const consistent_hash_t *ch, const char *key) {
    if (!key) return -1;
    return ch_get_node(ch, key, strlen(key));
}

// 获取节点数量
size_t ch_get_node_count(const consistent_hash_t *ch) {
    if (!ch) return 0;
    
    // 统计唯一节点数
    size_t count = 0;
    int last_node = -1;
    for (size_t i = 0; i < ch->count; i++) {
        if (ch->ring[i].node_id != last_node) {
            count++;
            last_node = ch->ring[i].node_id;
        }
    }
    return count;
}

// 获取副本数量
size_t ch_get_replica_count(const consistent_hash_t *ch, int node_id) {
    if (!ch) return 0;
    
    size_t count = 0;
    for (size_t i = 0; i < ch->count; i++) {
        if (ch->ring[i].node_id == node_id) {
            count++;
        }
    }
    return count;
}

// 检查节点是否存在
bool ch_node_exists(const consistent_hash_t *ch, int node_id) {
    if (!ch) return false;
    
    for (size_t i = 0; i < ch->count; i++) {
        if (ch->ring[i].node_id == node_id) {
            return true;
        }
    }
    return false;
}

// 清空所有节点
void ch_clear(consistent_hash_t *ch) {
    if (!ch) return;
    
    free(ch->ring);
    ch->ring = NULL;
    ch->count = 0;
    ch->capacity = 0;
}

// 释放一致性哈希
void ch_free(consistent_hash_t *ch) {
    if (!ch) return;
    
    free(ch->ring);
    ch->ring = NULL;
    ch->count = 0;
    ch->capacity = 0;
}

// 获取错误信息
const char* ch_error_string(ch_error_t error) {
    switch (error) {
        case CH_OK: return "Success";
        case CH_ERROR_INVALID_NODE: return "Invalid node";
        case CH_ERROR_INVALID_REPLICAS: return "Invalid replicas";
        case CH_ERROR_INVALID_KEY: return "Invalid key";
        case CH_ERROR_MEMORY_ALLOC: return "Memory allocation failed";
        case CH_ERROR_NODE_NOT_FOUND: return "Node not found";
        default: return "Unknown error";
    }
}
