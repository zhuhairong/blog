/**
 * Prim 算法演示程序
 *
 * 功能：
 * - 最小生成树计算
 * - 图操作
 * - 结果分析
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/prim.h"

// 演示 1: 基本概念
static void demo_concept(void) {
    printf("\n=== 演示 1: Prim 算法基本概念 ===\n");

    printf("Prim 算法:\n\n");

    printf("目的:\n");
    printf("  - 找到最小生成树 (MST)\n");
    printf("  - 连接所有节点的最小权重边集\n\n");

    printf("工作原理:\n");
    printf("  1. 从任意节点开始\n");
    printf("  2. 选择连接已选节点和未选节点的最小权重边\n");
    printf("  3. 重复直到所有节点都被选中\n\n");

    printf("时间复杂度:\n");
    printf("  - 邻接矩阵: O(V²)\n");
    printf("  - 邻接表 + 优先队列: O(E log V)\n\n");

    printf("适用场景:\n");
    printf("  - 稠密图\n");
    printf("  - 需要 MST 的应用\n");
}

// 演示 2: 数据结构
static void demo_data_structures(void) {
    printf("\n=== 演示 2: 数据结构 ===\n");

    printf("prim_edge_t 结构:\n");
    printf("  - src: 源节点\n");
    printf("  - dest: 目标节点\n");
    printf("  - weight: 边权重\n\n");

    printf("prim_result_t 结构:\n");
    printf("  - total_weight: MST 总权重\n");
    printf("  - edges: MST 边数组\n");
    printf("  - edge_count: MST 边数\n");
    printf("  - connected: 图是否连通\n\n");

    printf("prim_graph_t 结构:\n");
    printf("  - n: 节点数\n");
    printf("  - adj: 邻接矩阵\n");
    printf("  - config: 配置\n");
}

// 演示 3: 算法示例
static void demo_algorithm(void) {
    printf("\n=== 演示 3: 算法示例 ===\n");

    printf("示例图 (5个节点):\n");
    printf("\n");
    printf("    0 ---2--- 1\n");
    printf("    |         |\\\n");
    printf("    6         3 5\n");
    printf("    |         |  \\n");
    printf("    3 ---9--- 4---7\n");
    printf("              /\n");
    printf("             2\n");
    printf("\n");

    printf("边列表:\n");
    printf("  0-1: 2\n");
    printf("  0-3: 6\n");
    printf("  1-2: 3\n");
    printf("  1-3: 8\n");
    printf("  1-4: 5\n");
    printf("  2-4: 7\n");
    printf("  3-4: 9\n");
    printf("\n");

    printf("Prim 算法执行过程:\n");
    printf("  1. 从节点 0 开始\n");
    printf("  2. 选择最小边 0-1 (权重 2)\n");
    printf("  3. 选择最小边 1-2 (权重 3)\n");
    printf("  4. 选择最小边 1-4 (权重 5)\n");
    printf("  5. 选择最小边 0-3 (权重 6)\n");
    printf("\n");

    printf("MST 结果:\n");
    printf("  边: (0,1,2), (1,2,3), (1,4,5), (0,3,6)\n");
    printf("  总权重: 2+3+5+6 = 16\n");
}

// 演示 4: 配置选项
static void demo_config(void) {
    printf("\n=== 演示 4: 配置选项 ===\n");

    prim_config_t config;
    config.max_nodes = 100;
    config.infinity = 1000000;
    config.directed = false;
    config.compute_edges = true;

    printf("配置结构:\n");
    printf("  max_nodes: %zu (最大节点数)\n", config.max_nodes);
    printf("  infinity: %d (无穷大值)\n", config.infinity);
    printf("  directed: %s (是否为有向图)\n", config.directed ? "是" : "否");
    printf("  compute_edges: %s (是否计算 MST 边)\n", config.compute_edges ? "是" : "否");

    printf("\n常量定义:\n");
    printf("  PRIM_DEFAULT_MAX_NODES = %d\n", PRIM_DEFAULT_MAX_NODES);
    printf("  PRIM_DEFAULT_INFINITY = %d\n", PRIM_DEFAULT_INFINITY);
}

// 演示 5: 错误处理
static void demo_errors(void) {
    printf("\n=== 演示 5: 错误处理 ===\n");

    printf("可能的错误码:\n");
    printf("  PRIM_OK - 成功\n");
    printf("  PRIM_ERROR_NULL_PTR - 空指针\n");
    printf("  PRIM_ERROR_INVALID_GRAPH - 无效图结构\n");
    printf("  PRIM_ERROR_TOO_MANY_NODES - 节点数过多\n");
    printf("  PRIM_ERROR_NO_CONNECTION - 图不连通\n");

    printf("\n错误处理示例:\n");
    printf("  - 创建图时检查返回值\n");
    printf("  - 添加边时验证节点索引\n");
    printf("  - 确保图连通后再计算 MST\n");
}

// 演示 6: 与其他算法对比
static void demo_comparison(void) {
    printf("\n=== 演示 6: MST 算法对比 ===\n");

    printf("Prim 算法:\n");
    printf("  - 适合稠密图\n");
    printf("  - 从节点出发扩展\n");
    printf("  - 时间复杂度: O(V²) 或 O(E log V)\n\n");

    printf("Kruskal 算法:\n");
    printf("  - 适合稀疏图\n");
    printf("  - 按边权重排序\n");
    printf("  - 时间复杂度: O(E log E)\n\n");

    printf("选择建议:\n");
    printf("  - E < V² / log V: 使用 Kruskal\n");
    printf("  - E >= V² / log V: 使用 Prim\n");
}

// 演示 7: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 7: 应用场景 ===\n");

    printf("1. 网络设计\n");
    printf("   - 最小成本网络连接\n");
    printf("   - 电缆布线\n");
    printf("   - 通信网络\n\n");

    printf("2. 交通规划\n");
    printf("   - 道路建设\n");
    printf("   - 最小成本连接城市\n\n");

    printf("3. 电路设计\n");
    printf("   - 最小布线\n");
    printf("   - 芯片设计\n\n");

    printf("4. 聚类分析\n");
    printf("   - 单链接聚类\n");
    printf("   - 图像分割\n\n");

    printf("5. 近似算法\n");
    printf("   - 旅行商问题近似\n");
    printf("   - 斯坦纳树问题\n");
}

int main(void) {
    printf("========================================\n");
    printf("    Prim 最小生成树算法演示\n");
    printf("========================================\n");

    demo_concept();
    demo_data_structures();
    demo_algorithm();
    demo_config();
    demo_errors();
    demo_comparison();
    demo_applications();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
