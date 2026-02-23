#include "math_utils.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <float.h>
#include <float.h>
#include <limits.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// 传统函数实现
double math_clamp(double v, double min, double max) {
    if (v < min) return min;
    if (v > max) return max;
    return v;
}

double math_lerp(double a, double b, double t) {
    return a + t * (b - a);
}

int math_rand_int(int min, int max) {
    return min + rand() % (max - min + 1);
}

double math_rand_double(void) {
    return (double)rand() / (double)RAND_MAX;
}

bool math_is_pow2(size_t n) {
    return n > 0 && (n & (n - 1)) == 0;
}

// 创建数学工具上下文
math_utils_error_t math_utils_create(math_utils_ctx_t** ctx, const math_utils_config_t* config) {
    if (!ctx) {
        return MATH_UTILS_INVALID_PARAMS;
    }
    
    *ctx = (math_utils_ctx_t*)malloc(sizeof(math_utils_ctx_t));
    if (!*ctx) {
        return MATH_UTILS_MEMORY_ERROR;
    }
    
    memset(*ctx, 0, sizeof(math_utils_ctx_t));
    
    if (config) {
        (*ctx)->config = *config;
    } else {
        (*ctx)->config.check_range = true;
        (*ctx)->config.check_overflow = true;
        (*ctx)->config.use_fast_rand = false;
        (*ctx)->config.use_approximate = false;
        (*ctx)->config.epsilon = 1e-9;
        (*ctx)->config.max_array_size = 1000000;
    }
    
    (*ctx)->rand_state[0] = (uint64_t)rand();
    (*ctx)->rand_state[1] = (uint64_t)rand();
    (*ctx)->last_error = MATH_UTILS_OK;
    
    return MATH_UTILS_OK;
}

// 销毁数学工具上下文
void math_utils_destroy(math_utils_ctx_t* ctx) {
    if (ctx) {
        free(ctx);
    }
}

// 安全钳位
math_utils_error_t math_utils_clamp_safe(math_utils_ctx_t* ctx, double v, double min, double max, double* out) {
    if (!ctx || !out) {
        return MATH_UTILS_INVALID_PARAMS;
    }
    
    if (ctx->config.check_range && min > max) {
        return MATH_UTILS_RANGE_ERROR;
    }
    
    *out = math_clamp(v, min, max);
    ctx->clamp_count++;
    
    return MATH_UTILS_OK;
}

// 安全线性插值
math_utils_error_t math_utils_lerp_safe(math_utils_ctx_t* ctx, double a, double b, double t, double* out) {
    if (!ctx || !out) {
        return MATH_UTILS_INVALID_PARAMS;
    }
    
    if (ctx->config.check_range && (t < 0.0 || t > 1.0)) {
        return MATH_UTILS_RANGE_ERROR;
    }
    
    *out = math_lerp(a, b, t);
    ctx->lerp_count++;
    
    return MATH_UTILS_OK;
}

// 安全随机整数
math_utils_error_t math_utils_rand_int_safe(math_utils_ctx_t* ctx, int min, int max, int* out) {
    if (!ctx || !out) {
        return MATH_UTILS_INVALID_PARAMS;
    }
    
    if (min > max) {
        return MATH_UTILS_RANGE_ERROR;
    }
    
    *out = math_rand_int(min, max);
    ctx->rand_count++;
    
    return MATH_UTILS_OK;
}

// 安全随机浮点数
math_utils_error_t math_utils_rand_double_safe(math_utils_ctx_t* ctx, double* out) {
    if (!ctx || !out) {
        return MATH_UTILS_INVALID_PARAMS;
    }
    
    *out = math_rand_double();
    ctx->rand_count++;
    
    return MATH_UTILS_OK;
}

// 批量随机整数
math_utils_error_t math_utils_rand_int_batch(math_utils_ctx_t* ctx, int min, int max, int* out, size_t count) {
    if (!ctx || !out) {
        return MATH_UTILS_INVALID_PARAMS;
    }
    
    if (ctx->config.max_array_size > 0 && count > ctx->config.max_array_size) {
        return MATH_UTILS_OVERFLOW;
    }
    
    for (size_t i = 0; i < count; i++) {
        out[i] = math_rand_int(min, max);
        ctx->rand_count++;
    }
    
    return MATH_UTILS_OK;
}

