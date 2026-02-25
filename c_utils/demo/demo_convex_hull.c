/**
 * @file demo_convex_hull.c
 * @brief 凸包算法演示
 *
 * 本演示展示了凸包算法的实现和应用
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../c_utils/convex_hull.h"

// 演示 1: 基本凸包计算
static void demo_basic(void) {
    printf("\n=== 演示 1: 基本凸包计算 ===\n");

    // 创建点集 - 正方形
    point_2d_t points[] = {
        {0, 0}, {2, 0}, {2, 2}, {0, 2}
    };
    size_t n = sizeof(points) / sizeof(points[0]);

    printf("原始点集 (%zu个点):\n", n);
    for (size_t i = 0; i < n; i++) {
        printf("  (%.1f, %.1f)\n", points[i].x, points[i].y);
    }

    // 计算凸包
    convex_hull_error_t error;
    convex_hull_t* hull = convex_hull_compute(points, n, 
                                               CONVEX_HULL_ALGORITHM_MONOTONE_CHAIN, 
                                               &error);
    
    if (hull) {
        printf("\n凸包 (%zu个点):\n", hull->count);
        for (size_t i = 0; i < hull->count; i++) {
            printf("  (%.2f, %.2f)\n", hull->points[i].x, hull->points[i].y);
        }
        convex_hull_free(hull);
    } else {
        printf("计算失败，错误码: %d\n", error);
    }
}

// 演示 2: 错误处理
static void demo_error_handling(void) {
    printf("\n=== 演示 2: 错误处理 ===\n");

    printf("错误码说明:\n");
    printf("  CONVEX_HULL_OK (%d): 成功\n", CONVEX_HULL_OK);
    printf("  CONVEX_HULL_ERROR_INVALID_POINTS (%d): 无效点集\n", CONVEX_HULL_ERROR_INVALID_POINTS);
    printf("  CONVEX_HULL_ERROR_INSUFFICIENT_POINTS (%d): 点数量不足\n", CONVEX_HULL_ERROR_INSUFFICIENT_POINTS);
    printf("  CONVEX_HULL_ERROR_INVALID_HULL (%d): 无效凸包\n", CONVEX_HULL_ERROR_INVALID_HULL);
    printf("  CONVEX_HULL_ERROR_MEMORY_ALLOC (%d): 内存分配失败\n", CONVEX_HULL_ERROR_MEMORY_ALLOC);

    // 测试不足的点
    printf("\n测试不足的点:\n");
    point_2d_t single_point[] = {{1, 1}};
    convex_hull_error_t error;
    convex_hull_t* hull = convex_hull_compute(single_point, 1, 
                                               CONVEX_HULL_ALGORITHM_MONOTONE_CHAIN, 
                                               &error);
    if (!hull) {
        printf("  单点凸包失败: 错误码 %d\n", error);
    }

    // 测试两个点
    point_2d_t two_points[] = {{0, 0}, {1, 1}};
    hull = convex_hull_compute(two_points, 2, 
                               CONVEX_HULL_ALGORITHM_MONOTONE_CHAIN, 
                               &error);
    if (hull) {
        printf("  两点凸包成功: %zu个点\n", hull->count);
        convex_hull_free(hull);
    } else {
        printf("  两点凸包失败: 错误码 %d\n", error);
    }
}

// 演示 3: 应用场景
static void demo_use_cases(void) {
    printf("\n=== 演示 3: 应用场景 ===\n");

    printf("凸包算法的应用场景:\n\n");

    printf("1. 计算机图形学:\n");
    printf("   - 碰撞检测\n");
    printf("   - 包围盒计算\n");
    printf("   - 形状分析\n\n");

    printf("2. 地理信息系统:\n");
    printf("   - 区域边界计算\n");
    printf("   - 选址问题\n");
    printf("   - 路径规划\n\n");

    printf("3. 图像处理:\n");
    printf("   - 物体轮廓提取\n");
    printf("   - 形状识别\n");
    printf("   - 特征提取\n\n");

    printf("4. 机器人学:\n");
    printf("   - 可达空间计算\n");
    printf("   - 运动规划\n");
    printf("   - 避障\n\n");

    printf("5. 统计学:\n");
    printf("   - 异常值检测\n");
    printf("   - 数据边界分析\n");
    printf("   - 聚类分析\n");
}

// 演示 4: 算法说明
static void demo_algorithms_info(void) {
    printf("\n=== 演示 4: 算法说明 ===\n");

    printf("凸包算法:\n\n");

    printf("1. Monotone Chain (Andrew算法):\n");
    printf("   - 时间复杂度: O(n log n)\n");
    printf("   - 先排序，再分别构建上下凸包\n");
    printf("   - 数值稳定性好\n\n");

    printf("2. Graham Scan:\n");
    printf("   - 时间复杂度: O(n log n)\n");
    printf("   - 选择一个极点，按角度排序\n");
    printf("   - 使用栈构建凸包\n\n");

    printf("3. 算法选择建议:\n");
    printf("   - 一般情况: 使用 Monotone Chain\n");
    printf("   - 已知极值点: Graham Scan 可能更快\n");
    printf("   - 不确定时: 使用 AUTO 让库自动选择\n");
}

// 演示 5: API 说明
static void demo_api_info(void) {
    printf("\n=== 演示 5: API 说明 ===\n");

    printf("主要 API 函数:\n\n");

    printf("1. convex_hull_create():\n");
    printf("   - 创建空的凸包结构\n");
    printf("   - 返回: convex_hull_t*\n\n");

    printf("2. convex_hull_compute(pts, n, algorithm, error):\n");
    printf("   - 从点集计算凸包\n");
    printf("   - pts: 点数组\n");
    printf("   - n: 点的数量\n");
    printf("   - algorithm: 算法选择\n");
    printf("   - error: 错误码输出\n\n");

    printf("3. convex_hull_free(hull):\n");
    printf("   - 释放凸包结构\n\n");

    printf("数据结构:\n");
    printf("  point_2d_t: { double x, y; }\n");
    printf("  convex_hull_t: { point_2d_t* points, size_t count, ... }\n");
}

int main(void) {
    printf("========================================\n");
    printf("    凸包算法演示程序\n");
    printf("========================================\n");

    demo_basic();
    demo_error_handling();
    demo_use_cases();
    demo_algorithms_info();
    demo_api_info();

    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("========================================\n");

    return 0;
}
