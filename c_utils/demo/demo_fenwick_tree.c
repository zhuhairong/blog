/**
 * 树状数组 (Fenwick Tree) 演示程序
 * 
 * 功能：
 * - 单点更新和区间查询
 * - 前缀和查询
 * - 批量操作
 * - 与其他数据结构对比
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../c_utils/fenwick_tree.h"

// 打印数组
static void print_array(const char *name, double *arr, int n) {
    printf("%s: [", name);
    for (int i = 0; i < n; i++) {
        printf("%.0f", arr[i]);
        if (i < n - 1) printf(", ");
    }
    printf("]\n");
}

// 演示 1: 基本操作
static void demo_basic(void) {
    printf("\n=== 演示 1: 基本操作 ===\n");
    
    fenwick_error_t error;
    
    // 创建树状数组
    printf("创建大小为 10 的树状数组 (整数类型):\n");
    fenwick_tree_t *ft = fenwick_tree_create(10, FENWICK_TYPE_INT, &error);
    if (!ft) {
        printf("创建失败: %d\n", error);
        return;
    }
    
    // 初始化数据
    double values[] = {1, 7, 3, 0, 5, 8, 3, 2, 6, 4};
    printf("\n初始化数据: ");
    print_array("", values, 10);
    
    // 构建树状数组
    printf("\n构建树状数组:\n");
    for (int i = 0; i < 10; i++) {
        fenwick_tree_update(ft, i + 1, values[i], &error);
    }
    printf("  已构建完成\n");
    
    // 查询前缀和
    printf("\n前缀和查询:\n");
    double result;
    for (int i = 1; i <= 10; i++) {
        fenwick_tree_query(ft, i, &result, &error);
        printf("  sum[1..%d] = %.0f\n", i, result);
    }
    
    // 区间查询
    printf("\n区间和查询:\n");
    fenwick_tree_range_query(ft, 3, 7, &result, &error);
    printf("  sum[3..7] = %.0f (3+0+5+8+3=19)\n", result);
    
    fenwick_tree_range_query(ft, 1, 5, &result, &error);
    printf("  sum[1..5] = %.0f (1+7+3+0+5=16)\n", result);
    
    fenwick_tree_free(ft);
}

// 演示 2: 动态更新
static void demo_dynamic_update(void) {
    printf("\n=== 演示 2: 动态更新 ===\n");
    
    fenwick_error_t error;
    fenwick_tree_t *ft = fenwick_tree_create(8, FENWICK_TYPE_INT, &error);
    
    printf("初始状态 (全 0):\n");
    printf("  数组: [0, 0, 0, 0, 0, 0, 0, 0]\n");
    
    // 逐步更新
    printf("\n逐步更新:\n");
    
    fenwick_tree_update(ft, 3, 5, &error);
    printf("  update(3, 5): 在位置 3 增加 5\n");
    
    fenwick_tree_update(ft, 5, 3, &error);
    printf("  update(5, 3): 在位置 5 增加 3\n");
    
    fenwick_tree_update(ft, 3, 2, &error);
    printf("  update(3, 2): 在位置 3 再增加 2\n");
    
    fenwick_tree_update(ft, 7, 4, &error);
    printf("  update(7, 4): 在位置 7 增加 4\n");
    
    // 查询结果
    double result;
    printf("\n最终状态查询:\n");
    fenwick_tree_query(ft, 8, &result, &error);
    printf("  sum[1..8] = %.0f (5+3+2+4=14)\n", result);
    
    fenwick_tree_range_query(ft, 1, 5, &result, &error);
    printf("  sum[1..5] = %.0f (5+3+2=10)\n", result);
    
    fenwick_tree_free(ft);
}

// 演示 3: 批量操作
static void demo_batch_operations(void) {
    printf("\n=== 演示 3: 批量操作 ===\n");
    
    fenwick_error_t error;
    fenwick_tree_t *ft = fenwick_tree_create(10, FENWICK_TYPE_INT, &error);
    
    // 批量更新
    printf("批量更新:\n");
    size_t indices[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    double values[] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    
    printf("  更新位置: ");
    for (int i = 0; i < 10; i++) {
        printf("%zu ", indices[i]);
    }
    printf("\n");
    printf("  更新数值: ");
    for (int i = 0; i < 10; i++) {
        printf("%.0f ", values[i]);
    }
    printf("\n");
    
    fenwick_tree_batch_update(ft, indices, values, 10, &error);
    
    // 批量查询
    printf("\n批量查询前缀和:\n");
    size_t query_indices[] = {3, 5, 7, 10};
    double results[4];
    
    fenwick_tree_batch_query(ft, query_indices, results, 4, &error);
    
    for (int i = 0; i < 4; i++) {
        printf("  sum[1..%zu] = %.0f\n", query_indices[i], results[i]);
    }
    
    fenwick_tree_free(ft);
}

// 演示 4: 不同数据类型
static void demo_data_types(void) {
    printf("\n=== 演示 4: 不同数据类型 ===\n");
    
    fenwick_error_t error;
    
    // 整数类型
    printf("整数类型 (FENWICK_TYPE_INT):\n");
    fenwick_tree_t *ft_int = fenwick_tree_create(5, FENWICK_TYPE_INT, &error);
    for (int i = 1; i <= 5; i++) {
        fenwick_tree_update(ft_int, i, i * 10, &error);
    }
    double result;
    fenwick_tree_query(ft_int, 5, &result, &error);
    printf("  sum[1..5] = %.0f\n", result);
    fenwick_tree_free(ft_int);
    
    // 浮点类型
    printf("\n浮点类型 (FENWICK_TYPE_FLOAT):\n");
    fenwick_tree_t *ft_float = fenwick_tree_create(5, FENWICK_TYPE_FLOAT, &error);
    for (int i = 1; i <= 5; i++) {
        fenwick_tree_update(ft_float, i, i * 1.5, &error);
    }
    fenwick_tree_query(ft_float, 5, &result, &error);
    printf("  sum[1..5] = %.2f (1.5+3.0+4.5+6.0+7.5=22.5)\n", result);
    fenwick_tree_free(ft_float);
    
    // 双精度类型
    printf("\n双精度类型 (FENWICK_TYPE_DOUBLE):\n");
    fenwick_tree_t *ft_double = fenwick_tree_create(5, FENWICK_TYPE_DOUBLE, &error);
    for (int i = 1; i <= 5; i++) {
        fenwick_tree_update(ft_double, i, i * 0.1, &error);
    }
    fenwick_tree_query(ft_double, 5, &result, &error);
    printf("  sum[1..5] = %.4f (0.1+0.2+0.3+0.4+0.5=1.5)\n", result);
    fenwick_tree_free(ft_double);
}

// 演示 5: 算法原理
static void demo_algorithm(void) {
    printf("\n=== 演示 5: 算法原理 ===\n");
    
    printf("树状数组结构:\n");
    printf("  基于二进制索引的树形结构\n");
    printf("  每个节点存储一个区间的和\n");
    printf("  区间长度 = lowbit(index)\n");
    printf("\n");
    
    printf("lowbit 运算:\n");
    printf("  lowbit(x) = x & (-x)\n");
    printf("  得到 x 的最低位 1 的值\n");
    printf("\n");
    
    printf("示例:\n");
    printf("  lowbit(1) = 1  (二进制: 0001)\n");
    printf("  lowbit(2) = 2  (二进制: 0010)\n");
    printf("  lowbit(3) = 1  (二进制: 0011)\n");
    printf("  lowbit(4) = 4  (二进制: 0100)\n");
    printf("  lowbit(5) = 1  (二进制: 0101)\n");
    printf("  lowbit(6) = 2  (二进制: 0110)\n");
    printf("\n");
    
    printf("更新操作:\n");
    printf("  从位置 i 开始，不断加上 lowbit(i)\n");
    printf("  直到超过数组范围\n");
    printf("\n");
    
    printf("查询操作:\n");
    printf("  从位置 i 开始，不断减去 lowbit(i)\n");
    printf("  直到 i 变为 0\n");
}

// 演示 6: 性能对比
static void demo_performance(void) {
    printf("\n=== 演示 6: 性能对比 ===\n");
    
    printf("树状数组 vs 普通数组:\n");
    printf("\n");
    printf("操作          普通数组    树状数组\n");
    printf("-----------------------------------\n");
    printf("单点更新      O(1)        O(log n)\n");
    printf("前缀和查询    O(n)        O(log n)\n");
    printf("区间和查询    O(n)        O(log n)\n");
    printf("\n");
    
    printf("树状数组 vs 线段树:\n");
    printf("\n");
    printf("特性          树状数组    线段树\n");
    printf("-----------------------------------\n");
    printf("代码复杂度    简单        较复杂\n");
    printf("空间复杂度    O(n)        O(4n)\n");
    printf("更新操作      O(log n)    O(log n)\n");
    printf("查询操作      O(log n)    O(log n)\n");
    printf("区间最值      不支持      支持\n");
    printf("\n");
    
    printf("适用场景:\n");
    printf("  - 频繁的单点更新和区间查询\n");
    printf("  - 前缀和问题\n");
    printf("  - 逆序对统计\n");
    printf("  - 动态频率统计\n");
}

// 演示 7: 实际应用
static void demo_applications(void) {
    printf("\n=== 演示 7: 实际应用场景 ===\n");
    
    printf("1. 逆序对统计\n");
    printf("   问题: 统计数组中逆序对的数量\n");
    printf("   解法: 从后向前遍历，查询已出现的小于当前元素的个数\n");
    printf("   复杂度: O(n log n)\n");
    printf("\n");
    
    printf("2. 动态频率统计\n");
    printf("   问题: 实时统计数字出现频率\n");
    printf("   解法: 用树状数组维护频率\n");
    printf("   应用: 数据流处理、实时分析\n");
    printf("\n");
    
    printf("3. 区间修改单点查询\n");
    printf("   问题: 区间增加，单点查询\n");
    printf("   解法: 差分 + 树状数组\n");
    printf("\n");
    
    printf("4. 二维树状数组\n");
    printf("   问题: 矩阵区域和查询\n");
    printf("   解法: 树状数组的二维扩展\n");
    printf("   应用: 图像处理、矩阵操作\n");
}

int main(void) {
    printf("========================================\n");
    printf("    树状数组 (Fenwick Tree) 演示\n");
    printf("========================================\n");
    
    demo_basic();
    demo_dynamic_update();
    demo_batch_operations();
    demo_data_types();
    demo_algorithm();
    demo_performance();
    demo_applications();
    
    printf("\n========================================\n");
    printf("演示完成!\n");
    
    return 0;
}
