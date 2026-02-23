#include "bellman_ford.h"
#include <limits.h>
#include <string.h>

// 验证输入参数是否有效
bool bellman_ford_validate_input(int n, int m, bf_edge_t *edges, int start, int *dist) {
    if (n <= 0 || m < 0 || !edges || !dist || start < 0 || start >= n) {
        return false;
    }
    
    // 验证边的有效性
    for (int i = 0; i < m; i++) {
        if (edges[i].u < 0 || edges[i].u >= n || edges[i].v < 0 || edges[i].v >= n) {
            return false;
        }
    }
    
    return true;
}

// 基本的 Bellman-Ford 算法
bool bellman_ford(int n, int m, bf_edge_t *edges, int start, int *dist) {
    if (!bellman_ford_validate_input(n, m, edges, start, dist)) {
        return false;
    }
    
    for (int i = 0; i < n; i++) dist[i] = INT_MAX;
    dist[start] = 0;
    for (int i = 1; i <= n - 1; i++) {
        for (int j = 0; j < m; j++) {
            int u = edges[j].u, v = edges[j].v, w = edges[j].w;
            if (dist[u] != INT_MAX && dist[u] + w < dist[v]) dist[v] = dist[u] + w;
        }
    }
    for (int j = 0; j < m; j++) {
        int u = edges[j].u, v = edges[j].v, w = edges[j].w;
        if (dist[u] != INT_MAX && dist[u] + w < dist[v]) return true; // Negative cycle
    }
    return false;
}

// 带路径重建的 Bellman-Ford 算法
bool bellman_ford_with_path(int n, int m, bf_edge_t *edges, int start, int *dist, int *pred) {
    if (!bellman_ford_validate_input(n, m, edges, start, dist) || !pred) {
        return false;
    }
    
    // 初始化距离和前驱数组
    for (int i = 0; i < n; i++) {
        dist[i] = INT_MAX;
        pred[i] = -1;
    }
    dist[start] = 0;
    
    // 松弛操作
    for (int i = 1; i <= n - 1; i++) {
        for (int j = 0; j < m; j++) {
            int u = edges[j].u, v = edges[j].v, w = edges[j].w;
            if (dist[u] != INT_MAX && dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                pred[v] = u;
            }
        }
    }
    
    // 检测负环
    for (int j = 0; j < m; j++) {
        int u = edges[j].u, v = edges[j].v, w = edges[j].w;
        if (dist[u] != INT_MAX && dist[u] + w < dist[v]) {
            return true; // Negative cycle
        }
    }
    
    return false;
}

// 重建从 start 到 end 的最短路径
bool bellman_ford_reconstruct_path(int start, int end, const int *pred, int *path, int *path_len, int max_path_len) {
    if (!pred || !path || !path_len || max_path_len <= 0) {
        return false;
    }
    
    if (start == end) {
        path[0] = start;
        *path_len = 1;
        return true;
    }
    
    // 检查是否存在路径
    if (pred[end] == -1) {
        *path_len = 0;
        return false;
    }
    
    // 首先反向收集路径
    int temp_path[max_path_len];
    int temp_len = 0;
    int current = end;
    
    while (current != start && temp_len < max_path_len) {
        temp_path[temp_len++] = current;
        current = pred[current];
        
        if (current == -1) {
            *path_len = 0;
            return false;
        }
    }
    
    if (temp_len >= max_path_len) {
        *path_len = 0;
        return false;
    }
    
    // 添加起点
    temp_path[temp_len++] = start;
    
    // 反转路径
    for (int i = 0; i < temp_len; i++) {
        path[i] = temp_path[temp_len - 1 - i];
    }
    
    *path_len = temp_len;
    return true;
}
