/**
 * @file demo_disjoint_set_forest.c
 * @brief 并查集（Disjoint Set Forest）演示
 *
 * 本演示展示了并查集数据结构的应用，包括：
 * - 集合的创建和初始化
 * - 查找和合并操作
 * - 路径压缩和按秩合并
 * - 实际应用场景（连通性检测、最小生成树等）
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "../c_utils/disjoint_set_forest.h"

// 演示 1: 基本操作
static void demo_basic(void) {
    printf("\n=== 演示 1: 基本操作 ===\n");

    printf("创建包含 10 个元素的并查集...\n");
    dsf_error_t error;
    dsf_t *dsf = dsf_create(10, &error);

    if (!dsf) {
        printf("创建失败，错误码: %d\n", error);
        return;
    }

    printf("初始状态:\n");
    printf("  集合数量: %zu\n", dsf->sets);
    printf("  每个元素自成一个集合\n");

    // 合并一些集合
    printf("\n合并操作:\n");
    printf("  合并(0, 1)...\n");
    dsf_union(dsf, 0, 1, &error);
    printf("  合并(2, 3)...\n");
    dsf_union(dsf, 2, 3, &error);
    printf("  合并(1, 2)...\n");
    dsf_union(dsf, 1, 2, &error);
    printf("  合并(4, 5)...\n");
    dsf_union(dsf, 4, 5, &error);

    printf("\n当前集合数量: %zu\n", dsf->sets);

    // 查找操作
    printf("\n查找操作:\n");
    for (int i = 0; i < 6; i++) {
        int root = dsf_find(dsf, i, &error);
        printf("  元素 %d 的根: %d\n", i, root);
    }

    // 检查连通性
    printf("\n连通性检查:\n");
    printf("  0 和 3 连通: %s\n", dsf_same_set(dsf, 0, 3, &error) ? "是" : "否");
    printf("  0 和 5 连通: %s\n", dsf_same_set(dsf, 0, 5, &error) ? "是" : "否");
    printf("  4 和 5 连通: %s\n", dsf_same_set(dsf, 4, 5, &error) ? "是" : "否");

    dsf_free(dsf);
}

// 演示 2: 路径压缩
static void demo_path_compression(void) {
    printf("\n=== 演示 2: 路径压缩 ===\n");

    printf("路径压缩优化:\n");
    printf("  - 查找时扁平化树结构\n");
    printf("  - 使后续查找更快\n");
    printf("  - 几乎常数时间复杂度\n\n");

    dsf_error_t error;
    dsf_t *dsf = dsf_create(7, &error);
    if (!dsf) return;

    // 构建一个链式结构: 0->1->2->3->4->5->6
    printf("构建链式结构: 0->1->2->3->4->5->6\n");
    for (int i = 0; i < 6; i++) {
        dsf_union(dsf, i, i + 1, &error);
    }

    printf("\n查找元素 6 的根（触发路径压缩）...\n");
    int root = dsf_find(dsf, 6, &error);
    printf("  根元素: %d\n", root);

    printf("\n再次查找元素 6（此时路径已压缩）...\n");
    root = dsf_find(dsf, 6, &error);
    printf("  根元素: %d (直接访问)\n", root);

    printf("\n路径压缩后，所有元素直接指向根节点\n");

    dsf_free(dsf);
}

// 演示 3: 按秩合并
static void demo_union_by_rank(void) {
    printf("\n=== 演示 3: 按秩合并 ===\n");

    printf("按秩合并优化:\n");
    printf("  - 将小树合并到大树下\n");
    printf("  - 保持树的平衡\n");
    printf("  - 限制树的高度\n\n");

    dsf_error_t error;
    dsf_t *dsf = dsf_create(8, &error);
    if (!dsf) return;

    printf("创建两个独立集合:\n");
    printf("  集合 A: 合并(0, 1), 合并(1, 2) -> 大小 3\n");
    dsf_union(dsf, 0, 1, &error);
    dsf_union(dsf, 1, 2, &error);

    printf("  集合 B: 合并(3, 4), 合并(4, 5), 合并(5, 6), 合并(6, 7) -> 大小 5\n");
    dsf_union(dsf, 3, 4, &error);
    dsf_union(dsf, 4, 5, &error);
    dsf_union(dsf, 5, 6, &error);
    dsf_union(dsf, 6, 7, &error);

    printf("\n合并两个集合: union(2, 7)\n");
    printf("  按秩合并会将集合 A 合并到集合 B 下\n");
    dsf_union(dsf, 2, 7, &error);

    printf("\n验证所有元素连通:\n");
    for (int i = 1; i < 8; i++) {
        printf("  0 和 %d 连通: %s\n", i, dsf_same_set(dsf, 0, i, &error) ? "是" : "否");
    }

    dsf_free(dsf);
}

// 演示 4: 连通分量检测
static void demo_connected_components(void) {
    printf("\n=== 演示 4: 连通分量检测 ===\n");

    printf("问题: 找出图中的连通分量\n\n");

    // 模拟图的边
    int edges[][2] = {
        {0, 1}, {1, 2}, {3, 4}, {5, 6}, {6, 7}, {7, 5}
    };
    int num_edges = sizeof(edges) / sizeof(edges[0]);

    printf("图的边:\n");
    for (int i = 0; i < num_edges; i++) {
        printf("  (%d, %d)\n", edges[i][0], edges[i][1]);
    }

    dsf_error_t error;
    dsf_t *dsf = dsf_create(8, &error);
    if (!dsf) return;

    printf("\n使用并查集处理...\n");
    for (int i = 0; i < num_edges; i++) {
        dsf_union(dsf, edges[i][0], edges[i][1], &error);
    }

    printf("\n连通分量:\n");
    int component_id[8];
    int num_components = 0;

    for (int i = 0; i < 8; i++) {
        int root = dsf_find(dsf, i, &error);

        // 检查是否是新分量
        bool found = false;
        for (int j = 0; j < num_components; j++) {
            if (component_id[j] == root) {
                found = true;
                break;
            }
        }

        if (!found) {
            component_id[num_components++] = root;

            printf("  分量 %d: { ", num_components);
            for (int k = 0; k < 8; k++) {
                if (dsf_find(dsf, k, &error) == root) {
                    printf("%d ", k);
                }
            }
            printf("}\n");
        }
    }

    printf("\n总连通分量数: %d\n", num_components);

    dsf_free(dsf);
}

// 演示 5: 最小生成树（Kruskal 算法）
static void demo_mst_kruskal(void) {
    printf("\n=== 演示 5: 最小生成树（Kruskal 算法）===\n");

    printf("使用并查集实现 Kruskal 算法\n\n");

    // 图的边: {u, v, weight}
    int edges[][3] = {
        {0, 1, 4},
        {0, 2, 3},
        {1, 2, 1},
        {1, 3, 2},
        {2, 3, 4},
        {3, 4, 2},
        {4, 5, 6}
    };
    int num_edges = sizeof(edges) / sizeof(edges[0]);
    int num_nodes = 6;

    printf("图的边（带权重）:\n");
    for (int i = 0; i < num_edges; i++) {
        printf("  %d --%d--> %d\n", edges[i][0], edges[i][2], edges[i][1]);
    }

    // 按权重排序边（简单冒泡排序）
    for (int i = 0; i < num_edges - 1; i++) {
        for (int j = 0; j < num_edges - i - 1; j++) {
            if (edges[j][2] > edges[j + 1][2]) {
                int temp[3];
                memcpy(temp, edges[j], sizeof(temp));
                memcpy(edges[j], edges[j + 1], sizeof(temp));
                memcpy(edges[j + 1], temp, sizeof(temp));
            }
        }
    }

    printf("\n按权重排序后的边:\n");
    for (int i = 0; i < num_edges; i++) {
        printf("  %d --%d--> %d\n", edges[i][0], edges[i][2], edges[i][1]);
    }

    dsf_error_t error;
    dsf_t *dsf = dsf_create(num_nodes, &error);
    if (!dsf) return;

    printf("\nKruskal 算法执行:\n");
    int mst_edges[10][2];
    int mst_weight = 0;
    int mst_count = 0;

    for (int i = 0; i < num_edges && mst_count < num_nodes - 1; i++) {
        int u = edges[i][0];
        int v = edges[i][1];
        int w = edges[i][2];

        if (!dsf_same_set(dsf, u, v, &error)) {
            dsf_union(dsf, u, v, &error);
            mst_edges[mst_count][0] = u;
            mst_edges[mst_count][1] = v;
            mst_weight += w;
            mst_count++;
            printf("  选择边: %d --%d--> %d\n", u, w, v);
        } else {
            printf("  跳过边: %d --%d--> %d (会形成环)\n", u, w, v);
        }
    }

    printf("\n最小生成树:\n");
    printf("  边数: %d\n", mst_count);
    printf("  总权重: %d\n", mst_weight);
    printf("  边列表:\n");
    for (int i = 0; i < mst_count; i++) {
        printf("    %d - %d\n", mst_edges[i][0], mst_edges[i][1]);
    }

    dsf_free(dsf);
}

// 演示 6: 错误处理
static void demo_errors(void) {
    printf("\n=== 演示 6: 错误处理 ===\n");

    printf("错误码说明:\n");
    printf("  DSF_OK (%d): 成功\n", DSF_OK);
    printf("  DSF_ERROR_INVALID_PARAM (%d): 无效参数\n", DSF_ERROR_INVALID_PARAM);
    printf("  DSF_ERROR_INVALID_ELEMENT (%d): 无效元素\n", DSF_ERROR_INVALID_ELEMENT);
    printf("  DSF_ERROR_MEMORY_ALLOC (%d): 内存分配失败\n", DSF_ERROR_MEMORY_ALLOC);
    printf("  DSF_ERROR_EMPTY_SET (%d): 空集合\n", DSF_ERROR_EMPTY_SET);

    printf("\n错误场景测试:\n");

    dsf_error_t error;

    // 创建 0 个元素
    dsf_t *dsf = dsf_create(0, &error);
    printf("  创建 0 个元素: %s, 错误码: %d\n", dsf ? "成功" : "失败", error);
    if (dsf) dsf_free(dsf);

    // 创建有效并查集
    dsf = dsf_create(5, &error);
    if (dsf) {
        // 无效元素索引
        int result = dsf_find(dsf, -1, &error);
        printf("  查找元素 -1: %s, 错误码: %d\n", result >= 0 ? "成功" : "失败", error);

        result = dsf_find(dsf, 10, &error);
        printf("  查找元素 10: %s, 错误码: %d\n", result >= 0 ? "成功" : "失败", error);

        dsf_free(dsf);
    }
}

// 演示 7: 性能测试
static void demo_performance(void) {
    printf("\n=== 演示 7: 性能测试 ===\n");

    printf("并查集性能测试:\n\n");

    int sizes[] = {1000, 10000, 100000, 1000000};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    printf("%-12s %-15s %-15s\n", "元素数量", "操作次数", "时间(ms)");
    printf("----------------------------------------\n");

    for (int i = 0; i < num_sizes; i++) {
        int n = sizes[i];
        int operations = n * 2;  // n 次合并 + n 次查找

        dsf_error_t error;
        dsf_t *dsf = dsf_create(n, &error);
        if (!dsf) continue;

        // 计时
        clock_t start = clock();

        // 随机合并
        srand(12345);
        for (int j = 0; j < n; j++) {
            int a = rand() % n;
            int b = rand() % n;
            dsf_union(dsf, a, b, &error);
        }

        // 随机查找
        for (int j = 0; j < n; j++) {
            int a = rand() % n;
            dsf_find(dsf, a, &error);
        }

        clock_t end = clock();
        double time_ms = (double)(end - start) * 1000 / CLOCKS_PER_SEC;

        if (n < 1000000) {
            printf("%-12d %-15d %-15.3f\n", n, operations, time_ms);
        } else {
            printf("%-12.1fM %-15d %-15.3f\n", n / 1000000.0, operations, time_ms);
        }

        dsf_free(dsf);
    }

    printf("\n时间复杂度（带路径压缩和按秩合并）:\n");
    printf("  查找: O(α(n)) ≈ O(1)\n");
    printf("  合并: O(α(n)) ≈ O(1)\n");
    printf("  α(n) 是阿克曼函数的反函数，增长极慢\n");
}

// 演示 8: 应用场景
static void demo_use_cases(void) {
    printf("\n=== 演示 8: 应用场景 ===\n");

    printf("并查集的应用场景:\n\n");

    printf("1. 连通性问题:\n");
    printf("   - 网络连通性检测\n");
    printf("   - 社交网络好友圈\n");
    printf("   - 图像区域标记\n\n");

    printf("2. 最小生成树:\n");
    printf("   - Kruskal 算法\n");
    printf("   - 网络设计\n");
    printf("   - 聚类分析\n\n");

    printf("3. 等价关系:\n");
    printf("   - 变量等价性\n");
    printf("   - 类型统一\n");
    printf("   - 编译器优化\n\n");

    printf("4. 迷宫生成:\n");
    printf("   - 随机迷宫生成\n");
    printf("   - 确保连通性\n");
    printf("   - 游戏地图生成\n");
}

// 演示 9: 与其他数据结构比较
static void demo_comparison(void) {
    printf("\n=== 演示 9: 与其他数据结构比较 ===\n");

    printf("连通性检测方法比较:\n\n");

    printf("方法              查找      合并      空间      适用场景\n");
    printf("--------------------------------------------------------\n");
    printf("并查集            O(1)      O(1)      O(n)      动态连通性\n");
    printf("DFS/BFS           O(V+E)    -         O(V)      静态图\n");
    printf("邻接矩阵          O(1)      O(1)      O(V²)     稠密图\n");
    printf("邻接表            O(V)      O(1)      O(V+E)    稀疏图\n");

    printf("\n并查集优势:\n");
    printf("  - 动态处理连通性变化\n");
    printf("  - 接近常数时间的操作\n");
    printf("  - 空间效率高\n");
    printf("  - 实现简单\n");
}

// 演示 10: 最佳实践
static void demo_best_practices(void) {
    printf("\n=== 演示 10: 最佳实践 ===\n");

    printf("并查集最佳实践:\n\n");

    printf("1. 优化策略:\n");
    printf("   - 始终使用路径压缩\n");
    printf("   - 始终使用按秩合并\n");
    printf("   - 两者结合效果最佳\n\n");

    printf("2. 实现细节:\n");
    printf("   - 使用数组存储父节点\n");
    printf("   - 用负数表示根节点和大小\n");
    printf("   - 或分别存储 parent 和 rank\n\n");

    printf("3. 注意事项:\n");
    printf("   - 检查元素范围\n");
    printf("   - 处理错误返回值\n");
    printf("   - 及时释放内存\n\n");

    printf("4. 扩展应用:\n");
    printf("   - 记录集合大小\n");
    printf("   - 维护额外信息\n");
    printf("   - 支持删除操作\n");
}

int main(void) {
    printf("========================================\n");
    printf("    并查集（Disjoint Set Forest）演示\n");
    printf("========================================\n");

    demo_basic();
    demo_path_compression();
    demo_union_by_rank();
    demo_connected_components();
    demo_mst_kruskal();
    demo_errors();
    demo_performance();
    demo_use_cases();
    demo_comparison();
    demo_best_practices();

    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("========================================\n");

    return 0;
}
