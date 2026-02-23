#include "disjoint_set_forest.h"
#include <stdlib.h>
#include <string.h>

// 创建并查集
dsf_t* dsf_create(size_t n, dsf_error_t *error) {
    if (n == 0) {
        if (error) *error = DSF_ERROR_INVALID_PARAM;
        return NULL;
    }
    
    dsf_t *dsf = calloc(1, sizeof(dsf_t));
    if (!dsf) {
        if (error) *error = DSF_ERROR_MEMORY_ALLOC;
        return NULL;
    }
    
    if (!dsf_init(dsf, n, error)) {
        free(dsf);
        return NULL;
    }
    
    return dsf;
}

// 初始化并查集
bool dsf_init(dsf_t *dsf, size_t n, dsf_error_t *error) {
    if (!dsf || n == 0) {
        if (error) *error = DSF_ERROR_INVALID_PARAM;
        return false;
    }
    
    dsf->parent = malloc(n * sizeof(int));
    dsf->rank = calloc(n, sizeof(int));
    dsf->size = malloc(n * sizeof(size_t));
    
    if (!dsf->parent || !dsf->rank || !dsf->size) {
        free(dsf->parent);
        free(dsf->rank);
        free(dsf->size);
        if (error) *error = DSF_ERROR_MEMORY_ALLOC;
        return false;
    }
    
    for (size_t i = 0; i < n; i++) {
        dsf->parent[i] = (int)i;
        dsf->size[i] = 1;
    }
    
    dsf->n = n;
    dsf->sets = n;
    dsf->has_error = false;
    dsf->error = DSF_OK;
    
    if (error) *error = DSF_OK;
    return true;
}

// 释放并查集
void dsf_free(dsf_t *dsf) {
    if (!dsf) return;
    
    free(dsf->parent);
    free(dsf->rank);
    free(dsf->size);
    
    dsf->parent = NULL;
    dsf->rank = NULL;
    dsf->size = NULL;
    dsf->n = 0;
    dsf->sets = 0;
}

// 查找元素的根
int dsf_find(dsf_t *dsf, int i, dsf_error_t *error) {
    if (!dsf) {
        if (error) *error = DSF_ERROR_INVALID_PARAM;
        return -1;
    }
    
    if (i < 0 || i >= (int)dsf->n) {
        if (error) *error = DSF_ERROR_INVALID_ELEMENT;
        return -1;
    }
    
    // 路径压缩
    if (dsf->parent[i] != i) {
        dsf->parent[i] = dsf_find(dsf, dsf->parent[i], NULL);
    }
    
    if (error) *error = DSF_OK;
    return dsf->parent[i];
}

// 合并两个集合
bool dsf_union(dsf_t *dsf, int i, int j, dsf_error_t *error) {
    if (!dsf) {
        if (error) *error = DSF_ERROR_INVALID_PARAM;
        return false;
    }
    
    if (i < 0 || i >= (int)dsf->n || j < 0 || j >= (int)dsf->n) {
        if (error) *error = DSF_ERROR_INVALID_ELEMENT;
        return false;
    }
    
    int root_i = dsf_find(dsf, i, NULL);
    int root_j = dsf_find(dsf, j, NULL);
    
    if (root_i == root_j) {
        if (error) *error = DSF_OK;
        return true; // 已经在同一集合
    }
    
    // 按秩合并
    if (dsf->rank[root_i] < dsf->rank[root_j]) {
        dsf->parent[root_i] = root_j;
        dsf->size[root_j] += dsf->size[root_i];
    } else if (dsf->rank[root_i] > dsf->rank[root_j]) {
        dsf->parent[root_j] = root_i;
        dsf->size[root_i] += dsf->size[root_j];
    } else {
        dsf->parent[root_j] = root_i;
        dsf->rank[root_i]++;
        dsf->size[root_i] += dsf->size[root_j];
    }
    
    dsf->sets--;
    
    if (error) *error = DSF_OK;
    return true;
}

// 检查两个元素是否在同一集合
bool dsf_same_set(dsf_t *dsf, int i, int j, dsf_error_t *error) {
    if (!dsf) {
        if (error) *error = DSF_ERROR_INVALID_PARAM;
        return false;
    }
    
    if (i < 0 || i >= (int)dsf->n || j < 0 || j >= (int)dsf->n) {
        if (error) *error = DSF_ERROR_INVALID_ELEMENT;
        return false;
    }
    
    bool result = dsf_find(dsf, i, NULL) == dsf_find(dsf, j, NULL);
    
    if (error) *error = DSF_OK;
    return result;
}

// 获取集合大小
size_t dsf_set_size(dsf_t *dsf, int i, dsf_error_t *error) {
    if (!dsf) {
        if (error) *error = DSF_ERROR_INVALID_PARAM;
        return 0;
    }
    
    if (i < 0 || i >= (int)dsf->n) {
        if (error) *error = DSF_ERROR_INVALID_ELEMENT;
        return 0;
    }
    
    int root = dsf_find(dsf, i, NULL);
    
    if (error) *error = DSF_OK;
    return dsf->size[root];
}

// 获取集合数量
size_t dsf_set_count(dsf_t *dsf, dsf_error_t *error) {
    if (!dsf) {
        if (error) *error = DSF_ERROR_INVALID_PARAM;
        return 0;
    }
    
    if (error) *error = DSF_OK;
    return dsf->sets;
}

// 获取错误信息
const char* dsf_strerror(dsf_error_t error) {
    switch (error) {
        case DSF_OK: return "Success";
        case DSF_ERROR_INVALID_PARAM: return "Invalid parameter";
        case DSF_ERROR_INVALID_ELEMENT: return "Invalid element";
        case DSF_ERROR_MEMORY_ALLOC: return "Memory allocation failed";
        case DSF_ERROR_EMPTY_SET: return "Empty set";
        default: return "Unknown error";
    }
}
