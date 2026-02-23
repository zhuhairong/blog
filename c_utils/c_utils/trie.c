#include "trie.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// 创建新节点
static trie_node_t* trie_node_create(void) {
    trie_node_t *node = calloc(1, sizeof(trie_node_t));
    return node;
}

// 递归释放节点
static void trie_node_free(trie_node_t *node, void (*value_free)(void*)) {
    if (!node) return;
    
    for (int i = 0; i < 256; i++) {
        if (node->children[i]) {
            trie_node_free(node->children[i], value_free);
        }
    }
    
    if (value_free && node->value) {
        value_free(node->value);
    }
    
    free(node);
}

// 默认配置
static trie_config_t trie_default_config(void) {
    trie_config_t config = {
        .case_sensitive = true,
        .allow_duplicates = false,
        .max_depth = 0,  // 0表示无限制
        .max_children = 0,  // 0表示无限制
        .value_free = NULL
    };
    return config;
}

// 创建 Trie
trie_t* trie_create(void) {
    return trie_create_with_config(NULL);
}

// 使用配置创建 Trie
trie_t* trie_create_with_config(const trie_config_t *config) {
    trie_t *t = malloc(sizeof(trie_t));
    if (!t) return NULL;
    
    t->root = trie_node_create();
    if (!t->root) {
        free(t);
        return NULL;
    }
    
    t->size = 0;
    t->config = config ? *config : trie_default_config();
    t->last_error = TRIE_OK;
    t->memory_usage = sizeof(trie_t) + sizeof(trie_node_t);
    
    return t;
}

// 销毁 Trie
void trie_free(trie_t *t) {
    if (!t) return;
    
    trie_node_free(t->root, t->config.value_free);
    free(t);
}

// 字符转换（处理大小写不敏感）
static unsigned char trie_char_convert(trie_t *t, unsigned char c) {
    if (!t->config.case_sensitive) {
        return (unsigned char)tolower(c);
    }
    return c;
}

// 插入键值对
trie_error_t trie_insert(trie_t *t, const char *key, void *value) {
    if (!t || !key) {
        if (t) t->last_error = TRIE_INVALID_PARAMS;
        return TRIE_INVALID_PARAMS;
    }
    
    if (*key == '\0') {
        t->last_error = TRIE_EMPTY_KEY;
        return TRIE_EMPTY_KEY;
    }
    
    trie_node_t *current = t->root;
    const char *p = key;
    size_t depth = 0;
    
    while (*p) {
        if (t->config.max_depth > 0 && depth >= t->config.max_depth) {
            t->last_error = TRIE_INVALID_PARAMS;
            return TRIE_INVALID_PARAMS;
        }
        
        unsigned char c = trie_char_convert(t, (unsigned char)*p);
        
        if (!current->children[c]) {
            // 检查子节点数限制
            if (t->config.max_children > 0 && current->child_count >= t->config.max_children) {
                t->last_error = TRIE_INVALID_PARAMS;
                return TRIE_INVALID_PARAMS;
            }
            
            current->children[c] = trie_node_create();
            if (!current->children[c]) {
                t->last_error = TRIE_MEMORY_ERROR;
                return TRIE_MEMORY_ERROR;
            }
            current->child_count++;
            t->memory_usage += sizeof(trie_node_t);
        }
        
        current = current->children[c];
        p++;
        depth++;
    }
    
    // 检查是否允许重复
    if (current->is_end && !t->config.allow_duplicates) {
        t->last_error = TRIE_DUPLICATE_KEY;
        return TRIE_DUPLICATE_KEY;
    }
    
    // 释放旧值
    if (current->is_end && t->config.value_free && current->value) {
        t->config.value_free(current->value);
    }
    
    current->is_end = true;
    current->value = value;
    
    if (!current->is_end || t->config.allow_duplicates) {
        t->size++;
    }
    
    t->last_error = TRIE_OK;
    return TRIE_OK;
}

// 查找节点
static trie_node_t* trie_find_node(trie_t *t, const char *key) {
    if (!t || !key || !t->root) return NULL;
    
    trie_node_t *current = t->root;
    const char *p = key;
    
    while (*p && current) {
        unsigned char c = trie_char_convert(t, (unsigned char)*p);
        current = current->children[c];
        p++;
    }
    
    return current;
}

// 获取键对应的值
void* trie_get(trie_t *t, const char *key) {
    if (!t || !key) {
        if (t) t->last_error = TRIE_INVALID_PARAMS;
        return NULL;
    }
    
    trie_node_t *node = trie_find_node(t, key);
    
    if (!node || !node->is_end) {
        t->last_error = TRIE_KEY_NOT_FOUND;
        return NULL;
    }
    
    t->last_error = TRIE_OK;
    return node->value;
}

// 检查键是否存在
bool trie_contains(trie_t *t, const char *key) {
    if (!t || !key) {
        if (t) t->last_error = TRIE_INVALID_PARAMS;
        return false;
    }
    
    trie_node_t *node = trie_find_node(t, key);
    bool exists = (node && node->is_end);
    t->last_error = TRIE_OK;
    return exists;
}

