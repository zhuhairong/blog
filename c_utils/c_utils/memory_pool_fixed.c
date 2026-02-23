#include "memory_pool_fixed.h"
#include <stdlib.h>
#include <string.h>

mem_pool_fixed_config_t mem_pool_fixed_default_config(void) {
    mem_pool_fixed_config_t config = {
        .thread_safe = false,
        .zero_initialize = false,
        .alignment = 0
    };
    return config;
}

mem_pool_fixed_error_t mem_pool_fixed_init(mem_pool_fixed_t *mp, size_t block_size, size_t num_blocks) {
    if (!mp) return MEM_POOL_FIXED_ERROR_NULL_PTR;
    if (block_size == 0 || num_blocks == 0) return MEM_POOL_FIXED_ERROR_INVALID_SIZE;
    
    mp->block_size = block_size < sizeof(void*) ? sizeof(void*) : block_size;
    mp->num_blocks = num_blocks;
    mp->used_blocks = 0;
    mp->config = mem_pool_fixed_default_config();
    mp->mutex = NULL;
    
    mp->pool = malloc(mp->block_size * num_blocks);
    if (!mp->pool) return MEM_POOL_FIXED_ERROR_OUT_OF_MEMORY;
    
    mp->free_list = mp->pool;
    char *curr = mp->pool;
    for (size_t i = 0; i < num_blocks - 1; i++) {
        *(void**)curr = curr + mp->block_size;
        curr += mp->block_size;
    }
    *(void**)curr = NULL;
    return MEM_POOL_FIXED_OK;
}

mem_pool_fixed_error_t mem_pool_fixed_init_ex(mem_pool_fixed_t *mp, size_t block_size, size_t num_blocks, const mem_pool_fixed_config_t *config) {
    mem_pool_fixed_error_t err = mem_pool_fixed_init(mp, block_size, num_blocks);
    if (err == MEM_POOL_FIXED_OK && config) {
        mp->config = *config;
    }
    return err;
}

void* mem_pool_fixed_alloc(mem_pool_fixed_t *mp) {
    if (!mp || !mp->free_list) return NULL;
    void *ptr = mp->free_list;
    mp->free_list = *(void**)ptr;
    mp->used_blocks++;
    if (mp->config.zero_initialize) {
        memset(ptr, 0, mp->block_size);
    }
    return ptr;
}

void* mem_pool_fixed_alloc_ex(mem_pool_fixed_t *mp, mem_pool_fixed_error_t *error) {
    if (!mp) {
        if (error) *error = MEM_POOL_FIXED_ERROR_NULL_PTR;
        return NULL;
    }
    void *ptr = mem_pool_fixed_alloc(mp);
    if (error) *error = ptr ? MEM_POOL_FIXED_OK : MEM_POOL_FIXED_ERROR_OUT_OF_MEMORY;
    return ptr;
}

void mem_pool_fixed_free(mem_pool_fixed_t *mp, void *ptr) {
    if (!mp || !ptr) return;
    *(void**)ptr = mp->free_list;
    mp->free_list = ptr;
    mp->used_blocks--;
}

void mem_pool_fixed_destroy(mem_pool_fixed_t *mp) {
    if (!mp) return;
    free(mp->pool);
    mp->pool = NULL;
    mp->free_list = NULL;
    mp->num_blocks = 0;
    mp->used_blocks = 0;
}

mem_pool_fixed_error_t mem_pool_fixed_get_stats(const mem_pool_fixed_t *mp, mem_pool_fixed_stats_t *stats) {
    if (!mp || !stats) return MEM_POOL_FIXED_ERROR_NULL_PTR;
    
    stats->total_blocks = mp->num_blocks;
    stats->used_blocks = mp->used_blocks;
    stats->free_blocks = mp->num_blocks - mp->used_blocks;
    stats->block_size = mp->block_size;
    stats->total_memory = mp->block_size * mp->num_blocks;
    return MEM_POOL_FIXED_OK;
}

const char* mem_pool_fixed_strerror(mem_pool_fixed_error_t error) {
    switch (error) {
        case MEM_POOL_FIXED_OK: return "Success";
        case MEM_POOL_FIXED_ERROR_NULL_PTR: return "Null pointer";
        case MEM_POOL_FIXED_ERROR_INVALID_SIZE: return "Invalid size";
        case MEM_POOL_FIXED_ERROR_OUT_OF_MEMORY: return "Out of memory";
        case MEM_POOL_FIXED_ERROR_INVALID_BLOCK: return "Invalid block";
        case MEM_POOL_FIXED_ERROR_NOT_INITIALIZED: return "Not initialized";
        default: return "Unknown error";
    }
}
