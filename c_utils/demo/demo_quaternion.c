/**
 * 四元数演示程序
 *
 * 功能：
 * - 基本运算（加减乘、标量乘除）
 * - 共轭、逆、归一化
 * - 球面线性插值 (SLERP) 和线性插值 (LERP)
 * - 欧拉角转换
 * - 轴角表示
 * - 向量旋转
 * - 点积和距离
 */

#include <stdio.h>
#include <math.h>
#include "../c_utils/quaternion.h"

// 辅助函数：比较两个双精度浮点数
static bool double_eq(double a, double b, double eps) {
    return fabs(a - b) < eps;
}

// 演示 1: 基本运算（加法、减法、乘法、标量乘除）
static void demo_operations(void) {
    printf("\n=== 演示 1: 基本运算 ===\n");

    quat_t a = {1, 2, 3, 4};
    quat_t b = {2, 3, 4, 5};

    printf("a = (%.2f, %.2f, %.2f, %.2f)\n", a.w, a.x, a.y, a.z);
    printf("b = (%.2f, %.2f, %.2f, %.2f)\n", b.w, b.x, b.y, b.z);

    quat_t sum = quat_add(a, b);
    printf("a + b = (%.2f, %.2f, %.2f, %.2f)\n", sum.w, sum.x, sum.y, sum.z);

    quat_t diff = quat_sub(a, b);
    printf("a - b = (%.2f, %.2f, %.2f, %.2f)\n", diff.w, diff.x, diff.y, diff.z);

    quat_t prod = quat_mul(a, b);
    printf("a * b = (%.2f, %.2f, %.2f, %.2f)\n", prod.w, prod.x, prod.y, prod.z);

    quat_t scaled = quat_mul_scalar(a, 2.0);
    printf("a * 2 = (%.2f, %.2f, %.2f, %.2f)\n", scaled.w, scaled.x, scaled.y, scaled.z);

    quat_error_t err;
    quat_t divided = quat_div_scalar(a, 2.0, &err);
    printf("a / 2 = (%.2f, %.2f, %.2f, %.2f)\n", divided.w, divided.x, divided.y, divided.z);
}

// 演示 2: 共轭、逆、长度
static void demo_conjugate_inverse(void) {
    printf("\n=== 演示 2: 共轭与逆 ===\n");

    quat_t q = {1, 2, 3, 4};
    printf("q = (%.2f, %.2f, %.2f, %.2f)\n", q.w, q.x, q.y, q.z);

    quat_t conj = quat_conjugate(q);
    printf("共轭 q* = (%.2f, %.2f, %.2f, %.2f)\n", conj.w, conj.x, conj.y, conj.z);

    double len = quat_length(q);
    printf("长度 |q| = %.4f\n", len);

    double len_sq = quat_length_squared(q);
    printf("长度平方 |q|^2 = %.2f\n", len_sq);

    quat_error_t err;
    quat_t inv = quat_inverse(q, &err);
    if (err == QUAT_OK) {
        printf("逆 q^-1 = (%.4f, %.4f, %.4f, %.4f)\n", inv.w, inv.x, inv.y, inv.z);

        // 验证: q * q^-1 ≈ (1, 0, 0, 0)
        quat_t identity = quat_mul(q, inv);
        printf("验证 q * q^-1 = (%.4f, %.4f, %.4f, %.4f) [应为 (1,0,0,0)]\n",
               identity.w, identity.x, identity.y, identity.z);
    }
}

// 演示 3: 归一化与单位四元数检查
static void demo_normalize(void) {
    printf("\n=== 演示 3: 归一化 ===\n");

    quat_t q = {1, 2, 3, 4};
    printf("原始四元数: (%.2f, %.2f, %.2f, %.2f)\n", q.w, q.x, q.y, q.z);

    quat_t normalized = quat_normalize(q);
    printf("归一化后: (%.4f, %.4f, %.4f, %.4f)\n",
           normalized.w, normalized.x, normalized.y, normalized.z);

    double norm = quat_length(normalized);
    printf("归一化后长度: %.6f\n", norm);

    bool is_unit = quat_is_unit(normalized, 1e-6);
    printf("是否为单位四元数: %s\n", is_unit ? "是 ✓" : "否 ✗");
}