// 递归删除节点
static bool trie_node_remove(trie_node_t *node, const char *key, void (*value_free)(void*), size_t *freed_count) {
    if (!node || !key) return false;
    
    if (*key == '\0') {
        if (!node->is_end) return false;
        
        // 释放值
        if (value_free && node->value) {
            value_free(node->value);
        }
        
        node->is_end = false;
        node->value = NULL;
        (*freed_count)++;
        
        // 检查是否可以删除此节点
        for (int i = 0; i < 256; i++) {
            if (node->children[i]) return false;
        }
        
        return true;
    }
    
    unsigned char c = (unsigned char)*key;
    trie_node_t *child = node->children[c];
    
    if (!child) return false;
    
    bool can_delete_child = trie_node_remove(child, key + 1, value_free, freed_count);
    
    if (can_delete_child) {
        free(child);
        node->children[c] = NULL;
        node->child_count--;
        
        // 检查当前节点是否可以删除
        if (!node->is_end) {
            for (int i = 0; i < 256; i++) {
                if (node->children[i]) return false;
            }
            return true;
        }
    }
    
    return false;
}

// 删除键
bool trie_remove(trie_t *t, const char *key) {
    if (!t || !key || !t->root) {
        if (t) t->last_error = TRIE_INVALID_PARAMS;
        return false;
    }
    
    size_t freed_count = 0;
    bool result = trie_node_remove(t->root, key, t->config.value_free, &freed_count);
    
    if (result) {
        t->size -= freed_count;
        t->last_error = TRIE_OK;
    } else {
        t->last_error = TRIE_KEY_NOT_FOUND;
    }
    
    return result;
}

// 递归收集前缀匹配的键
static void trie_collect_prefix(trie_node_t *node, char *prefix, size_t prefix_len, 
                                 char **keys, void **values, size_t *count, size_t max_results) {
    if (!node || *count >= max_results) return;
    
    if (node->is_end) {
        keys[*count] = strdup(prefix);
        values[*count] = node->value;
        (*count)++;
    }
    
    for (int i = 0; i < 256 && *count < max_results; i++) {
        if (node->children[i]) {
            prefix[prefix_len] = (char)i;
            prefix[prefix_len + 1] = '\0';
            trie_collect_prefix(node->children[i], prefix, prefix_len + 1, 
                               keys, values, count, max_results);
        }
    }
}

// 获取前缀匹配的所有键值对
size_t trie_prefix_search(trie_t *t, const char *prefix, char **keys, void **values, size_t max_results) {
    if (!t || !prefix || !keys || !values || max_results == 0) {
        if (t) t->last_error = TRIE_INVALID_PARAMS;
        return 0;
    }
    
    trie_node_t *node = trie_find_node(t, prefix);
    if (!node) {
        t->last_error = TRIE_OK;
        return 0;
    }
    
    size_t count = 0;
    char buffer[1024];
    strncpy(buffer, prefix, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';
    
    trie_collect_prefix(node, buffer, strlen(prefix), keys, values, &count, max_results);
    
    t->last_error = TRIE_OK;
    return count;
}

// 递归遍历
static void trie_node_traverse(trie_node_t *node, char *prefix, size_t prefix_len,
                                trie_traverse_cb cb, void *user_data, bool *stop) {
    if (!node || *stop) return;
    
    if (node->is_end) {
        prefix[prefix_len] = '\0';
        if (!cb(prefix, node->value, user_data)) {
            *stop = true;
            return;
        }
    }
    
    for (int i = 0; i < 256 && !*stop; i++) {
        if (node->children[i]) {
            prefix[prefix_len] = (char)i;
            trie_node_traverse(node->children[i], prefix, prefix_len + 1, cb, user_data, stop);
        }
    }
}

// 遍历 Trie
void trie_traverse(trie_t *t, trie_traverse_cb cb, void *user_data) {
    if (!t || !cb || !t->root) {
        if (t) t->last_error = TRIE_INVALID_PARAMS;
        return;
    }
    
    char prefix[1024];
    bool stop = false;
    trie_node_traverse(t->root, prefix, 0, cb, user_data, &stop);
    t->last_error = TRIE_OK;
}

// 获取 Trie 大小
size_t trie_size(trie_t *t) {
    if (!t) return 0;
    return t->size;
}

// 获取内存使用情况
size_t trie_memory_usage(trie_t *t) {
    if (!t) return 0;
    return t->memory_usage;
}

// 获取最后一次错误
trie_error_t trie_get_last_error(trie_t *t) {
    if (!t) return TRIE_INVALID_PARAMS;
    return t->last_error;
}

// 获取错误信息
const char* trie_strerror(trie_error_t error) {
    switch (error) {
        case TRIE_OK: return "Success";
        case TRIE_INVALID_PARAMS: return "Invalid parameters";
        case TRIE_MEMORY_ERROR: return "Memory allocation failed";
        case TRIE_KEY_NOT_FOUND: return "Key not found";
        case TRIE_EMPTY_KEY: return "Empty key";
        case TRIE_DUPLICATE_KEY: return "Duplicate key";
        default: return "Unknown error";
    }
}

// 递归清空节点
static void trie_node_clear(trie_node_t *node, void (*value_free)(void*)) {
    if (!node) return;
    
    for (int i = 0; i < 256; i++) {
        if (node->children[i]) {
            trie_node_clear(node->children[i], value_free);
            free(node->children[i]);
            node->children[i] = NULL;
        }
    }
    
    if (value_free && node->value) {
        value_free(node->value);
    }
    
    node->is_end = false;
    node->value = NULL;
    node->child_count = 0;
}

// 清空 Trie
void trie_clear(trie_t *t) {
    if (!t || !t->root) return;
    
    trie_node_clear(t->root, t->config.value_free);
    t->size = 0;
    t->memory_usage = sizeof(trie_t) + sizeof(trie_node_t);
    t->last_error = TRIE_OK;
}
