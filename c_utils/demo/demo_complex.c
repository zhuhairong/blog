/**
 * @file demo_complex.c
 * @brief 复数运算演示
 *
 * 本演示展示了复数的各种运算，包括：
 * - 基本运算（加减乘除）
 * - 极坐标表示
 * - 复数函数
 * - 实际应用
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../c_utils/complex.h"

// 打印复数
static void print_complex(const char* label, complex_t z) {
    if (z.imag >= 0) {
        printf("%s: %.3f + %.3fi\n", label, z.real, z.imag);
    } else {
        printf("%s: %.3f - %.3fi\n", label, z.real, -z.imag);
    }
}

// 打印极坐标
static void print_polar(const char* label, complex_t z) {
    double r = complex_abs(z);
    double theta = complex_arg(z);
    printf("%s: %.3f ∠ %.3f°\n", label, r, theta * 180 / M_PI);
}

// 演示 1: 基本运算
static void demo_basic_operations(void) {
    printf("\n=== 演示 1: 基本运算 ===\n");

    complex_t a = {3, 4};   // 3 + 4i
    complex_t b = {1, 2};   // 1 + 2i

    print_complex("a", a);
    print_complex("b", b);
    printf("\n");

    // 加法
    complex_t sum = complex_add(a, b);
    print_complex("a + b", sum);

    // 减法
    complex_t diff = complex_sub(a, b);
    print_complex("a - b", diff);

    // 乘法
    complex_t prod = complex_mul(a, b);
    print_complex("a × b", prod);

    // 除法
    complex_error_t err;
    complex_t quot = complex_div(a, b, &err);
    if (err == COMPLEX_OK) {
        print_complex("a ÷ b", quot);
    }

    // 验证
    complex_t check = complex_mul(quot, b);
    print_complex("验证: (a÷b) × b", check);
}

// 演示 2: 极坐标表示
static void demo_polar_form(void) {
    printf("\n=== 演示 2: 极坐标表示 ===\n");

    complex_t numbers[] = {
        {1, 0},      // 实轴
        {0, 1},      // 虚轴
        {1, 1},      // 45度
        {-1, 1},     // 135度
        {-1, -1},    // 225度
        {1, -1}      // 315度
    };
    int num = sizeof(numbers) / sizeof(numbers[0]);

    printf("复数的极坐标形式:\n\n");

    for (int i = 0; i < num; i++) {
        print_complex("  直角坐标", numbers[i]);
        print_polar("  极坐标", numbers[i]);
        printf("  模: %.3f, 幅角: %.3f°\n\n",
               complex_abs(numbers[i]),
               complex_arg(numbers[i]) * 180 / M_PI);
    }
}

// 演示 3: 共轭和模
static void demo_conjugate_and_abs(void) {
    printf("\n=== 演示 3: 共轭和模 ===\n");

    complex_t z = {3, 4};

    print_complex("z", z);
    printf("\n");

    // 共轭
    complex_t conj = complex_conj(z);
    print_complex("共轭 z*", conj);

    // 模
    double abs_z = complex_abs(z);
    printf("模 |z|: %.3f\n", abs_z);

    // 验证 |z|² = z × z*
    complex_t prod = complex_mul(z, conj);
    print_complex("z × z*", prod);
    printf("验证: |z|² = %.3f, z×z* = %.3f\n", abs_z * abs_z, prod.real);
}

// 演示 4: 复数函数
static void demo_complex_functions(void) {
    printf("\n=== 演示 4: 复数函数 ===\n");

    complex_t z = {1, 1};  // 1 + i

    print_complex("z", z);
    printf("\n");

    // 指数
    complex_t exp_z = complex_exp(z);
    print_complex("exp(z)", exp_z);

    // 对数
    complex_error_t err;
    complex_t log_z = complex_log(z, &err);
    if (err == COMPLEX_OK) {
        print_complex("log(z)", log_z);
    }

    // 幂
    complex_t two = {2, 0};
    complex_t pow_z = complex_pow(z, two, &err);
    if (err == COMPLEX_OK) {
        print_complex("z²", pow_z);
    }

    // 平方根
    complex_t sqrt_z = complex_sqrt(z);
    print_complex("√z", sqrt_z);

    // 验证
    complex_t check = complex_mul(sqrt_z, sqrt_z);
    print_complex("验证: √z × √z", check);
}

// 演示 5: 三角函数
static void demo_trigonometric(void) {
    printf("\n=== 演示 5: 三角函数 ===\n");

    complex_t z = {0, 1};  // i

    print_complex("z", z);
    printf("\n");

    // sin
    complex_t sin_z = complex_sin(z);
    print_complex("sin(z)", sin_z);

    // cos
    complex_t cos_z = complex_cos(z);
    print_complex("cos(z)", cos_z);

    // tan
    complex_error_t err;
    complex_t tan_z = complex_tan(z, &err);
    if (err == COMPLEX_OK) {
        print_complex("tan(z)", tan_z);
    }

    // 验证欧拉公式: e^(ix) = cos(x) + i·sin(x)
    printf("\n欧拉公式验证 (z = i):\n");
    complex_t exp_i = complex_exp(z);
    print_complex("e^i", exp_i);

    complex_t i_sin = {-sin_z.imag, sin_z.real};  // i * sin(z)
    complex_t sum = complex_add(cos_z, i_sin);
    print_complex("cos(1) + i·sin(1)", sum);
}

// 演示 6: 旋转
static void demo_rotation(void) {
    printf("\n=== 演示 6: 旋转 ===\n");

    complex_t z = {1, 0};  // 实轴上的点
    double angle = M_PI / 4;  // 45度

    print_complex("原始点", z);
    printf("旋转角度: %.1f°\n\n", angle * 180 / M_PI);

    // 旋转因子
    complex_t rotator = complex_polar(1, angle);
    print_complex("旋转因子", rotator);
    printf("\n");

    // 多次旋转
    complex_t result = z;
    for (int i = 1; i <= 8; i++) {
        result = complex_mul(result, rotator);
        printf("旋转 %d 次 (%.0f°): ", i, i * angle * 180 / M_PI);
        print_complex("", result);
    }
}

// 演示 7: 单位根
static void demo_roots_of_unity(void) {
    printf("\n=== 演示 7: 单位根 ===\n");

    int n = 8;  // 8次单位根

    printf("%d 次单位根:\n\n", n);

    for (int k = 0; k < n; k++) {
        double angle = 2 * M_PI * k / n;
        complex_t root = complex_polar(1, angle);

        printf("ω^%d = ", k);
        print_complex("", root);
        printf("     = ");
        print_polar("", root);

        // 验证 ω^n = 1
        complex_error_t err;
        complex_t n_complex = {n, 0};
        complex_t power = complex_pow(root, n_complex, &err);
        printf("     (ω^%d)^%d = ", k, n);
        print_complex("", power);
        printf("\n");
    }
}

// 演示 8: 解方程
static void demo_equation_solving(void) {
    printf("\n=== 演示 8: 解方程 ===\n");

    // 解二次方程: z² + 2z + 2 = 0
    printf("解方程: z² + 2z + 2 = 0\n\n");

    complex_t a = {1, 0};
    complex_t b = {2, 0};
    complex_t c = {2, 0};

    // 计算判别式 b² - 4ac
    complex_t b_squared = complex_mul(b, b);
    complex_t four_ac = {4 * c.real, 0};
    complex_t discriminant = complex_sub(b_squared, four_ac);
    print_complex("判别式", discriminant);

    complex_t sqrt_disc = complex_sqrt(discriminant);
    print_complex("√判别式", sqrt_disc);

    // -b
    complex_t neg_b = {-b.real, -b.imag};

    complex_error_t err;
    complex_t z1_num = complex_add(neg_b, sqrt_disc);
    complex_t z1_den = {2, 0};
    complex_t z1 = complex_div(z1_num, z1_den, &err);

    complex_t z2_num = complex_sub(neg_b, sqrt_disc);
    complex_t z2 = complex_div(z2_num, z1_den, &err);

    print_complex("z₁", z1);
    print_complex("z₂", z2);

    // 验证
    complex_t check1 = complex_add(complex_add(complex_mul(z1, z1),
                                                complex_mul(b, z1)), c);
    print_complex("验证 z₁", check1);
}

// 演示 9: 信号处理应用
static void demo_signal_processing(void) {
    printf("\n=== 演示 9: 信号处理应用 ===\n");

    // 模拟信号: 两个正弦波的叠加
    printf("复数表示正弦信号:\n\n");

    double freq1 = 1.0;   // 1 Hz
    double freq2 = 3.0;   // 3 Hz
    double amplitude1 = 1.0;
    double amplitude2 = 0.5;

    printf("信号: %.1f·sin(2π·%.1f·t) + %.1f·sin(2π·%.1f·t)\n",
           amplitude1, freq1, amplitude2, freq2);
    printf("\n采样 (一个周期):\n");

    int samples = 16;
    for (int i = 0; i < samples; i++) {
        double t = (double)i / samples;
        double real = amplitude1 * sin(2 * M_PI * freq1 * t) +
                     amplitude2 * sin(2 * M_PI * freq2 * t);
        double imag = amplitude1 * cos(2 * M_PI * freq1 * t) +
                     amplitude2 * cos(2 * M_PI * freq2 * t);

        complex_t signal = {real, imag};
        printf("  t=%.3f: ", t);
        print_complex("", signal);
    }
}

// 演示 10: 电路分析
static void demo_circuit_analysis(void) {
    printf("\n=== 演示 10: 电路分析应用 ===\n");

    // 交流电路分析
    printf("RLC 串联电路分析:\n\n");

    double R = 10;      // 电阻 10Ω
    double L = 0.1;     // 电感 0.1H
    double C = 100e-6;  // 电容 100μF
    double f = 50;      // 频率 50Hz
    double V = 220;     // 电压 220V

    double omega = 2 * M_PI * f;

    // 阻抗
    complex_t Z_R = {R, 0};
    complex_t Z_L = {0, omega * L};
    complex_t Z_C = {0, -1 / (omega * C)};

    print_complex("电阻阻抗 Z_R", Z_R);
    print_complex("电感阻抗 Z_L", Z_L);
    print_complex("电容阻抗 Z_C", Z_C);

    complex_t Z_total = complex_add(complex_add(Z_R, Z_L), Z_C);
    print_complex("总阻抗 Z", Z_total);

    print_polar("极坐标形式", Z_total);

    // 电流
    complex_t V_source = {V, 0};
    complex_error_t err;
    complex_t I = complex_div(V_source, Z_total, &err);

    if (err == COMPLEX_OK) {
        print_complex("电流 I", I);
        printf("电流幅值: %.3f A\n", complex_abs(I));
        printf("相位差: %.3f°\n", complex_arg(I) * 180 / M_PI);
    }
}

int main(void) {
    printf("========================================\n");
    printf("    复数运算演示程序\n");
    printf("========================================\n");

    demo_basic_operations();
    demo_polar_form();
    demo_conjugate_and_abs();
    demo_complex_functions();
    demo_trigonometric();
    demo_rotation();
    demo_roots_of_unity();
    demo_equation_solving();
    demo_signal_processing();
    demo_circuit_analysis();

    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("========================================\n");

    return 0;
}
