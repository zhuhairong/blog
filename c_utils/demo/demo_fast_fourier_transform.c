/**
 * 快速傅里叶变换演示程序
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <complex.h>
#include "../c_utils/fast_fourier_transform.h"

static void demo_basic_fft(void) {
    printf("\n=== 演示 1: 基本 FFT ===\n");

    size_t n = 8;
    double _Complex signal[8];

    printf("创建正弦信号 (频率=1):\n");
    for (size_t i = 0; i < n; i++) {
        double t = (double)i / n;
        signal[i] = sin(2.0 * M_PI * t);
        printf("  x[%zu] = %.3f + %.3fi\n", i, creal(signal[i]), cimag(signal[i]));
    }

    fft_compute(signal, n);

    printf("\nFFT 结果:\n");
    for (size_t i = 0; i < n; i++) {
        double mag = cabs(signal[i]);
        printf("  X[%zu] = %.3f + %.3fi (幅度: %.3f)\n",
               i, creal(signal[i]), cimag(signal[i]), mag);
    }
}

static void demo_fft_context(void) {
    printf("\n=== 演示 2: 使用上下文 API ===\n");

    fft_ctx_t *ctx = NULL;
    fft_config_t config = {
        .use_optimized = true,
        .check_size = true,
        .use_cached_windows = true,
        .allow_odd_size = false,
        .max_fft_size = 4096,
        .max_batch_size = 16
    };

    fft_error_t err = fft_create(&ctx, &config);
    if (err != FFT_OK) {
        printf("创建 FFT 上下文失败: %s\n", fft_strerror(err));
        return;
    }
    printf("FFT 上下文创建成功\n");

    size_t n = 4;
    double _Complex data[4] = {1.0, 2.0, 3.0, 4.0};

    printf("\n原始数据:\n");
    for (size_t i = 0; i < n; i++) {
        printf("  x[%zu] = %.1f\n", i, creal(data[i]));
    }

    err = fft_compute_safe(ctx, data, n);
    if (err == FFT_OK) {
        printf("\nFFT 结果:\n");
        for (size_t i = 0; i < n; i++) {
            printf("  X[%zu] = %.1f + %.1fi\n", i, creal(data[i]), cimag(data[i]));
        }
    }

    err = fft_inverse(ctx, data, n);
    if (err == FFT_OK) {
        printf("\n逆 FFT 结果:\n");
        for (size_t i = 0; i < n; i++) {
            printf("  x[%zu] = %.1f\n", i, creal(data[i]) / n);
        }
    }

    fft_destroy(ctx);
    printf("\n上下文已销毁\n");
}

static void demo_real_fft(void) {
    printf("\n=== 演示 3: 实数 FFT ===\n");

    fft_ctx_t *ctx = NULL;
    fft_config_t config = {
        .use_optimized = false,
        .check_size = true,
        .use_cached_windows = false,
        .allow_odd_size = false,
        .max_fft_size = 1024,
        .max_batch_size = 1
    };

    fft_error_t err = fft_create(&ctx, &config);
    if (err != FFT_OK) {
        printf("创建失败\n");
        return;
    }

    size_t n = 8;
    double real_signal[8] = {1.0, 2.0, 3.0, 4.0, 5.0, 4.0, 3.0, 2.0};
    double _Complex complex_out[8];

    printf("实数输入:\n");
    for (size_t i = 0; i < n; i++) {
        printf("  x[%zu] = %.1f\n", i, real_signal[i]);
    }

    err = fft_compute_real(ctx, real_signal, n, complex_out);
    if (err == FFT_OK) {
        printf("\n复数输出:\n");
        for (size_t i = 0; i < n; i++) {
            printf("  X[%zu] = %.2f + %.2fi\n", i, creal(complex_out[i]), cimag(complex_out[i]));
        }
    }

    fft_destroy(ctx);
}

static void demo_frequency_analysis(void) {
    printf("\n=== 演示 4: 频率分析 ===\n");

    size_t n = 16;
    double _Complex signal[16];

    printf("创建混合信号 (频率 2 和 5):\n");
    for (size_t i = 0; i < n; i++) {
        double t = (double)i / n;
        signal[i] = sin(2.0 * M_PI * 2.0 * t) + 0.5 * sin(2.0 * M_PI * 5.0 * t);
    }

    fft_compute(signal, n);

    printf("\n频率分量 (幅度):\n");
    for (size_t i = 0; i < n / 2; i++) {
        double mag = cabs(signal[i]) / n;
        if (mag > 0.1) {
            printf("  频率 %2zu: %.3f %s\n", i, mag, mag > 0.3 ? "<-- 主要分量" : "");
        }
    }
}

int main(void) {
    printf("========================================\n");
    printf("    快速傅里叶变换演示\n");
    printf("========================================\n");

    demo_basic_fft();
    demo_fft_context();
    demo_real_fft();
    demo_frequency_analysis();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
