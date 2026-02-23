#ifndef C_UTILS_BELLMAN_FORD_H
#define C_UTILS_BELLMAN_FORD_H

#include <stdbool.h>

// 边的定义
typedef struct { int u, v, w; } bf_edge_t;

// 返回是否存在负环，dist 数组存储最短路径
bool bellman_ford(int n, int m, bf_edge_t *edges, int start, int *dist);

// 带路径重建的 Bellman-Ford 算法
// pred 数组存储前驱节点，用于重建路径
bool bellman_ford_with_path(int n, int m, bf_edge_t *edges, int start, int *dist, int *pred);

// 重建从 start 到 end 的最短路径
// path 数组存储路径节点，path_len 存储路径长度
// 返回是否成功重建路径
bool bellman_ford_reconstruct_path(int start, int end, const int *pred, int *path, int *path_len, int max_path_len);

// 验证输入参数是否有效
bool bellman_ford_validate_input(int n, int m, bf_edge_t *edges, int start, int *dist);

#endif // C_UTILS_BELLMAN_FORD_H
