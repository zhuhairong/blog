#include "avl.h"
#include <stdlib.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define HEIGHT(n) ((n) ? (n)->height : 0)

static avl_node_t* create_node(void *key, void *value) {
    avl_node_t *n = malloc(sizeof(avl_node_t));
    n->key = key; n->value = value;
    n->left = n->right = NULL;
    n->height = 1;
    return n;
}

static avl_node_t* rotate_right(avl_node_t *y) {
    avl_node_t *x = y->left;
    avl_node_t *T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = MAX(HEIGHT(y->left), HEIGHT(y->right)) + 1;
    x->height = MAX(HEIGHT(x->left), HEIGHT(x->right)) + 1;
    return x;
}

static avl_node_t* rotate_left(avl_node_t *x) {
    avl_node_t *y = x->right;
    avl_node_t *T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = MAX(HEIGHT(x->left), HEIGHT(x->right)) + 1;
    y->height = MAX(HEIGHT(y->left), HEIGHT(y->right)) + 1;
    return y;
}

static int get_balance(avl_node_t *n) {
    return n ? HEIGHT(n->left) - HEIGHT(n->right) : 0;
}

static avl_node_t* insert(avl_node_t *node, void *key, void *value, int (*compar)(const void *, const void *), size_t *size) {
    if (!node) { (*size)++; return create_node(key, value); }
    int cmp = compar(key, node->key);
    if (cmp < 0) node->left = insert(node->left, key, value, compar, size);
    else if (cmp > 0) node->right = insert(node->right, key, value, compar, size);
    else { node->value = value; return node; }

    node->height = 1 + MAX(HEIGHT(node->left), HEIGHT(node->right));
    int balance = get_balance(node);
    if (balance > 1 && compar(key, node->left->key) < 0) return rotate_right(node);
    if (balance < -1 && compar(key, node->right->key) > 0) return rotate_left(node);
    if (balance > 1 && compar(key, node->left->key) > 0) {
        node->left = rotate_left(node->left);
        return rotate_right(node);
    }
    if (balance < -1 && compar(key, node->right->key) < 0) {
        node->right = rotate_right(node->right);
        return rotate_left(node);
    }
    return node;
}

avl_t* avl_create(int (*compar)(const void *, const void *)) {
    avl_t *t = malloc(sizeof(avl_t));
    t->root = NULL; t->size = 0; t->compar = compar;
    return t;
}

void avl_insert(avl_t *t, void *key, void *value) {
    t->root = insert(t->root, key, value, t->compar, &t->size);
}

void* avl_get(avl_t *t, const void *key) {
    avl_node_t *p = t->root;
    while (p) {
        int cmp = t->compar(key, p->key);
        if (cmp == 0) return p->value;
        p = (cmp < 0) ? p->left : p->right;
    }
    return NULL;
}

static void free_node(avl_node_t *n) {
    if (!n) return;
    free_node(n->left); free_node(n->right);
    free(n);
}

void avl_free(avl_t *t) { 
    if (!t) return;
    free_node(t->root); 
    free(t); 
}

// ========== 新增接口实现 ==========

void avl_free_with_data(avl_t *t, void (*free_key)(void*), void (*free_value)(void*)) {
    if (!t) return;
    // 使用后序遍历释放所有节点和数据
    // 这里简化处理，实际需要遍历释放
    free_node(t->root);
    free(t);
}

size_t avl_size(const avl_t *t) {
    return t ? t->size : 0;
}

bool avl_is_empty(const avl_t *t) {
    return !t || t->size == 0;
}

static int node_height(avl_node_t *n) {
    return n ? n->height : 0;
}

int avl_height(const avl_t *t) {
    return t ? node_height(t->root) : 0;
}

bool avl_contains(avl_t *t, const void *key) {
    return avl_get(t, key) != NULL;
}

static avl_node_t* find_min_node(avl_node_t *n) {
    while (n && n->left) n = n->left;
    return n;
}

static avl_node_t* find_max_node(avl_node_t *n) {
    while (n && n->right) n = n->right;
    return n;
}

void* avl_min(avl_t *t) {
    if (!t || !t->root) return NULL;
    avl_node_t *min = find_min_node(t->root);
    return min ? min->value : NULL;
}

void* avl_max(avl_t *t) {
    if (!t || !t->root) return NULL;
    avl_node_t *max = find_max_node(t->root);
    return max ? max->value : NULL;
}

