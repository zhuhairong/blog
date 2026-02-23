#ifndef C_UTILS_PRIM_H
#define C_UTILS_PRIM_H

#include <stddef.h>
#include <stdbool.h>

/**
 * @brief Prim 算法错误码
 */
typedef enum {
    PRIM_OK = 0,                  /**< 成功 */
    PRIM_ERROR_NULL_PTR,          /**< 空指针错误 */
    PRIM_ERROR_INVALID_GRAPH,     /**< 无效图结构 */
    PRIM_ERROR_TOO_MANY_NODES,    /**< 节点数过多 */
    PRIM_ERROR_NO_CONNECTION,     /**< 图不连通 */
    PRIM_ERROR_MAX                /**< 最大错误码 */
} prim_error_t;

/**
 * @brief Prim 算法配置
 */
typedef struct {
    size_t max_nodes;             /**< 最大节点数 */
    int infinity;                 /**< 无穷大值 */
    bool directed;                /**< 是否为有向图 */
    bool compute_edges;           /**< 是否计算 MST 边 */
} prim_config_t;

/**
 * @brief 图边结构
 */
typedef struct {
    int src;                      /**< 源节点 */
    int dest;                     /**< 目标节点 */
    int weight;                   /**< 边权重 */
} prim_edge_t;

/**
 * @brief Prim 算法结果
 */
typedef struct {
    int total_weight;             /**< MST 总权重 */
    prim_edge_t *edges;           /**< MST 边数组 */
    size_t edge_count;            /**< MST 边数 */
    bool connected;               /**< 图是否连通 */
} prim_result_t;

/**
 * @brief 图结构
 */
typedef struct {
    int n;                        /**< 节点数 */
    int **adj;                    /**< 邻接矩阵 */
    prim_config_t config;         /**< 配置 */
    bool owns_memory;             /**< 是否拥有内存 */
} prim_graph_t;

/**
 * @brief 默认最大节点数
 */
#define PRIM_DEFAULT_MAX_NODES 100

/**
 * @brief 默认无穷大值
 */
#define PRIM_DEFAULT_INFINITY 1000000

/**
 * @brief 获取默认配置
 * @return 默认配置
 */
prim_config_t prim_default_config(void);

/**
 * @brief 创建图
 * @param n 节点数
 * @param config 配置选项
 * @param error 错误码输出
 * @return 图结构，失败返回 NULL
 */
prim_graph_t* prim_graph_create(int n, const prim_config_t *config, prim_error_t *error);

/**
 * @brief 销毁图
 * @param g 图结构
 */
void prim_graph_destroy(prim_graph_t *g);

/**
 * @brief 添加边
 * @param g 图结构
 * @param src 源节点
 * @param dest 目标节点
 * @param weight 边权重
 * @param error 错误码输出
 * @return 是否成功
 */
bool prim_graph_add_edge(prim_graph_t *g, int src, int dest, int weight, prim_error_t *error);

/**
 * @brief 获取边权重
 * @param g 图结构
 * @param src 源节点
 * @param dest 目标节点
 * @param weight 权重输出
 * @param error 错误码输出
 * @return 是否成功
 */
bool prim_graph_get_edge(const prim_graph_t *g, int src, int dest, int *weight, prim_error_t *error);

/**
 * @brief 计算最小生成树
 * @param g 图结构
 * @param error 错误码输出
 * @return MST 总权重，失败返回 -1
 */
int prim_mst(prim_graph_t *g, prim_error_t *error);

/**
 * @brief 计算最小生成树（带详细结果）
 * @param g 图结构
 * @param result 结果输出
 * @param error 错误码输出
 * @return 是否成功
 */
bool prim_mst_ex(prim_graph_t *g, prim_result_t *result, prim_error_t *error);

/**
 * @brief 释放结果
 * @param result 结果结构
 */
void prim_result_free(prim_result_t *result);

/**
 * @brief 检查图是否连通
 * @param g 图结构
 * @param error 错误码输出
 * @return 是否连通
 */
bool prim_graph_is_connected(const prim_graph_t *g, prim_error_t *error);

/**
 * @brief 获取图的节点数
 * @param g 图结构
 * @return 节点数
 */
int prim_graph_get_node_count(const prim_graph_t *g);

/**
 * @brief 打印图
 * @param g 图结构
 */
void prim_graph_print(const prim_graph_t *g);

/**
 * @brief 打印 MST 结果
 * @param result 结果结构
 */
void prim_result_print(const prim_result_t *result);

/**
 * @brief 获取错误信息
 * @param error 错误码
 * @return 错误信息字符串
 */
const char* prim_error_string(prim_error_t error);

#endif // C_UTILS_PRIM_H
