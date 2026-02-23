#include "segment_tree.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// 默认配置
segment_tree_config_t segment_tree_default_config(void) {
    segment_tree_config_t config = {
        .operation = SEGMENT_TREE_OP_SUM,
        .use_optimized = false,
        .check_bounds = true,
        .use_lazy_propagation = false,
        .max_size = 0,
        .default_value = 0
    };
    return config;
}

// 获取默认状态
static segment_tree_state_t segment_tree_default_state(void) {
    segment_tree_state_t state = {
        .size = 0,
        .tree_size = 0,
        .operation = SEGMENT_TREE_OP_SUM,
        .initialized = false,
        .use_lazy = false
    };
    return state;
}

// 辅助函数：构建线段树
static void build_tree(int *tree, int *arr, size_t node, size_t start, size_t end) {
    if (start == end) {
        tree[node] = arr[start];
    } else {
        size_t mid = (start + end) / 2;
        build_tree(tree, arr, 2 * node + 1, start, mid);
        build_tree(tree, arr, 2 * node + 2, mid + 1, end);
        tree[node] = tree[2 * node + 1] + tree[2 * node + 2];
    }
}

// 初始化线段树
void segment_tree_init(segment_tree_t *st, int *arr, size_t n) {
    segment_tree_config_t config = segment_tree_default_config();
    segment_tree_init_ex(st, arr, n, &config, NULL);
}

// 使用自定义配置初始化线段树
bool segment_tree_init_ex(segment_tree_t *st, int *arr, size_t n, const segment_tree_config_t *config, segment_tree_error_t *error) {
    if (!st) {
        if (error) *error = SEGMENT_TREE_ERROR_NULL_PTR;
        return false;
    }
    
    if (!arr || n == 0) {
        if (error) *error = SEGMENT_TREE_ERROR_INVALID_ARGS;
        return false;
    }
    
    // 清理旧的内存
    if (st->tree) {
        free(st->tree);
    }
    if (st->lazy) {
        free(st->lazy);
    }
    
    st->config = config ? *config : segment_tree_default_config();
    st->n = n;
    st->tree_size = 4 * n; // 线段树通常需要4倍空间
    
    st->tree = calloc(st->tree_size, sizeof(int));
    if (!st->tree) {
        if (error) *error = SEGMENT_TREE_ERROR_MEMORY_ALLOC;
        return false;
    }
    
    if (st->config.use_lazy_propagation) {
        st->lazy = calloc(st->tree_size, sizeof(int));
        if (!st->lazy) {
            free(st->tree);
            st->tree = NULL;
            if (error) *error = SEGMENT_TREE_ERROR_MEMORY_ALLOC;
            return false;
        }
    } else {
        st->lazy = NULL;
    }
    
    // 构建线段树
    build_tree(st->tree, arr, 0, 0, n - 1);
    
    st->initialized = true;
    if (error) *error = SEGMENT_TREE_OK;
    return true;
}

// 辅助函数：更新节点
static void update_tree(int *tree, size_t node, size_t start, size_t end, size_t idx, int val) {
    if (start == end) {
        tree[node] = val;
    } else {
        size_t mid = (start + end) / 2;
        if (idx <= mid) {
            update_tree(tree, 2 * node + 1, start, mid, idx, val);
        } else {
            update_tree(tree, 2 * node + 2, mid + 1, end, idx, val);
        }
        tree[node] = tree[2 * node + 1] + tree[2 * node + 2];
    }
}

// 更新线段树中的单个元素
void segment_tree_update(segment_tree_t *st, size_t idx, int val) {
    segment_tree_update_ex(st, idx, val, NULL);
}

// 更新线段树中的单个元素（带错误处理）
bool segment_tree_update_ex(segment_tree_t *st, size_t idx, int val, segment_tree_error_t *error) {
    if (!st || !st->tree) {
        if (error) *error = SEGMENT_TREE_ERROR_NULL_PTR;
        return false;
    }
    
    if (!st->initialized) {
        if (error) *error = SEGMENT_TREE_ERROR_NOT_INITIALIZED;
        return false;
    }
    
    if (st->config.check_bounds && idx >= st->n) {
        if (error) *error = SEGMENT_TREE_ERROR_INDEX_OUT_OF_BOUNDS;
        return false;
    }
    
    update_tree(st->tree, 0, 0, st->n - 1, idx, val);
    
    if (error) *error = SEGMENT_TREE_OK;
    return true;
}

// 辅助函数：查询区间
static int query_tree(int *tree, size_t node, size_t start, size_t end, size_t left, size_t right) {
    if (right < start || left > end) {
        return 0;
    }
    if (left <= start && end <= right) {
        return tree[node];
    }
    size_t mid = (start + end) / 2;
    return query_tree(tree, 2 * node + 1, start, mid, left, right) +
           query_tree(tree, 2 * node + 2, mid + 1, end, left, right);
}

