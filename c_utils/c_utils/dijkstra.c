#include "dijkstra.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>

// 创建图
dijkstra_graph_t* dijkstra_graph_create(size_t nodes, dijkstra_error_t *error) {
    if (nodes == 0) {
        if (error) *error = DIJKSTRA_ERROR_INVALID_PARAM;
        return NULL;
    }
    
    dijkstra_graph_t *graph = calloc(1, sizeof(dijkstra_graph_t));
    if (!graph) {
        if (error) *error = DIJKSTRA_ERROR_MEMORY_ALLOC;
        return NULL;
    }
    
    graph->adj = calloc(nodes, sizeof(dijkstra_edge_s*));
    if (!graph->adj) {
        free(graph);
        if (error) *error = DIJKSTRA_ERROR_MEMORY_ALLOC;
        return NULL;
    }
    
    graph->nodes = nodes;
    graph->capacity = nodes;
    
    if (error) *error = DIJKSTRA_OK;
    return graph;
}

// 释放图
void dijkstra_graph_free(dijkstra_graph_t *graph) {
    if (!graph) return;
    
    for (size_t i = 0; i < graph->nodes; i++) {
        dijkstra_edge_s *edge = graph->adj[i];
        while (edge) {
            dijkstra_edge_s *next = edge->next;
            free(edge);
            edge = next;
        }
    }
    
    free(graph->adj);
    free(graph);
}

// 添加边
bool dijkstra_graph_add_edge(dijkstra_graph_t *graph, int src, int dest, int weight, dijkstra_error_t *error) {
    if (!graph) {
        if (error) *error = DIJKSTRA_ERROR_INVALID_PARAM;
        return false;
    }
    
    if (src < 0 || src >= (int)graph->nodes || dest < 0 || dest >= (int)graph->nodes) {
        if (error) *error = DIJKSTRA_ERROR_INVALID_NODE;
        return false;
    }
    
    dijkstra_edge_s *new_edge = malloc(sizeof(dijkstra_edge_s));
    if (!new_edge) {
        if (error) *error = DIJKSTRA_ERROR_MEMORY_ALLOC;
        return false;
    }
    
    new_edge->dest = dest;
    new_edge->weight = weight;
    new_edge->next = graph->adj[src];
    graph->adj[src] = new_edge;
    
    if (error) *error = DIJKSTRA_OK;
    return true;
}

// 执行 Dijkstra 算法
bool dijkstra(dijkstra_graph_t *graph, int start, int *dist, int *pred, dijkstra_error_t *error) {
    if (!graph || !dist) {
        if (error) *error = DIJKSTRA_ERROR_INVALID_PARAM;
        return false;
    }
    
    if (start < 0 || start >= (int)graph->nodes) {
        if (error) *error = DIJKSTRA_ERROR_INVALID_NODE;
        return false;
    }
    
    bool *visited = calloc(graph->nodes, sizeof(bool));
    if (!visited) {
        if (error) *error = DIJKSTRA_ERROR_MEMORY_ALLOC;
        return false;
    }
    
    // 初始化
    for (size_t i = 0; i < graph->nodes; i++) {
        dist[i] = INT_MAX;
        if (pred) pred[i] = -1;
    }
    dist[start] = 0;
    
    // 主循环
    for (size_t count = 0; count < graph->nodes - 1; count++) {
        // 找到最小距离节点
        int min_dist = INT_MAX;
        int u = -1;
        for (size_t v = 0; v < graph->nodes; v++) {
            if (!visited[v] && dist[v] < min_dist) {
                min_dist = dist[v];
                u = (int)v;
            }
        }
        
        if (u == -1) break; // 所有可达节点都已处理
        
        visited[u] = true;
        
        // 更新邻接节点距离
        dijkstra_edge_s *edge = graph->adj[u];
        while (edge) {
            int v = edge->dest;
            if (!visited[v] && dist[u] != INT_MAX && 
                dist[u] + edge->weight < dist[v]) {
                dist[v] = dist[u] + edge->weight;
                if (pred) pred[v] = u;
            }
            edge = edge->next;
        }
    }
    
    free(visited);
    
    if (error) *error = DIJKSTRA_OK;
    return true;
}

// 重建路径
bool dijkstra_reconstruct_path(const int *pred, int start, int end, int *path, size_t *path_len, size_t max_path_len) {
    if (!pred || !path || !path_len) {
        return false;
    }
    
    if (pred[end] == -1 && end != start) {
        return false;
    }
    
    // 反向构建路径
    int temp_path[max_path_len];
    size_t len = 0;
    int current = end;
    
    while (current != -1 && len < max_path_len) {
        temp_path[len++] = current;
        if (current == start) break;
        current = pred[current];
    }
    
    if (current != start) {
        return false;
    }
    
    // 反转路径
    for (size_t i = 0; i < len; i++) {
        path[i] = temp_path[len - 1 - i];
    }
    
    *path_len = len;
    return true;
}

// 获取错误信息
const char* dijkstra_strerror(dijkstra_error_t error) {
    switch (error) {
        case DIJKSTRA_OK: return "Success";
        case DIJKSTRA_ERROR_INVALID_PARAM: return "Invalid parameter";
        case DIJKSTRA_ERROR_INVALID_NODE: return "Invalid node";
        case DIJKSTRA_ERROR_NO_PATH: return "No path exists";
        case DIJKSTRA_ERROR_MEMORY_ALLOC: return "Memory allocation failed";
        case DIJKSTRA_ERROR_GRAPH_EMPTY: return "Graph is empty";
        default: return "Unknown error";
    }
}
