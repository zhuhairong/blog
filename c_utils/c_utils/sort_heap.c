#include "sort_heap.h"
#include <stdlib.h>
#include <string.h>

void heap_config_init(heap_config_t *config) {
    if (!config) return;
    config->type = HEAP_TYPE_MAX;
    config->use_malloc = true;
    config->enable_resize = true;
    config->enable_statistics = false;
    config->initial_capacity = 16;
    config->max_capacity = 0;
    config->resize_factor = 2.0f;
}

void heap_state_init(heap_state_t *state) {
    if (!state) return;
    memset(state, 0, sizeof(heap_state_t));
    state->is_initialized = true;
}

static void heapify_int(int *arr, int n, int i) {
    int largest = i;
    int l = 2 * i + 1;
    int r = 2 * i + 2;
    
    if (l < n && arr[l] > arr[largest]) largest = l;
    if (r < n && arr[r] > arr[largest]) largest = r;
    
    if (largest != i) {
        int tmp = arr[i];
        arr[i] = arr[largest];
        arr[largest] = tmp;
        heapify_int(arr, n, largest);
    }
}

void sort_heap(int *arr, size_t n) {
    if (!arr || n < 2) return;
    
    for (int i = (int)n / 2 - 1; i >= 0; i--) {
        heapify_int(arr, (int)n, i);
    }
    
    for (int i = (int)n - 1; i > 0; i--) {
        int tmp = arr[0];
        arr[0] = arr[i];
        arr[i] = tmp;
        heapify_int(arr, i, 0);
    }
}

static void heapify_generic(void *arr, size_t n, size_t element_size, size_t i,
                           heap_type_t type, int (*compar)(const void *, const void *)) {
    size_t target = i;
    size_t l = 2 * i + 1;
    size_t r = 2 * i + 2;
    
    char *data = (char *)arr;
    
    if (l < n) {
        int cmp = compar(data + l * element_size, data + target * element_size);
        if ((type == HEAP_TYPE_MAX && cmp > 0) || (type == HEAP_TYPE_MIN && cmp < 0)) {
            target = l;
        }
    }
    
    if (r < n) {
        int cmp = compar(data + r * element_size, data + target * element_size);
        if ((type == HEAP_TYPE_MAX && cmp > 0) || (type == HEAP_TYPE_MIN && cmp < 0)) {
            target = r;
        }
    }
    
    if (target != i) {
        char *a = data + i * element_size;
        char *b = data + target * element_size;
        for (size_t j = 0; j < element_size; j++) {
            char tmp = a[j];
            a[j] = b[j];
            b[j] = tmp;
        }
        heapify_generic(arr, n, element_size, target, type, compar);
    }
}

heap_error_t sort_heap_ex(void *arr, size_t n, size_t element_size,
                         heap_type_t type,
                         int (*compar)(const void *, const void *),
                         const heap_config_t *config,
                         heap_state_t *state) {
    if (!arr || !compar) return HEAP_ERROR_INVALID_PARAMS;
    if (n < 2) return HEAP_OK;
    
    if (state) {
        heap_state_init(state);
        state->type = type;
        state->size = n;
        state->capacity = n;
    }
    
    for (int i = (int)n / 2 - 1; i >= 0; i--) {
        heapify_generic(arr, n, element_size, (size_t)i, type, compar);
    }
    
    for (size_t i = n - 1; i > 0; i--) {
        char *data = (char *)arr;
        char *a = data;
        char *b = data + i * element_size;
        for (size_t j = 0; j < element_size; j++) {
            char tmp = a[j];
            a[j] = b[j];
            b[j] = tmp;
        }
        heapify_generic(arr, i, element_size, 0, type, compar);
    }
    
    if (state) {
        state->operations = n;
    }
    
    return HEAP_OK;
}

