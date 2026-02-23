#ifndef C_UTILS_TARJAN_SCC_H
#define C_UTILS_TARJAN_SCC_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#define TARJAN_MAX_NODES 100

typedef struct {
    int n;
    int adj[TARJAN_MAX_NODES][TARJAN_MAX_NODES];
    int adj_size[TARJAN_MAX_NODES];
} tarjan_graph_t;

typedef enum {
    TARJAN_OK = 0,
    TARJAN_ERROR_INVALID_PARAMS = -1,
    TARJAN_ERROR_GRAPH_NULL = -2,
    TARJAN_ERROR_SCC = -3,
    TARJAN_ERROR_MEMORY = -4,
    TARJAN_ERROR_NODE_OUT_OF_BOUNDS = -5,
    TARJAN_ERROR_EDGE_OUT_OF_BOUNDS = -6,
    TARJAN_ERROR_GRAPH_TOO_LARGE = -7,
    TARJAN_ERROR_FILE_OPEN = -8,
    TARJAN_ERROR_FILE_READ = -9,
    TARJAN_ERROR_FILE_WRITE = -10,
    TARJAN_ERROR_GRAPH_EMPTY = -11,
    TARJAN_ERROR_DYNAMIC_ALLOC = -12,
    TARJAN_ERROR_VALIDATION = -13
} tarjan_error_t;

typedef struct {
    bool use_dynamic_memory;        // 使用动态内存
    bool enable_statistics;         // 启用统计
    bool enable_validation;         // 启用验证
    bool enable_optimizations;      // 启用优化
    size_t max_nodes;               // 最大节点数
    size_t initial_capacity;        // 初始容量
    bool allow_self_edges;          // 允许自环
    bool allow_multiple_edges;      // 允许多重边
} tarjan_config_t;

typedef struct {
    tarjan_error_t last_error;     // 最后一次错误
    size_t scc_computations;       // SCC计算次数
    size_t nodes_processed;        // 处理的节点数
    size_t edges_processed;        // 处理的边数
    bool is_initialized;           // 是否初始化
    size_t average_graph_size;     // 平均图大小
    size_t average_scc_count;      // 平均SCC数量
} tarjan_state_t;

/**
 * @brief 动态图结构体
 */
typedef struct {
    int n;
    int **adj;
    int *adj_size;
    size_t capacity;
    tarjan_config_t config;
    tarjan_state_t state;
} tarjan_dynamic_graph_t;

/**
 * @brief 返回 SCC 的数量
 * @param g 图结构体
 * @param scc_map 输出SCC映射
 * @return SCC数量
 */
int tarjan_scc(tarjan_graph_t *g, int *scc_map);

/**
 * @brief 增强版SCC计算
 * @param g 图结构体
 * @param scc_map 输出SCC映射
 * @param config 配置选项
 * @param state 状态输出
 * @return SCC数量，失败返回-1
 */
int tarjan_scc_ex(tarjan_graph_t *g, int *scc_map,
                 const tarjan_config_t *config,
                 tarjan_state_t *state);

/**
 * @brief 创建动态图
 * @param n 节点数
 * @param config 配置选项
 * @param graph 输出动态图
 * @return 错误码
 */
tarjan_error_t tarjan_graph_create(int n,
                                 const tarjan_config_t *config,
                                 tarjan_dynamic_graph_t *graph);

/**
 * @brief 释放动态图
 * @param graph 动态图
 * @return 错误码
 */
tarjan_error_t tarjan_graph_free(tarjan_dynamic_graph_t *graph);

/**
 * @brief 添加边
 * @param graph 动态图
 * @param u 起点
 * @param v 终点
 * @return 错误码
 */
tarjan_error_t tarjan_graph_add_edge(tarjan_dynamic_graph_t *graph,
                                   int u, int v);

/**
 * @brief 删除边
 * @param graph 动态图
 * @param u 起点
 * @param v 终点
 * @return 错误码
 */
tarjan_error_t tarjan_graph_remove_edge(tarjan_dynamic_graph_t *graph,
                                      int u, int v);

/**
 * @brief 计算动态图的SCC
 * @param graph 动态图
 * @param scc_map 输出SCC映射
 * @param scc_count 输出SCC数量
 * @return 错误码
 */
