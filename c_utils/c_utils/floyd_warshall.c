#include "floyd_warshall.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// 计算全源最短路径
bool floyd_warshall(size_t n, const int **adj, floyd_result_t *result, floyd_error_t *error) {
    if (n == 0 || !adj || !result) {
        if (error) *error = FLOYD_ERROR_INVALID_PARAM;
        return false;
    }
    
    // 初始化结果结构
    result->dist = calloc(n, sizeof(int*));
    result->next = calloc(n, sizeof(int*));
    
    if (!result->dist || !result->next) {
        free(result->dist);
        free(result->next);
        if (error) *error = FLOYD_ERROR_MEMORY_ALLOC;
        return false;
    }
    
    for (size_t i = 0; i < n; i++) {
        result->dist[i] = malloc(n * sizeof(int));
        result->next[i] = malloc(n * sizeof(int));
        
        if (!result->dist[i] || !result->next[i]) {
            for (size_t j = 0; j <= i; j++) {
                free(result->dist[j]);
                free(result->next[j]);
            }
            free(result->dist);
            free(result->next);
            if (error) *error = FLOYD_ERROR_MEMORY_ALLOC;
            return false;
        }
    }
    
    result->nodes = n;
    result->has_negative_cycle = false;
    result->has_error = false;
    result->error = FLOYD_OK;
    memset(result->error_msg, 0, sizeof(result->error_msg));
    
    // 初始化距离矩阵和路径矩阵
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            result->dist[i][j] = adj[i][j];
            if (i == j) {
                result->next[i][j] = -1; // 自身
            } else if (adj[i][j] != FLOYD_INF) {
                result->next[i][j] = (int)j;
            } else {
                result->next[i][j] = -1; // 无路径
            }
        }
    }
    
    // Floyd-Warshall 主算法
    for (size_t k = 0; k < n; k++) {
        for (size_t i = 0; i < n; i++) {
            for (size_t j = 0; j < n; j++) {
                if (result->dist[i][k] != FLOYD_INF && 
                    result->dist[k][j] != FLOYD_INF &&
                    result->dist[i][k] + result->dist[k][j] < result->dist[i][j]) {
                    result->dist[i][j] = result->dist[i][k] + result->dist[k][j];
                    result->next[i][j] = result->next[i][k];
                }
            }
        }
    }
    
    // 检测负权环
    for (size_t i = 0; i < n; i++) {
        if (result->dist[i][i] < 0) {
            result->has_negative_cycle = true;
            result->has_error = true;
            result->error = FLOYD_ERROR_NEGATIVE_CYCLE;
            snprintf(result->error_msg, sizeof(result->error_msg), "Negative cycle detected at node %zu", i);
            if (error) *error = FLOYD_ERROR_NEGATIVE_CYCLE;
            return true; // 返回true但标记有负权环
        }
    }
    
    if (error) *error = FLOYD_OK;
    return true;
}

// 释放 Floyd-Warshall 结果
void floyd_warshall_free(floyd_result_t *result) {
    if (!result) return;
    
    if (result->dist) {
        for (size_t i = 0; i < result->nodes; i++) {
            free(result->dist[i]);
        }
        free(result->dist);
        result->dist = NULL;
    }
    
    if (result->next) {
        for (size_t i = 0; i < result->nodes; i++) {
            free(result->next[i]);
        }
        free(result->next);
        result->next = NULL;
    }
    
    result->nodes = 0;
    result->has_negative_cycle = false;
}

// 重建路径
bool floyd_warshall_reconstruct_path(const floyd_result_t *result, size_t start, size_t end, 
                                     size_t *path, size_t *path_len, size_t max_path_len, 
                                     floyd_error_t *error) {
    if (!result || !path || !path_len) {
        if (error) *error = FLOYD_ERROR_INVALID_PARAM;
        return false;
    }
    
    if (start >= result->nodes || end >= result->nodes) {
        if (error) *error = FLOYD_ERROR_INVALID_NODE;
        return false;
    }
    
    if (result->dist[start][end] == FLOYD_INF) {
        if (error) *error = FLOYD_ERROR_INVALID_NODE;
        return false;
    }
    
    // 构建路径
    size_t len = 0;
    size_t current = start;
    
    while (current != end && len < max_path_len) {
        path[len++] = current;
        current = (size_t)result->next[current][end];
        if (current == (size_t)-1) {
            if (error) *error = FLOYD_ERROR_INVALID_NODE;
            return false;
        }
    }
    
    if (len < max_path_len) {
        path[len++] = end;
    }
    
    *path_len = len;
    
    if (error) *error = FLOYD_OK;
    return true;
}

// 获取最短路径长度
int floyd_warshall_get_distance(const floyd_result_t *result, size_t start, size_t end, floyd_error_t *error) {
    if (!result) {
        if (error) *error = FLOYD_ERROR_INVALID_PARAM;
        return FLOYD_INF;
    }
    
    if (start >= result->nodes || end >= result->nodes) {
        if (error) *error = FLOYD_ERROR_INVALID_NODE;
        return FLOYD_INF;
    }
    
    if (error) *error = FLOYD_OK;
    return result->dist[start][end];
}

// 检查是否存在路径
bool floyd_warshall_has_path(const floyd_result_t *result, size_t start, size_t end, floyd_error_t *error) {
    if (!result) {
        if (error) *error = FLOYD_ERROR_INVALID_PARAM;
        return false;
    }
    
    if (start >= result->nodes || end >= result->nodes) {
        if (error) *error = FLOYD_ERROR_INVALID_NODE;
        return false;
    }
    
    if (error) *error = FLOYD_OK;
    return result->dist[start][end] != FLOYD_INF;
}

// 获取错误信息
const char* floyd_warshall_strerror(floyd_error_t error) {
    switch (error) {
        case FLOYD_OK: return "Success";
        case FLOYD_ERROR_INVALID_PARAM: return "Invalid parameter";
        case FLOYD_ERROR_INVALID_NODE: return "Invalid node";
        case FLOYD_ERROR_MEMORY_ALLOC: return "Memory allocation failed";
        case FLOYD_ERROR_NEGATIVE_CYCLE: return "Negative cycle detected";
        case FLOYD_ERROR_EMPTY_GRAPH: return "Empty graph";
        default: return "Unknown error";
    }
}
