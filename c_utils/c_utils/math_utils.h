#ifndef C_UTILS_MATH_UTILS_H
#define C_UTILS_MATH_UTILS_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

// 数学工具错误码
typedef enum {
    MATH_UTILS_OK = 0,
    MATH_UTILS_INVALID_PARAMS = -1,
    MATH_UTILS_DIVISION_BY_ZERO = -2,
    MATH_UTILS_OVERFLOW = -3,
    MATH_UTILS_UNDERFLOW = -4,
    MATH_UTILS_RANGE_ERROR = -5,
    MATH_UTILS_DOMAIN_ERROR = -6,
    MATH_UTILS_MEMORY_ERROR = -7
} math_utils_error_t;

// 数学工具配置选项
typedef struct {
    bool check_range;           // 是否检查范围
    bool check_overflow;        // 是否检查溢出
    bool use_fast_rand;         // 是否使用快速随机数
    bool use_approximate;       // 是否使用近似计算
    double epsilon;             // 浮点数比较精度
    size_t max_array_size;      // 最大数组大小
} math_utils_config_t;

// 数学工具上下文
typedef struct {
    math_utils_config_t config;
    uint64_t rand_state[2];     // 随机数状态
    math_utils_error_t last_error;
    size_t clamp_count;
    size_t lerp_count;
    size_t rand_count;
    size_t array_count;
} math_utils_ctx_t;

// 创建数学工具上下文
// 返回 MATH_UTILS_OK 表示成功，其他值表示错误
math_utils_error_t math_utils_create(math_utils_ctx_t** ctx, const math_utils_config_t* config);

// 销毁数学工具上下文
void math_utils_destroy(math_utils_ctx_t* ctx);

// 数值钳位
double math_clamp(double v, double min, double max);

// 线性插值
double math_lerp(double a, double b, double t);

// 随机整数 [min, max]
int    math_rand_int(int min, int max);

// 随机浮点数 [0, 1]
double math_rand_double(void);

// 检查是否为 2 的幂
bool   math_is_pow2(size_t n);

// 安全钳位（带错误检查）
// 返回 MATH_UTILS_OK 表示成功，其他值表示错误
math_utils_error_t math_utils_clamp_safe(math_utils_ctx_t* ctx, double v, double min, double max, double* out);

// 安全线性插值（带错误检查）
// 返回 MATH_UTILS_OK 表示成功，其他值表示错误
math_utils_error_t math_utils_lerp_safe(math_utils_ctx_t* ctx, double a, double b, double t, double* out);

// 安全随机整数（带错误检查）
// 返回 MATH_UTILS_OK 表示成功，其他值表示错误
math_utils_error_t math_utils_rand_int_safe(math_utils_ctx_t* ctx, int min, int max, int* out);

// 安全随机浮点数（带错误检查）
// 返回 MATH_UTILS_OK 表示成功，其他值表示错误
math_utils_error_t math_utils_rand_double_safe(math_utils_ctx_t* ctx, double* out);

// 批量随机整数
// 返回 MATH_UTILS_OK 表示成功，其他值表示错误
math_utils_error_t math_utils_rand_int_batch(math_utils_ctx_t* ctx, int min, int max, int* out, size_t count);

// 批量随机浮点数
// 返回 MATH_UTILS_OK 表示成功，其他值表示错误
math_utils_error_t math_utils_rand_double_batch(math_utils_ctx_t* ctx, double* out, size_t count);

// 映射值到新范围
// 返回 MATH_UTILS_OK 表示成功，其他值表示错误
math_utils_error_t math_utils_map_range(math_utils_ctx_t* ctx, double value, double in_min, double in_max, double out_min, double out_max, double* out);

// 平滑步进函数
// 返回 MATH_UTILS_OK 表示成功，其他值表示错误
math_utils_error_t math_utils_smoothstep(math_utils_ctx_t* ctx, double edge0, double edge1, double x, double* out);

// 指数平滑
// 返回 MATH_UTILS_OK 表示成功，其他值表示错误
math_utils_error_t math_utils_exponential_smooth(math_utils_ctx_t* ctx, double current, double previous, double alpha, double* out);

// 计算数组最小值
// 返回 MATH_UTILS_OK 表示成功，其他值表示错误
math_utils_error_t math_utils_array_min(math_utils_ctx_t* ctx, const double* array, size_t size, double* out);

// 计算数组最大值
// 返回 MATH_UTILS_OK 表示成功，其他值表示错误
math_utils_error_t math_utils_array_max(math_utils_ctx_t* ctx, const double* array, size_t size, double* out);

// 计算数组平均值
// 返回 MATH_UTILS_OK 表示成功，其他值表示错误
math_utils_error_t math_utils_array_mean(math_utils_ctx_t* ctx, const double* array, size_t size, double* out);

// 计算数组总和
// 返回 MATH_UTILS_OK 表示成功，其他值表示错误
math_utils_error_t math_utils_array_sum(math_utils_ctx_t* ctx, const double* array, size_t size, double* out);

// 计算数组方差
// 返回 MATH_UTILS_OK 表示成功，其他值表示错误
math_utils_error_t math_utils_array_variance(math_utils_ctx_t* ctx, const double* array, size_t size, double* out);

// 获取 2 的下一个幂
// 返回大于等于 n 的最小 2 的幂
size_t math_utils_next_pow2(size_t n);

// 获取 2 的上一个幂
// 返回小于等于 n 的最大 2 的幂
size_t math_utils_prev_pow2(size_t n);

// 角度转换：度到弧度
double math_utils_deg_to_rad(double deg);

// 角度转换：弧度到度
double math_utils_rad_to_deg(double rad);

// 限制角度在 [0, 360) 范围内
double math_utils_normalize_angle(double deg);

// 重置随机数种子
void math_utils_srand(math_utils_ctx_t* ctx, uint64_t seed);

// 获取最后一次错误
math_utils_error_t math_utils_get_last_error(math_utils_ctx_t* ctx);

// 获取错误信息
const char* math_utils_strerror(math_utils_error_t error);

#endif // C_UTILS_MATH_UTILS_H
