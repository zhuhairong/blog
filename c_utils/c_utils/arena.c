#include "arena.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct arena_chunk_s {
    size_t size;
    size_t used;
    struct arena_chunk_s *next;
    uint8_t data[];
} arena_chunk_t;

struct arena_s {
    arena_chunk_t *current;
    size_t chunk_size;
    size_t chunk_count;
    size_t total_allocated;
};

arena_t* arena_create(size_t initial_size) {
    if (initial_size == 0) initial_size = 4096;
    
    arena_t *a = malloc(sizeof(arena_t));
    if (!a) return NULL;
    
    a->chunk_size = initial_size > 1024 ? initial_size : 4096;
    a->chunk_count = 1;
    a->total_allocated = 0;
    a->current = malloc(sizeof(arena_chunk_t) + a->chunk_size);
    if (!a->current) {
        free(a);
        return NULL;
    }
    a->current->size = a->chunk_size;
    a->current->used = 0;
    a->current->next = NULL;
    return a;
}

arena_t* arena_create_default(void) {
    return arena_create(4096);
}

void arena_destroy(arena_t *a) {
    if (!a) return;
    arena_chunk_t *curr = a->current;
    while (curr) {
        arena_chunk_t *next = curr->next;
        free(curr);
        curr = next;
    }
    free(a);
}

void* arena_alloc(arena_t *a, size_t size) {
    if (!a || size == 0) return NULL;
    
    // 8 字节对齐
    size = (size + 7) & ~7;

    if (a->current->used + size > a->current->size) {
        size_t next_size = size > a->chunk_size ? size : a->chunk_size;
        arena_chunk_t *new_chunk = malloc(sizeof(arena_chunk_t) + next_size);
        if (!new_chunk) return NULL;
        new_chunk->size = next_size;
        new_chunk->used = 0;
        new_chunk->next = a->current;
        a->current = new_chunk;
        a->chunk_count++;
    }

    void *ptr = a->current->data + a->current->used;
    a->current->used += size;
    a->total_allocated += size;
    return ptr;
}

void* arena_alloc_zeroed(arena_t *a, size_t size) {
    void *ptr = arena_alloc(a, size);
    if (ptr) memset(ptr, 0, size);
    return ptr;
}

void* arena_realloc(arena_t *a, void *ptr, size_t old_size, size_t new_size) {
    if (!a) return NULL;
    if (!ptr) return arena_alloc(a, new_size);
    if (new_size == 0) return NULL;
    
    // 检查是否是当前 chunk 的最后一个分配
    old_size = (old_size + 7) & ~7;
    new_size = (new_size + 7) & ~7;
    
    if (ptr == a->current->data + a->current->used - old_size && 
        a->current->used - old_size + new_size <= a->current->size) {
        // 可以原地扩展/收缩
        a->current->used = a->current->used - old_size + new_size;
        a->total_allocated = a->total_allocated - old_size + new_size;
        return ptr;
    }
    
    // 否则分配新内存并复制
    void *new_ptr = arena_alloc(a, new_size);
    if (new_ptr && ptr) {
        size_t copy_size = old_size < new_size ? old_size : new_size;
        memcpy(new_ptr, ptr, copy_size);
    }
    return new_ptr;
}

void arena_reset(arena_t *a) {
    if (!a) return;
    
    arena_chunk_t *curr = a->current;
    while (curr->next) {
        arena_chunk_t *next = curr->next;
        free(curr);
        curr = next;
        a->chunk_count--;
    }
    a->current = curr;
    a->current->used = 0;
    a->total_allocated = 0;
}

void arena_reset_keep_first(arena_t *a) {
    if (!a) return;
    
    // 只保留第一个 chunk
    arena_chunk_t *first = a->current;
    while (first->next) {
        arena_chunk_t *next = first->next;
        first = next;
    }
    
    // 释放其他 chunks
    arena_chunk_t *curr = a->current;
    while (curr != first) {
        arena_chunk_t *next = curr->next;
        free(curr);
        curr = next;
        a->chunk_count--;
    }
    
    a->current = first;
    a->current->used = 0;
    a->total_allocated = 0;
}

bool arena_contains(const arena_t *a, const void *ptr) {
    if (!a || !ptr) return false;
    
    arena_chunk_t *curr = a->current;
    while (curr) {
        if (ptr >= (void*)curr->data && ptr < (void*)(curr->data + curr->size)) {
            return true;
        }
        curr = curr->next;
    }
    return false;
}

size_t arena_total_allocated(const arena_t *a) {
    return a ? a->total_allocated : 0;
}

size_t arena_chunk_count(const arena_t *a) {
    return a ? a->chunk_count : 0;
}