// 演示 4: SLERP 球面线性插值
static void demo_slerp(void) {
    printf("\n=== 演示 4: SLERP 球面线性插值 ===\n");

    // 绕 Y 轴旋转 0 度
    quat_t from = quat_from_axis_angle((vec3_t){0, 1, 0}, 0);
    // 绕 Y 轴旋转 90 度
    quat_t to = quat_from_axis_angle((vec3_t){0, 1, 0}, M_PI / 2);

    printf("起始: 绕 Y 轴 0°\n");
    printf("  四元数: (%.4f, %.4f, %.4f, %.4f)\n", from.w, from.x, from.y, from.z);
    printf("目标: 绕 Y 轴 90°\n");
    printf("  四元数: (%.4f, %.4f, %.4f, %.4f)\n", to.w, to.x, to.y, to.z);

    printf("\nSLERP 插值:\n");
    double steps[] = {0.0, 0.25, 0.5, 0.75, 1.0};
    for (int i = 0; i < 5; i++) {
        quat_error_t err;
        quat_t result = quat_slerp(from, to, steps[i], &err);
        if (err == QUAT_OK) {
            euler_t euler;
            quat_to_euler(result, &euler);
            printf("  t=%.2f: (%.4f, %.4f, %.4f, %.4f) → yaw=%.1f°\n",
                   steps[i], result.w, result.x, result.y, result.z,
                   euler.yaw * 180.0 / M_PI);
        }
    }
}

// 演示 5: LERP 线性插值
static void demo_lerp(void) {
    printf("\n=== 演示 5: LERP 线性插值 ===\n");

    quat_t a = {1, 0, 0, 0};  // 单位四元数
    quat_t b = {0, 1, 0, 0};

    printf("a = (%.2f, %.2f, %.2f, %.2f)\n", a.w, a.x, a.y, a.z);
    printf("b = (%.2f, %.2f, %.2f, %.2f)\n", b.w, b.x, b.y, b.z);

    printf("LERP 插值:\n");
    for (int i = 0; i <= 4; i++) {
        double t = i / 4.0;
        quat_t result = quat_lerp(a, b, t);
        printf("  t=%.2f: (%.4f, %.4f, %.4f, %.4f)\n",
               t, result.w, result.x, result.y, result.z);
    }
}

// 演示 6: 欧拉角转换
static void demo_euler(void) {
    printf("\n=== 演示 6: 欧拉角转换 ===\n");

    // 创建代表不同旋转的欧拉角
    euler_t rotations[] = {
        {0, 0, M_PI / 2},           // 偏航 90°
        {M_PI / 4, 0, 0},           // 滚转 45°
        {0, M_PI / 6, 0},           // 俯仰 30°
        {M_PI / 6, M_PI / 4, M_PI / 3},  // 复合旋转
    };
    const char *labels[] = {
        "偏航 90°",
        "滚转 45°",
        "俯仰 30°",
        "复合旋转 (30°,45°,60°)",
    };

    for (int i = 0; i < 4; i++) {
        printf("\n%s:\n", labels[i]);
        printf("  欧拉角 (r,p,y): (%.1f°, %.1f°, %.1f°)\n",
               rotations[i].roll * 180.0 / M_PI,
               rotations[i].pitch * 180.0 / M_PI,
               rotations[i].yaw * 180.0 / M_PI);

        quat_t q = quat_from_euler(rotations[i]);
        printf("  → 四元数: (%.4f, %.4f, %.4f, %.4f)\n", q.w, q.x, q.y, q.z);

        euler_t back;
        if (quat_to_euler(q, &back)) {
            printf("  → 还原欧拉角 (r,p,y): (%.1f°, %.1f°, %.1f°)\n",
                   back.roll * 180.0 / M_PI,
                   back.pitch * 180.0 / M_PI,
                   back.yaw * 180.0 / M_PI);
        }
    }
}