heap_t* heap_create(size_t capacity, size_t element_size,
                   heap_type_t type,
                   int (*compar)(const void *, const void *),
                   const heap_config_t *config,
                   heap_state_t *state) {
    if (capacity == 0 || element_size == 0 || !compar) return NULL;
    
    heap_t *heap = (heap_t *)malloc(sizeof(heap_t));
    if (!heap) return NULL;
    
    heap->data = malloc(capacity * element_size);
    if (!heap->data) {
        free(heap);
        return NULL;
    }
    
    heap->size = 0;
    heap->capacity = capacity;
    heap->element_size = element_size;
    heap->type = type;
    heap->compar = compar;
    
    heap->state = (heap_state_t *)malloc(sizeof(heap_state_t));
    if (!heap->state) {
        free(heap->data);
        free(heap);
        return NULL;
    }
    
    heap_state_init(heap->state);
    heap->state->type = type;
    heap->state->capacity = capacity;
    
    if (state) {
        memcpy(state, heap->state, sizeof(heap_state_t));
    }
    
    return heap;
}

heap_error_t heap_destroy(heap_t *heap, heap_state_t *state) {
    if (!heap) return HEAP_ERROR_INVALID_PARAMS;
    
    if (state && heap->state) {
        memcpy(state, heap->state, sizeof(heap_state_t));
    }
    
    if (heap->data) free(heap->data);
    if (heap->state) free(heap->state);
    free(heap);
    
    return HEAP_OK;
}

heap_error_t heap_insert(heap_t *heap, const void *element) {
    if (!heap || !element) return HEAP_ERROR_INVALID_PARAMS;
    
    if (heap->size >= heap->capacity) {
        return HEAP_ERROR_FULL;
    }
    
    char *data = (char *)heap->data;
    size_t i = heap->size;
    memcpy(data + i * heap->element_size, element, heap->element_size);
    heap->size++;
    
    while (i > 0) {
        size_t parent = (i - 1) / 2;
        char *p = data + parent * heap->element_size;
        char *c = data + i * heap->element_size;
        
        int cmp = heap->compar(c, p);
        if ((heap->type == HEAP_TYPE_MAX && cmp > 0) ||
            (heap->type == HEAP_TYPE_MIN && cmp < 0)) {
            for (size_t j = 0; j < heap->element_size; j++) {
                char tmp = p[j];
                p[j] = c[j];
                c[j] = tmp;
            }
            i = parent;
        } else {
            break;
        }
    }
    
    if (heap->state) {
        heap->state->size = heap->size;
        heap->state->insertions++;
        heap->state->operations++;
    }
    
    return HEAP_OK;
}

heap_error_t heap_delete(heap_t *heap, size_t index) {
    if (!heap) return HEAP_ERROR_INVALID_PARAMS;
    if (index >= heap->size) return HEAP_ERROR_INDEX_OUT_OF_BOUNDS;
    
    char *data = (char *)heap->data;
    char *last = data + (heap->size - 1) * heap->element_size;
    char *target = data + index * heap->element_size;
    
    memcpy(target, last, heap->element_size);
    heap->size--;
    
    if (index < heap->size) {
        heapify_generic(heap->data, heap->size, heap->element_size, index,
                       heap->type, heap->compar);
    }
    
    if (heap->state) {
        heap->state->size = heap->size;
        heap->state->deletions++;
        heap->state->operations++;
    }
    
    return HEAP_OK;
}

heap_error_t heap_pop(heap_t *heap, void *element) {
    if (!heap || !element) return HEAP_ERROR_INVALID_PARAMS;
    if (heap->size == 0) return HEAP_ERROR_EMPTY;
    
    char *data = (char *)heap->data;
    memcpy(element, data, heap->element_size);
    
    return heap_delete(heap, 0);
}

heap_error_t heap_peek(const heap_t *heap, void *element) {
    if (!heap || !element) return HEAP_ERROR_INVALID_PARAMS;
    if (heap->size == 0) return HEAP_ERROR_EMPTY;
    
    memcpy(element, heap->data, heap->element_size);
    return HEAP_OK;
}

