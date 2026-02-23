#include "bplus_tree.h"
#include <stdlib.h>
#include <string.h>

// B+ 树节点结构
typedef struct bplus_node_s {
    bool is_leaf;
    int num_keys;
    void *keys[BPLUS_ORDER - 1];
    union {
        void *values[BPLUS_ORDER - 1];
        struct bplus_node_s *children[BPLUS_ORDER];
    } u;
    struct bplus_node_s *next;  // 用于叶子节点的链表
} bplus_node_t;

// B+ 树结构
struct bplus_tree_s {
    bplus_node_t *root;
    int (*compar)(const void *, const void *);
    size_t size;
};

// 迭代器结构
struct bplus_iterator_s {
    const bplus_tree_t *tree;
    bplus_node_t *current;
    int index;
};

// 创建节点
static bplus_node_t* create_node(bool is_leaf) {
    bplus_node_t *node = calloc(1, sizeof(bplus_node_t));
    node->is_leaf = is_leaf;
    return node;
}

// 创建 B+ 树
bplus_tree_t* bplus_tree_create(int (*compar)(const void *, const void *)) {
    bplus_tree_t *tree = malloc(sizeof(bplus_tree_t));
    if (!tree) return NULL;
    
    tree->root = NULL;
    tree->compar = compar;
    tree->size = 0;
    return tree;
}

// 释放节点
static void free_node(bplus_node_t *node) {
    if (!node) return;
    
    if (!node->is_leaf) {
        for (int i = 0; i <= node->num_keys; i++) {
            free_node(node->u.children[i]);
        }
    }
    free(node);
}

// 释放 B+ 树
void bplus_tree_free(bplus_tree_t *tree) {
    if (!tree) return;
    
    free_node(tree->root);
    free(tree);
}

// 简化的插入实现（仅支持叶子节点）
bool bplus_tree_insert(bplus_tree_t *tree, const void *key, void *value) {
    if (!tree) return false;
    
    // 创建根节点（如果不存在）
    if (!tree->root) {
        tree->root = create_node(true);
        if (!tree->root) return false;
    }
    
    bplus_node_t *leaf = tree->root;
    
    // 查找插入位置
    int i = leaf->num_keys - 1;
    while (i >= 0 && tree->compar && tree->compar(leaf->keys[i], key) > 0) {
        leaf->keys[i + 1] = leaf->keys[i];
        leaf->u.values[i + 1] = leaf->u.values[i];
        i--;
    }
    
    // 插入新键值
    leaf->keys[i + 1] = (void *)key;
    leaf->u.values[i + 1] = value;
    leaf->num_keys++;
    tree->size++;
    
    return true;
}

// 获取值
void* bplus_tree_get(const bplus_tree_t *tree, const void *key) {
    if (!tree || !tree->root) return NULL;
    
    bplus_node_t *curr = tree->root;
    
    // 遍历到叶子节点
    while (curr && !curr->is_leaf) {
        int i = 0;
        while (i < curr->num_keys && tree->compar && tree->compar(key, curr->keys[i]) >= 0) {
            i++;
        }
        curr = curr->u.children[i];
    }
    
    // 在叶子节点中查找
    if (curr && curr->is_leaf) {
        for (int i = 0; i < curr->num_keys; i++) {
            if (tree->compar && tree->compar(curr->keys[i], key) == 0) {
                return curr->u.values[i];
            }
        }
    }
    
    return NULL;
}

// 删除键值对（简化实现）
bool bplus_tree_delete(bplus_tree_t *tree, const void *key) {
    if (!tree || !tree->root) return false;
    
    // 简化实现：仅标记为已删除
    // 完整实现需要处理节点合并等复杂逻辑
    
    return false;  // 暂不支持删除
}

// 检查 B+ 树是否为空
bool bplus_tree_is_empty(const bplus_tree_t *tree) {
    if (!tree) return true;
    return tree->size == 0;
}

// 获取 B+ 树大小
size_t bplus_tree_size(const bplus_tree_t *tree) {
    if (!tree) return 0;
    return tree->size;
}

