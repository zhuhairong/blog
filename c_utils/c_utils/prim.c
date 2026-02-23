#include "prim.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <limits.h>

// 获取默认配置
prim_config_t prim_default_config(void) {
    prim_config_t config = {
        .max_nodes = PRIM_DEFAULT_MAX_NODES,
        .infinity = PRIM_DEFAULT_INFINITY,
        .directed = false,
        .compute_edges = true
    };
    return config;
}

// 创建图
prim_graph_t* prim_graph_create(int n, const prim_config_t *config, prim_error_t *error) {
    if (n <= 0 || n > (int)PRIM_DEFAULT_MAX_NODES) {
        if (error) *error = PRIM_ERROR_TOO_MANY_NODES;
        return NULL;
    }
    
    prim_graph_t *g = calloc(1, sizeof(prim_graph_t));
    if (!g) {
        if (error) *error = PRIM_ERROR_NULL_PTR;
        return NULL;
    }
    
    g->n = n;
    g->owns_memory = true;
    
    if (config) {
        g->config = *config;
    } else {
        g->config = prim_default_config();
    }
    
    // 分配邻接矩阵
    g->adj = calloc((size_t)n, sizeof(int*));
    if (!g->adj) {
        free(g);
        if (error) *error = PRIM_ERROR_NULL_PTR;
        return NULL;
    }
    
    for (int i = 0; i < n; i++) {
        g->adj[i] = calloc((size_t)n, sizeof(int));
        if (!g->adj[i]) {
            for (int j = 0; j < i; j++) {
                free(g->adj[j]);
            }
            free(g->adj);
            free(g);
            if (error) *error = PRIM_ERROR_NULL_PTR;
            return NULL;
        }
    }
    
    if (error) *error = PRIM_OK;
    return g;
}

// 销毁图
void prim_graph_destroy(prim_graph_t *g) {
    if (!g) return;
    
    if (g->adj && g->owns_memory) {
        for (int i = 0; i < g->n; i++) {
            free(g->adj[i]);
        }
        free(g->adj);
    }
    
    free(g);
}

// 添加边
bool prim_graph_add_edge(prim_graph_t *g, int src, int dest, int weight, prim_error_t *error) {
    if (!g || !g->adj) {
        if (error) *error = PRIM_ERROR_NULL_PTR;
        return false;
    }
    
    if (src < 0 || src >= g->n || dest < 0 || dest >= g->n) {
        if (error) *error = PRIM_ERROR_INVALID_GRAPH;
        return false;
    }
    
    g->adj[src][dest] = weight;
    
    if (!g->config.directed) {
        g->adj[dest][src] = weight;
    }
    
    if (error) *error = PRIM_OK;
    return true;
}

// 获取边权重
bool prim_graph_get_edge(const prim_graph_t *g, int src, int dest, int *weight, prim_error_t *error) {
    if (!g || !weight) {
        if (error) *error = PRIM_ERROR_NULL_PTR;
        return false;
    }
    
    if (src < 0 || src >= g->n || dest < 0 || dest >= g->n) {
        if (error) *error = PRIM_ERROR_INVALID_GRAPH;
        return false;
    }
    
    *weight = g->adj[src][dest];
    
    if (error) *error = PRIM_OK;
    return true;
}

// 计算最小生成树
int prim_mst(prim_graph_t *g, prim_error_t *error) {
    if (!g) {
        if (error) *error = PRIM_ERROR_NULL_PTR;
        return -1;
    }
    
    prim_result_t result;
    if (!prim_mst_ex(g, &result, error)) {
        return -1;
    }
    
    int total_weight = result.total_weight;
    prim_result_free(&result);
    
    return total_weight;
}

