#include "heap.h"
#include <stdlib.h>
#include <string.h>

struct heap_s {
    void *data;
    size_t item_size;
    size_t size;
    size_t capacity;
    heap_type_t type;
    heap_compar_t compar;
    bool auto_resize;
    heap_error_t last_error;
};

// 默认比较函数（最小堆）
static int min_heap_compar(const void *a, const void *b) {
    // 假设数据是int类型进行比较，实际应该由用户提供比较函数
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return (ia > ib) - (ia < ib);
}

// 默认比较函数（最大堆）
static int max_heap_compar(const void *a, const void *b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return (ib > ia) - (ib < ia);
}

// 默认堆配置
heap_config_t heap_default_config(heap_type_t type, size_t item_size, heap_compar_t compar) {
    heap_config_t config = {
        .type = type,
        .item_size = item_size,
        .capacity = 16,
        .auto_resize = true,
        .compar = compar
    };
    
    if (type == HEAP_TYPE_MIN && !compar) {
        config.compar = min_heap_compar;
    } else if (type == HEAP_TYPE_MAX && !compar) {
        config.compar = max_heap_compar;
    }
    
    return config;
}

// 创建堆
heap_t* heap_create(const heap_config_t *config, heap_error_t *error) {
    heap_config_t default_config = heap_default_config(HEAP_TYPE_MIN, sizeof(int), NULL);
    const heap_config_t *cfg = config ? config : &default_config;
    
    if (cfg->item_size == 0) {
        if (error) *error = HEAP_ERROR_INVALID_PARAM;
        return NULL;
    }
    
    heap_t *h = malloc(sizeof(heap_t));
    if (!h) {
        if (error) *error = HEAP_ERROR_MEMORY_ALLOC;
        return NULL;
    }
    
    size_t cap = cfg->capacity > 0 ? cfg->capacity : 16;
    h->data = malloc(cfg->item_size * cap);
    if (!h->data) {
        free(h);
        if (error) *error = HEAP_ERROR_MEMORY_ALLOC;
        return NULL;
    }
    
    h->item_size = cfg->item_size;
    h->size = 0;
    h->capacity = cap;
    h->type = cfg->type;
    h->compar = cfg->compar;
    h->auto_resize = cfg->auto_resize;
    h->last_error = HEAP_OK;
    
    if (error) *error = HEAP_OK;
    return h;
}

// 释放堆
void heap_free(heap_t *h) {
    if (h) {
        free(h->data);
        free(h);
    }
}

// 属性
size_t heap_size(const heap_t *h) { return h ? h->size : 0; }
bool heap_is_empty(const heap_t *h) { return !h || h->size == 0; }
bool heap_is_full(const heap_t *h) { return h && h->size >= h->capacity; }
size_t heap_capacity(const heap_t *h) { return h ? h->capacity : 0; }

static void swap(char *a, char *b, size_t size) {
    char tmp;
    while (size--) {
        tmp = *a;
        *a++ = *b;
        *b++ = tmp;
    }
}

// 扩容
static bool heap_resize(heap_t *h, heap_error_t *error) {
    if (!h->auto_resize) {
        if (error) *error = HEAP_ERROR_FULL_HEAP;
        return false;
    }
    
    size_t new_cap = h->capacity * 2;
    void *new_data = realloc(h->data, h->item_size * new_cap);
    if (!new_data) {
        if (error) *error = HEAP_ERROR_MEMORY_ALLOC;
        return false;
    }
    
    h->data = new_data;
    h->capacity = new_cap;
    return true;
}

// 上浮
static void heapify_up(heap_t *h, size_t idx) {
    while (idx > 0) {
        size_t parent = (idx - 1) / 2;
        char *curr_ptr = (char*)h->data + idx * h->item_size;
        char *parent_ptr = (char*)h->data + parent * h->item_size;
        
        if (h->compar(curr_ptr, parent_ptr) < 0) {
            swap(curr_ptr, parent_ptr, h->item_size);
            idx = parent;
        } else break;
    }
}

