#include "vec.h"
#include <stdlib.h>
#include <string.h>

struct vec_s {
    size_t item_size;
    size_t size;
    size_t cap;
    char  *data;
};

vec_t* vec_create(size_t item_size) {
    vec_t *v = malloc(sizeof(vec_t));
    if (!v) return NULL;
    v->item_size = item_size;
    v->size = 0;
    v->cap = 8; // 初始容量
    v->data = malloc(v->item_size * v->cap);
    if (!v->data) {
        free(v);
        return NULL;
    }
    return v;
}

void vec_free(vec_t *v) {
    if (v) {
        free(v->data);
        free(v);
    }
}

void vec_clear(vec_t *v) {
    if (v) v->size = 0;
}

size_t vec_size(const vec_t *v) {
    return v ? v->size : 0;
}

size_t vec_capacity(const vec_t *v) {
    return v ? v->cap : 0;
}

bool vec_is_empty(const vec_t *v) {
    return !v || v->size == 0;
}

static bool vec_grow(vec_t *v) {
    size_t new_cap = v->cap * 2;
    char *new_data = realloc(v->data, v->item_size * new_cap);
    if (!new_data) return false;
    v->data = new_data;
    v->cap = new_cap;
    return true;
}

bool vec_push(vec_t *v, const void *item) {
    if (!v || !item) return false;
    if (v->size == v->cap) {
        if (!vec_grow(v)) return false;
    }
    memcpy(v->data + (v->size * v->item_size), item, v->item_size);
    v->size++;
    return true;
}

bool vec_pop(vec_t *v, void *out_item) {
    if (vec_is_empty(v)) return false;
    v->size--;
    if (out_item) {
        memcpy(out_item, v->data + (v->size * v->item_size), v->item_size);
    }
    return true;
}

void* vec_at(const vec_t *v, size_t index) {
    if (!v || index >= v->size) return NULL;
    return v->data + (index * v->item_size);
}

bool vec_insert(vec_t *v, size_t index, const void *item) {
    if (!v || index > v->size) return false;
    if (v->size == v->cap) {
        if (!vec_grow(v)) return false;
    }
    if (index < v->size) {
        memmove(v->data + (index + 1) * v->item_size,
                v->data + index * v->item_size,
                (v->size - index) * v->item_size);
    }
    memcpy(v->data + index * v->item_size, item, v->item_size);
    v->size++;
    return true;
}

bool vec_remove(vec_t *v, size_t index) {
    if (!v || index >= v->size) return false;
    if (index < v->size - 1) {
        memmove(v->data + index * v->item_size,
                v->data + (index + 1) * v->item_size,
                (v->size - index - 1) * v->item_size);
    }
    v->size--;
    return true;
}
