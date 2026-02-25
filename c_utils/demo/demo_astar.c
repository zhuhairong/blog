/**
 * @file demo_astar.c
 * @brief A* 寻路算法演示
 *
 * 本演示展示了 A* 寻路算法的各种用法，包括：
 * - 基础寻路
 * - 不同启发函数对比
 * - 障碍处理
 * - 路径优化
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/astar.h"

#define GRID_WIDTH 20
#define GRID_HEIGHT 15

// 网格世界结构
typedef struct {
    int width;
    int height;
    char grid[GRID_HEIGHT][GRID_WIDTH];
} grid_world_t;

// 检查位置是否可行走
static bool is_walkable(void* world, point_t pos) {
    grid_world_t* grid = (grid_world_t*)world;
    if (pos.x < 0 || pos.x >= grid->width ||
        pos.y < 0 || pos.y >= grid->height) {
        return false;
    }
    return grid->grid[pos.y][pos.x] != '#';
}

// 计算移动代价（支持对角线移动）
static int move_cost(void* world, point_t from, point_t to) {
    (void)world;
    int dx = abs(to.x - from.x);
    int dy = abs(to.y - from.y);

    // 对角线移动代价为 14，直线为 10
    if (dx == 1 && dy == 1) {
        return 14;
    }
    return 10;
}

// 打印网格和路径
static void print_grid(grid_world_t* grid, astar_result_t* result,
                       point_t start, point_t goal) {
    // 创建路径标记数组
    bool path_map[GRID_HEIGHT][GRID_WIDTH] = {false};
    if (result && result->found) {
        for (size_t i = 0; i < result->path_len; i++) {
            path_map[result->path[i].y][result->path[i].x] = true;
        }
    }

    printf("  ");
    for (int x = 0; x < grid->width; x++) {
        printf("%d", x % 10);
    }
    printf("\n");

    for (int y = 0; y < grid->height; y++) {
        printf("%d ", y % 10);
        for (int x = 0; x < grid->width; x++) {
            point_t pos = {x, y};
            if (pos.x == start.x && pos.y == start.y) {
                printf("S");  // 起点
            } else if (pos.x == goal.x && pos.y == goal.y) {
                printf("G");  // 终点
            } else if (path_map[y][x]) {
                printf("*");  // 路径
            } else {
                printf("%c", grid->grid[y][x]);
            }
        }
        printf("\n");
    }
}

// 初始化简单网格
static void init_simple_grid(grid_world_t* grid) {
    grid->width = GRID_WIDTH;
    grid->height = GRID_HEIGHT;

    // 初始化为空地
    for (int y = 0; y < grid->height; y++) {
        for (int x = 0; x < grid->width; x++) {
            grid->grid[y][x] = '.';
        }
    }
}

// 初始化带障碍的网格
static void init_obstacle_grid(grid_world_t* grid) {
    init_simple_grid(grid);

    // 添加一些障碍物
    // 垂直墙
    for (int y = 3; y < 12; y++) {
        grid->grid[y][8] = '#';
    }
    // 水平墙
    for (int x = 12; x < 18; x++) {
        grid->grid[7][x] = '#';
    }
    // 一些分散的障碍
    grid->grid[4][4] = '#';
    grid->grid[4][5] = '#';
    grid->grid[5][4] = '#';
    grid->grid[10][15] = '#';
    grid->grid[11][15] = '#';
    grid->grid[12][15] = '#';
}

// 初始化迷宫网格
static void init_maze_grid(grid_world_t* grid) {
    init_simple_grid(grid);

    // 创建迷宫-like 结构
    for (int x = 2; x < 18; x += 4) {
        for (int y = 0; y < grid->height; y++) {
            if (y % 7 != 3) {
                grid->grid[y][x] = '#';
            }
        }
    }
    for (int y = 2; y < 13; y += 4) {
        for (int x = 0; x < grid->width; x++) {
            if (x % 6 != 2) {
                grid->grid[y][x] = '#';
            }
        }
    }
}

// 演示 1: 基础寻路
static void demo_basic_pathfinding(void) {
    printf("\n=== 演示 1: 基础寻路 ===\n");

    grid_world_t grid;
    init_simple_grid(&grid);

    point_t start = {2, 2};
    point_t goal = {17, 12};

    printf("起点: (%d, %d), 终点: (%d, %d)\n", start.x, start.y, goal.x, goal.y);
    printf("使用曼哈顿距离启发函数\n\n");

    astar_result_t result = astar_search(&grid, start, goal,
                                          is_walkable, move_cost,
                                          ASTAR_MANHATTAN, 10000);

    if (result.found) {
        printf("找到路径!\n");
        printf("路径长度: %zu\n", result.path_len);
        printf("扩展节点数: %d\n", result.nodes_expanded);
        printf("\n网格表示 (. = 空地, # = 障碍, S = 起点, G = 终点, * = 路径):\n");
        print_grid(&grid, &result, start, goal);

        // 打印路径坐标
        printf("\n路径坐标:\n");
        for (size_t i = 0; i < result.path_len; i++) {
            printf("  (%d, %d)", result.path[i].x, result.path[i].y);
            if ((i + 1) % 5 == 0) printf("\n");
        }
        printf("\n");
    } else {
        printf("未找到路径!\n");
    }

    astar_result_free(&result);
}

// 演示 2: 不同启发函数对比
static void demo_heuristic_comparison(void) {
    printf("\n=== 演示 2: 不同启发函数对比 ===\n");

    grid_world_t grid;
    init_obstacle_grid(&grid);

    point_t start = {2, 2};
    point_t goal = {17, 12};

    printf("起点: (%d, %d), 终点: (%d, %d)\n", start.x, start.y, goal.x, goal.y);
    printf("\n带障碍的网格:\n");
    print_grid(&grid, NULL, start, goal);
    printf("\n");

    // 测试不同的启发函数
    astar_heuristic_type_t heuristics[] = {
        ASTAR_MANHATTAN,
        ASTAR_EUCLIDEAN,
        ASTAR_DIAGONAL,
        ASTAR_CHEBYSHEV
    };
    const char* heuristic_names[] = {
        "曼哈顿距离",
        "欧几里得距离",
        "对角线距离",
        "切比雪夫距离"
    };

    for (int i = 0; i < 4; i++) {
        printf("--- %s ---\n", heuristic_names[i]);

        astar_result_t result = astar_search(&grid, start, goal,
                                              is_walkable, move_cost,
                                              heuristics[i], 10000);

        if (result.found) {
            printf("  路径长度: %zu\n", result.path_len);
            printf("  扩展节点数: %d\n", result.nodes_expanded);
        } else {
            printf("  未找到路径!\n");
        }

        astar_result_free(&result);
    }
}

// 演示 3: 迷宫寻路
static void demo_maze_pathfinding(void) {
    printf("\n=== 演示 3: 迷宫寻路 ===\n");

    grid_world_t grid;
    init_maze_grid(&grid);

    point_t start = {1, 1};
    point_t goal = {18, 13};

    printf("起点: (%d, %d), 终点: (%d, %d)\n", start.x, start.y, goal.x, goal.y);
    printf("\n迷宫网格:\n");
    print_grid(&grid, NULL, start, goal);

    astar_result_t result = astar_search(&grid, start, goal,
                                          is_walkable, move_cost,
                                          ASTAR_MANHATTAN, 10000);

    if (result.found) {
        printf("\n找到路径!\n");
        printf("路径长度: %zu\n", result.path_len);
        printf("扩展节点数: %d\n", result.nodes_expanded);
        printf("\n带路径的网格:\n");
        print_grid(&grid, &result, start, goal);
    } else {
        printf("\n未找到路径!\n");
    }

    astar_result_free(&result);
}

// 演示 4: 简化版 A* 搜索
static void demo_simple_search(void) {
    printf("\n=== 演示 4: 简化版 A* 搜索 ===\n");

    grid_world_t grid;
    init_obstacle_grid(&grid);

    point_t start = {2, 2};
    point_t goal = {17, 12};

    printf("起点: (%d, %d), 终点: (%d, %d)\n", start.x, start.y, goal.x, goal.y);
    printf("使用简化版搜索（自动使用曼哈顿距离）\n");

    astar_result_t result = astar_search_simple(&grid, start, goal,
                                                 is_walkable,
                                                 grid.width, grid.height);

    if (result.found) {
        printf("找到路径!\n");
        printf("路径长度: %zu\n", result.path_len);
        printf("扩展节点数: %d\n", result.nodes_expanded);
        printf("\n网格:\n");
        print_grid(&grid, &result, start, goal);
    } else {
        printf("未找到路径!\n");
    }

    astar_result_free(&result);
}

// 演示 5: 路径操作
static void demo_path_operations(void) {
    printf("\n=== 演示 5: 路径操作 ===\n");

    grid_world_t grid;
    init_simple_grid(&grid);

    point_t start = {2, 2};
    point_t goal = {10, 10};

    astar_result_t result = astar_search(&grid, start, goal,
                                          is_walkable, move_cost,
                                          ASTAR_MANHATTAN, 10000);

    if (!result.found) {
        printf("未找到路径!\n");
        astar_result_free(&result);
        return;
    }

    printf("原始路径:\n");
    printf("  起点 -> ");
    for (size_t i = 0; i < result.path_len; i++) {
        printf("(%d,%d) ", result.path[i].x, result.path[i].y);
    }
    printf("-> 终点\n");

    // 反转路径
    astar_result_reverse(&result);
    printf("\n反转后的路径:\n");
    printf("  终点 -> ");
    for (size_t i = 0; i < result.path_len; i++) {
        printf("(%d,%d) ", result.path[i].x, result.path[i].y);
    }
    printf("-> 起点\n");

    // 检查路径包含的点
    point_t test_point = {5, 5};
    printf("\n路径是否包含点 (%d,%d): %s\n",
           test_point.x, test_point.y,
           astar_path_contains(&result, test_point) ? "是" : "否");

    test_point.x = 100;
    test_point.y = 100;
    printf("路径是否包含点 (%d,%d): %s\n",
           test_point.x, test_point.y,
           astar_path_contains(&result, test_point) ? "是" : "否");

    astar_result_free(&result);
}

// 演示 6: 启发函数计算
static void demo_heuristic_calculation(void) {
    printf("\n=== 演示 6: 启发函数计算 ===\n");

    point_t a = {0, 0};
    point_t b = {3, 4};

    printf("点 A: (%d, %d)\n", a.x, a.y);
    printf("点 B: (%d, %d)\n", b.x, b.y);
    printf("\n不同启发函数计算的估计代价:\n");
    printf("  曼哈顿距离: %d\n", astar_heuristic_manhattan(a, b));
    printf("  欧几里得距离: %d\n", astar_heuristic_euclidean(a, b));
    printf("  对角线距离: %d\n", astar_heuristic_diagonal(a, b));
    printf("  切比雪夫距离: %d\n", astar_heuristic_chebyshev(a, b));

    // 使用通用函数
    printf("\n使用通用启发函数:\n");
    printf("  曼哈顿距离: %d\n", astar_heuristic(a, b, ASTAR_MANHATTAN));
    printf("  欧几里得距离: %d\n", astar_heuristic(a, b, ASTAR_EUCLIDEAN));
    printf("  对角线距离: %d\n", astar_heuristic(a, b, ASTAR_DIAGONAL));
    printf("  切比雪夫距离: %d\n", astar_heuristic(a, b, ASTAR_CHEBYSHEV));
}

// 演示 7: 无路可走的情况
static void demo_no_path(void) {
    printf("\n=== 演示 7: 无路可走的情况 ===\n");

    grid_world_t grid;
    init_simple_grid(&grid);

    // 创建完全阻挡的墙
    for (int x = 0; x < grid.width; x++) {
        grid.grid[7][x] = '#';
    }

    point_t start = {2, 2};
    point_t goal = {17, 12};

    printf("起点: (%d, %d), 终点: (%d, %d)\n", start.x, start.y, goal.x, goal.y);
    printf("中间有一堵完整的墙阻挡\n\n");
    print_grid(&grid, NULL, start, goal);

    astar_result_t result = astar_search(&grid, start, goal,
                                          is_walkable, move_cost,
                                          ASTAR_MANHATTAN, 10000);

    if (result.found) {
        printf("\n找到路径!\n");
    } else {
        printf("\n未找到路径（预期结果）\n");
        printf("扩展节点数: %d\n", result.nodes_expanded);
    }

    astar_result_free(&result);
}

int main(void) {
    printf("========================================\n");
    printf("    A* 寻路算法演示程序\n");
    printf("========================================\n");

    demo_basic_pathfinding();
    demo_heuristic_comparison();
    demo_maze_pathfinding();
    demo_simple_search();
    demo_path_operations();
    demo_heuristic_calculation();
    demo_no_path();

    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("========================================\n");

    return 0;
}