// 下沉
static void heapify_down(heap_t *h, size_t idx) {
    while (true) {
        size_t left = 2 * idx + 1;
        size_t right = 2 * idx + 2;
        size_t smallest = idx;
        
        char *idx_ptr = (char*)h->data + idx * h->item_size;
        
        if (left < h->size) {
            char *left_ptr = (char*)h->data + left * h->item_size;
            if (h->compar(left_ptr, idx_ptr) < 0) {
                smallest = left;
                idx_ptr = left_ptr;
            }
        }
        if (right < h->size) {
            char *right_ptr = (char*)h->data + right * h->item_size;
            char *smallest_ptr = (char*)h->data + smallest * h->item_size;
            if (h->compar(right_ptr, smallest_ptr) < 0) {
                smallest = right;
            }
        }
        
        if (smallest != idx) {
            char *curr_ptr = (char*)h->data + idx * h->item_size;
            char *smallest_ptr = (char*)h->data + smallest * h->item_size;
            swap(curr_ptr, smallest_ptr, h->item_size);
            idx = smallest;
        } else break;
    }
}

// 操作
bool heap_push(heap_t *h, const void *item, heap_error_t *error) {
    if (!h || !item) {
        if (error) *error = HEAP_ERROR_INVALID_PARAM;
        return false;
    }
    
    if (heap_is_full(h)) {
        if (!heap_resize(h, error)) {
            return false;
        }
    }
    
    char *dest = (char*)h->data + (h->size * h->item_size);
    memcpy(dest, item, h->item_size);
    heapify_up(h, h->size);
    h->size++;
    
    if (error) *error = HEAP_OK;
    return true;
}

bool heap_pop(heap_t *h, void *out_item, heap_error_t *error) {
    if (!h || heap_is_empty(h)) {
        if (error) *error = HEAP_ERROR_EMPTY_HEAP;
        return false;
    }
    
    char *root = (char*)h->data;
    if (out_item) {
        memcpy(out_item, root, h->item_size);
    }
    
    h->size--;
    if (h->size > 0) {
        char *last = (char*)h->data + h->size * h->item_size;
        memcpy(root, last, h->item_size);
        heapify_down(h, 0);
    }
    
    if (error) *error = HEAP_OK;
    return true;
}

void* heap_peek(const heap_t *h, heap_error_t *error) {
    if (!h || heap_is_empty(h)) {
        if (error) *error = HEAP_ERROR_EMPTY_HEAP;
        return NULL;
    }
    
    if (error) *error = HEAP_OK;
    return h->data;
}

bool heap_remove(heap_t *h, const void *item, heap_error_t *error) {
    if (!h || !item || heap_is_empty(h)) {
        if (error) *error = HEAP_ERROR_INVALID_PARAM;
        return false;
    }
    
    // 查找元素
    for (size_t i = 0; i < h->size; i++) {
        char *curr = (char*)h->data + i * h->item_size;
        if (h->compar(curr, item) == 0) {
            // 找到元素，用最后一个元素替换
            h->size--;
            if (i < h->size) {
                char *last = (char*)h->data + h->size * h->item_size;
                memcpy(curr, last, h->item_size);
                heapify_down(h, i);
                heapify_up(h, i);
            }
            if (error) *error = HEAP_OK;
            return true;
        }
    }
    
    if (error) *error = HEAP_ERROR_ITEM_NOT_FOUND;
    return false;
}

bool heap_contains(const heap_t *h, const void *item, heap_error_t *error) {
    if (!h || !item) {
        if (error) *error = HEAP_ERROR_INVALID_PARAM;
        return false;
    }
    
    for (size_t i = 0; i < h->size; i++) {
        char *curr = (char*)h->data + i * h->item_size;
        if (h->compar(curr, item) == 0) {
            if (error) *error = HEAP_OK;
            return true;
        }
    }
    
    if (error) *error = HEAP_OK;
    return false;
}