heap_error_t heap_is_empty(const heap_t *heap, bool *is_empty) {
    if (!heap || !is_empty) return HEAP_ERROR_INVALID_PARAMS;
    *is_empty = (heap->size == 0);
    return HEAP_OK;
}

heap_error_t heap_is_full(const heap_t *heap, bool *is_full) {
    if (!heap || !is_full) return HEAP_ERROR_INVALID_PARAMS;
    *is_full = (heap->size >= heap->capacity);
    return HEAP_OK;
}

size_t heap_size(const heap_t *heap) {
    return heap ? heap->size : 0;
}

size_t heap_capacity(const heap_t *heap) {
    return heap ? heap->capacity : 0;
}

heap_error_t heap_resize(heap_t *heap, size_t new_capacity) {
    if (!heap) return HEAP_ERROR_INVALID_PARAMS;
    if (new_capacity < heap->size) return HEAP_ERROR_INVALID_PARAMS;
    
    void *new_data = realloc(heap->data, new_capacity * heap->element_size);
    if (!new_data) return HEAP_ERROR_MEMORY;
    
    heap->data = new_data;
    heap->capacity = new_capacity;
    
    if (heap->state) {
        heap->state->capacity = new_capacity;
    }
    
    return HEAP_OK;
}

heap_error_t heap_build(void *arr, size_t n, size_t element_size,
                       heap_type_t type,
                       int (*compar)(const void *, const void *),
                       heap_state_t *state) {
    if (!arr || !compar) return HEAP_ERROR_INVALID_PARAMS;
    
    for (int i = (int)n / 2 - 1; i >= 0; i--) {
        heapify_generic(arr, n, element_size, (size_t)i, type, compar);
    }
    
    if (state) {
        heap_state_init(state);
        state->type = type;
        state->size = n;
        state->capacity = n;
    }
    
    return HEAP_OK;
}

heap_error_t heap_heapify(void *arr, size_t n, size_t element_size,
                         size_t index,
                         heap_type_t type,
                         int (*compar)(const void *, const void *),
                         heap_state_t *state) {
    if (!arr || !compar || index >= n) return HEAP_ERROR_INVALID_PARAMS;
    
    heapify_generic(arr, n, element_size, index, type, compar);
    
    if (state) {
        state->operations++;
    }
    
    return HEAP_OK;
}

heap_error_t heap_merge(heap_t *heap1, heap_t *heap2, heap_t *result) {
    if (!heap1 || !heap2 || !result) return HEAP_ERROR_INVALID_PARAMS;
    if (heap1->element_size != heap2->element_size) return HEAP_ERROR_INVALID_PARAMS;
    
    size_t total_size = heap1->size + heap2->size;
    if (result->capacity < total_size) {
        heap_error_t err = heap_resize(result, total_size);
        if (err != HEAP_OK) return err;
    }
    
    char *src1 = (char *)heap1->data;
    char *src2 = (char *)heap2->data;
    char *dst = (char *)result->data;
    
    for (size_t i = 0; i < heap1->size; i++) {
        memcpy(dst + i * result->element_size, src1 + i * heap1->element_size, result->element_size);
    }
    for (size_t i = 0; i < heap2->size; i++) {
        memcpy(dst + (heap1->size + i) * result->element_size, src2 + i * heap2->element_size, result->element_size);
    }
    
    result->size = total_size;
    
    for (int i = (int)total_size / 2 - 1; i >= 0; i--) {
        heapify_generic(result->data, result->size, result->element_size, (size_t)i, result->type, result->compar);
    }
    
    return HEAP_OK;
}

heap_error_t heap_get_min(const heap_t *heap, void *element) {
    if (!heap || !element) return HEAP_ERROR_INVALID_PARAMS;
    if (heap->size == 0) return HEAP_ERROR_EMPTY;
    
    if (heap->type == HEAP_TYPE_MIN) {
        return heap_peek(heap, element);
    }
    
    char *data = (char *)heap->data;
    memcpy(element, data, heap->element_size);
    
    for (size_t i = 1; i < heap->size; i++) {
        if (heap->compar(data + i * heap->element_size, element) < 0) {
            memcpy(element, data + i * heap->element_size, heap->element_size);
        }
    }
    
    return HEAP_OK;
}

