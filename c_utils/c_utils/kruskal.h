#ifndef C_UTILS_KRUSKAL_H
#define C_UTILS_KRUSKAL_H

#include <stddef.h>
#include <stdbool.h>

// Kruskal 错误码
typedef enum {
    KRUSKAL_OK = 0,
    KRUSKAL_INVALID_INPUT = -1,
    KRUSKAL_MEMORY_ERROR = -2,
    KRUSKAL_DISCONNECTED = -3,
    KRUSKAL_EDGE_ERROR = -4,
    KRUSKAL_VERTEX_ERROR = -5
} kruskal_error_t;

// Kruskal 配置
typedef struct {
    bool enable_validation;
    bool return_edges;
    size_t max_edges;
} kruskal_config_t;

// 边结构
typedef struct {
    int u;      // 起点
    int v;      // 终点
    int w;      // 权重
} kruskal_edge_t;

// 最小生成树结果
typedef struct {
    int total_weight;
    kruskal_edge_t *edges;
    size_t edge_count;
    bool has_error;
    kruskal_error_t error;
    char error_msg[256];
} kruskal_result_t;

// 返回 MST 的总权重
int kruskal_mst(int n, int m, kruskal_edge_t *edges);

// 带错误处理的 Kruskal 算法
int kruskal_mst_ex(int n, int m, kruskal_edge_t *edges, kruskal_error_t *error);

// 带配置的 Kruskal 算法
kruskal_result_t *kruskal_mst_with_config(int n, int m, kruskal_edge_t *edges, 
                                          const kruskal_config_t *config, 
                                          kruskal_error_t *error);

// 释放 Kruskal 结果
void kruskal_free_result(kruskal_result_t *result);

// 获取默认配置
void kruskal_get_default_config(kruskal_config_t *config);

#endif // C_UTILS_KRUSKAL_H