// 批量随机浮点数
math_utils_error_t math_utils_rand_double_batch(math_utils_ctx_t* ctx, double* out, size_t count) {
    if (!ctx || !out) {
        return MATH_UTILS_INVALID_PARAMS;
    }
    
    if (ctx->config.max_array_size > 0 && count > ctx->config.max_array_size) {
        return MATH_UTILS_OVERFLOW;
    }
    
    for (size_t i = 0; i < count; i++) {
        out[i] = math_rand_double();
        ctx->rand_count++;
    }
    
    return MATH_UTILS_OK;
}

// 映射值到新范围
math_utils_error_t math_utils_map_range(math_utils_ctx_t* ctx, double value, double in_min, double in_max, double out_min, double out_max, double* out) {
    if (!ctx || !out) {
        return MATH_UTILS_INVALID_PARAMS;
    }
    
    if (ctx->config.check_range) {
        if (in_min == in_max) {
            return MATH_UTILS_DIVISION_BY_ZERO;
        }
    }
    
    double t = (value - in_min) / (in_max - in_min);
    *out = out_min + t * (out_max - out_min);
    
    return MATH_UTILS_OK;
}

// 平滑步进函数
math_utils_error_t math_utils_smoothstep(math_utils_ctx_t* ctx, double edge0, double edge1, double x, double* out) {
    if (!ctx || !out) {
        return MATH_UTILS_INVALID_PARAMS;
    }
    
    if (ctx->config.check_range && edge0 == edge1) {
        return MATH_UTILS_RANGE_ERROR;
    }
    
    double t = (x - edge0) / (edge1 - edge0);
    t = math_clamp(t, 0.0, 1.0);
    *out = t * t * (3.0 - 2.0 * t);
    
    return MATH_UTILS_OK;
}

// 指数平滑
math_utils_error_t math_utils_exponential_smooth(math_utils_ctx_t* ctx, double current, double previous, double alpha, double* out) {
    if (!ctx || !out) {
        return MATH_UTILS_INVALID_PARAMS;
    }
    
    if (ctx->config.check_range && (alpha < 0.0 || alpha > 1.0)) {
        return MATH_UTILS_RANGE_ERROR;
    }
    
    *out = alpha * current + (1.0 - alpha) * previous;
    
    return MATH_UTILS_OK;
}

// 计算数组最小值
math_utils_error_t math_utils_array_min(math_utils_ctx_t* ctx, const double* array, size_t size, double* out) {
    if (!ctx || !array || !out) {
        return MATH_UTILS_INVALID_PARAMS;
    }
    
    if (size == 0) {
        return MATH_UTILS_RANGE_ERROR;
    }
    
    if (ctx->config.max_array_size > 0 && size > ctx->config.max_array_size) {
        return MATH_UTILS_OVERFLOW;
    }
    
    double min_val = array[0];
    for (size_t i = 1; i < size; i++) {
        if (array[i] < min_val) {
            min_val = array[i];
        }
    }
    
    *out = min_val;
    ctx->array_count++;
    
    return MATH_UTILS_OK;
}

// 计算数组最大值
math_utils_error_t math_utils_array_max(math_utils_ctx_t* ctx, const double* array, size_t size, double* out) {
    if (!ctx || !array || !out) {
        return MATH_UTILS_INVALID_PARAMS;
    }
    
    if (size == 0) {
        return MATH_UTILS_RANGE_ERROR;
    }
    
    if (ctx->config.max_array_size > 0 && size > ctx->config.max_array_size) {
        return MATH_UTILS_OVERFLOW;
    }
    
    double max_val = array[0];
    for (size_t i = 1; i < size; i++) {
        if (array[i] > max_val) {
            max_val = array[i];
        }
    }
    
    *out = max_val;
    ctx->array_count++;
    
    return MATH_UTILS_OK;
}

