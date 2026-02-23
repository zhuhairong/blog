#ifndef C_UTILS_COMPLEX_H
#define C_UTILS_COMPLEX_H

#include <stdbool.h>

// 复数类型
typedef struct {
    double real;
    double imag;
} complex_t;

// 复数错误码
typedef enum {
    COMPLEX_OK = 0,
    COMPLEX_ERROR_DIVISION_BY_ZERO,
    COMPLEX_ERROR_INVALID_PARAM,
    COMPLEX_ERROR_OVERFLOW
} complex_error_t;

// 复数加法
// a: 第一个复数
// b: 第二个复数
// 返回: 和
complex_t complex_add(complex_t a, complex_t b);

// 复数减法
// a: 被减数
// b: 减数
// 返回: 差
complex_t complex_sub(complex_t a, complex_t b);

// 复数乘法
// a: 第一个复数
// b: 第二个复数
// 返回: 积
complex_t complex_mul(complex_t a, complex_t b);

// 复数除法
// a: 被除数
// b: 除数
// error: 错误码（输出参数，可为 NULL）
// 返回: 商
complex_t complex_div(complex_t a, complex_t b, complex_error_t *error);

// 复数绝对值（模）
// a: 复数
// 返回: 模
double complex_abs(complex_t a);

// 复数共轭
// a: 复数
// 返回: 共轭复数
complex_t complex_conj(complex_t a);

// 复数角度（辐角）
// a: 复数
// 返回: 辐角（弧度）
double complex_arg(complex_t a);

// 复数指数
// a: 复数
// 返回: e^a
complex_t complex_exp(complex_t a);

// 复数对数（自然对数）
// a: 复数
// error: 错误码（输出参数，可为 NULL）
// 返回: ln(a)
complex_t complex_log(complex_t a, complex_error_t *error);

// 复数幂
// a: 底数
// b: 指数
// error: 错误码（输出参数，可为 NULL）
// 返回: a^b
complex_t complex_pow(complex_t a, complex_t b, complex_error_t *error);

// 复数平方根
// a: 复数
// 返回: 平方根
complex_t complex_sqrt(complex_t a);

// 复数正弦
// a: 复数
// 返回: sin(a)
complex_t complex_sin(complex_t a);

// 复数余弦
// a: 复数
// 返回: cos(a)
complex_t complex_cos(complex_t a);

// 复数正切
// a: 复数
// error: 错误码（输出参数，可为 NULL）
// 返回: tan(a)
complex_t complex_tan(complex_t a, complex_error_t *error);

// 从极坐标创建复数
// r: 模
// theta: 辐角（弧度）
// 返回: 复数
complex_t complex_polar(double r, double theta);

// 检查复数是否为实数
// a: 复数
// 返回: 是实数返回 true，否则返回 false
bool complex_is_real(complex_t a);

// 检查复数是否为虚数
// a: 复数
// 返回: 是虚数返回 true，否则返回 false
bool complex_is_imag(complex_t a);

// 检查两个复数是否相等
// a: 第一个复数
// b: 第二个复数
// epsilon: 误差容限（可为 0 使用默认值）
// 返回: 相等返回 true，否则返回 false
bool complex_equal(complex_t a, complex_t b, double epsilon);

// 获取错误信息
// error: 错误码
// 返回: 错误信息字符串
const char* complex_strerror(complex_error_t error);

// 预定义常量
#define COMPLEX_I ((complex_t){0.0, 1.0})
#define COMPLEX_ZERO ((complex_t){0.0, 0.0})
#define COMPLEX_ONE ((complex_t){1.0, 0.0})
#define COMPLEX_NEGATIVE_ONE ((complex_t){-1.0, 0.0})

#endif // C_UTILS_COMPLEX_H
