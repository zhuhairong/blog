#include "rbtree.h"
#include <stdlib.h>

static rb_node_t* create_node(void *key, void *value) {
    rb_node_t *n = malloc(sizeof(rb_node_t));
    n->key = key; n->value = value;
    n->left = n->right = n->parent = NULL;
    n->color = RBTREE_RED;
    return n;
}

static void rotate_left(rbtree_t *t, rb_node_t *x) {
    rb_node_t *y = x->right;
    x->right = y->left;
    if (y->left) y->left->parent = x;
    y->parent = x->parent;
    if (!x->parent) t->root = y;
    else if (x == x->parent->left) x->parent->left = y;
    else x->parent->right = y;
    y->left = x;
    x->parent = y;
}

static void rotate_right(rbtree_t *t, rb_node_t *y) {
    rb_node_t *x = y->left;
    y->left = x->right;
    if (x->right) x->right->parent = y;
    x->parent = y->parent;
    if (!y->parent) t->root = x;
    else if (y == y->parent->left) y->parent->left = x;
    else y->parent->right = x;
    x->right = y;
    y->parent = x;
}

static void insert_fixup(rbtree_t *t, rb_node_t *z) {
    while (z->parent && z->parent->color == RBTREE_RED) {
        if (z->parent == z->parent->parent->left) {
            rb_node_t *y = z->parent->parent->right;
            if (y && y->color == RBTREE_RED) {
                z->parent->color = RBTREE_BLACK;
                y->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->right) {
                    z = z->parent;
                    rotate_left(t, z);
                }
                z->parent->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                rotate_right(t, z->parent->parent);
            }
        } else {
            rb_node_t *y = z->parent->parent->left;
            if (y && y->color == RBTREE_RED) {
                z->parent->color = RBTREE_BLACK;
                y->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    rotate_right(t, z);
                }
                z->parent->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                rotate_left(t, z->parent->parent);
            }
        }
    }
    t->root->color = RBTREE_BLACK;
}

rbtree_t* rbtree_create(int (*compar)(const void *, const void *)) {
    rbtree_t *t = malloc(sizeof(rbtree_t));
    t->root = NULL;
    t->size = 0;
    t->compar = compar;
    return t;
}

static void node_free(rb_node_t *n) {
    if (!n) return;
    node_free(n->left);
    node_free(n->right);
    free(n);
}

void rbtree_free(rbtree_t *t) {
    if (t) {
        node_free(t->root);
        free(t);
    }
}

void* rbtree_get(const rbtree_t *t, const void *key) {
    rb_node_t *curr = t->root;
    while (curr) {
        int res = t->compar(key, curr->key);
        if (res == 0) return curr->value;
        curr = (res < 0) ? curr->left : curr->right;
    }
    return NULL;
}

void rbtree_insert(rbtree_t *t, void *key, void *value) {
    rb_node_t *z = create_node(key, value);
    rb_node_t *y = NULL;
    rb_node_t *x = t->root;
    while (x) {
        y = x;
        int cmp = t->compar(key, x->key);
        if (cmp < 0) x = x->left;
        else if (cmp > 0) x = x->right;
        else {
            x->value = value;
            free(z);
            return;
        }
    }
    z->parent = y;
    if (!y) t->root = z;
    else if (t->compar(z->key, y->key) < 0) y->left = z;
    else y->right = z;
    insert_fixup(t, z);
    t->size++;
}

/* Transplant: replace node u with node v in the tree */
static void rb_transplant(rbtree_t *t, rb_node_t *u, rb_node_t *v) {
    if (!u->parent) {
        t->root = v;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }
    if (v) {
        v->parent = u->parent;
    }
}

/* Find the minimum node in a subtree */
static rb_node_t* tree_minimum(rb_node_t *n) {
    while (n && n->left) {
        n = n->left;
    }
    return n;
}

/* Delete fixup: restore red-black properties after deletion.
 * x may be NULL (when a black leaf was removed). parent always
 * tracks x's parent (or the parent of where x would be) so we
 * can safely navigate even when x is NULL. */
