/**
 * 标量卡尔曼滤波器演示程序
 *
 * 功能：
 * - 一维信号滤波
 * - 噪声抑制
 * - 状态估计
 */

#include <stdio.h>
#include <math.h>
#include "../c_utils/kalman_scalar.h"

// 演示 1: 基本使用
static void demo_basic(void) {
    printf("\n=== 演示 1: 基本使用 ===\n");

    kalman_scalar_t kf;

    // 初始化: 初始值=0, 初始协方差=1, 过程噪声=0.01, 测量噪声=0.1
    kalman_scalar_error_t err = kalman_scalar_init(&kf, 0.0, 1.0, 0.01, 0.1);
    if (err != KALMAN_SCALAR_OK) {
        printf("初始化失败: %d\n", err);
        return;
    }

    printf("卡尔曼滤波器初始化:\n");
    printf("  初始值: 0.0\n");
    printf("  初始协方差: 1.0\n");
    printf("  过程噪声 Q: 0.01\n");
    printf("  测量噪声 R: 0.1\n\n");

    // 模拟测量值 (真实值为 10，带有噪声)
    double measurements[] = {10.2, 9.8, 10.5, 9.9, 10.1, 10.3, 9.7, 10.0};
    int n = sizeof(measurements) / sizeof(measurements[0]);

    printf("滤波过程:\n");
    printf("  真实值: 10.0\n\n");
    printf("  %-8s %-12s %-12s %-12s\n", "测量", "预测", "更新", "卡尔曼增益");
    printf("  %-8s %-12s %-12s %-12s\n", "--------", "------------", "------------", "------------");

    for (int i = 0; i < n; i++) {
        // 预测
        double predicted = kalman_scalar_predict(&kf);

        // 更新
        double updated = kalman_scalar_update(&kf, measurements[i]);

        printf("  %-8.2f %-12.4f %-12.4f %-12.4f\n",
               measurements[i], predicted, updated, kf.K);
    }

    printf("\n最终估计值: %.4f\n", kf.x);
}

// 演示 2: 噪声抑制效果
static void demo_noise_reduction(void) {
    printf("\n=== 演示 2: 噪声抑制效果 ===\n");

    kalman_scalar_t kf;
    kalman_scalar_init(&kf, 0.0, 1.0, 0.001, 0.5);

    // 模拟恒定信号 + 高斯噪声
    double true_value = 50.0;
    double measurements[] = {52.3, 48.7, 51.2, 49.5, 50.8, 47.9, 53.1, 50.2};
    int n = sizeof(measurements) / sizeof(measurements[0]);

    printf("恒定信号滤波:\n");
    printf("  真实值: %.1f\n\n", true_value);

    double sum_raw = 0, sum_filtered = 0;

    printf("  %-6s %-12s %-12s %-12s\n", "步骤", "测量值", "滤波值", "误差");
    printf("  %-6s %-12s %-12s %-12s\n", "------", "------------", "------------", "------------");

    for (int i = 0; i < n; i++) {
        double filtered = kalman_scalar_update(&kf, measurements[i]);
        double error = fabs(filtered - true_value);

        sum_raw += fabs(measurements[i] - true_value);
        sum_filtered += error;

        printf("  %-6d %-12.2f %-12.4f %-12.4f\n", i + 1, measurements[i], filtered, error);
    }

    printf("\n平均误差:\n");
    printf("  原始测量: %.4f\n", sum_raw / n);
    printf("  滤波后:   %.4f\n", sum_filtered / n);
    printf("  改善:     %.1f%%\n", (1 - sum_filtered / sum_raw) * 100);
}

