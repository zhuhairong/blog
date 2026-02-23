#ifndef C_UTILS_FAST_FOURIER_TRANSFORM_H
#define C_UTILS_FAST_FOURIER_TRANSFORM_H

#include <complex.h>
#include <stddef.h>
#include <stdbool.h>

// FFT 错误码
typedef enum {
    FFT_OK = 0,
    FFT_INVALID_PARAMS = -1,
    FFT_INVALID_SIZE = -2,
    FFT_MEMORY_ERROR = -3,
    FFT_BUFFER_TOO_SMALL = -4,
    FFT_UNSUPPORTED_SIZE = -5,
    FFT_COMPUTATION_ERROR = -6
} fft_error_t;

// FFT 配置选项
typedef struct {
    bool use_optimized;         // 是否使用优化版本
    bool check_size;            // 是否检查大小（必须是2的幂）
    bool use_cached_windows;    // 是否使用缓存的旋转因子
    bool allow_odd_size;        // 是否允许奇数大小
    size_t max_fft_size;        // 最大 FFT 大小
    size_t max_batch_size;      // 最大批量大小
} fft_config_t;

// FFT 上下文
typedef struct {
    fft_config_t config;
    double _Complex* twiddle_factors;  // 旋转因子缓存
    size_t twiddle_size;        // 旋转因子大小
    fft_error_t last_error;
    size_t compute_count;
    size_t inverse_count;
    size_t batch_count;
} fft_ctx_t;

// 创建 FFT 上下文
// 返回 FFT_OK 表示成功，其他值表示错误
fft_error_t fft_create(fft_ctx_t** ctx, const fft_config_t* config);

// 销毁 FFT 上下文
void fft_destroy(fft_ctx_t* ctx);

// 计算 FFT (Cooley-Tukey)
// 返回 FFT_OK 表示成功，其他值表示错误
fft_error_t fft_compute_safe(fft_ctx_t* ctx, double _Complex *a, size_t n);

// 计算逆 FFT
// 返回 FFT_OK 表示成功，其他值表示错误
fft_error_t fft_inverse(fft_ctx_t* ctx, double _Complex *a, size_t n);

// 批量计算 FFT
// 返回 FFT_OK 表示成功，其他值表示错误
fft_error_t fft_compute_batch(fft_ctx_t* ctx, double _Complex** arrays, size_t* sizes, size_t count);

// 计算实数 FFT
// 返回 FFT_OK 表示成功，其他值表示错误
fft_error_t fft_compute_real(fft_ctx_t* ctx, const double* real, size_t n, double _Complex* out);

// 从复数 FFT 结果获取实数
// 返回 FFT_OK 表示成功，其他值表示错误
fft_error_t fft_get_real(fft_ctx_t* ctx, const double _Complex* in, size_t n, double* real);

// 传统 FFT 计算函数（向后兼容）
void fft_compute(double _Complex *a, size_t n);

// 获取最后一次错误
fft_error_t fft_get_last_error(fft_ctx_t* ctx);

// 获取错误信息
const char* fft_strerror(fft_error_t error);

#endif // C_UTILS_FAST_FOURIER_TRANSFORM_H