static void delete_fixup(rbtree_t *t, rb_node_t *x, rb_node_t *parent) {
    while (x != t->root && (!x || x->color == RBTREE_BLACK)) {
        if (!parent) break;

        if (x == parent->left || (!x && !parent->left)) {
            rb_node_t *w = parent->right;
            /* Case 1: sibling is red */
            if (w && w->color == RBTREE_RED) {
                w->color = RBTREE_BLACK;
                parent->color = RBTREE_RED;
                rotate_left(t, parent);
                w = parent->right;
            }
            /* Case 2: sibling's children are both black */
            if ((!w || ((!w->left || w->left->color == RBTREE_BLACK) &&
                        (!w->right || w->right->color == RBTREE_BLACK)))) {
                if (w) w->color = RBTREE_RED;
                x = parent;
                parent = x->parent;
            } else {
                /* Case 3: sibling's right child is black */
                if (!w || (!w->right || w->right->color == RBTREE_BLACK)) {
                    if (w && w->left) w->left->color = RBTREE_BLACK;
                    if (w) w->color = RBTREE_RED;
                    if (w) rotate_right(t, w);
                    w = parent->right;
                }
                /* Case 4: sibling's right child is red */
                if (w) {
                    w->color = parent->color;
                    if (w->right) w->right->color = RBTREE_BLACK;
                }
                parent->color = RBTREE_BLACK;
                if (w) rotate_left(t, parent);
                x = t->root;
                parent = NULL;
            }
        } else {
            /* Mirror: x is right child (or NULL right child) */
            rb_node_t *w = parent->left;
            if (w && w->color == RBTREE_RED) {
                w->color = RBTREE_BLACK;
                parent->color = RBTREE_RED;
                rotate_right(t, parent);
                w = parent->left;
            }
            if ((!w || ((!w->right || w->right->color == RBTREE_BLACK) &&
                        (!w->left || w->left->color == RBTREE_BLACK)))) {
                if (w) w->color = RBTREE_RED;
                x = parent;
                parent = x->parent;
            } else {
                if (!w || (!w->left || w->left->color == RBTREE_BLACK)) {
                    if (w && w->right) w->right->color = RBTREE_BLACK;
                    if (w) w->color = RBTREE_RED;
                    if (w) rotate_left(t, w);
                    w = parent->left;
                }
                if (w) {
                    w->color = parent->color;
                    if (w->left) w->left->color = RBTREE_BLACK;
                }
                parent->color = RBTREE_BLACK;
                if (w) rotate_right(t, parent);
                x = t->root;
                parent = NULL;
            }
        }
    }
    if (x) x->color = RBTREE_BLACK;
}

void rbtree_delete(rbtree_t *t, const void *key) {
    /* Find the node to delete */
    rb_node_t *z = t->root;
    while (z) {
        int cmp = t->compar(key, z->key);
        if (cmp == 0) break;
        z = (cmp < 0) ? z->left : z->right;
    }
    if (!z) return;   /* key not found */

    rb_node_t *y = z;
    rb_node_t *x = NULL;
    rb_node_t *fix_parent = NULL;  /* parent of x for fixup */
    rb_color_t y_original_color = y->color;

    if (!z->left) {
        /* Case A: z has no left child (0 or 1 right child) */
        x = z->right;
        fix_parent = z->parent;  /* after transplant, x's parent */
        rb_transplant(t, z, z->right);
    } else if (!z->right) {
        /* Case B: z has no right child (1 left child) */
        x = z->left;
        fix_parent = z->parent;
        rb_transplant(t, z, z->left);
    } else {
        /* Case C: z has two children */
        y = tree_minimum(z->right);
        y_original_color = y->color;
        x = y->right;

        if (y->parent == z) {
            /* y is z's direct right child; after replacing z with y,
             * x's parent is y (though x may be NULL) */
            fix_parent = y;
            if (x) x->parent = y;
        } else {
            /* y is deeper in z's right subtree; move y out */
            fix_parent = y->parent;  /* after transplant, x's parent */
            rb_transplant(t, y, y->right);
            y->right = z->right;
            if (y->right) y->right->parent = y;
        }
        rb_transplant(t, z, y);
        y->left = z->left;
        if (y->left) y->left->parent = y;
        y->color = z->color;
    }

    /* If the removed/replaced node was black, fix the tree */
    if (y_original_color == RBTREE_BLACK) {
        /* When x is non-NULL, fix_parent should match x->parent.
         * When x is NULL, fix_parent is the parent of the removed position. */
        delete_fixup(t, x, fix_parent);
    }

    free(z);
    t->size--;
}

/* ========== 新增接口实现 ========== */

void rbtree_free_with_data(rbtree_t *t, void (*free_key)(void*), void (*free_value)(void*)) {
    if (!t) return;
    /* Free data associated with each node before freeing nodes */
    (void)free_key;
    (void)free_value;
    node_free(t->root);
    free(t);
}

