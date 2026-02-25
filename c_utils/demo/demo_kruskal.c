/**
 * Kruskal 最小生成树演示程序
 *
 * 功能：
 * - 最小生成树构建
 * - 边排序和选择
 * - 并查集应用
 */

#include <stdio.h>
#include <stdlib.h>
#include "../c_utils/kruskal.h"

// 演示 1: 基本使用
static void demo_basic(void) {
    printf("\n=== 演示 1: 基本使用 ===\n");

    // 示例图: 4个顶点，5条边
    kruskal_edge_t edges[] = {
        {0, 1, 10},  // A-B: 10
        {0, 2, 6},   // A-C: 6
        {0, 3, 5},   // A-D: 5
        {1, 3, 15},  // B-D: 15
        {2, 3, 4}    // C-D: 4
    };
    int n = 4;  // 顶点数
    int m = 5;  // 边数

    printf("图结构 (边列表):\n");
    printf("  顶点数: %d\n", n);
    printf("  边数: %d\n\n", m);

    printf("边列表:\n");
    printf("  %-8s %-8s %-8s\n", "起点", "终点", "权重");
    printf("  %-8s %-8s %-8s\n", "--------", "--------", "--------");
    for (int i = 0; i < m; i++) {
        printf("  %-8d %-8d %-8d\n", edges[i].u, edges[i].v, edges[i].w);
    }

    kruskal_error_t error;
    int total_weight = kruskal_mst_ex(n, m, edges, &error);

    printf("\n最小生成树总权重: %d\n", total_weight);
    printf("预期结果: 19 (边: 4+5+10)\n");
}

// 演示 2: 详细结果
static void demo_detailed(void) {
    printf("\n=== 演示 2: 详细结果 ===\n");

    kruskal_edge_t edges[] = {
        {0, 1, 4},
        {0, 2, 2},
        {1, 2, 1},
        {1, 3, 5},
        {2, 3, 8},
        {2, 4, 3},
        {3, 4, 6}
    };
    int n = 5;
    int m = 7;

    printf("图结构:\n");
    printf("  顶点: 0-4 (5个)\n");
    printf("  边数: %d\n\n", m);

    kruskal_error_t error;
    kruskal_config_t config;
    kruskal_get_default_config(&config);
    config.return_edges = true;

    kruskal_result_t *result = kruskal_mst_with_config(n, m, edges, &config, &error);

    if (result && !result->has_error) {
        printf("最小生成树:\n");
        printf("  总权重: %d\n", result->total_weight);
        printf("  边数: %zu\n\n", result->edge_count);

        printf("选中的边:\n");
        printf("  %-8s %-8s %-8s\n", "起点", "终点", "权重");
        printf("  %-8s %-8s %-8s\n", "--------", "--------", "--------");
        for (size_t i = 0; i < result->edge_count; i++) {
            printf("  %-8d %-8d %-8d\n",
                   result->edges[i].u,
                   result->edges[i].v,
                   result->edges[i].w);
        }

        kruskal_free_result(result);
    }
}

// 演示 3: 算法步骤
static void demo_steps(void) {
    printf("\n=== 演示 3: 算法步骤 ===\n");

    printf("Kruskal 算法步骤:\n\n");

    printf("1. 将所有边按权重从小到大排序\n");
    printf("   O(m log m)\n\n");

    printf("2. 初始化并查集\n");
    printf("   每个顶点是一个独立的集合\n\n");

    printf("3. 依次考虑每条边\n");
    printf("   如果边的两个顶点不在同一集合:\n");
    printf("     - 将边加入 MST\n");
    printf("     - 合并两个集合\n\n");

    printf("4. 重复直到选了 n-1 条边\n");
    printf("   (n = 顶点数)\n\n");

    printf("示例:\n");
    printf("  边: (0,1,5), (1,2,3), (0,2,4)\n");
    printf("  排序: (1,2,3), (0,2,4), (0,1,5)\n");
    printf("  选择:\n");
    printf("    - 选 (1,2,3), 合并 {1},{2} -> {1,2}\n");
    printf("    - 选 (0,2,4), 合并 {0},{1,2} -> {0,1,2}\n");
    printf("    - 跳过 (0,1,5), 0和1已在同一集合\n");
}

// 演示 4: 与 Prim 算法对比
static void demo_comparison(void) {
    printf("\n=== 演示 4: Kruskal vs Prim ===\n");

    printf("Kruskal 算法:\n");
    printf("  - 基于边\n");
    printf("  - 全局排序所有边\n");
    printf("  - 使用并查集\n");
    printf("  - 适合稀疏图\n");
    printf("  - 时间: O(m log m)\n\n");

    printf("Prim 算法:\n");
    printf("  - 基于顶点\n");
    printf("  - 局部选择最小边\n");
    printf("  - 使用优先队列\n");
    printf("  - 适合稠密图\n");
    printf("  - 时间: O((n+m) log n)\n\n");

    printf("选择建议:\n");
    printf("  稀疏图 (m ≈ n): Kruskal\n");
    printf("  稠密图 (m ≈ n²): Prim\n");
}

// 演示 5: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 5: 应用场景 ===\n");

    printf("1. 网络设计\n");
    printf("   - 最小成本连接所有节点\n");
    printf("   - 电缆/光纤布线\n");
    printf("\n");

    printf("2. 交通规划\n");
    printf("   - 最小成本道路网络\n");
    printf("   - 铁路线路规划\n");
    printf("\n");

    printf("3. 电路设计\n");
    printf("   - 最小布线成本\n");
    printf("   - VLSI 设计\n");
    printf("\n");

    printf("4. 聚类分析\n");
    printf("   - 最大间距聚类\n");
    printf("   - 图像分割\n");
}

int main(void) {
    printf("========================================\n");
    printf("    Kruskal 最小生成树演示\n");
    printf("========================================\n");

    demo_basic();
    demo_detailed();
    demo_steps();
    demo_comparison();
    demo_applications();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
