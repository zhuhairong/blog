#ifndef C_UTILS_HASHMAP_H
#define C_UTILS_HASHMAP_H

#include <stddef.h>
#include <stdbool.h>

typedef struct hashmap_s hashmap_t;

// 创建与销毁
hashmap_t* hashmap_create(void);
void       hashmap_free(hashmap_t *m);

// 存取操作
bool  hashmap_set(hashmap_t *m, const char *key, void *value);
void* hashmap_get(const hashmap_t *m, const char *key);
bool  hashmap_remove(hashmap_t *m, const char *key);

// 属性获取
size_t hashmap_size(const hashmap_t *m);
static inline bool hashmap_is_empty(const hashmap_t *m) {
    return hashmap_size(m) == 0;
}

// 迭代器支持
typedef struct {
    const hashmap_t *m;
    size_t bucket;
    void *entry;
} hashmap_iter_t;

hashmap_iter_t hashmap_iter_begin(const hashmap_t *m);
bool hashmap_iter_valid(const hashmap_iter_t *iter);
void hashmap_iter_next(hashmap_iter_t *iter);

// 清空哈希表
void hashmap_clear(hashmap_t *m);

#endif // C_UTILS_HASHMAP_H
