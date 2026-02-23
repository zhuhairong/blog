#include "fenwick_tree.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// 创建 Fenwick Tree
fenwick_tree_t* fenwick_tree_create(size_t n, fenwick_type_t type, fenwick_error_t *error) {
    fenwick_tree_t *ft = malloc(sizeof(fenwick_tree_t));
    if (!ft) {
        if (error) *error = FENWICK_ERROR_MEMORY_ALLOC;
        return NULL;
    }
    
    ft->n = n;
    ft->capacity = n + 1; // 1-based indexing
    ft->type = type;
    ft->has_error = false;
    ft->error = FENWICK_OK;
    strcpy(ft->error_msg, "Success");
    
    size_t elem_size = (type == FENWICK_TYPE_DOUBLE || type == FENWICK_TYPE_FLOAT) ? sizeof(double) : sizeof(long long);
    ft->tree = calloc(ft->capacity, elem_size);
    if (!ft->tree) {
        free(ft);
        if (error) *error = FENWICK_ERROR_MEMORY_ALLOC;
        return NULL;
    }
    
    if (error) *error = FENWICK_OK;
    return ft;
}

// 释放 Fenwick Tree
void fenwick_tree_free(fenwick_tree_t *ft) {
    if (ft) {
        free(ft->tree);
        free(ft);
    }
}

// 初始化 Fenwick Tree
bool fenwick_tree_init(fenwick_tree_t *ft, size_t n, fenwick_type_t type, fenwick_error_t *error) {
    if (!ft) {
        if (error) *error = FENWICK_ERROR_INVALID_PARAM;
        return false;
    }
    
    free(ft->tree);
    ft->n = n;
    ft->capacity = n + 1;
    ft->type = type;
    ft->has_error = false;
    ft->error = FENWICK_OK;
    
    size_t elem_size = (type == FENWICK_TYPE_DOUBLE || type == FENWICK_TYPE_FLOAT) ? sizeof(double) : sizeof(long long);
    ft->tree = calloc(ft->capacity, elem_size);
    if (!ft->tree) {
        if (error) *error = FENWICK_ERROR_MEMORY_ALLOC;
        return false;
    }
    
    if (error) *error = FENWICK_OK;
    return true;
}

// 更新 Fenwick Tree
bool fenwick_tree_update(fenwick_tree_t *ft, size_t idx, double val, fenwick_error_t *error) {
    if (!ft || !ft->tree) {
        if (error) *error = FENWICK_ERROR_INVALID_PARAM;
        return false;
    }
    
    if (idx == 0 || idx > ft->n) {
        if (error) *error = FENWICK_ERROR_INVALID_INDEX;
        return false;
    }
    
    if (ft->type == FENWICK_TYPE_DOUBLE || ft->type == FENWICK_TYPE_FLOAT) {
        double *tree = (double*)ft->tree;
        while (idx <= ft->n) {
            tree[idx] += val;
            idx += idx & (-idx);
        }
    } else {
        long long *tree = (long long*)ft->tree;
        long long v = (long long)val;
        while (idx <= ft->n) {
            tree[idx] += v;
            idx += idx & (-idx);
        }
    }
    
    if (error) *error = FENWICK_OK;
    return true;
}

// 查询前缀和
bool fenwick_tree_query(fenwick_tree_t *ft, size_t idx, double *result, fenwick_error_t *error) {
    if (!ft || !ft->tree || !result) {
        if (error) *error = FENWICK_ERROR_INVALID_PARAM;
        return false;
    }
    
    if (idx > ft->n) {
        if (error) *error = FENWICK_ERROR_OUT_OF_BOUNDS;
        return false;
    }
    
    double sum = 0;
    if (ft->type == FENWICK_TYPE_DOUBLE || ft->type == FENWICK_TYPE_FLOAT) {
        double *tree = (double*)ft->tree;
        while (idx > 0) {
            sum += tree[idx];
            idx -= idx & (-idx);
        }
    } else {
        long long *tree = (long long*)ft->tree;
        long long s = 0;
        while (idx > 0) {
            s += tree[idx];
            idx -= idx & (-idx);
        }
        sum = (double)s;
    }
    
    *result = sum;
    if (error) *error = FENWICK_OK;
    return true;
}

// 查询范围和
bool fenwick_tree_range_query(fenwick_tree_t *ft, size_t l, size_t r, double *result, fenwick_error_t *error) {
    if (!ft || !result) {
        if (error) *error = FENWICK_ERROR_INVALID_PARAM;
        return false;
    }
    
    if (l > r || l == 0 || r > ft->n) {
        if (error) *error = FENWICK_ERROR_INVALID_INDEX;
        return false;
    }
    
    double sum_r, sum_l;
    if (!fenwick_tree_query(ft, r, &sum_r, error)) return false;
    if (l > 1) {
        if (!fenwick_tree_query(ft, l - 1, &sum_l, error)) return false;
        *result = sum_r - sum_l;
    } else {
        *result = sum_r;
    }
    
    return true;
}

// 批量更新
bool fenwick_tree_batch_update(fenwick_tree_t *ft, const size_t *indices, const double *values, size_t count, fenwick_error_t *error) {
    if (!ft || !indices || !values) {
        if (error) *error = FENWICK_ERROR_INVALID_PARAM;
        return false;
    }
    
    for (size_t i = 0; i < count; i++) {
        if (!fenwick_tree_update(ft, indices[i], values[i], error)) {
            return false;
        }
    }
    
    if (error) *error = FENWICK_OK;
    return true;
}

// 批量查询
bool fenwick_tree_batch_query(fenwick_tree_t *ft, const size_t *indices, double *results, size_t count, fenwick_error_t *error) {
    if (!ft || !indices || !results) {
        if (error) *error = FENWICK_ERROR_INVALID_PARAM;
        return false;
    }
    
    for (size_t i = 0; i < count; i++) {
        if (!fenwick_tree_query(ft, indices[i], &results[i], error)) {
            return false;
        }
    }
    
    if (error) *error = FENWICK_OK;
    return true;
}

// 获取大小
size_t fenwick_tree_size(const fenwick_tree_t *ft) {
    return ft ? ft->n : 0;
}

// 获取错误信息
const char* fenwick_strerror(fenwick_error_t error) {
    switch (error) {
        case FENWICK_OK: return "Success";
        case FENWICK_ERROR_INVALID_PARAM: return "Invalid parameter";
        case FENWICK_ERROR_INVALID_INDEX: return "Invalid index";
        case FENWICK_ERROR_MEMORY_ALLOC: return "Memory allocation failed";
        case FENWICK_ERROR_EMPTY_TREE: return "Empty tree";
        case FENWICK_ERROR_OUT_OF_BOUNDS: return "Index out of bounds";
        default: return "Unknown error";
    }
}
