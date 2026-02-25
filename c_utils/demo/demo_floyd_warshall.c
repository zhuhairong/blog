/**
 * Floyd-Warshall 全源最短路径演示程序
 * 
 * 功能：
 * - 计算所有节点对之间的最短路径
 * - 路径重建
 * - 负权环检测
 * - 与 Dijkstra 算法对比
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "../c_utils/floyd_warshall.h"

#define INF FLOYD_INF
#define N 6

// 打印矩阵
static void print_matrix(const char *name, int **matrix, int n) {
    printf("%s:\n", name);
    printf("     ");
    for (int i = 0; i < n; i++) {
        printf("%4d ", i);
    }
    printf("\n");
    
    for (int i = 0; i < n; i++) {
        printf("%2d [", i);
        for (int j = 0; j < n; j++) {
            if (matrix[i][j] == INF) {
                printf("   ∞ ");
            } else {
                printf("%4d ", matrix[i][j]);
            }
        }
        printf("]\n");
    }
}

// 创建邻接矩阵
static int** create_adj_matrix(int n) {
    int **adj = malloc(n * sizeof(int*));
    for (int i = 0; i < n; i++) {
        adj[i] = malloc(n * sizeof(int));
        for (int j = 0; j < n; j++) {
            if (i == j) {
                adj[i][j] = 0;
            } else {
                adj[i][j] = INF;
            }
        }
    }
    return adj;
}

// 释放矩阵
static void free_matrix(int **matrix, int n) {
    for (int i = 0; i < n; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

// 演示 1: 基本最短路径
static void demo_basic(void) {
    printf("\n=== 演示 1: 基本最短路径 ===\n");
    
    // 创建示例图
    //     0 --4--> 1 --1--> 2
    //     |        |        |
    //     2        3        5
    //     |        |        |
    //     v        v        v
    //     3 --2--> 4 --1--> 5
    
    int **adj = create_adj_matrix(N);
    
    // 添加边
    adj[0][1] = 4; adj[0][3] = 2;
    adj[1][2] = 1; adj[1][4] = 3;
    adj[2][5] = 5;
    adj[3][4] = 2;
    adj[4][5] = 1;
    
    printf("图结构 (有向图):\n");
    printf("  0 -> 1 (权重 4)\n");
    printf("  0 -> 3 (权重 2)\n");
    printf("  1 -> 2 (权重 1)\n");
    printf("  1 -> 4 (权重 3)\n");
    printf("  2 -> 5 (权重 5)\n");
    printf("  3 -> 4 (权重 2)\n");
    printf("  4 -> 5 (权重 1)\n");
    
    printf("\n初始邻接矩阵:\n");
    print_matrix("邻接矩阵", adj, N);
    
    // 执行 Floyd-Warshall
    floyd_error_t error;
    floyd_result_t result;
    
    if (!floyd_warshall(N, (const int**)adj, &result, &error)) {
        printf("Floyd-Warshall 失败: %d\n", error);
        free_matrix(adj, N);
        return;
    }
    
    printf("\n最短路径矩阵:\n");
    print_matrix("距离矩阵", result.dist, N);
    
    // 查询特定路径
    printf("\n特定路径查询:\n");
    printf("  0 -> 5 的最短距离: %d\n", 
           floyd_warshall_get_distance(&result, 0, 5, &error));
    printf("  1 -> 5 的最短距离: %d\n", 
           floyd_warshall_get_distance(&result, 1, 5, &error));
    printf("  0 -> 2 的最短距离: %d\n", 
           floyd_warshall_get_distance(&result, 0, 2, &error));
    
    floyd_warshall_free(&result);
    free_matrix(adj, N);
}

// 演示 2: 路径重建
static void demo_path_reconstruction(void) {
    printf("\n=== 演示 2: 路径重建 ===\n");
    
    // 创建示例图
    int **adj = create_adj_matrix(N);
    adj[0][1] = 4; adj[0][3] = 2;
    adj[1][2] = 1; adj[1][4] = 3;
    adj[2][5] = 5;
    adj[3][4] = 2;
    adj[4][5] = 1;
    
    floyd_error_t error;
    floyd_result_t result;
    
    floyd_warshall(N, (const int**)adj, &result, &error);
    
    printf("路径重建示例:\n");
    
    size_t path[N];
    size_t path_len;
    
    // 重建 0 -> 5 的路径
    printf("\n  路径 0 -> 5:\n");
    if (floyd_warshall_reconstruct_path(&result, 0, 5, path, &path_len, N, &error)) {
        printf("    距离: %d\n", result.dist[0][5]);
        printf("    路径: ");
        for (size_t i = 0; i < path_len; i++) {
            printf("%zu", path[i]);
            if (i < path_len - 1) printf(" -> ");
        }
        printf("\n");
    }
    
    // 重建 0 -> 2 的路径
    printf("\n  路径 0 -> 2:\n");
    if (floyd_warshall_reconstruct_path(&result, 0, 2, path, &path_len, N, &error)) {
        printf("    距离: %d\n", result.dist[0][2]);
        printf("    路径: ");
        for (size_t i = 0; i < path_len; i++) {
            printf("%zu", path[i]);
            if (i < path_len - 1) printf(" -> ");
        }
        printf("\n");
    }
    
    floyd_warshall_free(&result);
    free_matrix(adj, N);
}

// 演示 3: 负权边
static void demo_negative_weights(void) {
    printf("\n=== 演示 3: 负权边处理 ===\n");
    
    // 创建包含负权边的图
    int n = 4;
    int **adj = create_adj_matrix(n);
    
    adj[0][1] = 3;
    adj[0][2] = 8;
    adj[0][3] = -4;
    adj[1][2] = 1;
    adj[1][3] = 7;
    adj[2][0] = 4;
    adj[3][2] = 5;
    
    printf("图结构 (含负权边):\n");
    printf("  0 -> 1 (权重 3)\n");
    printf("  0 -> 2 (权重 8)\n");
    printf("  0 -> 3 (权重 -4)\n");
    printf("  1 -> 2 (权重 1)\n");
    printf("  1 -> 3 (权重 7)\n");
    printf("  2 -> 0 (权重 4)\n");
    printf("  3 -> 2 (权重 5)\n");
    
    printf("\n初始邻接矩阵:\n");
    print_matrix("邻接矩阵", adj, n);
    
    floyd_error_t error;
    floyd_result_t result;
    
    if (floyd_warshall(n, (const int**)adj, &result, &error)) {
        printf("\n最短路径矩阵:\n");
        print_matrix("距离矩阵", result.dist, n);
        
        printf("\n负权边效果:\n");
        printf("  0 -> 2 直接: 8\n");
        printf("  0 -> 3 -> 2: -4 + 5 = 1 (更短!)\n");
        printf("  实际最短: %d\n", result.dist[0][2]);
        
        floyd_warshall_free(&result);
    }
    
    free_matrix(adj, n);
}

// 演示 4: 负权环检测
static void demo_negative_cycle(void) {
    printf("\n=== 演示 4: 负权环检测 ===\n");
    
    // 创建包含负权环的图
    int n = 3;
    int **adj = create_adj_matrix(n);
    
    adj[0][1] = 1;
    adj[1][2] = -3;
    adj[2][0] = 1;  // 0->1->2->0 = 1-3+1 = -1 (负权环)
    
    printf("图结构 (含负权环):\n");
    printf("  0 -> 1 (权重 1)\n");
    printf("  1 -> 2 (权重 -3)\n");
    printf("  2 -> 0 (权重 1)\n");
    printf("\n  环 0->1->2->0 的总权重: 1 + (-3) + 1 = -1\n");
    
    floyd_error_t error;
    floyd_result_t result;
    
    if (floyd_warshall(n, (const int**)adj, &result, &error)) {
        printf("\n负权环检测:\n");
        printf("  检查对角线元素:\n");
        bool has_negative = false;
        for (int i = 0; i < n; i++) {
            if (result.dist[i][i] < 0) {
                has_negative = true;
                printf("    dist[%d][%d] = %d < 0, 存在负权环!\n", i, i, result.dist[i][i]);
            }
        }
        if (!has_negative) {
            printf("  未检测到负权环\n");
        } else {
            printf("  这意味着某些最短路径无定义 (可以无限减小)\n");
        }
        
        printf("\n距离矩阵 (对角线应为 0，负值表示负权环):\n");
        print_matrix("距离矩阵", result.dist, n);
        
        floyd_warshall_free(&result);
    }
    
    free_matrix(adj, n);
}

// 演示 5: 算法原理
static void demo_algorithm(void) {
    printf("\n=== 演示 5: 算法原理 ===\n");
    
    printf("Floyd-Warshall 算法:\n");
    printf("  动态规划求解全源最短路径\n");
    printf("  时间复杂度: O(V³)\n");
    printf("  空间复杂度: O(V²)\n");
    printf("\n");
    
    printf("核心思想:\n");
    printf("  dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j])\n");
    printf("  对于每个中间节点 k，检查是否可以通过 k 缩短 i 到 j 的距离\n");
    printf("\n");
    
    printf("算法步骤:\n");
    printf("  1. 初始化距离矩阵\n");
    printf("     dist[i][j] = 边(i,j)的权重，或 INF (无边)，或 0 (i=j)\n");
    printf("\n");
    printf("  2. 三重循环更新\n");
    printf("     for k = 0 to V-1\n");
    printf("       for i = 0 to V-1\n");
    printf("         for j = 0 to V-1\n");
    printf("           dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j])\n");
    printf("\n");
    printf("  3. 检测负权环\n");
    printf("     如果对角线 dist[i][i] < 0，存在负权环\n");
}

// 演示 6: 与 Dijkstra 对比
static void demo_comparison(void) {
    printf("\n=== 演示 6: 算法对比 ===\n");
    
    printf("Floyd-Warshall vs Dijkstra:\n");
    printf("\n");
    printf("特性              Floyd-Warshall    Dijkstra\n");
    printf("------------------------------------------------\n");
    printf("问题类型          全源最短路径      单源最短路径\n");
    printf("时间复杂度        O(V³)             O((V+E)logV)\n");
    printf("空间复杂度        O(V²)             O(V)\n");
    printf("负权边            支持              不支持\n");
    printf("负权环            可检测            不支持\n");
    printf("图类型            有向/无向         有向/无向\n");
    printf("\n");
    
    printf("选择建议:\n");
    printf("  - 需要所有节点对的最短路径: Floyd-Warshall\n");
    printf("  - 只需要从一个节点出发: Dijkstra (更快)\n");
    printf("  - 有负权边: Floyd-Warshall 或 Bellman-Ford\n");
    printf("  - 稀疏大图: Dijkstra 更高效\n");
    printf("  - 密集小图: Floyd-Warshall 更合适\n");
}

// 演示 7: 实际应用
static void demo_applications(void) {
    printf("\n=== 演示 7: 实际应用场景 ===\n");
    
    printf("1. 网络路由\n");
    printf("   - 计算网络中所有节点间的最短路径\n");
    printf("   - 路由表生成\n");
    printf("\n");
    
    printf("2. 交通规划\n");
    printf("   - 城市间最短路径计算\n");
    printf("   - 多目的地路线规划\n");
    printf("\n");
    
    printf("3. 社交网络分析\n");
    printf("   - 计算用户间的最短关系链\n");
    printf("   - 影响力传播分析\n");
    printf("\n");
    
    printf("4. 游戏开发\n");
    printf("   - NPC 路径规划\n");
    printf("   - 地图导航\n");
    printf("\n");
    
    printf("5. 电路设计\n");
    printf("   - 信号传输延迟计算\n");
    printf("   - 电路网络分析\n");
}

int main(void) {
    printf("========================================\n");
    printf("    Floyd-Warshall 算法演示\n");
    printf("========================================\n");
    
    demo_basic();
    demo_path_reconstruction();
    demo_negative_weights();
    demo_negative_cycle();
    demo_algorithm();
    demo_comparison();
    demo_applications();
    
    printf("\n========================================\n");
    printf("演示完成!\n");
    
    return 0;
}
