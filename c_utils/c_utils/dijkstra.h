#ifndef C_UTILS_DIJKSTRA_H
#define C_UTILS_DIJKSTRA_H

#include <stddef.h>
#include <stdbool.h>

// Dijkstra 错误码
typedef enum {
    DIJKSTRA_OK = 0,
    DIJKSTRA_ERROR_INVALID_PARAM,
    DIJKSTRA_ERROR_INVALID_NODE,
    DIJKSTRA_ERROR_NO_PATH,
    DIJKSTRA_ERROR_MEMORY_ALLOC,
    DIJKSTRA_ERROR_GRAPH_EMPTY
} dijkstra_error_t;

// 图的边
typedef struct {
    int dest;
    int weight;
    struct dijkstra_edge_s *next;
} dijkstra_edge_s;

// 图结构
typedef struct {
    dijkstra_edge_s **adj;
    size_t nodes;
    size_t capacity;
} dijkstra_graph_t;

// 创建图
// nodes: 节点数量
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回图指针，失败返回 NULL
dijkstra_graph_t* dijkstra_graph_create(size_t nodes, dijkstra_error_t *error);

// 释放图
// graph: 图
void dijkstra_graph_free(dijkstra_graph_t *graph);

// 添加边
// graph: 图
// src: 源节点
// dest: 目标节点
// weight: 边权重
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool dijkstra_graph_add_edge(dijkstra_graph_t *graph, int src, int dest, int weight, dijkstra_error_t *error);

// 执行 Dijkstra 算法
// graph: 图
// start: 起始节点
// dist: 距离数组（输出参数，需要足够大）
// pred: 前驱节点数组（输出参数，用于路径重建，可为 NULL）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool dijkstra(dijkstra_graph_t *graph, int start, int *dist, int *pred, dijkstra_error_t *error);

// 重建路径
// pred: 前驱节点数组
// start: 起始节点
// end: 目标节点
// path: 路径数组（输出参数）
// path_len: 路径长度（输出参数）
// max_path_len: 最大路径长度
// 返回: 成功返回 true，失败返回 false
bool dijkstra_reconstruct_path(const int *pred, int start, int end, int *path, size_t *path_len, size_t max_path_len);

// 获取错误信息
// error: 错误码
// 返回: 错误信息字符串
const char* dijkstra_strerror(dijkstra_error_t error);

#endif // C_UTILS_DIJKSTRA_H