// 演示 7: 轴角表示
static void demo_axis_angle(void) {
    printf("\n=== 演示 7: 轴角表示 ===\n");

    vec3_t axis = {0, 0, 1};  // Z 轴
    double angle = M_PI / 3;  // 60 度

    printf("旋转轴: (%.2f, %.2f, %.2f)\n", axis.x, axis.y, axis.z);
    printf("旋转角: %.1f°\n", angle * 180.0 / M_PI);

    quat_t q = quat_from_axis_angle(axis, angle);
    printf("→ 四元数: (%.4f, %.4f, %.4f, %.4f)\n", q.w, q.x, q.y, q.z);

    // 转回轴角
    vec3_t recovered_axis;
    double recovered_angle;
    quat_error_t err;
    if (quat_to_axis_angle(q, &recovered_axis, &recovered_angle, &err)) {
        printf("→ 还原轴: (%.4f, %.4f, %.4f)\n",
               recovered_axis.x, recovered_axis.y, recovered_axis.z);
        printf("→ 还原角度: %.1f°\n", recovered_angle * 180.0 / M_PI);
    }
}

// 演示 8: 向量旋转
static void demo_rotate_vector(void) {
    printf("\n=== 演示 8: 向量旋转 ===\n");

    // 绕 Z 轴旋转 90 度
    vec3_t axis = {0, 0, 1};
    quat_t rot = quat_from_axis_angle(axis, M_PI / 2);

    vec3_t v = {1, 0, 0};  // X 轴方向
    printf("原始向量: (%.2f, %.2f, %.2f)\n", v.x, v.y, v.z);
    printf("旋转: 绕 Z 轴 90°\n");

    vec3_t rotated = quat_rotate_vector(rot, v);
    printf("旋转后: (%.4f, %.4f, %.4f) [应接近 (0, 1, 0)]\n",
           rotated.x, rotated.y, rotated.z);

    // 使用 rotate_point
    double x_out, y_out, z_out;
    if (quat_rotate_point(rot, 1, 0, 0, &x_out, &y_out, &z_out)) {
        printf("rotate_point: (%.4f, %.4f, %.4f)\n", x_out, y_out, z_out);
    }
}

// 演示 9: 点积与距离
static void demo_dot_distance(void) {
    printf("\n=== 演示 9: 点积与距离 ===\n");

    quat_t a = {1, 0, 0, 0};
    quat_t b = {0, 1, 0, 0};
    quat_t c = {1, 0, 0, 0};  // 与 a 相同

    double dot_ab = quat_dot(a, b);
    printf("a·b = %.2f (正交四元数)\n", dot_ab);

    double dot_ac = quat_dot(a, c);
    printf("a·c = %.2f (相同四元数)\n", dot_ac);

    double dist_ab = quat_distance(a, b);
    printf("dist(a, b) = %.4f\n", dist_ab);

    double dist_ac = quat_distance(a, c);
    printf("dist(a, c) = %.4f\n", dist_ac);
}

// 演示 10: 旋转矩阵转换
static void demo_matrix(void) {
    printf("\n=== 演示 10: 旋转矩阵转换 ===\n");

    // 绕 Z 轴旋转 90 度的四元数
    vec3_t axis = {0, 0, 1};
    quat_t q = quat_from_axis_angle(axis, M_PI / 2);

    printf("绕 Z 轴 90° 的四元数: (%.4f, %.4f, %.4f, %.4f)\n",
           q.w, q.x, q.y, q.z);

    mat4_t mat;
    if (quat_to_matrix(q, &mat)) {
        printf("旋转矩阵:\n");
        for (int i = 0; i < 4; i++) {
            printf("  [");
            for (int j = 0; j < 4; j++) {
                printf("%7.3f", mat.m[i][j]);
                if (j < 3) printf(", ");
            }
            printf("]\n");
        }
    }

    // 从矩阵还原四元数
    quat_t recovered = quat_from_matrix(mat);
    printf("从矩阵还原: (%.4f, %.4f, %.4f, %.4f)\n",
           recovered.w, recovered.x, recovered.y, recovered.z);
}

int main(void) {
    printf("========================================\n");
    printf("    四元数演示\n");
    printf("========================================\n");

    demo_operations();
    demo_conjugate_inverse();
    demo_normalize();
    demo_slerp();
    demo_lerp();
    demo_euler();
    demo_axis_angle();
    demo_rotate_vector();
    demo_dot_distance();
    demo_matrix();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