heap_error_t heap_get_max(const heap_t *heap, void *element) {
    if (!heap || !element) return HEAP_ERROR_INVALID_PARAMS;
    if (heap->size == 0) return HEAP_ERROR_EMPTY;
    
    if (heap->type == HEAP_TYPE_MAX) {
        return heap_peek(heap, element);
    }
    
    char *data = (char *)heap->data;
    memcpy(element, data, heap->element_size);
    
    for (size_t i = 1; i < heap->size; i++) {
        if (heap->compar(data + i * heap->element_size, element) > 0) {
            memcpy(element, data + i * heap->element_size, heap->element_size);
        }
    }
    
    return HEAP_OK;
}

heap_error_t heap_clear(heap_t *heap) {
    if (!heap) return HEAP_ERROR_INVALID_PARAMS;
    heap->size = 0;
    
    if (heap->state) {
        heap->state->size = 0;
    }
    
    return HEAP_OK;
}

heap_error_t heap_validate(const heap_t *heap, bool *is_valid) {
    if (!heap || !is_valid) return HEAP_ERROR_INVALID_PARAMS;
    
    *is_valid = true;
    char *data = (char *)heap->data;
    
    for (size_t i = 0; i < heap->size; i++) {
        size_t l = 2 * i + 1;
        size_t r = 2 * i + 2;
        
        if (l < heap->size) {
            int cmp = heap->compar(data + l * heap->element_size, data + i * heap->element_size);
            if ((heap->type == HEAP_TYPE_MAX && cmp > 0) ||
                (heap->type == HEAP_TYPE_MIN && cmp < 0)) {
                *is_valid = false;
                return HEAP_OK;
            }
        }
        
        if (r < heap->size) {
            int cmp = heap->compar(data + r * heap->element_size, data + i * heap->element_size);
            if ((heap->type == HEAP_TYPE_MAX && cmp > 0) ||
                (heap->type == HEAP_TYPE_MIN && cmp < 0)) {
                *is_valid = false;
                return HEAP_OK;
            }
        }
    }
    
    return HEAP_OK;
}

heap_error_t heap_get_statistics(const heap_t *heap, heap_state_t *state) {
    if (!heap || !state) return HEAP_ERROR_INVALID_PARAMS;
    
    if (heap->state) {
        memcpy(state, heap->state, sizeof(heap_state_t));
    } else {
        heap_state_init(state);
        state->size = heap->size;
        state->capacity = heap->capacity;
        state->type = heap->type;
    }
    
    return HEAP_OK;
}

const char* heap_strerror(const heap_state_t *state) {
    if (!state) return "Invalid state";
    
    switch (state->last_error) {
        case HEAP_OK: return "Success";
        case HEAP_ERROR_INVALID_PARAMS: return "Invalid parameters";
        case HEAP_ERROR_CREATE: return "Failed to create heap";
        case HEAP_ERROR_INSERT: return "Failed to insert element";
        case HEAP_ERROR_DELETE: return "Failed to delete element";
        case HEAP_ERROR_POP: return "Failed to pop element";
        case HEAP_ERROR_PEEK: return "Failed to peek element";
        case HEAP_ERROR_SORT: return "Failed to sort";
        case HEAP_ERROR_MEMORY: return "Memory allocation error";
        case HEAP_ERROR_EMPTY: return "Heap is empty";
        case HEAP_ERROR_FULL: return "Heap is full";
        case HEAP_ERROR_INDEX_OUT_OF_BOUNDS: return "Index out of bounds";
        case HEAP_ERROR_COMPARATOR_NULL: return "Comparator is null";
        default: return "Unknown error";
    }
}
