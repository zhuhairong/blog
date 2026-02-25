/**
 * 矩阵运算演示程序
 *
 * 功能：
 * - 矩阵创建和基本操作
 * - 矩阵运算
 * - 特殊矩阵
 */

#include <stdio.h>
#include <stdlib.h>
#include "../c_utils/matrix.h"

// 打印矩阵
static void print_matrix(const matrix_t *m, const char *name) {
    printf("%s (%zux%zu):\n", name, m->rows, m->cols);
    for (size_t i = 0; i < m->rows; i++) {
        printf("  ");
        for (size_t j = 0; j < m->cols; j++) {
            printf("%8.2f ", matrix_get(m, i, j));
        }
        printf("\n");
    }
    printf("\n");
}

// 演示 1: 基本操作
static void demo_basic(void) {
    printf("\n=== 演示 1: 基本操作 ===\n");

    printf("创建 3x3 矩阵...\n");
    matrix_t *m = matrix_create(3, 3);

    printf("\n设置元素:\n");
    matrix_set(m, 0, 0, 1); matrix_set(m, 0, 1, 2); matrix_set(m, 0, 2, 3);
    matrix_set(m, 1, 0, 4); matrix_set(m, 1, 1, 5); matrix_set(m, 1, 2, 6);
    matrix_set(m, 2, 0, 7); matrix_set(m, 2, 1, 8); matrix_set(m, 2, 2, 9);

    print_matrix(m, "矩阵 A");

    printf("获取元素 A[1][1]: %.1f\n", matrix_get(m, 1, 1));

    matrix_free(m);
}

// 演示 2: 矩阵填充
static void demo_fill(void) {
    printf("\n=== 演示 2: 矩阵填充 ===\n");

    matrix_t *m = matrix_create(3, 3);

    printf("填充为 5:\n");
    matrix_fill(m, 5);
    print_matrix(m, "填充后");

    printf("清零:\n");
    matrix_zero(m);
    print_matrix(m, "零矩阵");

    matrix_free(m);
}

// 演示 3: 矩阵加法
static void demo_add(void) {
    printf("\n=== 演示 3: 矩阵加法 ===\n");

    matrix_t *a = matrix_create(2, 2);
    matrix_t *b = matrix_create(2, 2);

    matrix_set(a, 0, 0, 1); matrix_set(a, 0, 1, 2);
    matrix_set(a, 1, 0, 3); matrix_set(a, 1, 1, 4);

    matrix_set(b, 0, 0, 5); matrix_set(b, 0, 1, 6);
    matrix_set(b, 1, 0, 7); matrix_set(b, 1, 1, 8);

    print_matrix(a, "矩阵 A");
    print_matrix(b, "矩阵 B");

    matrix_t *c = matrix_add(a, b);
    if (c) {
        print_matrix(c, "A + B");
        matrix_free(c);
    }

    matrix_free(a);
    matrix_free(b);
}

// 演示 4: 矩阵乘法
static void demo_multiply(void) {
    printf("\n=== 演示 4: 矩阵乘法 ===\n");

    matrix_t *a = matrix_create(2, 3);
    matrix_t *b = matrix_create(3, 2);

    matrix_set(a, 0, 0, 1); matrix_set(a, 0, 1, 2); matrix_set(a, 0, 2, 3);
    matrix_set(a, 1, 0, 4); matrix_set(a, 1, 1, 5); matrix_set(a, 1, 2, 6);

    matrix_set(b, 0, 0, 7);  matrix_set(b, 0, 1, 8);
    matrix_set(b, 1, 0, 9);  matrix_set(b, 1, 1, 10);
    matrix_set(b, 2, 0, 11); matrix_set(b, 2, 1, 12);

    print_matrix(a, "矩阵 A (2x3)");
    print_matrix(b, "矩阵 B (3x2)");

    matrix_t *c = matrix_mul(a, b);
    if (c) {
        print_matrix(c, "A × B (2x2)");
        matrix_free(c);
    }

    matrix_free(a);
    matrix_free(b);
}

