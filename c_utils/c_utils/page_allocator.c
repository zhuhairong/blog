#include "page_allocator.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

static page_alloc_stats_t g_stats = {0};
static bool g_stats_initialized = false;

page_alloc_config_t page_alloc_default_config(void) {
    page_alloc_config_t config = {
        .alignment = 0,
        .zero_initialize = true,
        .min_size = 1,
        .max_size = 0
    };
    return config;
}

size_t page_alloc_get_page_size(void) {
    static size_t page_size = 0;
    if (page_size == 0) {
        page_size = (size_t)sysconf(_SC_PAGESIZE);
        if (page_size == 0) page_size = 4096;
    }
    return page_size;
}

void* page_alloc(size_t size) {
    return page_alloc_ex(size, NULL, NULL);
}

void* page_alloc_ex(size_t size, const page_alloc_config_t *config, page_alloc_error_t *error) {
    if (size == 0) {
        if (error) *error = PAGE_ALLOC_ERROR_INVALID_SIZE;
        return NULL;
    }
    
    page_alloc_config_t cfg = config ? *config : page_alloc_default_config();
    
    if (cfg.alignment == 0) {
        cfg.alignment = page_alloc_get_page_size();
    }
    
    if (cfg.min_size > 0 && size < cfg.min_size) {
        size = cfg.min_size;
    }
    
    if (cfg.max_size > 0 && size > cfg.max_size) {
        if (error) *error = PAGE_ALLOC_ERROR_INVALID_SIZE;
        return NULL;
    }
    
    size_t page_size = page_alloc_get_page_size();
    size_t aligned_size = ((size + page_size - 1) / page_size) * page_size;
    
    void *ptr = NULL;
    if (posix_memalign(&ptr, cfg.alignment, aligned_size) != 0) {
        if (error) *error = PAGE_ALLOC_ERROR_ALLOCATION_FAILED;
        return NULL;
    }
    
    if (cfg.zero_initialize) {
        memset(ptr, 0, aligned_size);
    }
    
    if (!g_stats_initialized) {
        memset(&g_stats, 0, sizeof(g_stats));
        g_stats.page_size = page_size;
        g_stats_initialized = true;
    }
    
    g_stats.total_allocated += aligned_size;
    g_stats.current_allocated += aligned_size;
    g_stats.allocation_count++;
    
    if (error) *error = PAGE_ALLOC_OK;
    return ptr;
}

void page_free(void *ptr) {
    page_free_ex(ptr, NULL);
}

bool page_free_ex(void *ptr, page_alloc_error_t *error) {
    if (!ptr) {
        if (error) *error = PAGE_ALLOC_ERROR_NULL_PTR;
        return false;
    }
    
    free(ptr);
    g_stats.free_count++;
    
    if (error) *error = PAGE_ALLOC_OK;
    return true;
}

bool page_alloc_array(size_t count, size_t size, void **ptrs, page_alloc_error_t *error) {
    if (!ptrs || count == 0) {
        if (error) *error = PAGE_ALLOC_ERROR_NULL_PTR;
        return false;
    }
    
    for (size_t i = 0; i < count; i++) {
        ptrs[i] = page_alloc_ex(size, NULL, error);
        if (!ptrs[i]) {
            for (size_t j = 0; j < i; j++) {
                page_free(ptrs[j]);
            }
            return false;
        }
    }
    
    if (error) *error = PAGE_ALLOC_OK;
    return true;
}

bool page_free_array(size_t count, void **ptrs, page_alloc_error_t *error) {
    if (!ptrs) {
        if (error) *error = PAGE_ALLOC_ERROR_NULL_PTR;
        return false;
    }
    
    for (size_t i = 0; i < count; i++) {
        if (ptrs[i]) {
            page_free(ptrs[i]);
            ptrs[i] = NULL;
        }
    }
    
    if (error) *error = PAGE_ALLOC_OK;
    return true;
}

void* page_realloc(void *ptr, size_t old_size, size_t new_size, page_alloc_error_t *error) {
    if (new_size == 0) {
        page_free_ex(ptr, error);
        return NULL;
    }
    
    void *new_ptr = page_alloc_ex(new_size, NULL, error);
    if (!new_ptr) {
        return NULL;
    }
    
    if (ptr && old_size > 0) {
        size_t copy_size = old_size < new_size ? old_size : new_size;
        memcpy(new_ptr, ptr, copy_size);
        page_free(ptr);
    }
    
    return new_ptr;
}

bool page_is_aligned(const void *ptr, size_t alignment) {
    if (!ptr) return false;
    
    if (alignment == 0) {
        alignment = page_alloc_get_page_size();
    }
    
    return ((uintptr_t)ptr % alignment) == 0;
}

bool page_alloc_get_stats(page_alloc_stats_t *stats, page_alloc_error_t *error) {
    if (!stats) {
        if (error) *error = PAGE_ALLOC_ERROR_NULL_PTR;
        return false;
    }
    
    *stats = g_stats;
    if (error) *error = PAGE_ALLOC_OK;
    return true;
}

bool page_alloc_reset_stats(page_alloc_error_t *error) {
    memset(&g_stats, 0, sizeof(g_stats));
    g_stats.page_size = page_alloc_get_page_size();
    g_stats_initialized = true;
    
    if (error) *error = PAGE_ALLOC_OK;
    return true;
}

const char* page_alloc_error_string(page_alloc_error_t error) {
    switch (error) {
        case PAGE_ALLOC_OK: return "Success";
        case PAGE_ALLOC_ERROR_NULL_PTR: return "Null pointer error";
        case PAGE_ALLOC_ERROR_INVALID_SIZE: return "Invalid size";
        case PAGE_ALLOC_ERROR_ALLOCATION_FAILED: return "Allocation failed";
        case PAGE_ALLOC_ERROR_INVALID_ALIGNMENT: return "Invalid alignment";
        default: return "Unknown error";
    }
}
