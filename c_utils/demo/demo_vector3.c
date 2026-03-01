/**
 * @file demo_vector3.c
 * @brief 3D向量操作演示
 *
 * 本演示展示了3D向量的基本操作，包括：
 * - 向量创建和初始化
 * - 向量加减乘除
 * - 向量点积和叉积
 * - 向量长度和归一化
 * - 向量距离计算
 * - 向量角度计算
 * - 向量插值
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../c_utils/vector3.h"

// 打印向量
static void print_vector(const char *label, vec3_t v) {
    printf("%s: (%.2f, %.2f, %.2f)\n", label, v.x, v.y, v.z);
}

// 演示 1: 向量创建和初始化
static void demo_create(void) {
    printf("\n=== 演示 1: 向量创建和初始化 ===\n");

    // 创建零向量
    vec3_t v1 = vec3_zero();
    print_vector("零向量", v1);

    // 从分量创建
    vec3_t v2 = vec3_set(1.0, 2.0, 3.0);
    print_vector("自定义向量", v2);

    // 单位向量
    vec3_t v3 = vec3_unit_x();
    print_vector("X轴单位向量", v3);

    vec3_t v4 = vec3_unit_y();
    print_vector("Y轴单位向量", v4);

    vec3_t v5 = vec3_unit_z();
    print_vector("Z轴单位向量", v5);
}

// 演示 2: 向量加减乘除
static void demo_arithmetic(void) {
    printf("\n=== 演示 2: 向量加减乘除 ===\n");

    vec3_t v1 = vec3_set(1.0, 2.0, 3.0);
    vec3_t v2 = vec3_set(4.0, 5.0, 6.0);

    print_vector("向量 A", v1);
    print_vector("向量 B", v2);

    // 向量加法
    vec3_t v_add = vec3_add(v1, v2);
    print_vector("A + B", v_add);

    // 向量减法
    vec3_t v_sub = vec3_sub(v1, v2);
    print_vector("A - B", v_sub);

    // 向量数乘
    vec3_t v_scale = vec3_scale(v1, 2.0);
    print_vector("A * 2", v_scale);

    // 向量数除
    vec3_ctx_t *ctx = NULL;
    vec3_create(&ctx, NULL);
    vec3_t v_div;
    vec3_divide(ctx, v1, 2.0, &v_div);
    print_vector("A / 2", v_div);
    vec3_destroy(ctx);
}

// 演示 3: 向量点积和叉积
static void demo_products(void) {
    printf("\n=== 演示 3: 向量点积和叉积 ===\n");

    vec3_t v1 = vec3_set(1.0, 0.0, 0.0);
    vec3_t v2 = vec3_set(0.0, 1.0, 0.0);
    vec3_t v3 = vec3_set(1.0, 1.0, 1.0);

    print_vector("向量 A", v1);
    print_vector("向量 B", v2);
    print_vector("向量 C", v3);

    // 点积
    double dot_ab = vec3_dot(v1, v2);
    double dot_ac = vec3_dot(v1, v3);
    printf("A · B = %.2f\n", dot_ab);
    printf("A · C = %.2f\n", dot_ac);

    // 叉积
    vec3_t cross_ab = vec3_cross(v1, v2);
    vec3_t cross_ba = vec3_cross(v2, v1);
    print_vector("A × B", cross_ab);
    print_vector("B × A", cross_ba);
}

// 演示 4: 向量长度和归一化
static void demo_length(void) {
    printf("\n=== 演示 4: 向量长度和归一化 ===\n");

    vec3_t v1 = vec3_set(3.0, 4.0, 0.0);
    print_vector("向量", v1);

    // 计算长度
    double len = vec3_length(v1);
    printf("长度: %.2f\n", len);

    // 归一化
    vec3_t v_normalized = vec3_normalize(v1);
    print_vector("归一化向量", v_normalized);
    printf("归一化后长度: %.2f\n", vec3_length(v_normalized));
}

// 演示 5: 向量距离
static void demo_distance(void) {
    printf("\n=== 演示 5: 向量距离 ===\n");

    vec3_t v1 = vec3_set(1.0, 2.0, 3.0);
    vec3_t v2 = vec3_set(4.0, 5.0, 6.0);

    print_vector("点 A", v1);
    print_vector("点 B", v2);

    // 计算距离
    double dist = vec3_distance(v1, v2);
    printf("A 到 B 的距离: %.2f\n", dist);

    // 计算距离平方
    double dist_sq = vec3_distance_squared(v1, v2);
    printf("距离平方: %.2f\n", dist_sq);
}

// 演示 6: 向量角度
static void demo_angle(void) {
    printf("\n=== 演示 6: 向量角度 ===\n");

    vec3_t v1 = vec3_set(1.0, 0.0, 0.0);
    vec3_t v2 = vec3_set(0.0, 1.0, 0.0);
    vec3_t v3 = vec3_set(1.0, 1.0, 0.0);

    print_vector("向量 A", v1);
    print_vector("向量 B", v2);
    print_vector("向量 C", v3);

    // 计算角度
    double angle_ab = vec3_angle(v1, v2);
    double angle_ac = vec3_angle(v1, v3);

    printf("A 和 B 的角度: %.2f 弧度 (%.2f 度)\n", angle_ab, angle_ab * 180 / M_PI);
    printf("A 和 C 的角度: %.2f 弧度 (%.2f 度)\n", angle_ac, angle_ac * 180 / M_PI);
}

// 演示 7: 向量插值
static void demo_lerp(void) {
    printf("\n=== 演示 7: 向量插值 ===\n");

    vec3_t v1 = vec3_set(0.0, 0.0, 0.0);
    vec3_t v2 = vec3_set(1.0, 1.0, 1.0);

    print_vector("起点", v1);
    print_vector("终点", v2);

    // 线性插值
    vec3_t v_lerp_0 = vec3_lerp(v1, v2, 0.0);
    print_vector("t=0.0", v_lerp_0);

    vec3_t v_lerp_05 = vec3_lerp(v1, v2, 0.5);
    print_vector("t=0.5", v_lerp_05);

    vec3_t v_lerp_1 = vec3_lerp(v1, v2, 1.0);
    print_vector("t=1.0", v_lerp_1);

    // 球面线性插值
    vec3_t v_slerp_05 = vec3_slerp(v1, v2, 0.5);
    print_vector("球面插值 t=0.5", v_slerp_05);
}

// 演示 8: 向量应用场景
static void demo_applications(void) {
    printf("\n=== 演示 8: 向量应用场景 ===\n");

    printf("1. 3D 游戏和图形:\n");
    printf("   - 位置和方向表示\n");
    printf("   - 物理引擎中的力和速度\n");
    printf("   - 碰撞检测\n");
    printf("   - 相机控制\n");

    printf("\n2. 机器人学:\n");
    printf("   - 关节位置和姿态\n");
    printf("   - 运动规划\n");
    printf("   - 传感器数据处理\n");

    printf("\n3. 计算机视觉:\n");
    printf("   - 3D 点云处理\n");
    printf("   - 相机标定\n");
    printf("   - 姿态估计\n");

    printf("\n4. 物理模拟:\n");
    printf("   - 粒子系统\n");
    printf("   - 流体模拟\n");
    printf("   - 刚体动力学\n");
}

int main(void) {
    printf("========================================\n");
    printf("    3D 向量操作演示程序\n");
    printf("========================================\n");

    demo_create();
    demo_arithmetic();
    demo_products();
    demo_length();
    demo_distance();
    demo_angle();
    demo_lerp();
    demo_applications();

    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("========================================\n");

    return 0;
}