// 演示 5: 矩阵转置
static void demo_transpose(void) {
    printf("\n=== 演示 5: 矩阵转置 ===\n");

    matrix_t *m = matrix_create(2, 3);

    matrix_set(m, 0, 0, 1); matrix_set(m, 0, 1, 2); matrix_set(m, 0, 2, 3);
    matrix_set(m, 1, 0, 4); matrix_set(m, 1, 1, 5); matrix_set(m, 1, 2, 6);

    print_matrix(m, "原矩阵");

    matrix_t *t = matrix_transpose(m);
    if (t) {
        print_matrix(t, "转置矩阵");
        matrix_free(t);
    }

    matrix_free(m);
}

// 演示 6: 标量乘法
static void demo_scalar(void) {
    printf("\n=== 演示 6: 标量乘法 ===\n");

    matrix_t *m = matrix_create(2, 2);

    matrix_set(m, 0, 0, 1); matrix_set(m, 0, 1, 2);
    matrix_set(m, 1, 0, 3); matrix_set(m, 1, 1, 4);

    print_matrix(m, "矩阵 A");

    matrix_t *result = matrix_scalar_mul(m, 2.5);
    if (result) {
        print_matrix(result, "A × 2.5");
        matrix_free(result);
    }

    matrix_free(m);
}

// 演示 7: 单位矩阵
static void demo_identity(void) {
    printf("\n=== 演示 7: 单位矩阵 ===\n");

    printf("3x3 单位矩阵:\n");
    matrix_t *i = matrix_identity(3);
    if (i) {
        print_matrix(i, "I");
        matrix_free(i);
    }
}

// 演示 8: 矩阵比较
static void demo_compare(void) {
    printf("\n=== 演示 8: 矩阵比较 ===\n");

    matrix_t *a = matrix_create(2, 2);
    matrix_t *b = matrix_create(2, 2);

    matrix_set(a, 0, 0, 1); matrix_set(a, 0, 1, 2);
    matrix_set(a, 1, 0, 3); matrix_set(a, 1, 1, 4);

    matrix_set(b, 0, 0, 1); matrix_set(b, 0, 1, 2);
    matrix_set(b, 1, 0, 3); matrix_set(b, 1, 1, 4.0001);

    print_matrix(a, "矩阵 A");
    print_matrix(b, "矩阵 B");

    bool equal = matrix_equal(a, b, 0.001);
    printf("A == B (精度 0.001): %s\n", equal ? "是" : "否");

    equal = matrix_equal(a, b, 0.00001);
    printf("A == B (精度 0.00001): %s\n", equal ? "是" : "否");

    matrix_free(a);
    matrix_free(b);
}

// 演示 9: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 9: 应用场景 ===\n");

    printf("1. 计算机图形学\n");
    printf("   - 3D 变换 (旋转、平移、缩放)\n");
    printf("   - 投影矩阵\n");
    printf("   - 视图变换\n");
    printf("\n");

    printf("2. 机器学习\n");
    printf("   - 神经网络权重\n");
    printf("   - 数据预处理\n");
    printf("   - 特征变换\n");
    printf("\n");

    printf("3. 物理模拟\n");
    printf("   - 刚体变换\n");
    printf("   - 坐标系转换\n");
    printf("   - 力学计算\n");
    printf("\n");

    printf("4. 数据分析\n");
    printf("   - 线性回归\n");
    printf("   - 主成分分析\n");
    printf("   - 协方差矩阵\n");
}

int main(void) {
    printf("========================================\n");
    printf("    矩阵运算演示\n");
    printf("========================================\n");

    demo_basic();
    demo_fill();
    demo_add();
    demo_multiply();
    demo_transpose();
    demo_scalar();
    demo_identity();
    demo_compare();
    demo_applications();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
