/**
 * Prim 算法演示程序
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/prim.h"

static void demo_basic_mst(void) {
    printf("\n=== 演示 1: 基本最小生成树 ===\n");

    prim_graph_t *graph = prim_graph_create(5, NULL, NULL);
    if (!graph) {
        printf("创建图失败\n");
        return;
    }

    printf("创建 5 个节点的图\n");
    printf("添加边:\n");
    prim_graph_add_edge(graph, 0, 1, 2, NULL);
    printf("  0-1, 权重=2\n");
    prim_graph_add_edge(graph, 0, 3, 6, NULL);
    printf("  0-3, 权重=6\n");
    prim_graph_add_edge(graph, 1, 2, 3, NULL);
    printf("  1-2, 权重=3\n");
    prim_graph_add_edge(graph, 1, 3, 8, NULL);
    printf("  1-3, 权重=8\n");
    prim_graph_add_edge(graph, 1, 4, 5, NULL);
    printf("  1-4, 权重=5\n");
    prim_graph_add_edge(graph, 2, 4, 7, NULL);
    printf("  2-4, 权重=7\n");
    prim_graph_add_edge(graph, 3, 4, 9, NULL);
    printf("  3-4, 权重=9\n");

    prim_result_t result;
    prim_error_t error;

    bool ok = prim_mst_ex(graph, &result, &error);
    if (ok) {
        printf("\n最小生成树:\n");
        printf("  总权重: %d\n", result.total_weight);
        printf("  边数: %zu\n", result.edge_count);
        printf("  边列表:\n");
        for (size_t i = 0; i < result.edge_count; i++) {
            printf("    %d - %d, 权重=%d\n",
                   result.edges[i].src,
                   result.edges[i].dest,
                   result.edges[i].weight);
        }
        printf("  图连通: %s\n", result.connected ? "是" : "否");

        prim_result_free(&result);
    } else {
        printf("计算 MST 失败: %d\n", error);
    }

    prim_graph_destroy(graph);
}

static void demo_dense_graph(void) {
    printf("\n=== 演示 2: 稠密图 ===\n");

    prim_graph_t *graph = prim_graph_create(4, NULL, NULL);
    if (!graph) {
        printf("创建图失败\n");
        return;
    }

    printf("创建 4x4 完全图\n");
    prim_graph_add_edge(graph, 0, 1, 10, NULL);
    prim_graph_add_edge(graph, 0, 2, 6, NULL);
    prim_graph_add_edge(graph, 0, 3, 5, NULL);
    prim_graph_add_edge(graph, 1, 2, 3, NULL);
    prim_graph_add_edge(graph, 1, 3, 15, NULL);
    prim_graph_add_edge(graph, 2, 3, 4, NULL);

    prim_result_t result;
    prim_error_t error;

    bool ok = prim_mst_ex(graph, &result, &error);
    if (ok) {
        printf("MST 总权重: %d\n", result.total_weight);
        for (size_t i = 0; i < result.edge_count; i++) {
            printf("  %d - %d (%d)\n",
                   result.edges[i].src,
                   result.edges[i].dest,
                   result.edges[i].weight);
        }
        prim_result_free(&result);
    }

    prim_graph_destroy(graph);
}

static void demo_disconnected_graph(void) {
    printf("\n=== 演示 3: 非连通图 ===\n");

    prim_graph_t *graph = prim_graph_create(4, NULL, NULL);
    if (!graph) {
        printf("创建图失败\n");
        return;
    }

    printf("创建 4 个节点的图（只添加 2 条边，不连通）\n");
    prim_graph_add_edge(graph, 0, 1, 1, NULL);
    prim_graph_add_edge(graph, 2, 3, 2, NULL);

    prim_result_t result;
    prim_error_t error;

    bool ok = prim_mst_ex(graph, &result, &error);
    if (ok) {
        printf("MST 总权重: %d\n", result.total_weight);
        printf("图连通: %s\n", result.connected ? "是" : "否");
        printf("边数: %zu\n", result.edge_count);
        prim_result_free(&result);
    } else {
        printf("计算失败: %d\n", error);
    }

    prim_graph_destroy(graph);
}

int main(void) {
    printf("========================================\n");
    printf("    Prim 算法演示\n");
    printf("========================================\n");

    demo_basic_mst();
    demo_dense_graph();
    demo_disconnected_graph();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
