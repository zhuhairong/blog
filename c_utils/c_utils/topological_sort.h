#ifndef C_UTILS_TOPOLOGICAL_SORT_H
#define C_UTILS_TOPOLOGICAL_SORT_H

#include <stddef.h>
#include <stdbool.h>

// 拓扑排序错误码
typedef enum {
    TOPOLOGICAL_SORT_OK = 0,
    TOPOLOGICAL_SORT_INVALID_PARAMS = -1,
    TOPOLOGICAL_SORT_CYCLE_DETECTED = -2,
    TOPOLOGICAL_SORT_MEMORY_ERROR = -3,
    TOPOLOGICAL_SORT_OVERFLOW = -4
} topological_sort_error_t;

// 拓扑排序配置选项
typedef struct {
    bool detect_cycles;     // 是否检测环
    bool use_adjacency_list; // 是否使用邻接表 (更高效)
    size_t max_nodes;       // 最大节点数
} topological_sort_config_t;

// 邻接表节点
typedef struct adj_list_node {
    int dest;
    struct adj_list_node* next;
} adj_list_node_t;

// 邻接表
typedef struct {
    adj_list_node_t* head;
} adj_list_t;

// 拓扑排序上下文
typedef struct {
    topological_sort_config_t config;
    adj_list_t* adj_list;
    int* in_degree;
    int* result;
    size_t result_size;
    topological_sort_error_t last_error;
} topological_sort_ctx_t;

// 创建拓扑排序上下文
topological_sort_error_t topological_sort_create(topological_sort_ctx_t** ctx, const topological_sort_config_t* config);

// 销毁拓扑排序上下文
void topological_sort_destroy(topological_sort_ctx_t* ctx);

// 添加边
// 返回 TOPOLOGICAL_SORT_OK 表示成功，其他值表示错误
topological_sort_error_t topological_sort_add_edge(topological_sort_ctx_t* ctx, int src, int dest);

// 执行拓扑排序 (Kahn 算法)
// 返回 TOPOLOGICAL_SORT_OK 表示成功，TOPOLOGICAL_SORT_CYCLE_DETECTED 表示检测到环
// result 将包含排序结果，size 是结果大小
topological_sort_error_t topological_sort_execute(topological_sort_ctx_t* ctx, int** result, size_t* size);

// 执行拓扑排序 (基于邻接矩阵，向后兼容)
// 返回 TOPOLOGICAL_SORT_OK 表示成功，TOPOLOGICAL_SORT_CYCLE_DETECTED 表示检测到环
// 如果返回 false 表示失败
bool topological_sort(int n, int adj[10][10], int result[10]);

// 获取最后一次错误信息
const char* topological_sort_strerror(topological_sort_error_t error);

#endif // C_UTILS_TOPOLOGICAL_SORT_H
