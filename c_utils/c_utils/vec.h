#ifndef C_UTILS_VEC_H
#define C_UTILS_VEC_H

#include <stddef.h>
#include <stdbool.h>

/*
 * vec_t (Generic Vector)
 * 通用动态数组，支持任意类型的元素。
 */
typedef struct vec_s vec_t;

// 创建与销毁
vec_t* vec_create(size_t item_size);
void   vec_free(vec_t *v);
void   vec_clear(vec_t *v);

// 属性获取
size_t vec_size(const vec_t *v);
size_t vec_capacity(const vec_t *v);
bool   vec_is_empty(const vec_t *v);

// 增删改查
bool   vec_push(vec_t *v, const void *item);
bool   vec_pop(vec_t *v, void *out_item);
bool   vec_insert(vec_t *v, size_t index, const void *item);
bool   vec_remove(vec_t *v, size_t index);

// 获取元素指针
void*  vec_at(const vec_t *v, size_t index);

#endif // C_UTILS_VEC_H