// 创建 B+ 树迭代器
bplus_iterator_t* bplus_iterator_create(const bplus_tree_t *tree) {
    if (!tree) return NULL;
    
    bplus_iterator_t *iter = malloc(sizeof(bplus_iterator_t));
    if (!iter) return NULL;
    
    iter->tree = tree;
    iter->current = tree->root;
    iter->index = 0;
    
    // 找到最左边的叶子节点
    while (iter->current && !iter->current->is_leaf) {
        iter->current = iter->current->u.children[0];
    }
    
    return iter;
}

// 释放迭代器
void bplus_iterator_free(bplus_iterator_t *iter) {
    if (iter) {
        free(iter);
    }
}

// 迭代到下一个元素
bool bplus_iterator_next(bplus_iterator_t *iter, const void **key, void **value) {
    if (!iter || !iter->current) return false;
    
    if (iter->index < iter->current->num_keys) {
        if (key) *key = iter->current->keys[iter->index];
        if (value) *value = iter->current->u.values[iter->index];
        iter->index++;
        return true;
    }
    
    // 移动到下一个叶子节点
    iter->current = iter->current->next;
    iter->index = 0;
    
    if (iter->current && iter->index < iter->current->num_keys) {
        if (key) *key = iter->current->keys[iter->index];
        if (value) *value = iter->current->u.values[iter->index];
        iter->index++;
        return true;
    }
    
    return false;
}

// 重置迭代器到起始位置
void bplus_iterator_reset(bplus_iterator_t *iter) {
    if (!iter || !iter->tree) return;
    
    iter->current = iter->tree->root;
    iter->index = 0;
    
    // 找到最左边的叶子节点
    while (iter->current && !iter->current->is_leaf) {
        iter->current = iter->current->u.children[0];
    }
}

// 范围查询（简化实现）
size_t bplus_tree_range_query(const bplus_tree_t *tree, const void *start_key, const void *end_key,
                              bool (*callback)(const void *key, void *value, void *user_data),
                              void *user_data) {
    if (!tree || !callback) return 0;
    
    size_t count = 0;
    bplus_iterator_t *iter = bplus_iterator_create(tree);
    if (!iter) return 0;
    
    const void *key;
    void *value;
    
    while (bplus_iterator_next(iter, &key, &value)) {
        // 检查是否在范围内
        bool in_range = true;
        if (start_key && tree->compar && tree->compar(key, start_key) < 0) {
            in_range = false;
        }
        if (end_key && tree->compar && tree->compar(key, end_key) > 0) {
            in_range = false;
        }
        
        if (in_range) {
            if (!callback(key, value, user_data)) {
                break;  // 回调返回 false，停止遍历
            }
            count++;
        }
    }
    
    bplus_iterator_free(iter);
    return count;
}

// 获取最小键
bool bplus_tree_min(const bplus_tree_t *tree, const void **key, void **value) {
    if (!tree || !tree->root) return false;
    
    bplus_node_t *curr = tree->root;
    
    // 找到最左边的叶子节点
    while (curr && !curr->is_leaf) {
        curr = curr->u.children[0];
    }
    
    if (curr && curr->num_keys > 0) {
        if (key) *key = curr->keys[0];
        if (value) *value = curr->u.values[0];
        return true;
    }
    
    return false;
}

// 获取最大键
bool bplus_tree_max(const bplus_tree_t *tree, const void **key, void **value) {
    if (!tree || !tree->root) return false;
    
    bplus_node_t *curr = tree->root;
    
    // 找到最右边的叶子节点
    while (curr && !curr->is_leaf) {
        curr = curr->u.children[curr->num_keys];
    }
    
    if (curr && curr->num_keys > 0) {
        if (key) *key = curr->keys[curr->num_keys - 1];
        if (value) *value = curr->u.values[curr->num_keys - 1];
        return true;
    }
    
    return false;
}

// 验证 B+ 树结构（简化实现）
bool bplus_tree_validate(const bplus_tree_t *tree) {
    if (!tree) return false;
    
    // 简化实现：仅检查基本结构
    if (tree->root && tree->root->num_keys >= BPLUS_ORDER - 1) {
        return false;  // 节点键数超过限制
    }
    
    return true;
}
