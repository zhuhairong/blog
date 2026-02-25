/**
 * 线段交点计算演示程序
 *
 * 功能：
 * - 线段交点计算
 * - 点在线段上判断
 * - 平行和共线检测
 */

#include <stdio.h>
#include <math.h>
#include "../c_utils/line_intersection.h"

// 演示 1: 基本交点计算
static void demo_basic(void) {
    printf("\n=== 演示 1: 基本交点计算 ===\n");

    // 两条相交线段
    point_t p1 = {0, 0}, p2 = {10, 10};
    point_t p3 = {0, 10}, p4 = {10, 0};

    printf("线段 1: (%.1f, %.1f) -> (%.1f, %.1f)\n", p1.x, p1.y, p2.x, p2.y);
    printf("线段 2: (%.1f, %.1f) -> (%.1f, %.1f)\n", p3.x, p3.y, p4.x, p4.y);

    point_t intersection;
    if (line_intersection(p1, p2, p3, p4, &intersection)) {
        printf("\n交点: (%.2f, %.2f)\n", intersection.x, intersection.y);
    } else {
        printf("\n无交点\n");
    }
}

// 演示 2: 各种位置关系
static void demo_positions(void) {
    printf("\n=== 演示 2: 线段位置关系 ===\n");

    struct {
        point_t p1, p2, p3, p4;
        const char *desc;
    } cases[] = {
        {{0, 0}, {10, 10}, {5, 5}, {15, 5}, "相交"},
        {{0, 0}, {5, 5}, {10, 10}, {15, 15}, "平行（不相交）"},
        {{0, 0}, {10, 0}, {5, 0}, {15, 0}, "共线重叠"},
        {{0, 0}, {5, 5}, {6, 6}, {10, 10}, "平行（不相交）"},
    };

    for (int i = 0; i < 4; i++) {
        printf("\n情况 %d: %s\n", i + 1, cases[i].desc);
        printf("  线段 1: (%.0f,%.0f)->(%.0f,%.0f)\n",
               cases[i].p1.x, cases[i].p1.y, cases[i].p2.x, cases[i].p2.y);
        printf("  线段 2: (%.0f,%.0f)->(%.0f,%.0f)\n",
               cases[i].p3.x, cases[i].p3.y, cases[i].p4.x, cases[i].p4.y);

        point_t inter;
        bool has_inter = line_intersection(cases[i].p1, cases[i].p2,
                                           cases[i].p3, cases[i].p4, &inter);
        printf("  结果: %s\n", has_inter ? "相交" : "不相交");
    }
}

// 演示 3: 点在线段上
static void demo_point_on_segment(void) {
    printf("\n=== 演示 3: 点在线段上判断 ===\n");

    line_segment_t seg = {{0, 0}, {10, 10}};

    point_t points[] = {
        {5, 5},     // 在线段上
        {0, 0},     // 端点
        {10, 10},   // 端点
        {15, 15},   // 延长线上
        {5, 6},     // 不在线段上
    };

    printf("线段: (%.0f,%.0f) -> (%.0f,%.0f)\n\n",
           seg.p1.x, seg.p1.y, seg.p2.x, seg.p2.y);

    printf("点位置判断:\n");
    for (int i = 0; i < 5; i++) {
        bool on_seg = point_on_segment(points[i], seg, 0.0001);
        printf("  (%.0f, %.0f): %s\n",
               points[i].x, points[i].y, on_seg ? "在线段上" : "不在线段上");
    }
}

// 演示 4: 距离计算
static void demo_distance(void) {
    printf("\n=== 演示 4: 距离计算 ===\n");

    point_t line_p1 = {0, 0}, line_p2 = {10, 0};
    point_t points[] = {
        {5, 0},
        {5, 3},
        {5, -4},
        {0, 5},
    };

    printf("直线: (%.0f,%.0f) -> (%.0f,%.0f)\n\n",
           line_p1.x, line_p1.y, line_p2.x, line_p2.y);

    printf("点到直线距离:\n");
    for (int i = 0; i < 4; i++) {
        double dist = point_to_line_distance(points[i], line_p1, line_p2);
        printf("  (%.0f, %.0f): %.2f\n", points[i].x, points[i].y, dist);
    }
}

// 演示 5: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 5: 应用场景 ===\n");

    printf("1. 计算机图形学\n");
    printf("   - 碰撞检测\n");
    printf("   - 光线追踪\n");
    printf("\n");

    printf("2. GIS 地理信息系统\n");
    printf("   - 道路交叉检测\n");
    printf("   - 区域边界计算\n");
    printf("\n");

    printf("3. 游戏开发\n");
    printf("   - 物体碰撞\n");
    printf("   - 视线检测\n");
    printf("\n");

    printf("4. CAD/CAM\n");
    printf("   - 几何建模\n");
    printf("   - 路径规划\n");
}

int main(void) {
    printf("========================================\n");
    printf("    线段交点计算演示\n");
    printf("========================================\n");

    demo_basic();
    demo_positions();
    demo_point_on_segment();
    demo_distance();
    demo_applications();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
