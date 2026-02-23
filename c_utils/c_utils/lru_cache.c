#include "lru_cache.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct lru_node_s {
    char *key;
    void *value;
    struct lru_node_s *prev, *next;
} lru_node_t;

struct lru_cache_s {
    size_t capacity;
    size_t size;
    size_t max_key_length;
    bool enable_stats;
    bool copy_values;
    void (*value_free)(void*);
    lru_node_t *head, *tail;
    // 统计信息
    size_t hits;
    size_t misses;
    size_t puts;
    size_t evictions;
};

static void move_to_head(lru_cache_t *cache, lru_node_t *node) {
    if (node == cache->head) return;
    if (node->prev) node->prev->next = node->next;
    if (node->next) node->next->prev = node->prev;
    if (node == cache->tail) cache->tail = node->prev;
    
    node->next = cache->head;
    node->prev = NULL;
    if (cache->head) cache->head->prev = node;
    cache->head = node;
    if (!cache->tail) cache->tail = node;
}

static lru_node_t *find_node(lru_cache_t *cache, const char *key) {
    lru_node_t *curr = cache->head;
    while (curr) {
        if (strcmp(curr->key, key) == 0) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

static void evict_node(lru_cache_t *cache) {
    if (!cache->tail) return;

    lru_node_t *old_tail = cache->tail;
    if (old_tail->prev) old_tail->prev->next = NULL;
    cache->tail = old_tail->prev;
    if (cache->head == old_tail) cache->head = NULL;

    if (cache->value_free && old_tail->value) {
        cache->value_free(old_tail->value);
    }
    free(old_tail->key);
    free(old_tail);
    cache->size--;
    cache->evictions++;
}

lru_cache_t* lru_cache_create(size_t capacity) {
    lru_cache_config_t config;
    lru_cache_get_default_config(&config);
    config.capacity = capacity;
    lru_cache_error_t error;
    return lru_cache_create_ex(&config, &error);
}

lru_cache_t* lru_cache_create_ex(const lru_cache_config_t *config, lru_cache_error_t *error) {
    if (!config || config->capacity == 0) {
        if (error) *error = LRU_CACHE_INVALID_INPUT;
        return NULL;
    }

    lru_cache_t *cache = calloc(1, sizeof(lru_cache_t));
    if (!cache) {
        if (error) *error = LRU_CACHE_MEMORY_ERROR;
        return NULL;
    }

    cache->capacity = config->capacity;
    cache->max_key_length = config->max_key_length > 0 ? config->max_key_length : 256;
    cache->enable_stats = config->enable_stats;
    cache->copy_values = config->copy_values;
    cache->value_free = config->value_free;

    if (error) *error = LRU_CACHE_OK;
    return cache;
}

void lru_cache_free(lru_cache_t *cache) {
    if (!cache) return;

    lru_node_t *curr = cache->head;
    while (curr) {
        lru_node_t *next = curr->next;
        if (cache->value_free && curr->value) {
            cache->value_free(curr->value);
        }
        free(curr->key);
        free(curr);
        curr = next;
    }
    free(cache);
}

void lru_cache_put(lru_cache_t *cache, const char *key, void *value) {
    lru_cache_put_ex(cache, key, value, 0);
}

lru_cache_error_t lru_cache_put_ex(lru_cache_t *cache, const char *key, void *value, size_t value_size) {
    if (!cache || !key) {
        return LRU_CACHE_INVALID_INPUT;
    }

    size_t key_len = strlen(key);
    if (key_len > cache->max_key_length) {
        return LRU_CACHE_KEY_TOO_LONG;
    }

    // 查找是否已存在
    lru_node_t *node = find_node(cache, key);
    if (node) {
        // 更新值
        if (cache->value_free && node->value) {
            cache->value_free(node->value);
        }
        if (cache->copy_values && value_size > 0) {
            node->value = malloc(value_size);
            if (!node->value) {
                return LRU_CACHE_MEMORY_ERROR;
            }
            memcpy(node->value, value, value_size);
        } else {
            node->value = value;
        }
        move_to_head(cache, node);
        cache->puts++;
        return LRU_CACHE_OK;
    }

    // 检查容量
    if (cache->size >= cache->capacity) {
        evict_node(cache);
    }

    // 创建新节点
    node = malloc(sizeof(lru_node_t));
    if (!node) {
        return LRU_CACHE_MEMORY_ERROR;
    }

    node->key = strdup(key);
    if (!node->key) {
        free(node);
        return LRU_CACHE_MEMORY_ERROR;
    }

    if (cache->copy_values && value_size > 0) {
        node->value = malloc(value_size);
        if (!node->value) {
            free(node->key);
            free(node);
            return LRU_CACHE_MEMORY_ERROR;
        }
        memcpy(node->value, value, value_size);
    } else {
        node->value = value;
    }

    node->prev = NULL;
    node->next = cache->head;
    if (cache->head) cache->head->prev = node;
    cache->head = node;
    if (!cache->tail) cache->tail = node;
    cache->size++;
    cache->puts++;

    return LRU_CACHE_OK;
}

void* lru_cache_get(lru_cache_t *cache, const char *key) {
    if (!cache || !key) {
        return NULL;
    }

    void *value;
    lru_cache_error_t error = lru_cache_get_ex(cache, key, &value);
    return (error == LRU_CACHE_OK) ? value : NULL;
}

lru_cache_error_t lru_cache_get_ex(lru_cache_t *cache, const char *key, void **value) {
    if (!cache || !key || !value) {
        return LRU_CACHE_INVALID_INPUT;
    }

    lru_node_t *node = find_node(cache, key);
    if (node) {
        move_to_head(cache, node);
        *value = node->value;
        cache->hits++;
        return LRU_CACHE_OK;
    }

    cache->misses++;
    return LRU_CACHE_KEY_NOT_FOUND;
}

lru_cache_error_t lru_cache_remove(lru_cache_t *cache, const char *key) {
    if (!cache || !key) {
        return LRU_CACHE_INVALID_INPUT;
    }

    lru_node_t *node = find_node(cache, key);
    if (!node) {
        return LRU_CACHE_KEY_NOT_FOUND;
    }

    if (node->prev) node->prev->next = node->next;
    if (node->next) node->next->prev = node->prev;
    if (node == cache->head) cache->head = node->next;
    if (node == cache->tail) cache->tail = node->prev;

    if (cache->value_free && node->value) {
        cache->value_free(node->value);
    }
    free(node->key);
    free(node);
    cache->size--;

    return LRU_CACHE_OK;
}

bool lru_cache_contains(lru_cache_t *cache, const char *key) {
    if (!cache || !key) {
        return false;
    }
    return find_node(cache, key) != NULL;
}

lru_cache_error_t lru_cache_clear(lru_cache_t *cache) {
    if (!cache) {
        return LRU_CACHE_INVALID_INPUT;
    }

    lru_node_t *curr = cache->head;
    while (curr) {
        lru_node_t *next = curr->next;
        if (cache->value_free && curr->value) {
            cache->value_free(curr->value);
        }
        free(curr->key);
        free(curr);
        curr = next;
    }

    cache->head = NULL;
    cache->tail = NULL;
    cache->size = 0;
    cache->evictions = 0;

    return LRU_CACHE_OK;
}

size_t lru_cache_size(const lru_cache_t *cache) {
    return cache ? cache->size : 0;
}

size_t lru_cache_capacity(const lru_cache_t *cache) {
    return cache ? cache->capacity : 0;
}

lru_cache_error_t lru_cache_set_capacity(lru_cache_t *cache, size_t capacity) {
    if (!cache || capacity == 0) {
        return LRU_CACHE_INVALID_INPUT;
    }

    cache->capacity = capacity;

    // 如果容量减小，需要驱逐多余的节点
    while (cache->size > cache->capacity) {
        evict_node(cache);
    }

    return LRU_CACHE_OK;
}

lru_cache_error_t lru_cache_get_stats(const lru_cache_t *cache, lru_cache_stats_t *stats) {
    if (!cache || !stats) {
        return LRU_CACHE_INVALID_INPUT;
    }

    stats->hits = cache->hits;
    stats->misses = cache->misses;
    stats->puts = cache->puts;
    stats->evictions = cache->evictions;
    stats->current_size = cache->size;
    stats->capacity = cache->capacity;

    size_t total = stats->hits + stats->misses;
    stats->hit_rate = total > 0 ? (double)stats->hits / (double)total : 0.0;

    return LRU_CACHE_OK;
}

lru_cache_error_t lru_cache_reset_stats(lru_cache_t *cache) {
    if (!cache) {
        return LRU_CACHE_INVALID_INPUT;
    }

    cache->hits = 0;
    cache->misses = 0;
    cache->puts = 0;
    cache->evictions = 0;

    return LRU_CACHE_OK;
}

void lru_cache_get_default_config(lru_cache_config_t *config) {
    if (config) {
        config->capacity = 100;
        config->max_key_length = 256;
        config->thread_safe = false;
        config->enable_stats = true;
        config->copy_keys = true;
        config->copy_values = false;
        config->value_free = NULL;
    }
}