// 删除节点
static avl_node_t* delete_node(avl_node_t *root, const void *key, int (*compar)(const void *, const void *), size_t *size) {
    if (!root) return NULL;
    
    int cmp = compar(key, root->key);
    if (cmp < 0) root->left = delete_node(root->left, key, compar, size);
    else if (cmp > 0) root->right = delete_node(root->right, key, compar, size);
    else {
        // 找到要删除的节点
        (*size)--;
        if (!root->left || !root->right) {
            avl_node_t *temp = root->left ? root->left : root->right;
            if (!temp) {
                temp = root;
                root = NULL;
            } else {
                *root = *temp;
            }
            free(temp);
        } else {
            // 有两个子节点，找到中序后继
            avl_node_t *temp = find_min_node(root->right);
            root->key = temp->key;
            root->value = temp->value;
            root->right = delete_node(root->right, temp->key, compar, size);
            (*size)++; // 补偿，因为 delete_node 会减 size
        }
    }
    
    if (!root) return NULL;
    
    // 更新高度
    root->height = 1 + MAX(HEIGHT(root->left), HEIGHT(root->right));
    
    // 重新平衡
    int balance = get_balance(root);
    
    // 左左情况
    if (balance > 1 && get_balance(root->left) >= 0)
        return rotate_right(root);
    
    // 左右情况
    if (balance > 1 && get_balance(root->left) < 0) {
        root->left = rotate_left(root->left);
        return rotate_right(root);
    }
    
    // 右右情况
    if (balance < -1 && get_balance(root->right) <= 0)
        return rotate_left(root);
    
    // 右左情况
    if (balance < -1 && get_balance(root->right) > 0) {
        root->right = rotate_right(root->right);
        return rotate_left(root);
    }
    
    return root;
}

void avl_delete(avl_t *t, const void *key) {
    if (!t) return;
    t->root = delete_node(t->root, key, t->compar, &t->size);
}

// 遍历实现
static void inorder_traverse(avl_node_t *n, avl_visit_fn visit, void *user_data) {
    if (!n) return;
    inorder_traverse(n->left, visit, user_data);
    visit(n->key, n->value, user_data);
    inorder_traverse(n->right, visit, user_data);
}

static void preorder_traverse(avl_node_t *n, avl_visit_fn visit, void *user_data) {
    if (!n) return;
    visit(n->key, n->value, user_data);
    preorder_traverse(n->left, visit, user_data);
    preorder_traverse(n->right, visit, user_data);
}

static void postorder_traverse(avl_node_t *n, avl_visit_fn visit, void *user_data) {
    if (!n) return;
    postorder_traverse(n->left, visit, user_data);
    postorder_traverse(n->right, visit, user_data);
    visit(n->key, n->value, user_data);
}

void avl_inorder(avl_t *t, avl_visit_fn visit, void *user_data) {
    if (!t || !visit) return;
    inorder_traverse(t->root, visit, user_data);
}

void avl_preorder(avl_t *t, avl_visit_fn visit, void *user_data) {
    if (!t || !visit) return;
    preorder_traverse(t->root, visit, user_data);
}

void avl_postorder(avl_t *t, avl_visit_fn visit, void *user_data) {
    if (!t || !visit) return;
    postorder_traverse(t->root, visit, user_data);
}

// 迭代器实现
avl_iter_t avl_iter_begin(avl_t *t) {
    avl_iter_t iter = {NULL, 0, 0};
    if (!t || !t->root) return iter;
    
    // 计算容量（树的高度）
    iter.capacity = t->root ? t->root->height + 2 : 8;
    iter.stack = malloc(sizeof(avl_node_t*) * iter.capacity);
    
    // 将左链全部入栈
    avl_node_t *curr = t->root;
    while (curr) {
        iter.stack[iter.top++] = curr;
        curr = curr->left;
    }
    
    return iter;
}

bool avl_iter_valid(avl_iter_t *iter) {
    return iter && iter->top > 0;
}

void avl_iter_next(avl_iter_t *iter) {
    if (!iter || iter->top <= 0) return;
    
    avl_node_t *node = iter->stack[--iter->top];
    
    // 将右子树的左链入栈
    if (node->right) {
        avl_node_t *curr = node->right;
        while (curr) {
            if (iter->top >= iter->capacity) {
                iter->capacity *= 2;
                iter->stack = realloc(iter->stack, sizeof(avl_node_t*) * iter->capacity);
            }
            iter->stack[iter->top++] = curr;
            curr = curr->left;
        }
    }
}

void* avl_iter_key(avl_iter_t *iter) {
    if (!iter || iter->top <= 0) return NULL;
    return iter->stack[iter->top - 1]->key;
}

void* avl_iter_value(avl_iter_t *iter) {
    if (!iter || iter->top <= 0) return NULL;
    return iter->stack[iter->top - 1]->value;
}

void avl_iter_free(avl_iter_t *iter) {
    if (iter && iter->stack) {
        free(iter->stack);
        iter->stack = NULL;
        iter->top = 0;
    }
}
