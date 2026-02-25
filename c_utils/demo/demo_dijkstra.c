/**
 * @file demo_dijkstra.c
 * @brief Dijkstra 最短路径算法演示
 *
 * 本演示展示了 Dijkstra 算法的应用，包括：
 * - 图的创建和边添加
 * - 单源最短路径计算
 * - 路径重建
 * - 实际应用场景
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include "../c_utils/dijkstra.h"

// 演示 1: 基本图创建
static void demo_basic_graph(void) {
    printf("\n=== 演示 1: 基本图创建 ===\n");

    printf("创建一个包含 5 个节点的图...\n");
    dijkstra_error_t error;
    dijkstra_graph_t *graph = dijkstra_graph_create(5, &error);
    
    if (!graph) {
        printf("创建图失败，错误码: %d\n", error);
        return;
    }

    printf("图创建成功，节点数: 5\n");

    // 添加边
    printf("\n添加边:\n");
    dijkstra_graph_add_edge(graph, 0, 1, 4, &error);
    printf("  0 -> 1 (权重 4)\n");
    
    dijkstra_graph_add_edge(graph, 0, 2, 1, &error);
    printf("  0 -> 2 (权重 1)\n");
    
    dijkstra_graph_add_edge(graph, 2, 1, 2, &error);
    printf("  2 -> 1 (权重 2)\n");
    
    dijkstra_graph_add_edge(graph, 1, 3, 1, &error);
    printf("  1 -> 3 (权重 1)\n");
    
    dijkstra_graph_add_edge(graph, 2, 3, 5, &error);
    printf("  2 -> 3 (权重 5)\n");
    
    dijkstra_graph_add_edge(graph, 3, 4, 3, &error);
    printf("  3 -> 4 (权重 3)\n");

    dijkstra_graph_free(graph);
    printf("\n图已释放\n");
}

// 演示 2: 最短路径计算
static void demo_shortest_path(void) {
    printf("\n=== 演示 2: 最短路径计算 ===\n");

    // 创建示例图
    dijkstra_error_t error;
    dijkstra_graph_t *graph = dijkstra_graph_create(6, &error);
    if (!graph) return;

    // 添加边（构建一个示例网络）
    dijkstra_graph_add_edge(graph, 0, 1, 7, &error);
    dijkstra_graph_add_edge(graph, 0, 2, 9, &error);
    dijkstra_graph_add_edge(graph, 0, 5, 14, &error);
    dijkstra_graph_add_edge(graph, 1, 2, 10, &error);
    dijkstra_graph_add_edge(graph, 1, 3, 15, &error);
    dijkstra_graph_add_edge(graph, 2, 3, 11, &error);
    dijkstra_graph_add_edge(graph, 2, 5, 2, &error);
    dijkstra_graph_add_edge(graph, 3, 4, 6, &error);
    dijkstra_graph_add_edge(graph, 4, 5, 9, &error);

    printf("图结构:\n");
    printf("  节点 0: ->1(7) ->2(9) ->5(14)\n");
    printf("  节点 1: ->2(10) ->3(15)\n");
    printf("  节点 2: ->3(11) ->5(2)\n");
    printf("  节点 3: ->4(6)\n");
    printf("  节点 4: ->5(9)\n");
    printf("  节点 5: (无出边)\n\n");

    // 计算从节点 0 到所有其他节点的最短路径
    int dist[6];
    int prev[6];
    
    if (dijkstra(graph, 0, dist, prev, &error)) {
        printf("从节点 0 出发的最短路径:\n\n");
        
        for (int i = 0; i < 6; i++) {
            printf("  到节点 %d: ", i);
            if (dist[i] == INT_MAX) {
                printf("不可达\n");
            } else {
                printf("距离 = %d", dist[i]);
                
                // 重建路径
                printf(", 路径: ");
                int path[6];
                int path_len = 0;
                int curr = i;
                
                while (curr != -1) {
                    path[path_len++] = curr;
                    curr = prev[curr];
                }
                
                // 反向输出路径
                for (int j = path_len - 1; j >= 0; j--) {
                    printf("%d", path[j]);
                    if (j > 0) printf(" -> ");
                }
                printf("\n");
            }
        }
    } else {
        printf("计算失败，错误码: %d\n", error);
    }

    dijkstra_graph_free(graph);
}

// 演示 3: 实际地图示例
static void demo_map_example(void) {
    printf("\n=== 演示 3: 实际地图示例 ===\n");

    printf("城市路线图:\n");
    printf("  0: 北京\n");
    printf("  1: 天津\n");
    printf("  2: 石家庄\n");
    printf("  3: 济南\n");
    printf("  4: 郑州\n");
    printf("  5: 南京\n\n");

    dijkstra_error_t error;
    dijkstra_graph_t *graph = dijkstra_graph_create(6, &error);
    if (!graph) return;

    // 添加城市间距离（公里）
    dijkstra_graph_add_edge(graph, 0, 1, 120, &error);  // 北京-天津
    dijkstra_graph_add_edge(graph, 0, 2, 280, &error);  // 北京-石家庄
    dijkstra_graph_add_edge(graph, 1, 3, 300, &error);  // 天津-济南
    dijkstra_graph_add_edge(graph, 2, 4, 220, &error);  // 石家庄-郑州
    dijkstra_graph_add_edge(graph, 3, 5, 600, &error);  // 济南-南京
    dijkstra_graph_add_edge(graph, 4, 5, 650, &error);  // 郑州-南京
    dijkstra_graph_add_edge(graph, 2, 3, 280, &error);  // 石家庄-济南

    int dist[6];
    int prev[6];
    
    if (dijkstra(graph, 0, dist, prev, &error)) {
        printf("从北京出发的最短路线:\n\n");
        
        const char *cities[] = {"北京", "天津", "石家庄", "济南", "郑州", "南京"};
        
        for (int i = 1; i < 6; i++) {
            printf("到 %s: %d 公里\n", cities[i], dist[i]);
            
            // 重建路径
            printf("  路线: ");
            int path[6];
            int path_len = 0;
            int curr = i;
            
            while (curr != -1) {
                path[path_len++] = curr;
                curr = prev[curr];
            }
            
            for (int j = path_len - 1; j >= 0; j--) {
                printf("%s", cities[path[j]]);
                if (j > 0) printf(" -> ");
            }
            printf("\n\n");
        }
    }

    dijkstra_graph_free(graph);
}

// 演示 4: 网络路由示例
static void demo_network_routing(void) {
    printf("\n=== 演示 4: 网络路由示例 ===\n");

    printf("网络拓扑（延迟，毫秒）:\n");
    printf("  路由器 A (0) --5ms--> 路由器 B (1)\n");
    printf("  路由器 A (0) --3ms--> 路由器 C (2)\n");
    printf("  路由器 B (1) --2ms--> 路由器 D (3)\n");
    printf("  路由器 C (2) --4ms--> 路由器 B (1)\n");
    printf("  路由器 C (2) --6ms--> 路由器 D (3)\n");
    printf("  路由器 D (3) --1ms--> 路由器 E (4)\n\n");

    dijkstra_error_t error;
    dijkstra_graph_t *graph = dijkstra_graph_create(5, &error);
    if (!graph) return;

    dijkstra_graph_add_edge(graph, 0, 1, 5, &error);
    dijkstra_graph_add_edge(graph, 0, 2, 3, &error);
    dijkstra_graph_add_edge(graph, 1, 3, 2, &error);
    dijkstra_graph_add_edge(graph, 2, 1, 4, &error);
    dijkstra_graph_add_edge(graph, 2, 3, 6, &error);
    dijkstra_graph_add_edge(graph, 3, 4, 1, &error);

    int dist[5];
    int prev[5];
    
    if (dijkstra(graph, 0, dist, prev, &error)) {
        printf("从路由器 A 出发的最优路由:\n\n");
        
        const char *routers[] = {"A", "B", "C", "D", "E"};
        
        for (int i = 1; i < 5; i++) {
            printf("到路由器 %s:\n", routers[i]);
            printf("  总延迟: %d ms\n", dist[i]);
            
            // 重建路径
            printf("  路由路径: ");
            int path[5];
            int path_len = 0;
            int curr = i;
            
            while (curr != -1) {
                path[path_len++] = curr;
                curr = prev[curr];
            }
            
            for (int j = path_len - 1; j >= 0; j--) {
                printf("%s", routers[path[j]]);
                if (j > 0) printf(" -> ");
            }
            printf("\n\n");
        }
    }

    dijkstra_graph_free(graph);
}

// 演示 5: 错误处理
static void demo_errors(void) {
    printf("\n=== 演示 5: 错误处理 ===\n");

    printf("错误码说明:\n");
    printf("  DIJKSTRA_OK (%d): 成功\n", DIJKSTRA_OK);
    printf("  DIJKSTRA_ERROR_INVALID_PARAM (%d): 无效参数\n", DIJKSTRA_ERROR_INVALID_PARAM);
    printf("  DIJKSTRA_ERROR_INVALID_NODE (%d): 无效节点\n", DIJKSTRA_ERROR_INVALID_NODE);
    printf("  DIJKSTRA_ERROR_NO_PATH (%d): 无路径\n", DIJKSTRA_ERROR_NO_PATH);
    printf("  DIJKSTRA_ERROR_MEMORY_ALLOC (%d): 内存分配失败\n", DIJKSTRA_ERROR_MEMORY_ALLOC);
    printf("  DIJKSTRA_ERROR_GRAPH_EMPTY (%d): 图为空\n", DIJKSTRA_ERROR_GRAPH_EMPTY);

    printf("\n错误场景测试:\n");
    
    dijkstra_error_t error;
    
    // 无效节点数
    dijkstra_graph_t *graph = dijkstra_graph_create(0, &error);
    printf("  0 个节点: %s, 错误码: %d\n", graph ? "成功" : "失败", error);
    if (graph) dijkstra_graph_free(graph);

    // 无效边
    graph = dijkstra_graph_create(3, &error);
    if (graph) {
        bool result = dijkstra_graph_add_edge(graph, 0, 5, 10, &error);  // 节点 5 超出范围
        printf("  无效目标节点: %s, 错误码: %d\n", result ? "成功" : "失败", error);
        dijkstra_graph_free(graph);
    }

    // 错误信息
    printf("\n错误信息: %s\n", dijkstra_strerror(DIJKSTRA_ERROR_INVALID_PARAM));
}

// 演示 6: 性能测试
static void demo_performance(void) {
    printf("\n=== 演示 6: 性能测试 ===\n");

    const int num_nodes = 1000;
    const int num_edges = 5000;

    dijkstra_error_t error;
    dijkstra_graph_t *graph = dijkstra_graph_create(num_nodes, &error);
    if (!graph) {
        printf("创建图失败\n");
        return;
    }

    // 随机添加边
    srand(42);
    for (int i = 0; i < num_edges; i++) {
        int src = rand() % num_nodes;
        int dest = rand() % num_nodes;
        int weight = rand() % 100 + 1;
        dijkstra_graph_add_edge(graph, src, dest, weight, &error);
    }

    printf("图规模: %d 节点, %d 边\n", num_nodes, num_edges);

    // 测试多次最短路径计算
    int dist[1000];
    int prev[1000];
    
    clock_t start = clock();
    for (int i = 0; i < 100; i++) {
        dijkstra(graph, 0, dist, prev, &error);
    }
    clock_t end = clock();
    
    double time_ms = (double)(end - start) * 1000 / CLOCKS_PER_SEC;
    
    printf("100 次最短路径计算:\n");
    printf("  总时间: %.3f ms\n", time_ms);
    printf("  平均每次: %.3f ms\n", time_ms / 100);

    dijkstra_graph_free(graph);
}

// 演示 7: 算法比较
static void demo_comparison(void) {
    printf("\n=== 演示 7: 算法比较 ===\n");

    printf("最短路径算法比较:\n\n");

    printf("算法          时间复杂度      空间复杂度      适用场景\n");
    printf("--------------------------------------------------------\n");
    printf("Dijkstra      O((V+E)logV)    O(V)            非负权图\n");
    printf("Bellman-Ford  O(VE)           O(V)            含负权图\n");
    printf("Floyd-Warshall O(V³)          O(V²)           全源最短路径\n");
    printf("A*            O(E)            O(V)            有启发信息\n\n");

    printf("Dijkstra 算法优势:\n");
    printf("  - 效率高，适合大规模图\n");
    printf("  - 实现简单\n");
    printf("  - 保证最优解\n");
    printf("  - 广泛应用\n\n");

    printf("Dijkstra 算法限制:\n");
    printf("  - 不支持负权边\n");
    printf("  - 单源最短路径\n");
    printf("  - 需要存储整个图\n");
}

// 演示 8: 应用场景
static void demo_use_cases(void) {
    printf("\n=== 演示 8: 应用场景 ===\n");

    printf("Dijkstra 算法的应用场景:\n\n");

    printf("1. 导航系统:\n");
    printf("   - GPS 路径规划\n");
    printf("   - 地图应用\n");
    printf("   - 物流配送\n\n");

    printf("2. 网络路由:\n");
    printf("   - OSPF 协议\n");
    printf("   - 链路状态路由\n");
    printf("   - 网络拓扑优化\n\n");

    printf("3. 游戏开发:\n");
    printf("   - NPC 寻路\n");
    printf("   - 地图探索\n");
    printf("   - 策略游戏\n\n");

    printf("4. 社交网络:\n");
    printf("   - 好友推荐\n");
    printf("   - 关系链分析\n");
    printf("   - 影响力传播\n");
}

// 演示 9: 最佳实践
static void demo_best_practices(void) {
    printf("\n=== 演示 9: 最佳实践 ===\n");

    printf("Dijkstra 算法最佳实践:\n\n");

    printf("1. 图表示:\n");
    printf("   - 使用邻接表存储稀疏图\n");
    printf("   - 使用邻接矩阵存储稠密图\n");
    printf("   - 预分配内存避免重复分配\n\n");

    printf("2. 优先队列:\n");
    printf("   - 使用二叉堆：O((V+E)logV)\n");
    printf("   - 使用斐波那契堆：O(VlogV + E)\n");
    printf("   - 小规模图可用简单数组\n\n");

    printf("3. 优化技巧:\n");
    printf("   - 提前终止（找到目标节点）\n");
    printf("   - 双向搜索\n");
    printf("   - 启发式剪枝\n\n");

    printf("4. 注意事项:\n");
    printf("   - 检查负权边\n");
    printf("   - 处理不可达节点\n");
    printf("   - 考虑内存限制\n");
}

// 演示 10: 变体算法
static void demo_variants(void) {
    printf("\n=== 演示 10: Dijkstra 变体 ===\n");

    printf("Dijkstra 算法的常见变体:\n\n");

    printf("1. 带堆优化的 Dijkstra:\n");
    printf("   - 使用优先队列\n");
    printf("   - 时间复杂度: O((V+E)logV)\n");
    printf("   - 适合稀疏图\n\n");

    printf("2. 双向 Dijkstra:\n");
    printf("   - 从起点和终点同时搜索\n");
    printf("   - 减少搜索空间\n");
    printf("   - 适合单目标查询\n\n");

    printf("3. 增量式 Dijkstra:\n");
    printf("   - 动态图更新\n");
    printf("   - 避免重新计算\n");
    printf("   - 适合实时系统\n\n");

    printf("4. 并行 Dijkstra:\n");
    printf("   - 多线程加速\n");
    printf("   - GPU 加速\n");
    printf("   - 适合超大规模图\n");
}

int main(void) {
    printf("========================================\n");
    printf("    Dijkstra 最短路径算法演示\n");
    printf("========================================\n");

    demo_basic_graph();
    demo_shortest_path();
    demo_map_example();
    demo_network_routing();
    demo_errors();
    demo_performance();
    demo_comparison();
    demo_use_cases();
    demo_best_practices();
    demo_variants();

    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("========================================\n");

    return 0;
}
