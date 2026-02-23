#ifndef C_UTILS_AVL_H
#define C_UTILS_AVL_H

#include <stddef.h>
#include <stdbool.h>

typedef struct avl_node_s {
    void *key;
    void *value;
    struct avl_node_s *left, *right;
    int height;
} avl_node_t;

typedef struct {
    avl_node_t *root;
    size_t size;
    int (*compar)(const void *, const void *);
} avl_t;

// 创建与销毁
avl_t* avl_create(int (*compar)(const void *, const void *));
void   avl_free(avl_t *t);
void   avl_free_with_data(avl_t *t, void (*free_key)(void*), void (*free_value)(void*));

// 基本操作
void   avl_insert(avl_t *t, void *key, void *value);
void*  avl_get(avl_t *t, const void *key);
bool   avl_contains(avl_t *t, const void *key);
void   avl_delete(avl_t *t, const void *key);

// 属性获取
size_t avl_size(const avl_t *t);
bool   avl_is_empty(const avl_t *t);
int    avl_height(const avl_t *t);

// 查找极值
void*  avl_min(avl_t *t);
void*  avl_max(avl_t *t);

// 遍历回调函数类型
typedef void (*avl_visit_fn)(void *key, void *value, void *user_data);

// 遍历 (中序遍历，结果按 key 排序)
void   avl_inorder(avl_t *t, avl_visit_fn visit, void *user_data);
void   avl_preorder(avl_t *t, avl_visit_fn visit, void *user_data);
void   avl_postorder(avl_t *t, avl_visit_fn visit, void *user_data);

// 迭代器支持
typedef struct {
    avl_node_t **stack;
    int top;
    int capacity;
} avl_iter_t;

avl_iter_t avl_iter_begin(avl_t *t);
bool       avl_iter_valid(avl_iter_t *iter);
void       avl_iter_next(avl_iter_t *iter);
void*      avl_iter_key(avl_iter_t *iter);
void*      avl_iter_value(avl_iter_t *iter);
void       avl_iter_free(avl_iter_t *iter);

#endif // C_UTILS_AVL_H
