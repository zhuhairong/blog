#include "list.h"
#include <stdlib.h>
#include <string.h>

list_t* list_create(void) {
    return calloc(1, sizeof(list_t));
}

void list_push_back(list_t *l, void *data) {
    list_node_t *node = calloc(1, sizeof(list_node_t));
    node->data = data;
    if (!l->tail) {
        l->head = l->tail = node;
    } else {
        node->prev = l->tail;
        l->tail->next = node;
        l->tail = node;
    }
    l->size++;
}

void list_push_front(list_t *l, void *data) {
    list_node_t *node = calloc(1, sizeof(list_node_t));
    node->data = data;
    if (!l->head) {
        l->head = l->tail = node;
    } else {
        node->next = l->head;
        l->head->prev = node;
        l->head = node;
    }
    l->size++;
}

void* list_pop_front(list_t *l) {
    if (!l->head) return NULL;
    list_node_t *node = l->head;
    void *data = node->data;
    l->head = node->next;
    if (l->head) l->head->prev = NULL;
    else l->tail = NULL;
    free(node);
    l->size--;
    return data;
}

void* list_pop_back(list_t *l) {
    if (!l->tail) return NULL;
    list_node_t *node = l->tail;
    void *data = node->data;
    l->tail = node->prev;
    if (l->tail) l->tail->next = NULL;
    else l->head = NULL;
    free(node);
    l->size--;
    return data;
}

void list_free(list_t *l) {
    if (!l) return;
    while (l->head) list_pop_front(l);
    free(l);
}

void list_free_with_data(list_t *l, void (*free_fn)(void*)) {
    if (!l) return;
    if (free_fn) {
        list_foreach(l, node) {
            free_fn(node->data);
        }
    }
    list_clear(l);
    free(l);
}

size_t list_size(const list_t *l) {
    return l ? l->size : 0;
}

bool list_is_empty(const list_t *l) {
    return !l || l->size == 0;
}

void* list_front(const list_t *l) {
    return (l && l->head) ? l->head->data : NULL;
}

void* list_back(const list_t *l) {
    return (l && l->tail) ? l->tail->data : NULL;
}

bool list_insert(list_t *l, size_t index, void *data) {
    if (!l || index > l->size) return false;
    if (index == 0) {
        list_push_front(l, data);
        return true;
    }
    if (index == l->size) {
        list_push_back(l, data);
        return true;
    }
    list_node_t *new_node = calloc(1, sizeof(list_node_t));
    new_node->data = data;
    list_node_t *curr = l->head;
    for (size_t i = 0; i < index; i++) {
        curr = curr->next;
    }
    new_node->next = curr;
    new_node->prev = curr->prev;
    curr->prev->next = new_node;
    curr->prev = new_node;
    l->size++;
    return true;
}

bool list_remove(list_t *l, size_t index) {
    if (!l || index >= l->size) return false;
    if (index == 0) {
        list_pop_front(l);
        return true;
    }
    if (index == l->size - 1) {
        list_pop_back(l);
        return true;
    }
    list_node_t *curr = l->head;
    for (size_t i = 0; i < index; i++) {
        curr = curr->next;
    }
    curr->prev->next = curr->next;
    curr->next->prev = curr->prev;
    free(curr);
    l->size--;
    return true;
}

bool list_remove_node(list_t *l, list_node_t *node) {
    if (!l || !node) return false;
    if (node == l->head) {
        list_pop_front(l);
        return true;
    }
    if (node == l->tail) {
        list_pop_back(l);
        return true;
    }
    node->prev->next = node->next;
    node->next->prev = node->prev;
    free(node);
    l->size--;
    return true;
}

void list_clear(list_t *l) {
    if (!l) return;
    while (l->head) list_pop_front(l);
}

void list_clear_with_data(list_t *l, void (*free_fn)(void*)) {
    if (!l || !free_fn) return;
    list_foreach(l, node) {
        free_fn(node->data);
    }
    list_clear(l);
}

void* list_at(const list_t *l, size_t index) {
    if (!l || index >= l->size) return NULL;
    list_node_t *curr = l->head;
    for (size_t i = 0; i < index; i++) {
        curr = curr->next;
    }
    return curr->data;
}

list_node_t* list_find(const list_t *l, const void *data, int (*cmp)(const void*, const void*)) {
    if (!l) return NULL;
    list_foreach(l, node) {
        if (cmp ? cmp(node->data, data) == 0 : node->data == data) {
            return node;
        }
    }
    return NULL;
}

bool list_contains(const list_t *l, const void *data, int (*cmp)(const void*, const void*)) {
    return list_find(l, data, cmp) != NULL;
}

void list_reverse(list_t *l) {
    if (!l || l->size < 2) return;
    list_node_t *curr = l->head;
    list_node_t *temp = NULL;
    while (curr) {
        temp = curr->prev;
        curr->prev = curr->next;
        curr->next = temp;
        curr = curr->prev;
    }
    temp = l->head;
    l->head = l->tail;
    l->tail = temp;
}

list_t* list_copy(const list_t *l) {
    if (!l) return NULL;
    list_t *new_list = list_create();
    list_foreach(l, node) {
        list_push_back(new_list, node->data);
    }
    return new_list;
}

static list_node_t* merge_sort(list_node_t *head, int (*cmp)(const void*, const void*));
static list_node_t* split(list_node_t *head);
static list_node_t* merge(list_node_t *a, list_node_t *b, int (*cmp)(const void*, const void*));

void list_sort(list_t *l, int (*cmp)(const void*, const void*)) {
    if (!l || l->size < 2) return;
    l->head = merge_sort(l->head, cmp);
    list_node_t *curr = l->head;
    while (curr->next) {
        curr = curr->next;
    }
    l->tail = curr;
}

static list_node_t* merge_sort(list_node_t *head, int (*cmp)(const void*, const void*)) {
    if (!head || !head->next) return head;
    list_node_t *second = split(head);
    head = merge_sort(head, cmp);
    second = merge_sort(second, cmp);
    return merge(head, second, cmp);
}

static list_node_t* split(list_node_t *head) {
    list_node_t *fast = head;
    list_node_t *slow = head;
    while (fast->next && fast->next->next) {
        fast = fast->next->next;
        slow = slow->next;
    }
    list_node_t *temp = slow->next;
    slow->next = NULL;
    return temp;
}

static list_node_t* merge(list_node_t *a, list_node_t *b, int (*cmp)(const void*, const void*)) {
    if (!a) return b;
    if (!b) return a;
    if (cmp(a->data, b->data) <= 0) {
        a->next = merge(a->next, b, cmp);
        a->next->prev = a;
        a->prev = NULL;
        return a;
    } else {
        b->next = merge(a, b->next, cmp);
        b->next->prev = b;
        b->prev = NULL;
        return b;
    }
}
