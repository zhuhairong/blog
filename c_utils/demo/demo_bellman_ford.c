/**
 * @file demo_bellman_ford.c
 * @brief Bellman-Ford 最短路径算法演示
 *
 * 本演示展示了 Bellman-Ford 算法的各种用法，包括：
 * - 基本最短路径计算
 * - 负权边处理
 * - 负环检测
 * - 路径重建
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "../c_utils/bellman_ford.h"

#define INF INT_MAX
#define MAX_NODES 10
#define MAX_EDGES 20

// 打印距离数组
static void print_distances(int* dist, int n, int start) {
    printf("从节点 %d 出发的最短距离:\n", start);
    for (int i = 0; i < n; i++) {
        if (dist[i] == INF) {
            printf("  到节点 %d: 不可达\n", i);
        } else {
            printf("  到节点 %d: %d\n", i, dist[i]);
        }
    }
}

// 打印路径
static void print_path(int* path, int path_len) {
    for (int i = 0; i < path_len; i++) {
        printf("%d", path[i]);
        if (i < path_len - 1) printf(" -> ");
    }
    printf("\n");
}

// 演示 1: 基本最短路径
static void demo_basic_shortest_path(void) {
    printf("\n=== 演示 1: 基本最短路径 ===\n");

    // 创建图: 5 个节点，8 条边
    // 0 --4--> 1 --1--> 2
    // |        |        |
    // 2        3        1
    // v        v        v
    // 3 --2--> 4 <--5--+
    bf_edge_t edges[] = {
        {0, 1, 4},  // 0->1, 权重 4
        {0, 3, 2},  // 0->3, 权重 2
        {1, 2, 1},  // 1->2, 权重 1
        {1, 4, 3},  // 1->4, 权重 3
        {2, 4, 1},  // 2->4, 权重 1
        {3, 4, 2},  // 3->4, 权重 2
        {3, 1, 1},  // 3->1, 权重 1
        {4, 2, 5}   // 4->2, 权重 5
    };
    int n = 5, m = 8;
    int start = 0;

    int dist[MAX_NODES];

    printf("图结构 (5 个节点, 8 条边):\n");
    printf("  0 --4--> 1 --1--> 2\n");
    printf("  |        |        |\n");
    printf("  2        3        1\n");
    printf("  v        v        v\n");
    printf("  3 --2--> 4 <--5---+\n\n");

    if (bellman_ford(n, m, edges, start, dist)) {
        print_distances(dist, n, start);

        printf("\n最短路径示例:\n");
        printf("  0 -> 4: %d (路径: 0->3->4)\n", dist[4]);
        printf("  0 -> 2: %d (路径: 0->3->1->2)\n", dist[2]);
    } else {
        printf("图中存在负权环!\n");
    }
}

// 演示 2: 负权边处理
static void demo_negative_weights(void) {
    printf("\n=== 演示 2: 负权边处理 ===\n");

    // 包含负权边的图
    bf_edge_t edges[] = {
        {0, 1, 5},
        {0, 2, 3},
        {1, 2, -2},  // 负权边
        {1, 3, 1},
        {2, 3, 4},
        {2, 4, 2},
        {2, 1, -4},  // 负权边
        {3, 4, -1},  // 负权边
        {3, 2, -1}   // 负权边
    };
    int n = 5, m = 9;
    int start = 0;

    int dist[MAX_NODES];

    printf("包含负权边的图:\n");
    printf("  边 1->2 权重: -2\n");
    printf("  边 2->1 权重: -4\n");
    printf("  边 3->4 权重: -1\n");
    printf("  边 3->2 权重: -1\n\n");

    if (bellman_ford(n, m, edges, start, dist)) {
        print_distances(dist, n, start);
    } else {
        printf("图中存在负权环!\n");
    }
}

// 演示 3: 负权环检测
static void demo_negative_cycle(void) {
    printf("\n=== 演示 3: 负权环检测 ===\n");

    // 包含负权环的图
    bf_edge_t edges[] = {
        {0, 1, 1},
        {1, 2, 2},
        {2, 3, 3},
        {3, 1, -7}  // 形成负权环: 1->2->3->1, 总权重 -2
    };
    int n = 4, m = 4;

    printf("包含负权环的图:\n");
    printf("  1 -> 2 (权重 2)\n");
    printf("  2 -> 3 (权重 3)\n");
    printf("  3 -> 1 (权重 -7)\n");
    printf("  环总权重: 2 + 3 - 7 = -2\n\n");

    int dist[MAX_NODES];
    bool result = bellman_ford(n, m, edges, 0, dist);

    if (result) {
        printf("未检测到负权环\n");
    } else {
        printf("检测到负权环!\n");
        printf("说明: 当图中存在负权环时，最短路径无定义\n");
    }
}

// 演示 4: 路径重建
static void demo_path_reconstruction(void) {
    printf("\n=== 演示 4: 路径重建 ===\n");

    bf_edge_t edges[] = {
        {0, 1, 4},
        {0, 2, 2},
        {1, 2, 3},
        {1, 3, 2},
        {1, 4, 3},
        {2, 1, 1},
        {2, 3, 4},
        {2, 4, 5},
        {4, 3, -5}   // 负权边，使 0->2->1->4->3 成为最短路径
    };
    int n = 5, m = 9;
    int start = 0;

    int dist[MAX_NODES];
    int pred[MAX_NODES];

    printf("图结构:\n");
    printf("  0 --4--> 1 --2--> 3\n");
    printf("  |        |        ^\n");
    printf("  2        3        |\n");
    printf("  v        v       -5\n");
    printf("  2 --1--> 1 --3--> 4\n\n");

    if (bellman_ford_with_path(n, m, edges, start, dist, pred)) {
        print_distances(dist, n, start);

        printf("\n路径重建:\n");
        for (int end = 1; end < n; end++) {
            int path[MAX_NODES];
            int path_len;

            if (bellman_ford_reconstruct_path(start, end, pred, path, &path_len, MAX_NODES)) {
                printf("  %d -> %d (距离 %d): ", start, end, dist[end]);
                print_path(path, path_len);
            }
        }
    }
}

// 演示 5: 不可达节点
static void demo_unreachable_nodes(void) {
    printf("\n=== 演示 5: 不可达节点 ===\n");

    // 不连通的图
    bf_edge_t edges[] = {
        {0, 1, 5},
        {1, 2, 3},
        {3, 4, 2}  // 节点 3,4 与 0,1,2 不连通
    };
    int n = 5, m = 3;
    int start = 0;

    int dist[MAX_NODES];

    printf("不连通图:\n");
    printf("  组件 1: 0 -> 1 -> 2\n");
    printf("  组件 2: 3 -> 4 (与组件 1 不连通)\n\n");

    if (bellman_ford(n, m, edges, start, dist)) {
        print_distances(dist, n, start);
        printf("\n说明: 节点 3 和 4 不可达，距离标记为无穷大\n");
    }
}

// 演示 6: 多源最短路径
static void demo_multi_source(void) {
    printf("\n=== 演示 6: 多源最短路径 ===\n");

    bf_edge_t edges[] = {
        {0, 1, 1},
        {0, 2, 4},
        {1, 2, 2},
        {1, 3, 5},
        {2, 3, 1}
    };
    int n = 4, m = 5;

    printf("从每个节点运行 Bellman-Ford:\n\n");

    for (int start = 0; start < n; start++) {
        int dist[MAX_NODES];
        printf("起点 %d:\n", start);

        if (bellman_ford(n, m, edges, start, dist)) {
            for (int i = 0; i < n; i++) {
                if (dist[i] == INF) {
                    printf("  到 %d: INF\n", i);
                } else {
                    printf("  到 %d: %d\n", i, dist[i]);
                }
            }
        }
        printf("\n");
    }
}

// 演示 7: 与 Dijkstra 对比场景
static void demo_comparison(void) {
    printf("\n=== 演示 7: Bellman-Ford vs Dijkstra ===\n");

    printf("Bellman-Ford 特点:\n");
    printf("  - 可以处理负权边\n");
    printf("  - 可以检测负权环\n");
    printf("  - 时间复杂度: O(VE)\n");
    printf("  - 适用于稀疏图和含负权边的图\n\n");

    printf("Dijkstra 特点:\n");
    printf("  - 不能处理负权边\n");
    printf("  - 时间复杂度: O((V+E)logV) 使用优先队列\n");
    printf("  - 适用于非负权重的稠密图\n\n");

    // 一个需要 Bellman-Ford 的例子
    bf_edge_t edges[] = {
        {0, 1, 100},
        {0, 2, 500},
        {1, 2, -200},  // 负权边，使 0->1->2 比 0->2 更短
        {2, 3, 100}
    };
    int n = 4, m = 4;
    int start = 0;

    int dist[MAX_NODES];

    printf("示例图 (含负权边):\n");
    printf("  0 --100--> 1 --(-200)--> 2 --100--> 3\n");
    printf("  |                                  ^\n");
    printf("  +----------- 500 ------------------+\n\n");

    if (bellman_ford(n, m, edges, start, dist)) {
        printf("最短距离:\n");
        printf("  0 -> 2: %d (路径: 0->1->2, 不是直接的 500)\n", dist[2]);
        printf("  0 -> 3: %d (路径: 0->1->2->3)\n", dist[3]);
    }
}

// 演示 8: 输入验证
static void demo_input_validation(void) {
    printf("\n=== 演示 8: 输入验证 ===\n");

    bf_edge_t valid_edges[] = {
        {0, 1, 5},
        {1, 2, 3}
    };

    bf_edge_t invalid_edges[] = {
        {0, 5, 5},   // 节点 5 超出范围 (n=3)
        {1, 2, 3}
    };

    printf("有效输入 (n=3, m=2):\n");
    printf("  边: {0,1,5}, {1,2,3}\n");
    bool valid = bellman_ford_validate_input(3, 2, valid_edges, 0, NULL);
    printf("  验证结果: %s\n\n", valid ? "有效" : "无效");

    printf("无效输入 (n=3, m=2):\n");
    printf("  边: {0,5,5} (节点 5 超出范围)\n");
    valid = bellman_ford_validate_input(3, 2, invalid_edges, 0, NULL);
    printf("  验证结果: %s\n", valid ? "有效" : "无效");
}

int main(void) {
    printf("========================================\n");
    printf("    Bellman-Ford 最短路径算法演示\n");
    printf("========================================\n");

    demo_basic_shortest_path();
    demo_negative_weights();
    demo_negative_cycle();
    demo_path_reconstruction();
    demo_unreachable_nodes();
    demo_multi_source();
    demo_comparison();
    demo_input_validation();

    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("========================================\n");

    return 0;
}
