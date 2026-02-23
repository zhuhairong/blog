#ifndef C_UTILS_ASTAR_H
#define C_UTILS_ASTAR_H

#include <stdbool.h>
#include <stddef.h>

// 2D 点坐标
typedef struct { int x, y; } point_t;

// A* 节点
typedef struct astar_node_s {
    point_t pos;
    int g;  // 从起点到当前节点的代价
    int h;  // 启发函数估计值（到终点的代价）
    int f;  // g + h
    struct astar_node_s *parent;
} astar_node_t;

// A* 路径结果
typedef struct {
    point_t *path;      // 路径点数组
    size_t path_len;    // 路径长度
    bool found;         // 是否找到路径
    int nodes_expanded; // 扩展的节点数
} astar_result_t;

// 网格世界回调函数类型
typedef bool (*astar_is_walkable_fn)(void *world, point_t pos);
typedef int  (*astar_cost_fn)(void *world, point_t from, point_t to);

// 启发函数类型
typedef enum {
    ASTAR_MANHATTAN,   // 曼哈顿距离（四方向移动）
    ASTAR_EUCLIDEAN,   // 欧几里得距离
    ASTAR_DIAGONAL,    // 对角线距离（八方向移动）
    ASTAR_CHEBYSHEV,   // 切比雪夫距离
} astar_heuristic_type_t;

// 创建与销毁
astar_result_t* astar_result_create(void);
void            astar_result_free(astar_result_t *result);

// 启发函数
int astar_heuristic(point_t a, point_t b, astar_heuristic_type_t type);
int astar_heuristic_manhattan(point_t a, point_t b);
int astar_heuristic_euclidean(point_t a, point_t b);
int astar_heuristic_diagonal(point_t a, point_t b);
int astar_heuristic_chebyshev(point_t a, point_t b);

// A* 搜索
astar_result_t astar_search(
    void *world,
    point_t start,
    point_t goal,
    astar_is_walkable_fn is_walkable,
    astar_cost_fn cost_fn,
    astar_heuristic_type_t heuristic_type,
    int max_iterations
);

// 简化版 A* 搜索（使用默认参数）
astar_result_t astar_search_simple(
    void *world,
    point_t start,
    point_t goal,
    astar_is_walkable_fn is_walkable,
    int width,
    int height
);

// 路径操作
void astar_result_reverse(astar_result_t *result);
bool astar_path_contains(const astar_result_t *result, point_t point);

#endif // C_UTILS_ASTAR_H
