/**
 * 并查集森林演示程序
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/disjoint_set_forest.h"

static void demo_basic_usage(void) {
    printf("\n=== 演示 1: 基本使用 ===\n");

    size_t n = 10;
    dsf_error_t error;
    dsf_t *dsf = dsf_create(n, &error);
    if (!dsf) {
        printf("创建失败: %s\n", dsf_strerror(error));
        return;
    }
    printf("创建并查集: %zu 个元素 (0-%zu)\n", n, n - 1);
    printf("初始集合数: %zu\n", dsf_set_count(dsf, NULL));

    printf("\n合并操作:\n");
    dsf_union(dsf, 0, 1, NULL);
    printf("  union(0, 1)\n");
    dsf_union(dsf, 2, 3, NULL);
    printf("  union(2, 3)\n");
    dsf_union(dsf, 1, 2, NULL);
    printf("  union(1, 2) - 连接两个集合\n");

    printf("\n查询操作:\n");
    printf("  find(0) = %d\n", dsf_find(dsf, 0, NULL));
    printf("  find(1) = %d\n", dsf_find(dsf, 1, NULL));
    printf("  find(2) = %d\n", dsf_find(dsf, 2, NULL));
    printf("  find(3) = %d\n", dsf_find(dsf, 3, NULL));

    printf("\n连通性检查:\n");
    printf("  0 和 3 连通: %s\n", dsf_same_set(dsf, 0, 3, NULL) ? "是" : "否");
    printf("  0 和 5 连通: %s\n", dsf_same_set(dsf, 0, 5, NULL) ? "是" : "否");

    printf("\n当前集合数: %zu\n", dsf_set_count(dsf, NULL));

    dsf_free(dsf);
}

static void demo_connected_components(void) {
    printf("\n=== 演示 2: 连通分量 ===\n");

    size_t n = 8;
    dsf_t *dsf = dsf_create(n, NULL);

    printf("创建 %zu 个节点的图\n", n);
    printf("添加边: (0-1), (1-2), (3-4), (5-6), (6-7)\n");

    dsf_union(dsf, 0, 1, NULL);
    dsf_union(dsf, 1, 2, NULL);
    dsf_union(dsf, 3, 4, NULL);
    dsf_union(dsf, 5, 6, NULL);
    dsf_union(dsf, 6, 7, NULL);

    printf("\n连通分量分析:\n");
    printf("  分量 1: {0, 1, 2}\n");
    printf("  分量 2: {3, 4}\n");
    printf("  分量 3: {5, 6, 7}\n");

    printf("\n验证连通性:\n");
    printf("  0-2 连通: %s\n", dsf_same_set(dsf, 0, 2, NULL) ? "是" : "否");
    printf("  3-4 连通: %s\n", dsf_same_set(dsf, 3, 4, NULL) ? "是" : "否");
    printf("  5-7 连通: %s\n", dsf_same_set(dsf, 5, 7, NULL) ? "是" : "否");
    printf("  0-5 连通: %s\n", dsf_same_set(dsf, 0, 5, NULL) ? "是" : "否");

    printf("\n连通分量数量: %zu\n", dsf_set_count(dsf, NULL));

    dsf_free(dsf);
}

static void demo_maze_example(void) {
    printf("\n=== 演示 3: 迷宫生成示例 ===\n");

    int rows = 3, cols = 3;
    int n = rows * cols;
    dsf_t *dsf = dsf_create(n, NULL);

    printf("创建 %dx%d 迷宫网格 (%d 个单元格)\n", rows, cols, n);
    printf("随机合并相邻单元格直到起点和终点连通...\n\n");

    int start = 0;
    int end = n - 1;

    srand(42);
    int steps = 0;
    while (!dsf_same_set(dsf, start, end, NULL) && steps < 100) {
        int cell = rand() % n;
        int neighbors[4];
        int num_neighbors = 0;

        int r = cell / cols;
        int c = cell % cols;

        if (r > 0) neighbors[num_neighbors++] = cell - cols;
        if (r < rows - 1) neighbors[num_neighbors++] = cell + cols;
        if (c > 0) neighbors[num_neighbors++] = cell - 1;
        if (c < cols - 1) neighbors[num_neighbors++] = cell + 1;

        if (num_neighbors > 0) {
            int neighbor = neighbors[rand() % num_neighbors];
            dsf_union(dsf, cell, neighbor, NULL);
        }
        steps++;
    }

    printf("在 %d 步后连通\n", steps);
    printf("起点(0)和终点(%d)连通: %s\n", end, dsf_same_set(dsf, start, end, NULL) ? "是" : "否");
    printf("最终集合数: %zu\n", dsf_set_count(dsf, NULL));

    dsf_free(dsf);
}

int main(void) {
    printf("========================================\n");
    printf("    并查集森林演示\n");
    printf("========================================\n");

    demo_basic_usage();
    demo_connected_components();
    demo_maze_example();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
