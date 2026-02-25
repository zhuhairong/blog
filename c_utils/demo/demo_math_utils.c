/**
 * 数学工具演示程序
 *
 * 功能：
 * - 数值钳位和插值
 * - 随机数生成
 * - 数学运算辅助
 */

#include <stdio.h>
#include <stdlib.h>
#include "../c_utils/math_utils.h"

// 演示 1: 数值钳位
static void demo_clamp(void) {
    printf("\n=== 演示 1: 数值钳位 (Clamp) ===\n");

    printf("将数值限制在 [0, 100] 范围内:\n\n");

    double values[] = {-10, 0, 50, 100, 150};
    int count = sizeof(values) / sizeof(values[0]);

    for (int i = 0; i < count; i++) {
        double result = math_clamp(values[i], 0, 100);
        printf("  clamp(%6.1f, 0, 100) = %6.1f\n", values[i], result);
    }

    printf("\n应用场景:\n");
    printf("  - 限制颜色值在 0-255\n");
    printf("  - 限制音量在 0-100\n");
    printf("  - 限制进度条在 0-100%%\n");
}

// 演示 2: 线性插值
static void demo_lerp(void) {
    printf("\n=== 演示 2: 线性插值 (Lerp) ===\n");

    printf("在 0 和 100 之间插值:\n\n");

    printf("  t    结果\n");
    printf("  ---  -----\n");

    for (double t = 0; t <= 1.0; t += 0.25) {
        double result = math_lerp(0, 100, t);
        printf("  %.2f  %.1f\n", t, result);
    }

    printf("\n应用场景:\n");
    printf("  - 动画过渡\n");
    printf("  - 颜色渐变\n");
    printf("  - 平滑移动\n");
}

// 演示 3: 随机数生成
static void demo_random(void) {
    printf("\n=== 演示 3: 随机数生成 ===\n");

    printf("随机整数 [1, 100]:\n  ");
    for (int i = 0; i < 10; i++) {
        printf("%d ", math_rand_int(1, 100));
    }
    printf("\n");

    printf("\n随机浮点数 [0, 1]:\n  ");
    for (int i = 0; i < 10; i++) {
        printf("%.3f ", math_rand_double());
    }
    printf("\n");

    printf("\n应用场景:\n");
    printf("  - 游戏随机事件\n");
    printf("  - 测试数据生成\n");
    printf("  - 蒙特卡洛模拟\n");
}

// 演示 4: 2 的幂检查
static void demo_pow2(void) {
    printf("\n=== 演示 4: 2 的幂检查 ===\n");

    printf("检查是否为 2 的幂:\n\n");

    size_t values[] = {0, 1, 2, 3, 4, 7, 8, 15, 16, 31, 32, 64, 100, 128, 256};
    int count = sizeof(values) / sizeof(values[0]);

    for (int i = 0; i < count; i++) {
        bool is_pow2 = math_is_pow2(values[i]);
        printf("  %3zu: %s\n", values[i], is_pow2 ? "是" : "否");
    }

    printf("\n应用场景:\n");
    printf("  - 内存对齐检查\n");
    printf("  - 缓冲区大小验证\n");
    printf("  - 位运算优化\n");
}

// 演示 5: 范围映射
static void demo_map_range(void) {
    printf("\n=== 演示 5: 范围映射 ===\n");

    math_utils_config_t config = {0};
    math_utils_ctx_t *ctx;
    math_utils_create(&ctx, &config);

    printf("将 [0, 100] 映射到 [0, 255]:\n\n");

    double inputs[] = {0, 25, 50, 75, 100};
    int count = sizeof(inputs) / sizeof(inputs[0]);

    printf("  输入    输出\n");
    printf("  ----    ----\n");

    for (int i = 0; i < count; i++) {
        double output;
        math_utils_map_range(ctx, inputs[i], 0, 100, 0, 255, &output);
        printf("  %6.1f  %6.1f\n", inputs[i], output);
    }

    printf("\n应用场景:\n");
    printf("  - 颜色空间转换\n");
    printf("  - 传感器数据映射\n");
    printf("  - UI 控件位置计算\n");

    math_utils_destroy(ctx);
}

// 演示 6: 平滑步进
static void demo_smoothstep(void) {
    printf("\n=== 演示 6: 平滑步进 (Smoothstep) ===\n");

    math_utils_config_t config = {0};
    math_utils_ctx_t *ctx;
    math_utils_create(&ctx, &config);

    printf("smoothstep(0, 1, x):\n\n");

    printf("  x      结果\n");
    printf("  ---    ----\n");

    for (double x = 0; x <= 1.0; x += 0.1) {
        double result;
        math_utils_smoothstep(ctx, 0, 1, x, &result);
        printf("  %.2f   %.4f\n", x, result);
    }

    printf("\n特点:\n");
    printf("  - 在边界处导数为 0 (平滑)\n");
    printf("  - 常用于动画缓动\n");
    printf("  - 比线性插值更自然\n");

    math_utils_destroy(ctx);
}

// 演示 7: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 7: 应用场景 ===\n");

    printf("1. 游戏开发\n");
    printf("   - 角色移动插值\n");
    printf("   - 伤害数值钳位\n");
    printf("   - 随机掉落\n");
    printf("\n");

    printf("2. 图形渲染\n");
    printf("   - 颜色混合\n");
    printf("   - 动画缓动\n");
    printf("   - 坐标变换\n");
    printf("\n");

    printf("3. 数据处理\n");
    printf("   - 数据归一化\n");
    printf("   - 范围映射\n");
    printf("   - 异常值处理\n");
    printf("\n");

    printf("4. 嵌入式系统\n");
    printf("   - 传感器数据处理\n");
    printf("   - 内存对齐检查\n");
    printf("   - 定时器计算\n");
}

int main(void) {
    printf("========================================\n");
    printf("    数学工具演示\n");
    printf("========================================\n");

    demo_clamp();
    demo_lerp();
    demo_random();
    demo_pow2();
    demo_map_range();
    demo_smoothstep();
    demo_applications();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
