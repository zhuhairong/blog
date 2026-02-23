#ifndef C_UTILS_ARENA_H
#define C_UTILS_ARENA_H

#include <stddef.h>
#include <stdbool.h>

typedef struct arena_s arena_t;

// 创建与销毁
arena_t* arena_create(size_t initial_size);
arena_t* arena_create_default(void);
void     arena_destroy(arena_t *a);

// 内存分配
void*    arena_alloc(arena_t *a, size_t size);
void*    arena_alloc_zeroed(arena_t *a, size_t size);
void*    arena_realloc(arena_t *a, void *ptr, size_t old_size, size_t new_size);

// 状态管理
void     arena_reset(arena_t *a);
void     arena_reset_keep_first(arena_t *a);
bool     arena_contains(const arena_t *a, const void *ptr);

// 属性获取
size_t   arena_total_allocated(const arena_t *a);
size_t   arena_chunk_count(const arena_t *a);

#endif // C_UTILS_ARENA_H