bool heap_clear(heap_t *h, heap_error_t *error) {
    if (!h) {
        if (error) *error = HEAP_ERROR_INVALID_PARAM;
        return false;
    }
    
    h->size = 0;
    if (error) *error = HEAP_OK;
    return true;
}

// 批量操作
bool heap_push_batch(heap_t *h, const void *items, size_t count, heap_error_t *error) {
    if (!h || !items) {
        if (error) *error = HEAP_ERROR_INVALID_PARAM;
        return false;
    }
    
    const char *p = (const char*)items;
    for (size_t i = 0; i < count; i++) {
        if (!heap_push(h, p + i * h->item_size, error)) {
            return false;
        }
    }
    
    if (error) *error = HEAP_OK;
    return true;
}

bool heap_pop_batch(heap_t *h, void *out_items, size_t count, size_t *popped, heap_error_t *error) {
    if (!h || !out_items) {
        if (error) *error = HEAP_ERROR_INVALID_PARAM;
        return false;
    }
    
    size_t actual = 0;
    char *out = (char*)out_items;
    
    while (actual < count && !heap_is_empty(h)) {
        if (!heap_pop(h, out + actual * h->item_size, error)) {
            break;
        }
        actual++;
    }
    
    if (popped) *popped = actual;
    if (error) *error = HEAP_OK;
    return actual > 0;
}

// 堆排序
bool heap_sort(void *items, size_t count, size_t item_size, const heap_config_t *config, heap_error_t *error) {
    if (!items || count == 0 || item_size == 0) {
        if (error) *error = HEAP_ERROR_INVALID_PARAM;
        return false;
    }
    
    heap_config_t cfg = config ? *config : heap_default_config(HEAP_TYPE_MIN, item_size, NULL);
    cfg.item_size = item_size;
    cfg.capacity = count;
    cfg.auto_resize = false;
    
    heap_t *h = heap_create(&cfg, error);
    if (!h) return false;
    
    // 将所有元素加入堆
    char *p = (char*)items;
    for (size_t i = 0; i < count; i++) {
        heap_push(h, p + i * item_size, NULL);
    }
    
    // 依次弹出（已排序）
    for (size_t i = 0; i < count; i++) {
        heap_pop(h, p + i * item_size, NULL);
    }
    
    heap_free(h);
    if (error) *error = HEAP_OK;
    return true;
}

// 调整堆容量
bool heap_reserve(heap_t *h, size_t new_capacity, heap_error_t *error) {
    if (!h) {
        if (error) *error = HEAP_ERROR_INVALID_PARAM;
        return false;
    }
    
    if (new_capacity <= h->capacity) {
        if (error) *error = HEAP_OK;
        return true;
    }
    
    void *new_data = realloc(h->data, h->item_size * new_capacity);
    if (!new_data) {
        if (error) *error = HEAP_ERROR_MEMORY_ALLOC;
        return false;
    }
    
    h->data = new_data;
    h->capacity = new_capacity;
    if (error) *error = HEAP_OK;
    return true;
}

// 检查堆是否有错误
bool heap_has_error(const heap_t *h, heap_error_t *error, const char **error_msg) {
    if (!h) {
        if (error) *error = HEAP_ERROR_INVALID_PARAM;
        if (error_msg) *error_msg = "Invalid heap";
        return true;
    }
    
    if (error) *error = h->last_error;
    if (error_msg) *error_msg = heap_strerror(h->last_error);
    return h->last_error != HEAP_OK;
}

// 获取错误信息
const char* heap_strerror(heap_error_t error) {
    switch (error) {
        case HEAP_OK:
            return "Success";
        case HEAP_ERROR_INVALID_PARAM:
            return "Invalid parameter";
        case HEAP_ERROR_MEMORY_ALLOC:
            return "Memory allocation failed";
        case HEAP_ERROR_EMPTY_HEAP:
            return "Heap is empty";
        case HEAP_ERROR_FULL_HEAP:
            return "Heap is full";
        case HEAP_ERROR_ITEM_NOT_FOUND:
            return "Item not found";
        default:
            return "Unknown error";
    }
}
