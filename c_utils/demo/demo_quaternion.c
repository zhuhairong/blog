/**
 * 四元数演示程序
 */

#include <stdio.h>
#include <math.h>
#include "../c_utils/quaternion.h"

static void demo_operations(void) {
    printf("\n=== 演示 1: 基本运算 ===\n");

    quat_t a = {1, 2, 3, 4};
    quat_t b = {2, 3, 4, 5};

    printf("a = (%.2f, %.2f, %.2f, %.2f)\n", a.w, a.x, a.y, a.z);
    printf("b = (%.2f, %.2f, %.2f, %.2f)\n", b.w, b.x, b.y, b.z);

    quat_t sum = quat_add(a, b);
    printf("a + b = (%.2f, %.2f, %.2f, %.2f)\n", sum.w, sum.x, sum.y, sum.z);

    quat_t prod = quat_mul(a, b);
    printf("a * b = (%.2f, %.2f, %.2f, %.2f)\n", prod.w, prod.x, prod.y, prod.z);
}

static void demo_normalize(void) {
    printf("\n=== 演示 2: 归一化 ===\n");

    quat_t q = {1, 2, 3, 4};
    printf("原始四元数: (%.2f, %.2f, %.2f, %.2f)\n", q.w, q.x, q.y, q.z);

    double len_sq = q.w*q.w + q.x*q.x + q.y*q.y + q.z*q.z;
    printf("长度平方: %.2f\n", len_sq);
    printf("长度: %.2f\n", sqrt(len_sq));

    quat_t normalized = quat_normalize(q);
    printf("归一化后: (%.2f, %.2f, %.2f, %.2f)\n", normalized.w, normalized.x, normalized.y, normalized.z);

    double norm_sq = normalized.w*normalized.w + normalized.x*normalized.x + 
                     normalized.y*normalized.y + normalized.z*normalized.z;
    printf("归一化后长度平方: %.2f\n", norm_sq);
}

int main(void) {
    printf("========================================\n");
    printf("    四元数演示\n");
    printf("========================================\n");

    demo_operations();
    demo_normalize();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
