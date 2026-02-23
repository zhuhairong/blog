#include "fast_fourier_transform.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <complex.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// 传统 FFT 计算函数
void fft_compute(double _Complex *a, size_t n) {
    if (n <= 1) return;
    double _Complex *even = malloc(n/2 * sizeof(double _Complex));
    double _Complex *odd = malloc(n/2 * sizeof(double _Complex));
    for (size_t i = 0; i < n/2; i++) {
        even[i] = a[2*i];
        odd[i] = a[2*i+1];
    }
    fft_compute(even, n/2);
    fft_compute(odd, n/2);
    for (size_t k = 0; k < n/2; k++) {
        double angle = -2.0 * M_PI * k / n;
        double _Complex twiddle = cos(angle) + sin(angle) * _Complex_I;
        double _Complex t = twiddle * odd[k];
        a[k] = even[k] + t;
        a[k + n/2] = even[k] - t;
    }
    free(even); free(odd);
}

// 创建 FFT 上下文
fft_error_t fft_create(fft_ctx_t** ctx, const fft_config_t* config) {
    if (!ctx) {
        return FFT_INVALID_PARAMS;
    }
    
    *ctx = (fft_ctx_t*)malloc(sizeof(fft_ctx_t));
    if (!*ctx) {
        return FFT_MEMORY_ERROR;
    }
    
    memset(*ctx, 0, sizeof(fft_ctx_t));
    
    if (config) {
        (*ctx)->config = *config;
    } else {
        (*ctx)->config.use_optimized = false;
        (*ctx)->config.check_size = true;
        (*ctx)->config.use_cached_windows = false;
        (*ctx)->config.allow_odd_size = false;
        (*ctx)->config.max_fft_size = 65536;
        (*ctx)->config.max_batch_size = 100;
    }
    
    (*ctx)->twiddle_factors = NULL;
    (*ctx)->twiddle_size = 0;
    (*ctx)->last_error = FFT_OK;
    
    return FFT_OK;
}

// 销毁 FFT 上下文
void fft_destroy(fft_ctx_t* ctx) {
    if (ctx) {
        if (ctx->twiddle_factors) {
            free(ctx->twiddle_factors);
        }
        free(ctx);
    }
}

// 检查是否为2的幂
static bool is_power_of_2(size_t n) {
    return n > 0 && (n & (n - 1)) == 0;
}

// 计算 FFT (Cooley-Tukey)
fft_error_t fft_compute_safe(fft_ctx_t* ctx, double _Complex *a, size_t n) {
    if (!ctx || !a) {
        return FFT_INVALID_PARAMS;
    }
    
    if (n == 0) {
        return FFT_INVALID_SIZE;
    }
    
    if (ctx->config.check_size && !is_power_of_2(n)) {
        if (!ctx->config.allow_odd_size) {
            return FFT_UNSUPPORTED_SIZE;
        }
    }
    
    if (ctx->config.max_fft_size > 0 && n > ctx->config.max_fft_size) {
        return FFT_BUFFER_TOO_SMALL;
    }
    
    fft_compute(a, n);
    ctx->compute_count++;
    
    return FFT_OK;
}

// 计算逆 FFT
fft_error_t fft_inverse(fft_ctx_t* ctx, double _Complex *a, size_t n) {
    if (!ctx || !a) {
        return FFT_INVALID_PARAMS;
    }
    
    if (n == 0) {
        return FFT_INVALID_SIZE;
    }
    
    if (ctx->config.check_size && !is_power_of_2(n)) {
        if (!ctx->config.allow_odd_size) {
            return FFT_UNSUPPORTED_SIZE;
        }
    }
    
    // 共轭
    for (size_t i = 0; i < n; i++) {
        a[i] = conj(a[i]);
    }
    
    // 正向 FFT
    fft_compute(a, n);
    
    // 再次共轭并除以 n
    for (size_t i = 0; i < n; i++) {
        a[i] = conj(a[i]) / n;
    }
    
    ctx->inverse_count++;
    
    return FFT_OK;
}

// 批量计算 FFT
fft_error_t fft_compute_batch(fft_ctx_t* ctx, double _Complex** arrays, size_t* sizes, size_t count) {
    if (!ctx || !arrays || !sizes) {
        return FFT_INVALID_PARAMS;
    }
    
    if (ctx->config.max_batch_size > 0 && count > ctx->config.max_batch_size) {
        return FFT_BUFFER_TOO_SMALL;
    }
    
    for (size_t i = 0; i < count; i++) {
        if (arrays[i]) {
            fft_error_t err = fft_compute_safe(ctx, arrays[i], sizes[i]);
            if (err != FFT_OK) {
                return err;
            }
        }
    }
    
    ctx->batch_count++;
    
    return FFT_OK;
}

// 计算实数 FFT
fft_error_t fft_compute_real(fft_ctx_t* ctx, const double* real, size_t n, double _Complex* out) {
    if (!ctx || !real || !out) {
        return FFT_INVALID_PARAMS;
    }
    
    if (n == 0) {
        return FFT_INVALID_SIZE;
    }
    
    // 转换为复数
    for (size_t i = 0; i < n; i++) {
        out[i] = real[i] + 0.0 * _Complex_I;
    }
    
    // 计算 FFT
    fft_error_t err = fft_compute_safe(ctx, out, n);
    
    return err;
}

// 从复数 FFT 结果获取实数
fft_error_t fft_get_real(fft_ctx_t* ctx, const double _Complex* in, size_t n, double* real) {
    if (!ctx || !in || !real) {
        return FFT_INVALID_PARAMS;
    }
    
    if (n == 0) {
        return FFT_INVALID_SIZE;
    }
    
    for (size_t i = 0; i < n; i++) {
        real[i] = creal(in[i]);
    }
    
    return FFT_OK;
}

// 获取最后一次错误
fft_error_t fft_get_last_error(fft_ctx_t* ctx) {
    if (!ctx) {
        return FFT_INVALID_PARAMS;
    }
    return ctx->last_error;
}

// 获取错误信息
const char* fft_strerror(fft_error_t error) {
    switch (error) {
        case FFT_OK: return "Success";
        case FFT_INVALID_PARAMS: return "Invalid parameters";
        case FFT_INVALID_SIZE: return "Invalid size";
        case FFT_MEMORY_ERROR: return "Memory error";
        case FFT_BUFFER_TOO_SMALL: return "Buffer too small";
        case FFT_UNSUPPORTED_SIZE: return "Unsupported size (must be power of 2)";
        case FFT_COMPUTATION_ERROR: return "Computation error";
        default: return "Unknown error";
    }
}