// 计算最小生成树（带详细结果）
bool prim_mst_ex(prim_graph_t *g, prim_result_t *result, prim_error_t *error) {
    if (!g || !result) {
        if (error) *error = PRIM_ERROR_NULL_PTR;
        return false;
    }
    
    int n = g->n;
    int *parent = malloc((size_t)n * sizeof(int));
    int *key = malloc((size_t)n * sizeof(int));
    bool *mst_set = calloc((size_t)n, sizeof(bool));
    
    if (!parent || !key || !mst_set) {
        free(parent);
        free(key);
        free(mst_set);
        if (error) *error = PRIM_ERROR_NULL_PTR;
        return false;
    }
    
    // 初始化
    for (int i = 0; i < n; i++) {
        key[i] = g->config.infinity;
        parent[i] = -1;
    }
    key[0] = 0;
    
    // 主循环
    for (int count = 0; count < n - 1; count++) {
        // 找到最小键值节点
        int min = g->config.infinity;
        int u = -1;
        for (int v = 0; v < n; v++) {
            if (!mst_set[v] && key[v] < min) {
                min = key[v];
                u = v;
            }
        }
        
        if (u == -1) break; // 图不连通
        
        mst_set[u] = true;
        
        // 更新邻接节点键值
        for (int v = 0; v < n; v++) {
            if (g->adj[u][v] && !mst_set[v] && 
                g->adj[u][v] < key[v] && g->adj[u][v] != 0) {
                parent[v] = u;
                key[v] = g->adj[u][v];
            }
        }
    }
    
    // 计算总权重
    result->total_weight = 0;
    for (int i = 1; i < n; i++) {
        if (parent[i] != -1) {
            result->total_weight += g->adj[i][parent[i]];
        }
    }
    
    // 填充边信息
    if (g->config.compute_edges) {
        result->edges = calloc((size_t)(n - 1), sizeof(prim_edge_t));
        if (result->edges) {
            result->edge_count = 0;
            for (int i = 1; i < n; i++) {
                if (parent[i] != -1) {
                    result->edges[result->edge_count].src = parent[i];
                    result->edges[result->edge_count].dest = i;
                    result->edges[result->edge_count].weight = g->adj[i][parent[i]];
                    result->edge_count++;
                }
            }
        }
    } else {
        result->edges = NULL;
        result->edge_count = 0;
    }
    
    result->connected = (result->edge_count == (size_t)(n - 1));
    
    free(parent);
    free(key);
    free(mst_set);
    
    if (error) *error = PRIM_OK;
    return true;
}

// 释放结果
void prim_result_free(prim_result_t *result) {
    if (!result) return;
    
    if (result->edges) {
        free(result->edges);
        result->edges = NULL;
    }
    result->edge_count = 0;
}

// 检查图是否连通
bool prim_graph_is_connected(const prim_graph_t *g, prim_error_t *error) {
    if (!g) {
        if (error) *error = PRIM_ERROR_NULL_PTR;
        return false;
    }
    
    bool *visited = calloc((size_t)g->n, sizeof(bool));
    if (!visited) {
        if (error) *error = PRIM_ERROR_NULL_PTR;
        return false;
    }
    
    // BFS/DFS 检查连通性
    int stack[g->n];
    int top = 0;
    stack[top++] = 0;
    visited[0] = true;
    int visited_count = 1;
    
    while (top > 0) {
        int u = stack[--top];
        for (int v = 0; v < g->n; v++) {
            if (g->adj[u][v] && !visited[v]) {
                visited[v] = true;
                stack[top++] = v;
                visited_count++;
            }
        }
    }
    
    free(visited);
    
    if (error) *error = PRIM_OK;
    return visited_count == g->n;
}

// 获取图的节点数
int prim_graph_get_node_count(const prim_graph_t *g) {
    return g ? g->n : 0;
}

// 打印图
void prim_graph_print(const prim_graph_t *g) {
    if (!g) {
        printf("Graph is NULL\n");
        return;
    }
    
    printf("Graph (%d nodes):\n", g->n);
    for (int i = 0; i < g->n; i++) {
        for (int j = 0; j < g->n; j++) {
            if (g->adj[i][j]) {
                printf("  %d -> %d: %d\n", i, j, g->adj[i][j]);
            }
        }
    }
}

// 打印 MST 结果
void prim_result_print(const prim_result_t *result) {
    if (!result) {
        printf("Result is NULL\n");
        return;
    }
    
    printf("MST Result:\n");
    printf("  Total weight: %d\n", result->total_weight);
    printf("  Connected: %s\n", result->connected ? "yes" : "no");
    printf("  Edges (%zu):\n", result->edge_count);
    
    for (size_t i = 0; i < result->edge_count; i++) {
        printf("    %d -> %d: %d\n", 
               result->edges[i].src, 
               result->edges[i].dest, 
               result->edges[i].weight);
    }
}
