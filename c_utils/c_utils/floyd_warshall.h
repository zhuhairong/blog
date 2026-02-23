#ifndef C_UTILS_FLOYD_WARSHALL_H
#define C_UTILS_FLOYD_WARSHALL_H

#include <stddef.h>
#include <stdbool.h>

// Floyd-Warshall 错误码
typedef enum {
    FLOYD_OK = 0,
    FLOYD_ERROR_INVALID_PARAM,
    FLOYD_ERROR_INVALID_NODE,
    FLOYD_ERROR_MEMORY_ALLOC,
    FLOYD_ERROR_NEGATIVE_CYCLE,
    FLOYD_ERROR_EMPTY_GRAPH
} floyd_error_t;

// 无穷大值
#define FLOYD_INF 1000000

// Floyd-Warshall 结果
typedef struct {
    int **dist;
    int **next;
    size_t nodes;
    bool has_negative_cycle;
    bool has_error;
    floyd_error_t error;
    char error_msg[256];
} floyd_result_t;

// 计算全源最短路径
// n: 节点数量
// adj: 邻接矩阵
// result: 结果（输出参数）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool floyd_warshall(size_t n, const int **adj, floyd_result_t *result, floyd_error_t *error);

// 释放 Floyd-Warshall 结果
// result: 结果
void floyd_warshall_free(floyd_result_t *result);

// 重建路径
// result: Floyd-Warshall 结果
// start: 起始节点
// end: 目标节点
// path: 路径数组（输出参数）
// path_len: 路径长度（输出参数）
// max_path_len: 最大路径长度
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool floyd_warshall_reconstruct_path(const floyd_result_t *result, size_t start, size_t end, size_t *path, size_t *path_len, size_t max_path_len, floyd_error_t *error);

// 获取最短路径长度
// result: Floyd-Warshall 结果
// start: 起始节点
// end: 目标节点
// error: 错误码（输出参数，可为 NULL）
// 返回: 最短路径长度，失败返回 FLOYD_INF
int floyd_warshall_get_distance(const floyd_result_t *result, size_t start, size_t end, floyd_error_t *error);

// 检查图中是否存在负权环
// result: Floyd-Warshall 结果
// 返回: 存在负权环返回 true，否则返回 false
bool floyd_warshall_has_negative_cycle(const floyd_result_t *result);

// 检查路径是否存在
// result: Floyd-Warshall 结果
// start: 起始节点
// end: 目标节点
// error: 错误码（输出参数，可为 NULL）
// 返回: 存在路径返回 true，否则返回 false
bool floyd_warshall_has_path(const floyd_result_t *result, size_t start, size_t end, floyd_error_t *error);

// 验证输入邻接矩阵
// n: 节点数量
// adj: 邻接矩阵
// error: 错误码（输出参数，可为 NULL）
// 返回: 有效返回 true，无效返回 false
bool floyd_warshall_validate_input(size_t n, const int **adj, floyd_error_t *error);

// 获取错误信息
// error: 错误码
// 返回: 错误信息字符串
const char* floyd_warshall_strerror(floyd_error_t error);

#endif // C_UTILS_FLOYD_WARSHALL_H
