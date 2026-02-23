#ifndef C_UTILS_LIST_H
#define C_UTILS_LIST_H

#include <stddef.h>
#include <stdbool.h>

typedef struct list_node_s {
    void *data;
    struct list_node_s *prev, *next;
} list_node_t;

typedef struct {
    list_node_t *head, *tail;
    size_t size;
} list_t;

// 创建与销毁
list_t* list_create(void);
void    list_free(list_t *l);
void    list_free_with_data(list_t *l, void (*free_fn)(void*));

// 基本操作
void    list_push_back(list_t *l, void *data);
void    list_push_front(list_t *l, void *data);
void*   list_pop_back(list_t *l);
void*   list_pop_front(list_t *l);

// 属性获取
size_t  list_size(const list_t *l);
bool    list_is_empty(const list_t *l);
void*   list_front(const list_t *l);
void*   list_back(const list_t *l);

// 插入与删除
bool    list_insert(list_t *l, size_t index, void *data);
bool    list_remove(list_t *l, size_t index);
bool    list_remove_node(list_t *l, list_node_t *node);
void    list_clear(list_t *l);
void    list_clear_with_data(list_t *l, void (*free_fn)(void*));

// 查找与访问
void*   list_at(const list_t *l, size_t index);
list_node_t* list_find(const list_t *l, const void *data, int (*cmp)(const void*, const void*));
bool    list_contains(const list_t *l, const void *data, int (*cmp)(const void*, const void*));

// 遍历
#define list_foreach(l, node) \
    for (list_node_t *node = (l)->head; node != NULL; node = node->next)

#define list_foreach_reverse(l, node) \
    for (list_node_t *node = (l)->tail; node != NULL; node = node->prev)

// 高级操作
void    list_reverse(list_t *l);
list_t* list_copy(const list_t *l);
void    list_sort(list_t *l, int (*cmp)(const void*, const void*));

#endif // C_UTILS_LIST_H