// 演示 3: 跟踪变化信号
static void demo_tracking(void) {
    printf("\n=== 演示 3: 跟踪变化信号 ===\n");

    kalman_scalar_t kf;
    // 较大的 Q 允许更快跟踪变化
    kalman_scalar_init(&kf, 0.0, 1.0, 0.1, 0.2);

    // 模拟递增信号
    double true_values[] = {10.0, 12.0, 14.0, 16.0, 18.0, 20.0};
    double measurements[] = {10.5, 11.8, 14.2, 15.9, 17.5, 20.3};
    int n = sizeof(true_values) / sizeof(true_values[0]);

    printf("跟踪变化信号:\n\n");
    printf("  %-6s %-12s %-12s %-12s\n", "步骤", "真实值", "测量值", "估计值");
    printf("  %-6s %-12s %-12s %-12s\n", "------", "------------", "------------", "------------");

    for (int i = 0; i < n; i++) {
        double estimated = kalman_scalar_update(&kf, measurements[i]);
        printf("  %-6d %-12.1f %-12.2f %-12.4f\n",
               i + 1, true_values[i], measurements[i], estimated);
    }
}

// 演示 4: 参数调优
static void demo_tuning(void) {
    printf("\n=== 演示 4: 参数调优 ===\n");

    printf("Q (过程噪声) 和 R (测量噪声) 的影响:\n\n");

    printf("Q 较大, R 较小:\n");
    printf("  - 更信任测量值\n");
    printf("  - 响应更快，但可能更噪声\n");
    printf("  - 适用于快速变化的系统\n\n");

    printf("Q 较小, R 较大:\n");
    printf("  - 更信任模型预测\n");
    printf("  - 更平滑，但可能滞后\n");
    printf("  - 适用于稳定的系统\n\n");

    // 对比示例
    double measurements[] = {10.0, 10.5, 9.5, 10.2, 9.8, 10.1};
    int n = sizeof(measurements) / sizeof(measurements[0]);

    printf("对比测试 (Q=0.01 vs Q=0.5):\n\n");

    kalman_scalar_t kf1, kf2;
    kalman_scalar_init(&kf1, 0.0, 1.0, 0.01, 0.1);  // 小 Q
    kalman_scalar_init(&kf2, 0.0, 1.0, 0.5, 0.1);   // 大 Q

    printf("  %-6s %-12s %-12s %-12s\n", "测量", "Q=0.01", "Q=0.5", "差异");
    printf("  %-6s %-12s %-12s %-12s\n", "------", "------------", "------------", "------------");

    for (int i = 0; i < n; i++) {
        double est1 = kalman_scalar_update(&kf1, measurements[i]);
        double est2 = kalman_scalar_update(&kf2, measurements[i]);
        printf("  %-6.1f %-12.4f %-12.4f %-12.4f\n",
               measurements[i], est1, est2, fabs(est1 - est2));
    }
}

// 演示 5: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 5: 应用场景 ===\n");

    printf("1. 传感器数据处理\n");
    printf("   - 温度传感器\n");
    printf("   - 压力传感器\n");
    printf("   - 加速度计\n");
    printf("\n");

    printf("2. 金融数据\n");
    printf("   - 股票价格平滑\n");
    printf("   - 汇率预测\n");
    printf("\n");

    printf("3. 运动跟踪\n");
    printf("   - GPS 位置滤波\n");
    printf("   - 速度估计\n");
    printf("\n");

    printf("4. 控制系统\n");
    printf("   - 电机位置控制\n");
    printf("   - 无人机姿态估计\n");
}

// 演示 6: 算法原理
static void demo_theory(void) {
    printf("\n=== 演示 6: 算法原理 ===\n");

    printf("卡尔曼滤波步骤:\n\n");

    printf("1. 预测步骤:\n");
    printf("   x_pred = x_prev\n");
    printf("   P_pred = P_prev + Q\n\n");

    printf("2. 更新步骤:\n");
    printf("   K = P_pred / (P_pred + R)\n");
    printf("   x = x_pred + K * (z - x_pred)\n");
    printf("   P = (1 - K) * P_pred\n\n");

    printf("变量说明:\n");
    printf("   x: 状态估计\n");
    printf("   P: 估计误差协方差\n");
    printf("   Q: 过程噪声协方差\n");
    printf("   R: 测量噪声协方差\n");
    printf("   K: 卡尔曼增益\n");
    printf("   z: 测量值\n");
}

int main(void) {
    printf("========================================\n");
    printf("    标量卡尔曼滤波器演示\n");
    printf("========================================\n");

    demo_basic();
    demo_noise_reduction();
    demo_tracking();
    demo_tuning();
    demo_applications();
    demo_theory();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
