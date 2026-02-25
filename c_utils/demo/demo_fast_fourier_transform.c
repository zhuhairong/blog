/**
 * 快速傅里叶变换 (FFT) 演示程序
 * 
 * 功能：
 * - FFT 正变换和逆变换
 * - 实数信号处理
 * - 频谱分析
 * - 信号滤波演示
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <string.h>
#include "../c_utils/fast_fourier_transform.h"

#define PI 3.14159265358979323846
#define N 64

// 打印复数数组
static void print_complex_array(const char *name, double _Complex *arr, int n, int limit) {
    printf("%s:\n", name);
    int count = (n < limit) ? n : limit;
    for (int i = 0; i < count; i++) {
        printf("  [%2d] %.4f %+.4fi\n", i, creal(arr[i]), cimag(arr[i]));
    }
    if (n > limit) {
        printf("  ... (%d more)\n", n - limit);
    }
}

// 打印幅度谱
static void print_magnitude_spectrum(const char *name, double _Complex *arr, int n, int limit) {
    printf("%s:\n", name);
    int count = (n < limit) ? n : limit;
    for (int i = 0; i < count; i++) {
        double mag = cabs(arr[i]);
        printf("  [%2d] %.4f\n", i, mag);
    }
    if (n > limit) {
        printf("  ... (%d more)\n", n - limit);
    }
}

// 生成正弦波
static void generate_sine_wave(double _Complex *signal, int n, double freq, double sample_rate) {
    for (int i = 0; i < n; i++) {
        double t = i / sample_rate;
        signal[i] = sin(2 * PI * freq * t);
    }
}

// 生成余弦波
static void generate_cosine_wave(double _Complex *signal, int n, double freq, double sample_rate) {
    for (int i = 0; i < n; i++) {
        double t = i / sample_rate;
        signal[i] = cos(2 * PI * freq * t);
    }
}

// 生成方波
static void generate_square_wave(double _Complex *signal, int n, double freq, double sample_rate) {
    double period = sample_rate / freq;
    for (int i = 0; i < n; i++) {
        signal[i] = (fmod(i, period) < period / 2) ? 1.0 : -1.0;
    }
}

// 演示 1: 基本 FFT
static void demo_basic_fft(void) {
    printf("\n=== 演示 1: 基本 FFT 变换 ===\n");
    
    fft_ctx_t *ctx = NULL;
    fft_config_t config = {
        .use_optimized = true,
        .check_size = true,
        .use_cached_windows = true,
        .allow_odd_size = false,
        .max_fft_size = 1024,
        .max_batch_size = 10
    };
    
    fft_error_t error = fft_create(&ctx, &config);
    if (error != FFT_OK) {
        printf("创建 FFT 上下文失败: %s\n", fft_strerror(error));
        return;
    }
    
    double _Complex signal[N];
    double _Complex original[N];
    
    // 生成测试信号：两个正弦波的叠加
    printf("生成测试信号 (10Hz + 25Hz 正弦波叠加):\n");
    for (int i = 0; i < N; i++) {
        double t = i / 256.0;  // 采样率 256Hz
        signal[i] = sin(2 * PI * 10 * t) + 0.5 * sin(2 * PI * 25 * t);
        original[i] = signal[i];
    }
    
    printf("  采样点数: %d\n", N);
    printf("  采样率: 256 Hz\n");
    printf("  频率分辨率: %.2f Hz\n", 256.0 / N);
    
    // FFT 正变换
    printf("\n执行 FFT 正变换...\n");
    error = fft_compute_safe(ctx, signal, N);
    if (error != FFT_OK) {
        printf("FFT 失败: %s\n", fft_strerror(error));
        fft_destroy(ctx);
        return;
    }
    
    print_magnitude_spectrum("频谱 (前 16 点)", signal, N, 16);
    
    // FFT 逆变换
    printf("\n执行 FFT 逆变换...\n");
    error = fft_inverse(ctx, signal, N);
    if (error != FFT_OK) {
        printf("逆 FFT 失败: %s\n", fft_strerror(error));
        fft_destroy(ctx);
        return;
    }
    
    // 验证
    printf("\n验证 (前 8 点):\n");
    printf("  原始 -> 逆变换\n");
    for (int i = 0; i < 8; i++) {
        printf("  %.4f -> %.4f (误差: %.6f)\n", 
               creal(original[i]), creal(signal[i]), 
               fabs(creal(original[i]) - creal(signal[i])));
    }
    
    fft_destroy(ctx);
}

// 演示 2: 实数 FFT
static void demo_real_fft(void) {
    printf("\n=== 演示 2: 实数 FFT ===\n");
    
    fft_ctx_t *ctx = NULL;
    fft_config_t config = {
        .use_optimized = true,
        .check_size = true,
        .use_cached_windows = true,
        .allow_odd_size = false,
        .max_fft_size = 1024,
        .max_batch_size = 10
    };
    
    fft_create(&ctx, &config);
    
    double real_signal[N];
    double _Complex spectrum[N];
    
    // 生成实数信号
    printf("生成实数信号 (20Hz 正弦波):\n");
    generate_sine_wave((double _Complex*)real_signal, N, 20.0, 256.0);
    for (int i = 0; i < N; i++) {
        real_signal[i] = creal(((double _Complex*)real_signal)[i]);
    }
    
    printf("  前 8 点: ");
    for (int i = 0; i < 8; i++) {
        printf("%.3f ", real_signal[i]);
    }
    printf("\n");
    
    // 实数 FFT
    printf("\n执行实数 FFT...\n");
    fft_error_t error = fft_compute_real(ctx, real_signal, N, spectrum);
    if (error != FFT_OK) {
        printf("实数 FFT 失败: %s\n", fft_strerror(error));
        fft_destroy(ctx);
        return;
    }
    
    print_magnitude_spectrum("频谱 (前 16 点)", spectrum, N, 16);
    
    fft_destroy(ctx);
}

// 演示 3: 频谱分析
static void demo_spectrum_analysis(void) {
    printf("\n=== 演示 3: 频谱分析 ===\n");
    
    fft_ctx_t *ctx = NULL;
    fft_config_t config = {
        .use_optimized = true,
        .check_size = true,
        .use_cached_windows = true,
        .allow_odd_size = false,
        .max_fft_size = 1024,
        .max_batch_size = 10
    };
    
    fft_create(&ctx, &config);
    
    double _Complex signal[N];
    double sample_rate = 256.0;
    double freq_resolution = sample_rate / N;
    
    // 生成复合信号
    printf("生成复合信号:\n");
    printf("  成分 1: 10Hz, 幅度 1.0\n");
    printf("  成分 2: 30Hz, 幅度 0.7\n");
    printf("  成分 3: 50Hz, 幅度 0.3\n");
    
    for (int i = 0; i < N; i++) {
        double t = i / sample_rate;
        signal[i] = 1.0 * sin(2 * PI * 10 * t) +
                    0.7 * sin(2 * PI * 30 * t) +
                    0.3 * sin(2 * PI * 50 * t);
    }
    
    // FFT
    fft_compute_safe(ctx, signal, N);
    
    // 分析频谱
    printf("\n频谱分析:\n");
    printf("  频率分辨率: %.2f Hz\n", freq_resolution);
    printf("\n  检测到的频率成分:\n");
    
    int peak_count = 0;
    for (int i = 1; i < N/2; i++) {
        double mag = cabs(signal[i]);
        double prev_mag = cabs(signal[i-1]);
        double next_mag = cabs(signal[i+1]);
        
        // 简单的峰值检测
        if (mag > prev_mag && mag > next_mag && mag > 5.0) {
            double freq = i * freq_resolution;
            printf("    峰值 %d: %.1f Hz, 幅度: %.2f\n", ++peak_count, freq, mag / N * 2);
        }
    }
    
    fft_destroy(ctx);
}

// 演示 4: 不同波形的频谱
static void demo_waveform_spectra(void) {
    printf("\n=== 演示 4: 不同波形的频谱 ===\n");
    
    fft_ctx_t *ctx = NULL;
    fft_config_t config = {
        .use_optimized = true,
        .check_size = true,
        .use_cached_windows = true,
        .allow_odd_size = false,
        .max_fft_size = 1024,
        .max_batch_size = 10
    };
    
    fft_create(&ctx, &config);
    
    double _Complex signal[N];
    double sample_rate = 256.0;
    
    // 正弦波
    printf("1. 正弦波 (10Hz):\n");
    generate_sine_wave(signal, N, 10.0, sample_rate);
    fft_compute_safe(ctx, signal, N);
    printf("   频谱特点: 单一频率峰值\n");
    printf("   主频率: %.1f Hz\n", 10.0);
    
    // 方波
    printf("\n2. 方波 (10Hz):\n");
    generate_square_wave(signal, N, 10.0, sample_rate);
    fft_compute_safe(ctx, signal, N);
    printf("   频谱特点: 基频 + 奇次谐波\n");
    printf("   谐波成分: 10Hz, 30Hz, 50Hz, 70Hz...\n");
    
    fft_destroy(ctx);
}

// 演示 5: FFT 应用
static void demo_applications(void) {
    printf("\n=== 演示 5: FFT 应用场景 ===\n");
    
    printf("1. 音频处理\n");
    printf("   - 频谱分析仪\n");
    printf("   - 均衡器\n");
    printf("   - 音频压缩 (MP3, AAC)\n");
    printf("   - 降噪处理\n");
    printf("\n");
    
    printf("2. 通信系统\n");
    printf("   - OFDM (正交频分复用)\n");
    printf("   - 调制解调\n");
    printf("   - 信道估计\n");
    printf("\n");
    
    printf("3. 图像处理\n");
    printf("   - 图像滤波\n");
    printf("   - 图像压缩 (JPEG)\n");
    printf("   - 模式识别\n");
    printf("\n");
    
    printf("4. 科学计算\n");
    printf("   - 信号处理\n");
    printf("   - 数值分析\n");
    printf("   - 微分方程求解\n");
    printf("\n");
    
    printf("5. 医学信号\n");
    printf("   - ECG/EKG 分析\n");
    printf("   - EEG 脑电波分析\n");
    printf("   - 医学成像 (MRI, CT)\n");
}

// 演示 6: 性能说明
static void demo_performance(void) {
    printf("\n=== 演示 6: FFT 性能说明 ===\n");
    
    printf("FFT 复杂度:\n");
    printf("  直接 DFT: O(N²)\n");
    printf("  FFT: O(N log N)\n");
    printf("\n");
    
    printf("性能对比 (N=1024):\n");
    printf("  DFT: ~1,048,576 次操作\n");
    printf("  FFT: ~10,240 次操作\n");
    printf("  加速比: ~100x\n");
    printf("\n");
    
    printf("优化选项:\n");
    printf("  use_optimized: 使用优化的算法实现\n");
    printf("  use_cached_windows: 缓存旋转因子\n");
    printf("  check_size: 验证输入大小为 2 的幂\n");
    printf("\n");
    
    printf("注意事项:\n");
    printf("  - FFT 要求输入大小为 2 的幂\n");
    printf("  - 逆变换后需要归一化 (除以 N)\n");
    printf("  - 频谱是对称的 (实数输入)\n");
}

int main(void) {
    printf("========================================\n");
    printf("    快速傅里叶变换 (FFT) 演示\n");
    printf("========================================\n");
    
    demo_basic_fft();
    demo_real_fft();
    demo_spectrum_analysis();
    demo_waveform_spectra();
    demo_applications();
    demo_performance();
    
    printf("\n========================================\n");
    printf("演示完成!\n");
    
    return 0;
}
