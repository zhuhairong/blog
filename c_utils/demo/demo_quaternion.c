/**
 * 四元数演示程序
 *
 * 功能：
 * - 四元数基本运算
 * - 旋转表示
 * - 插值计算
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../c_utils/quaternion.h"

// 打印四元数
static void print_quat(const char *name, quat_t q) {
    printf("%s = [%.3f, %.3f, %.3f, %.3f]\n", name, q.w, q.x, q.y, q.z);
}

// 演示 1: 基本概念
static void demo_concept(void) {
    printf("\n=== 演示 1: 四元数基本概念 ===\n");

    printf("四元数:\n\n");

    printf("定义:\n");
    printf("  q = w + xi + yj + zk\n");
    printf("  其中: w 是实部, (x, y, z) 是虚部\n\n");

    printf("优点:\n");
    printf("  - 避免万向节锁\n");
    printf("  - 平滑插值 (SLERP)\n");
    printf("  - 内存效率高\n");
    printf("  - 数值稳定\n\n");

    printf("应用:\n");
    printf("  - 3D 旋转\n");
    printf("  - 姿态表示\n");
    printf("  - 动画插值\n");
}

// 演示 2: 创建四元数
static void demo_creation(void) {
    printf("\n=== 演示 2: 创建四元数 ===\n");

    printf("单位四元数:\n");
    quat_t q1 = {1.0, 0.0, 0.0, 0.0};
    print_quat("  q1 (单位)", q1);

    printf("\n自定义四元数:\n");
    quat_t q2 = {0.707, 0.707, 0.0, 0.0};
    print_quat("  q2", q2);

    printf("\n零四元数:\n");
    quat_t q3 = {0.0, 0.0, 0.0, 0.0};
    print_quat("  q3 (零)", q3);
}

// 演示 3: 基本运算
static void demo_operations(void) {
    printf("\n=== 演示 3: 基本运算 ===\n");

    quat_t a = {1.0, 2.0, 3.0, 4.0};
    quat_t b = {0.5, 1.0, 1.5, 2.0};

    print_quat("a", a);
    print_quat("b", b);

    printf("\n加法 (a + b):\n");
    printf("  [%.3f, %.3f, %.3f, %.3f]\n",
           a.w + b.w, a.x + b.x, a.y + b.y, a.z + b.z);

    printf("\n减法 (a - b):\n");
    printf("  [%.3f, %.3f, %.3f, %.3f]\n",
           a.w - b.w, a.x - b.x, a.y - b.y, a.z - b.z);

    printf("\n乘法 (概念):\n");
    printf("  四元数乘法遵循哈密顿规则\n");
    printf("  i*j=k, j*i=-k, 等等\n");

    printf("\n共轭:\n");
    printf("  [%.3f, %.3f, %.3f, %.3f]\n",
           a.w, -a.x, -a.y, -a.z);
}

// 演示 4: 旋转表示
static void demo_rotation(void) {
    printf("\n=== 演示 4: 旋转表示 ===\n");

    printf("轴角表示:\n");
    printf("  绕 Y 轴旋转 90 度\n");

    vec3_t axis = {0.0, 1.0, 0.0};
    double angle = M_PI / 2;  // 90度

    printf("  旋转轴: [%.1f, %.1f, %.1f]\n", axis.x, axis.y, axis.z);
    printf("  旋转角: %.3f 弧度 (90°)\n", angle);

    printf("\n  四元数表示:\n");
    double half_angle = angle / 2;
    double s = sin(half_angle);
    double c = cos(half_angle);
    printf("  [%.3f, %.3f, %.3f, %.3f]\n", c, axis.x * s, axis.y * s, axis.z * s);

    printf("\n欧拉角表示:\n");
    euler_t euler = {0.0, M_PI/4, 0.0};  // 俯仰 45度
    printf("  Roll: %.1f°, Pitch: %.1f°, Yaw: %.1f°\n",
           euler.roll * 180 / M_PI,
           euler.pitch * 180 / M_PI,
           euler.yaw * 180 / M_PI);
}

// 演示 5: 配置选项
static void demo_config(void) {
    printf("\n=== 演示 5: 配置选项 ===\n");

    printf("quat_config_t 结构:\n");
    printf("  normalize_result: 自动归一化\n");
    printf("  check_validity: 检查 NaN/Inf\n");
    printf("  epsilon: 浮点比较精度\n\n");

    printf("配置说明:\n");
    printf("  - normalize_result: 自动归一化\n");
    printf("  - check_validity: 检查 NaN/Inf\n");
    printf("  - epsilon: 浮点比较精度\n");
}

// 演示 6: 与矩阵转换
static void demo_matrix(void) {
    printf("\n=== 演示 6: 与旋转矩阵转换 ===\n");

    printf("四元数转旋转矩阵:\n");
    quat_t q = {0.707, 0.0, 0.707, 0.0};
    print_quat("  四元数", q);

    printf("  旋转矩阵:\n");
    printf("    [ 0  0  1  0 ]\n");
    printf("    [ 0  1  0  0 ]\n");
    printf("    [-1  0  0  0 ]\n");
    printf("    [ 0  0  0  1 ]\n");

    printf("\n旋转矩阵转四元数:\n");
    printf("  从 4x4 矩阵提取四元数\n");
    printf("  处理数值稳定性问题\n");
}

// 演示 7: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 7: 应用场景 ===\n");

    printf("1. 计算机图形学\n");
    printf("   - 3D 模型旋转\n");
    printf("   - 相机控制\n");
    printf("   - 骨骼动画\n\n");

    printf("2. 游戏开发\n");
    printf("   - 角色朝向\n");
    printf("   - 物理模拟\n");
    printf("   - 平滑旋转\n\n");

    printf("3. 航空航天\n");
    printf("   - 飞行器姿态\n");
    printf("   - 导航系统\n");
    printf("   - 传感器融合\n\n");

    printf("4. 机器人学\n");
    printf("   - 机械臂控制\n");
    printf("   - 路径规划\n");
    printf("   - 姿态估计\n\n");

    printf("5. 虚拟现实\n");
    printf("   - 头显追踪\n");
    printf("   - 手柄定位\n");
    printf("   - 空间映射\n");
}

int main(void) {
    printf("========================================\n");
    printf("    四元数演示\n");
    printf("========================================\n");

    demo_concept();
    demo_creation();
    demo_operations();
    demo_rotation();
    demo_config();
    demo_matrix();
    demo_applications();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
