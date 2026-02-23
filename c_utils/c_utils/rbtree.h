#ifndef C_UTILS_RBTREE_H
#define C_UTILS_RBTREE_H

#include <stddef.h>
#include <stdbool.h>

typedef enum { RBTREE_RED, RBTREE_BLACK } rb_color_t;

typedef struct rb_node_s {
    struct rb_node_s *left, *right, *parent;
    rb_color_t color;
    void *key;
    void *value;
} rb_node_t;

typedef struct {
    rb_node_t *root;
    size_t size;
    int (*compar)(const void *, const void *);
} rbtree_t;

// 创建与销毁
rbtree_t* rbtree_create(int (*compar)(const void *, const void *));
void      rbtree_free(rbtree_t *t);
void      rbtree_free_with_data(rbtree_t *t, void (*free_key)(void*), void (*free_value)(void*));

// 基本操作
void      rbtree_insert(rbtree_t *t, void *key, void *value);
void*     rbtree_get(const rbtree_t *t, const void *key);
bool      rbtree_contains(const rbtree_t *t, const void *key);
void      rbtree_delete(rbtree_t *t, const void *key);

// 属性获取
size_t    rbtree_size(const rbtree_t *t);
bool      rbtree_is_empty(const rbtree_t *t);
int       rbtree_height(const rbtree_t *t);

// 查找极值
void*     rbtree_min(const rbtree_t *t);
void*     rbtree_max(const rbtree_t *t);

// 遍历回调函数类型
typedef void (*rbtree_visit_fn)(void *key, void *value, void *user_data);

// 遍历 (中序遍历，结果按 key 排序)
void      rbtree_inorder(rbtree_t *t, rbtree_visit_fn visit, void *user_data);
void      rbtree_preorder(rbtree_t *t, rbtree_visit_fn visit, void *user_data);
void      rbtree_postorder(rbtree_t *t, rbtree_visit_fn visit, void *user_data);

// 迭代器支持
typedef struct {
    rb_node_t **stack;
    int top;
    int capacity;
} rbtree_iter_t;

rbtree_iter_t rbtree_iter_begin(rbtree_t *t);
bool          rbtree_iter_valid(rbtree_iter_t *iter);
void          rbtree_iter_next(rbtree_iter_t *iter);
void*         rbtree_iter_key(rbtree_iter_t *iter);
void*         rbtree_iter_value(rbtree_iter_t *iter);
void          rbtree_iter_free(rbtree_iter_t *iter);

#endif // C_UTILS_RBTREE_H
