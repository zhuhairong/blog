/**
 * 线段交点计算演示程序
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/line_intersection.h"

static void demo_basic_intersection(void) {
    printf("\n=== 演示 1: 基本线段相交 ===\n");

    point_t p1 = {0.0, 0.0};
    point_t p2 = {10.0, 10.0};
    point_t p3 = {0.0, 10.0};
    point_t p4 = {10.0, 0.0};

    printf("线段 1: (%.1f, %.1f) -> (%.1f, %.1f)\n", p1.x, p1.y, p2.x, p2.y);
    printf("线段 2: (%.1f, %.1f) -> (%.1f, %.1f)\n", p3.x, p3.y, p4.x, p4.y);

    point_t res;
    bool intersect = line_intersection(p1, p2, p3, p4, &res);

    if (intersect) {
        printf("\n相交于: (%.2f, %.2f)\n", res.x, res.y);
    } else {
        printf("\n不相交\n");
    }
}

static void demo_parallel_lines(void) {
    printf("\n=== 演示 2: 平行线段 ===\n");

    point_t p1 = {0.0, 0.0};
    point_t p2 = {5.0, 0.0};
    point_t p3 = {0.0, 3.0};
    point_t p4 = {5.0, 3.0};

    printf("线段 1: (%.1f, %.1f) -> (%.1f, %.1f)\n", p1.x, p1.y, p2.x, p2.y);
    printf("线段 2: (%.1f, %.1f) -> (%.1f, %.1f)\n", p3.x, p3.y, p4.x, p4.y);

    point_t res;
    bool intersect = line_intersection(p1, p2, p3, p4, &res);

    printf("\n结果: %s\n", intersect ? "相交" : "不相交 (平行)");
}

static void demo_no_intersection(void) {
    printf("\n=== 演示 3: 不相交线段 ===\n");

    point_t p1 = {0.0, 0.0};
    point_t p2 = {2.0, 2.0};
    point_t p3 = {5.0, 5.0};
    point_t p4 = {7.0, 7.0};

    printf("线段 1: (%.1f, %.1f) -> (%.1f, %.1f)\n", p1.x, p1.y, p2.x, p2.y);
    printf("线段 2: (%.1f, %.1f) -> (%.1f, %.1f)\n", p3.x, p3.y, p4.x, p4.y);

    point_t res;
    bool intersect = line_intersection(p1, p2, p3, p4, &res);

    printf("\n结果: %s\n", intersect ? "相交" : "不相交");
}

static void demo_multiple_cases(void) {
    printf("\n=== 演示 4: 多种情况测试 ===\n");

    point_t p1 = {0.0, 0.0}, p2 = {5.0, 0.0}, p3 = {2.0, -2.0}, p4 = {2.0, 2.0};
    point_t res;
    bool intersect = line_intersection(p1, p2, p3, p4, &res);
    printf("\n1. 十字相交: %s", intersect ? "相交" : "不相交");
    if (intersect) printf(" 于 (%.2f, %.2f)", res.x, res.y);
    printf("\n");

    p1.x = 0.0; p1.y = 0.0; p2.x = 3.0; p2.y = 3.0;
    p3.x = 1.0; p3.y = 1.0; p4.x = 4.0; p4.y = 4.0;
    intersect = line_intersection(p1, p2, p3, p4, &res);
    printf("2. 共线重叠: %s\n", intersect ? "相交" : "不相交");

    p1.x = 0.0; p1.y = 0.0; p2.x = 2.0; p2.y = 2.0;
    p3.x = 3.0; p3.y = 3.0; p4.x = 5.0; p4.y = 5.0;
    intersect = line_intersection(p1, p2, p3, p4, &res);
    printf("3. 共线不重叠: %s\n", intersect ? "相交" : "不相交");

    p1.x = 0.0; p1.y = 0.0; p2.x = 4.0; p2.y = 0.0;
    p3.x = 2.0; p3.y = 0.0; p4.x = 6.0; p4.y = 0.0;
    intersect = line_intersection(p1, p2, p3, p4, &res);
    printf("4. 水平共线: %s\n", intersect ? "相交" : "不相交");
}

int main(void) {
    printf("========================================\n");
    printf("    线段交点演示\n");
    printf("========================================\n");

    demo_basic_intersection();
    demo_parallel_lines();
    demo_no_intersection();
    demo_multiple_cases();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