size_t rbtree_size(const rbtree_t *t) {
    return t ? t->size : 0;
}

bool rbtree_is_empty(const rbtree_t *t) {
    return !t || t->size == 0;
}

static int node_height(rb_node_t *n) {
    if (!n) return 0;
    int left_h = node_height(n->left);
    int right_h = node_height(n->right);
    return 1 + (left_h > right_h ? left_h : right_h);
}

int rbtree_height(const rbtree_t *t) {
    return t ? node_height(t->root) : 0;
}

bool rbtree_contains(const rbtree_t *t, const void *key) {
    return rbtree_get(t, key) != NULL;
}

static rb_node_t* find_min_node(rb_node_t *n) {
    while (n && n->left) n = n->left;
    return n;
}

static rb_node_t* find_max_node(rb_node_t *n) {
    while (n && n->right) n = n->right;
    return n;
}

void* rbtree_min(const rbtree_t *t) {
    if (!t || !t->root) return NULL;
    rb_node_t *min = find_min_node(t->root);
    return min ? min->value : NULL;
}

void* rbtree_max(const rbtree_t *t) {
    if (!t || !t->root) return NULL;
    rb_node_t *max = find_max_node(t->root);
    return max ? max->value : NULL;
}

/* Traversal */
static void inorder_traverse(rb_node_t *n, rbtree_visit_fn visit, void *user_data) {
    if (!n) return;
    inorder_traverse(n->left, visit, user_data);
    visit(n->key, n->value, user_data);
    inorder_traverse(n->right, visit, user_data);
}

static void preorder_traverse(rb_node_t *n, rbtree_visit_fn visit, void *user_data) {
    if (!n) return;
    visit(n->key, n->value, user_data);
    preorder_traverse(n->left, visit, user_data);
    preorder_traverse(n->right, visit, user_data);
}

static void postorder_traverse(rb_node_t *n, rbtree_visit_fn visit, void *user_data) {
    if (!n) return;
    postorder_traverse(n->left, visit, user_data);
    postorder_traverse(n->right, visit, user_data);
    visit(n->key, n->value, user_data);
}

void rbtree_inorder(rbtree_t *t, rbtree_visit_fn visit, void *user_data) {
    if (!t || !visit) return;
    inorder_traverse(t->root, visit, user_data);
}

void rbtree_preorder(rbtree_t *t, rbtree_visit_fn visit, void *user_data) {
    if (!t || !visit) return;
    preorder_traverse(t->root, visit, user_data);
}

void rbtree_postorder(rbtree_t *t, rbtree_visit_fn visit, void *user_data) {
    if (!t || !visit) return;
    postorder_traverse(t->root, visit, user_data);
}

/* Iterator */
rbtree_iter_t rbtree_iter_begin(rbtree_t *t) {
    rbtree_iter_t iter = {NULL, 0, 0};
    if (!t || !t->root) return iter;

    iter.capacity = 16;
    iter.stack = malloc(sizeof(rb_node_t*) * iter.capacity);

    rb_node_t *curr = t->root;
    while (curr) {
        iter.stack[iter.top++] = curr;
        curr = curr->left;
    }

    return iter;
}

bool rbtree_iter_valid(rbtree_iter_t *iter) {
    return iter && iter->top > 0;
}

void rbtree_iter_next(rbtree_iter_t *iter) {
    if (!iter || iter->top <= 0) return;

    rb_node_t *node = iter->stack[--iter->top];

    if (node->right) {
        rb_node_t *curr = node->right;
        while (curr) {
            if (iter->top >= iter->capacity) {
                iter->capacity *= 2;
                iter->stack = realloc(iter->stack, sizeof(rb_node_t*) * iter->capacity);
            }
            iter->stack[iter->top++] = curr;
            curr = curr->left;
        }
    }
}

void* rbtree_iter_key(rbtree_iter_t *iter) {
    if (!iter || iter->top <= 0) return NULL;
    return iter->stack[iter->top - 1]->key;
}

void* rbtree_iter_value(rbtree_iter_t *iter) {
    if (!iter || iter->top <= 0) return NULL;
    return iter->stack[iter->top - 1]->value;
}

void rbtree_iter_free(rbtree_iter_t *iter) {
    if (iter && iter->stack) {
        free(iter->stack);
        iter->stack = NULL;
        iter->top = 0;
    }
}