// 计算数组平均值
math_utils_error_t math_utils_array_mean(math_utils_ctx_t* ctx, const double* array, size_t size, double* out) {
    if (!ctx || !array || !out) {
        return MATH_UTILS_INVALID_PARAMS;
    }
    
    if (size == 0) {
        return MATH_UTILS_RANGE_ERROR;
    }
    
    if (ctx->config.max_array_size > 0 && size > ctx->config.max_array_size) {
        return MATH_UTILS_OVERFLOW;
    }
    
    double sum = 0.0;
    for (size_t i = 0; i < size; i++) {
        sum += array[i];
    }
    
    *out = sum / size;
    ctx->array_count++;
    
    return MATH_UTILS_OK;
}

// 计算数组总和
math_utils_error_t math_utils_array_sum(math_utils_ctx_t* ctx, const double* array, size_t size, double* out) {
    if (!ctx || !array || !out) {
        return MATH_UTILS_INVALID_PARAMS;
    }
    
    if (size == 0) {
        *out = 0.0;
        return MATH_UTILS_OK;
    }
    
    if (ctx->config.max_array_size > 0 && size > ctx->config.max_array_size) {
        return MATH_UTILS_OVERFLOW;
    }
    
    double sum = 0.0;
    for (size_t i = 0; i < size; i++) {
        sum += array[i];
    }
    
    *out = sum;
    ctx->array_count++;
    
    return MATH_UTILS_OK;
}

// 计算数组方差
math_utils_error_t math_utils_array_variance(math_utils_ctx_t* ctx, const double* array, size_t size, double* out) {
    if (!ctx || !array || !out) {
        return MATH_UTILS_INVALID_PARAMS;
    }
    
    if (size == 0) {
        return MATH_UTILS_RANGE_ERROR;
    }
    
    if (ctx->config.max_array_size > 0 && size > ctx->config.max_array_size) {
        return MATH_UTILS_OVERFLOW;
    }
    
    double mean;
    math_utils_array_mean(ctx, array, size, &mean);
    
    double sum_sq_diff = 0.0;
    for (size_t i = 0; i < size; i++) {
        double diff = array[i] - mean;
        sum_sq_diff += diff * diff;
    }
    
    *out = sum_sq_diff / size;
    ctx->array_count++;
    
    return MATH_UTILS_OK;
}

// 获取 2 的下一个幂
size_t math_utils_next_pow2(size_t n) {
    if (n == 0) return 1;
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
#if SIZE_MAX > 0xFFFFFFFF
    n |= n >> 32;
#endif
    return n + 1;
}

// 获取 2 的上一个幂
size_t math_utils_prev_pow2(size_t n) {
    if (n == 0) return 0;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
#if SIZE_MAX > 0xFFFFFFFF
    n |= n >> 32;
#endif
    return (n + 1) >> 1;
}

// 角度转换：度到弧度
double math_utils_deg_to_rad(double deg) {
    return deg * M_PI / 180.0;
}

// 角度转换：弧度到度
double math_utils_rad_to_deg(double rad) {
    return rad * 180.0 / M_PI;
}

// 限制角度在 [0, 360) 范围内
double math_utils_normalize_angle(double deg) {
    while (deg < 0) deg += 360.0;
    while (deg >= 360.0) deg -= 360.0;
    return deg;
}

// 重置随机数种子
void math_utils_srand(math_utils_ctx_t* ctx, uint64_t seed) {
    if (ctx) {
        ctx->rand_state[0] = seed;
        ctx->rand_state[1] = seed ^ 0x1234567890ABCDEFULL;
        srand((unsigned int)seed);
    }
}

// 获取最后一次错误
math_utils_error_t math_utils_get_last_error(math_utils_ctx_t* ctx) {
    if (!ctx) {
        return MATH_UTILS_INVALID_PARAMS;
    }
    return ctx->last_error;
}

// 获取错误信息
const char* math_utils_strerror(math_utils_error_t error) {
    switch (error) {
        case MATH_UTILS_OK: return "Success";
        case MATH_UTILS_INVALID_PARAMS: return "Invalid parameters";
        case MATH_UTILS_DIVISION_BY_ZERO: return "Division by zero";
        case MATH_UTILS_OVERFLOW: return "Overflow";
        case MATH_UTILS_UNDERFLOW: return "Underflow";
        case MATH_UTILS_RANGE_ERROR: return "Range error";
        case MATH_UTILS_DOMAIN_ERROR: return "Domain error";
        default: return "Unknown error";
    }
}
