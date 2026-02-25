/**
 * PID 控制器演示程序
 *
 * 功能：
 * - PID 参数调节
 * - 控制器更新
 * - 应用场景模拟
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../c_utils/pid_controller.h"

// 演示 1: 基本概念
static void demo_concept(void) {
    printf("\n=== 演示 1: PID 控制器基本概念 ===\n");

    printf("PID 控制器:\n\n");

    printf("P - 比例 (Proportional):\n");
    printf("  输出 = Kp × 误差\n");
    printf("  作用: 快速响应误差\n");
    printf("  问题: 存在稳态误差\n\n");

    printf("I - 积分 (Integral):\n");
    printf("  输出 = Ki × ∫误差 dt\n");
    printf("  作用: 消除稳态误差\n");
    printf("  问题: 可能引起超调\n\n");

    printf("D - 微分 (Derivative):\n");
    printf("  输出 = Kd × d(误差)/dt\n");
    printf("  作用: 预测误差变化，抑制超调\n");
    printf("  问题: 对噪声敏感\n\n");

    printf("公式:\n");
    printf("  u(t) = Kp×e(t) + Ki×∫e(t)dt + Kd×de(t)/dt\n");
}

// 演示 2: 基本使用
static void demo_basic(void) {
    printf("\n=== 演示 2: 基本使用 ===\n");

    pid_controller_t pid;

    printf("初始化 PID 控制器:\n");
    printf("  Kp = 1.0, Ki = 0.1, Kd = 0.01\n\n");

    pid_init(&pid, 1.0, 0.1, 0.01);

    double setpoint = 100.0;  // 目标值
    double measured = 0.0;    // 当前值
    double dt = 0.1;          // 时间步长

    printf("设定值: %.1f\n", setpoint);
    printf("初始值: %.1f\n\n", measured);

    printf("时间    测量值    误差      输出\n");
    printf("----    ------    ----      ----\n");

    for (int i = 0; i < 20; i++) {
        double output = pid_update(&pid, setpoint, measured, dt);

        // 模拟系统响应 (简化模型)
        measured += output * 0.1;

        printf("%.1f    %6.2f    %6.2f    %6.2f\n",
               i * dt, measured, setpoint - measured, output);

        // 接近目标时停止
        if (fabs(setpoint - measured) < 0.5 && i > 10) break;
    }
}

// 演示 3: 参数影响
static void demo_parameters(void) {
    printf("\n=== 演示 3: 参数影响 ===\n");

    printf("Kp (比例增益):\n");
    printf("  增大 -> 响应更快，但可能振荡\n");
    printf("  减小 -> 响应更慢，更稳定\n\n");

    printf("Ki (积分增益):\n");
    printf("  增大 -> 更快消除稳态误差\n");
    printf("  减小 -> 减少超调\n\n");

    printf("Kd (微分增益):\n");
    printf("  增大 -> 减少超调\n");
    printf("  减小 -> 对噪声不敏感\n\n");

    printf("调节方法:\n");
    printf("  1. 先调 Kp 到临界振荡\n");
    printf("  2. 加入 Ki 消除稳态误差\n");
    printf("  3. 加入 Kd 抑制超调\n");
}

// 演示 4: 配置选项
static void demo_config(void) {
    printf("\n=== 演示 4: 配置选项 ===\n");

    pid_config_t config = pid_default_config();

    printf("默认配置:\n");
    printf("  Kp: %.2f\n", config.kp);
    printf("  Ki: %.2f\n", config.ki);
    printf("  Kd: %.2f\n", config.kd);
    printf("  输出范围: [%.1f, %.1f]\n", config.output_min, config.output_max);
    printf("  抗积分饱和: %s\n", config.anti_windup ? "是" : "否");

    printf("\n高级配置:\n");
    printf("  积分限幅: 防止积分累积过大\n");
    printf("  微分滤波: 减少噪声影响\n");
    printf("  测量值微分: 避免设定值突变引起的问题\n");
}

// 演示 5: 控制器模式
static void demo_modes(void) {
    printf("\n=== 演示 5: 控制器模式 ===\n");

    printf("AUTO (自动模式):\n");
    printf("  - 正常 PID 控制\n");
    printf("  - 根据误差计算输出\n\n");

    printf("MANUAL (手动模式):\n");
    printf("  - 直接设置输出值\n");
    printf("  - 用于调试或特殊操作\n\n");

    printf("OFF (关闭模式):\n");
    printf("  - 输出为零\n");
    printf("  - 控制器停止工作\n");
}

// 演示 6: 温度控制模拟
static void demo_temperature(void) {
    printf("\n=== 演示 6: 温度控制模拟 ===\n");

    pid_controller_t pid;
    pid_init(&pid, 2.0, 0.5, 0.1);

    double setpoint = 50.0;   // 目标温度
    double temperature = 20.0; // 初始温度
    double dt = 1.0;          // 1秒

    printf("目标温度: %.1f°C\n", setpoint);
    printf("初始温度: %.1f°C\n\n", temperature);

    printf("时间(s)  温度(°C)  加热功率(%%)\n");
    printf("-------  --------  ------------\n");

    for (int t = 0; t <= 60; t++) {
        double power = pid_update(&pid, setpoint, temperature, dt);

        // 模拟温度变化 (简化模型)
        // 加热器功率影响 + 自然散热
        double heating = power * 0.3;
        double cooling = (temperature - 20) * 0.05;
        temperature += (heating - cooling) * dt;

        if (t % 5 == 0) {
            printf(" %3d      %5.1f      %6.1f\n", t, temperature, power);
        }

        // 稳定后停止
        if (fabs(setpoint - temperature) < 0.5 && t > 30) break;
    }
}

// 演示 7: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 7: 应用场景 ===\n");

    printf("1. 工业控制\n");
    printf("   - 温度控制\n");
    printf("   - 压力控制\n");
    printf("   - 流量控制\n\n");

    printf("2. 机器人\n");
    printf("   - 电机速度控制\n");
    printf("   - 位置控制\n");
    printf("   - 姿态控制\n\n");

    printf("3. 无人机\n");
    printf("   - 高度控制\n");
    printf("   - 悬停稳定\n");
    printf("   - 航向控制\n\n");

    printf("4. 自动驾驶\n");
    printf("   - 车速控制\n");
    printf("   - 转向控制\n");
    printf("   - 距离保持\n\n");

    printf("5. 家用电器\n");
    printf("   - 空调温控\n");
    printf("   - 洗衣机水位\n");
    printf("   - 烤箱温度\n");
}

int main(void) {
    printf("========================================\n");
    printf("    PID 控制器演示\n");
    printf("========================================\n");

    demo_concept();
    demo_basic();
    demo_parameters();
    demo_config();
    demo_modes();
    demo_temperature();
    demo_applications();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