// 查询线段树中的区间
int segment_tree_query(segment_tree_t *st, size_t left, size_t right) {
    segment_tree_error_t error;
    return segment_tree_query_ex(st, left, right, &error);
}

// 查询线段树中的区间（带错误处理）
int segment_tree_query_ex(segment_tree_t *st, size_t left, size_t right, segment_tree_error_t *error) {
    if (!st || !st->tree) {
        if (error) *error = SEGMENT_TREE_ERROR_NULL_PTR;
        return 0;
    }
    
    if (!st->initialized) {
        if (error) *error = SEGMENT_TREE_ERROR_NOT_INITIALIZED;
        return 0;
    }
    
    if (st->config.check_bounds) {
        if (left > right || left >= st->n || right >= st->n) {
            if (error) *error = SEGMENT_TREE_ERROR_RANGE_INVALID;
            return st->config.default_value;
        }
    }
    
    int result = query_tree(st->tree, 0, 0, st->n - 1, left, right);
    
    if (error) *error = SEGMENT_TREE_OK;
    return result;
}

// 获取线段树大小
size_t segment_tree_size(const segment_tree_t *st) {
    return st ? st->n : 0;
}

// 获取线段树状态
bool segment_tree_get_state(const segment_tree_t *st, segment_tree_state_t *state, segment_tree_error_t *error) {
    if (!st || !state) {
        if (error) *error = SEGMENT_TREE_ERROR_NULL_PTR;
        return false;
    }
    
    state->size = st->n;
    state->tree_size = st->tree_size;
    state->operation = st->config.operation;
    state->initialized = st->initialized;
    state->use_lazy = st->config.use_lazy_propagation;
    
    if (error) *error = SEGMENT_TREE_OK;
    return true;
}

// 释放线段树
void segment_tree_free(segment_tree_t *st) {
    if (st) {
        if (st->tree) {
            free(st->tree);
            st->tree = NULL;
        }
        if (st->lazy) {
            free(st->lazy);
            st->lazy = NULL;
        }
        st->initialized = false;
        st->n = 0;
        st->tree_size = 0;
    }
}

// 批量更新
bool segment_tree_batch_update(segment_tree_t *st, const size_t *indices, const int *values, size_t count, segment_tree_error_t *error) {
    if (!st || !indices || !values) {
        if (error) *error = SEGMENT_TREE_ERROR_NULL_PTR;
        return false;
    }
    
    for (size_t i = 0; i < count; i++) {
        if (!segment_tree_update_ex(st, indices[i], values[i], error)) {
            return false;
        }
    }
    
    if (error) *error = SEGMENT_TREE_OK;
    return true;
}

// 批量查询
bool segment_tree_batch_query(segment_tree_t *st, const size_t *left, const size_t *right, int *results, size_t count, segment_tree_error_t *error) {
    if (!st || !left || !right || !results) {
        if (error) *error = SEGMENT_TREE_ERROR_NULL_PTR;
        return false;
    }
    
    for (size_t i = 0; i < count; i++) {
        results[i] = segment_tree_query_ex(st, left[i], right[i], error);
        if (error && *error != SEGMENT_TREE_OK) {
            return false;
        }
    }
    
    if (error) *error = SEGMENT_TREE_OK;
    return true;
}

// 重置线段树
bool segment_tree_reset(segment_tree_t *st, int *arr, size_t n, segment_tree_error_t *error) {
    if (!st) {
        if (error) *error = SEGMENT_TREE_ERROR_NULL_PTR;
        return false;
    }
    
    if (!arr || n == 0) {
        if (error) *error = SEGMENT_TREE_ERROR_INVALID_ARGS;
        return false;
    }
    
    // 重新初始化
    return segment_tree_init_ex(st, arr, n, &st->config, error);
}

// 检查线段树是否已初始化
bool segment_tree_is_initialized(const segment_tree_t *st) {
    return st && st->initialized;
}

// 获取错误信息
const char* segment_tree_strerror(segment_tree_error_t error) {
    switch (error) {
        case SEGMENT_TREE_OK:
            return "Success";
        case SEGMENT_TREE_ERROR_NULL_PTR:
            return "Null pointer";
        case SEGMENT_TREE_ERROR_INVALID_ARGS:
            return "Invalid arguments";
        case SEGMENT_TREE_ERROR_SIZE_TOO_SMALL:
            return "Size too small";
        case SEGMENT_TREE_ERROR_INDEX_OUT_OF_BOUNDS:
            return "Index out of bounds";
        case SEGMENT_TREE_ERROR_RANGE_INVALID:
            return "Invalid range";
        case SEGMENT_TREE_ERROR_MEMORY_ALLOC:
            return "Memory allocation failed";
        case SEGMENT_TREE_ERROR_NOT_INITIALIZED:
            return "Not initialized";
        default:
            return "Unknown error";
    }
}
