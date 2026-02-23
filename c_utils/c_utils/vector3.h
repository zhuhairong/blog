#ifndef C_UTILS_VECTOR3_H
#define C_UTILS_VECTOR3_H

#include <stddef.h>
#include <stdbool.h>

// 向量 3D 错误码
typedef enum {
    VEC3_OK = 0,
    VEC3_INVALID_PARAMS = -1,
    VEC3_DIVISION_BY_ZERO = -2,
    VEC3_LENGTH_ZERO = -3,
    VEC3_OVERFLOW = -4,
    VEC3_UNDERFLOW = -5
} vec3_error_t;

// 向量 3D 配置选项
typedef struct {
    bool use_epsilon;           // 是否使用epsilon进行比较
    double epsilon;             // 比较精度
    bool check_div_zero;        // 是否检查除零错误
    bool check_length_zero;     // 是否检查长度为零错误
} vec3_config_t;

// 向量 3D 结构体
typedef struct {
    double x, y, z;
} vec3_t;

// 向量 3D 上下文
typedef struct {
    vec3_config_t config;
    vec3_error_t last_error;
    size_t operation_count;
} vec3_ctx_t;

// 创建向量 3D 上下文
vec3_error_t vec3_create(vec3_ctx_t** ctx, const vec3_config_t* config);

// 销毁向量 3D 上下文
void vec3_destroy(vec3_ctx_t* ctx);

// 向量加法
vec3_t vec3_add(vec3_t a, vec3_t b);

// 向量减法
vec3_t vec3_sub(vec3_t a, vec3_t b);

// 点积
double vec3_dot(vec3_t a, vec3_t b);

// 叉积
vec3_t vec3_cross(vec3_t a, vec3_t b);

// 长度计算
double vec3_length(vec3_t v);

// 归一化
vec3_t vec3_normalize(vec3_t v);

// 标量乘法
vec3_t vec3_scale(vec3_t v, double scalar);

// 标量除法
// 返回 VEC3_OK 表示成功，其他值表示错误
vec3_error_t vec3_divide(vec3_ctx_t* ctx, vec3_t v, double scalar, vec3_t* out);

// 距离计算
double vec3_distance(vec3_t a, vec3_t b);

// 平方距离计算
double vec3_distance_squared(vec3_t a, vec3_t b);

// 角度计算（弧度）
double vec3_angle(vec3_t a, vec3_t b);

// 线性插值
vec3_t vec3_lerp(vec3_t a, vec3_t b, double t);

// 球面线性插值
vec3_t vec3_slerp(vec3_t a, vec3_t b, double t);

// 反射
vec3_t vec3_reflect(vec3_t v, vec3_t normal);

// 折射
// 返回 VEC3_OK 表示成功，其他值表示错误
vec3_error_t vec3_refract(vec3_ctx_t* ctx, vec3_t v, vec3_t normal, double eta, vec3_t* out);

// 向量比较
// 返回 true 表示相等，false 表示不相等
bool vec3_equal(vec3_ctx_t* ctx, vec3_t a, vec3_t b);

// 批量向量加法
// 返回 VEC3_OK 表示成功，其他值表示错误
vec3_error_t vec3_add_batch(vec3_ctx_t* ctx, const vec3_t* a, const vec3_t* b, vec3_t* out, size_t count);

// 批量向量缩放
// 返回 VEC3_OK 表示成功，其他值表示错误
vec3_error_t vec3_scale_batch(vec3_ctx_t* ctx, const vec3_t* v, double scalar, vec3_t* out, size_t count);

// 向量设置
vec3_t vec3_set(double x, double y, double z);

// 零向量
vec3_t vec3_zero(void);

// 单位向量
vec3_t vec3_unit_x(void);
vec3_t vec3_unit_y(void);
vec3_t vec3_unit_z(void);

// 获取最后一次错误
vec3_error_t vec3_get_last_error(vec3_ctx_t* ctx);

// 获取错误信息
const char* vec3_strerror(vec3_error_t error);

#endif // C_UTILS_VECTOR3_H