tarjan_error_t tarjan_graph_scc(tarjan_dynamic_graph_t *graph,
                              int *scc_map,
                              int *scc_count);

/**
 * @brief 从文件加载图
 * @param filename 文件名
 * @param config 配置选项
 * @param graph 输出动态图
 * @param state 状态输出
 * @return 错误码
 */
tarjan_error_t tarjan_graph_load_from_file(const char *filename,
                                         const tarjan_config_t *config,
                                         tarjan_dynamic_graph_t *graph,
                                         tarjan_state_t *state);

/**
 * @brief 保存图到文件
 * @param graph 动态图
 * @param filename 文件名
 * @param state 状态输出
 * @return 错误码
 */
tarjan_error_t tarjan_graph_save_to_file(const tarjan_dynamic_graph_t *graph,
                                        const char *filename,
                                        tarjan_state_t *state);

/**
 * @brief 验证图
 * @param graph 动态图
 * @param valid 输出验证结果
 * @param state 状态输出
 * @return 错误码
 */
tarjan_error_t tarjan_graph_validate(const tarjan_dynamic_graph_t *graph,
                                    bool *valid,
                                    tarjan_state_t *state);

/**
 * @brief 检查图是否为空
 * @param graph 动态图
 * @param empty 输出结果
 * @return 错误码
 */
tarjan_error_t tarjan_graph_is_empty(const tarjan_dynamic_graph_t *graph,
                                   bool *empty);

/**
 * @brief 获取图的节点数
 * @param graph 动态图
 * @param nodes 输出节点数
 * @return 错误码
 */
tarjan_error_t tarjan_graph_get_node_count(const tarjan_dynamic_graph_t *graph,
                                         int *nodes);

/**
 * @brief 获取图的边数
 * @param graph 动态图
 * @param edges 输出边数
 * @return 错误码
 */
tarjan_error_t tarjan_graph_get_edge_count(const tarjan_dynamic_graph_t *graph,
                                         int *edges);

/**
 * @brief 检查边是否存在
 * @param graph 动态图
 * @param u 起点
 * @param v 终点
 * @param exists 输出结果
 * @return 错误码
 */
tarjan_error_t tarjan_graph_has_edge(const tarjan_dynamic_graph_t *graph,
                                   int u, int v,
                                   bool *exists);

/**
 * @brief 获取节点的出度
 * @param graph 动态图
 * @param u 节点
 * @param out_degree 输出出度
 * @return 错误码
 */
tarjan_error_t tarjan_graph_get_out_degree(const tarjan_dynamic_graph_t *graph,
                                         int u,
                                         int *out_degree);

/**
 * @brief 获取节点的入度
 * @param graph 动态图
 * @param u 节点
 * @param in_degree 输出入度
 * @return 错误码
 */
tarjan_error_t tarjan_graph_get_in_degree(const tarjan_dynamic_graph_t *graph,
                                        int u,
                                        int *in_degree);

/**
 * @brief 打印图
 * @param graph 动态图
 * @return 错误码
 */
tarjan_error_t tarjan_graph_print(const tarjan_dynamic_graph_t *graph);

/**
 * @brief 打印SCC结果
 * @param scc_map SCC映射
 * @param n 节点数
 * @param scc_count SCC数量
 * @return 错误码
 */
tarjan_error_t tarjan_print_scc(const int *scc_map, int n, int scc_count);

/**
 * @brief 获取图统计信息
 * @param graph 动态图
 * @param state 状态输出
 * @return 错误码
 */
tarjan_error_t tarjan_graph_get_statistics(const tarjan_dynamic_graph_t *graph,
                                         tarjan_state_t *state);

/**
 * @brief 重置图状态
 * @param state 状态结构
 */
void tarjan_reset_state(tarjan_state_t *state);

/**
 * @brief 获取最后一次错误信息
 * @param state 状态
 * @return 错误描述字符串
 */
const char* tarjan_strerror(const tarjan_state_t *state);

/**
 * @brief 初始化配置为默认值
 * @param config 配置结构
 */
void tarjan_config_init(tarjan_config_t *config);

/**
 * @brief 初始化状态为默认值
 * @param state 状态结构
 */
void tarjan_state_init(tarjan_state_t *state);

#endif // C_UTILS_TARJAN_SCC_H
