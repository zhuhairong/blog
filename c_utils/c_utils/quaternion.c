#include "quaternion.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

/* =========================================================================
 * 默认配置与创建
 * ========================================================================= */

quat_config_t quat_default_config(void) {
    return (quat_config_t){
        .normalize_result = false,
        .check_validity = true,
        .epsilon = 1e-9
    };
}

quat_t quat_create(double w, double x, double y, double z) {
    return (quat_t){w, x, y, z};
}

/* =========================================================================
 * 基本运算
 * ========================================================================= */

quat_t quat_add(quat_t a, quat_t b) {
    return (quat_t){a.w + b.w, a.x + b.x, a.y + b.y, a.z + b.z};
}

quat_t quat_sub(quat_t a, quat_t b) {
    return (quat_t){a.w - b.w, a.x - b.x, a.y - b.y, a.z - b.z};
}

quat_t quat_mul(quat_t a, quat_t b) {
    return (quat_t){
        a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z,
        a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y,
        a.w*b.y - a.x*b.z + a.y*b.w + a.z*b.x,
        a.w*b.z + a.x*b.y - a.y*b.x + a.z*b.w
    };
}

quat_t quat_mul_scalar(quat_t q, double scalar) {
    return (quat_t){q.w * scalar, q.x * scalar, q.y * scalar, q.z * scalar};
}

quat_t quat_div_scalar(quat_t q, double scalar, quat_error_t *error) {
    if (fabs(scalar) < 1e-12) {
        if (error) *error = QUAT_ERROR_DIVISION_BY_ZERO;
        return (quat_t){0, 0, 0, 0};
    }
    if (error) *error = QUAT_OK;
    return (quat_t){q.w / scalar, q.x / scalar, q.y / scalar, q.z / scalar};
}

/* =========================================================================
 * 共轭、逆、长度
 * ========================================================================= */

quat_t quat_conjugate(quat_t q) {
    return (quat_t){q.w, -q.x, -q.y, -q.z};
}

quat_t quat_inverse(quat_t q, quat_error_t *error) {
    double len_sq = q.w*q.w + q.x*q.x + q.y*q.y + q.z*q.z;
    if (len_sq < 1e-12) {
        if (error) *error = QUAT_ERROR_DIVISION_BY_ZERO;
        return (quat_t){0, 0, 0, 0};
    }
    if (error) *error = QUAT_OK;
    quat_t conj = quat_conjugate(q);
    return quat_div_scalar(conj, len_sq, NULL);
}

double quat_length(quat_t q) {
    return sqrt(q.w*q.w + q.x*q.x + q.y*q.y + q.z*q.z);
}

double quat_length_squared(quat_t q) {
    return q.w*q.w + q.x*q.x + q.y*q.y + q.z*q.z;
}

/* =========================================================================
 * 归一化
 * ========================================================================= */

quat_t quat_normalize(quat_t q) {
    double mag = sqrt(q.w*q.w + q.x*q.x + q.y*q.y + q.z*q.z);
    if (mag < 1e-12) return (quat_t){1, 0, 0, 0};
    return (quat_t){q.w/mag, q.x/mag, q.y/mag, q.z/mag};
}

quat_t quat_normalize_ex(quat_t q, const quat_config_t *config, quat_error_t *error) {
    (void)config;
    double mag = sqrt(q.w*q.w + q.x*q.x + q.y*q.y + q.z*q.z);
    if (mag < 1e-12) {
        if (error) *error = QUAT_ERROR_DIVISION_BY_ZERO;
        return (quat_t){1, 0, 0, 0};
    }
    if (error) *error = QUAT_OK;
    return (quat_t){q.w/mag, q.x/mag, q.y/mag, q.z/mag};
}

/* =========================================================================
 * 点积、距离
 * ========================================================================= */

double quat_dot(quat_t a, quat_t b) {
    return a.w*b.w + a.x*b.x + a.y*b.y + a.z*b.z;
}

double quat_distance(quat_t a, quat_t b) {
    quat_t diff = quat_sub(a, b);
    return quat_length(diff);
}

/* =========================================================================
 * 插值：SLERP 和 LERP
 * ========================================================================= */

quat_t quat_slerp(quat_t a, quat_t b, double t, quat_error_t *error) {
    // 归一化输入
    a = quat_normalize(a);
    b = quat_normalize(b);

    double dot = quat_dot(a, b);

    // 如果点积为负，取反一个四元数以走最短路径
    if (dot < 0.0) {
        b = (quat_t){-b.w, -b.x, -b.y, -b.z};
        dot = -dot;
    }

    // 如果非常接近，使用线性插值避免除零
    double theta = acos(dot > 1.0 ? 1.0 : dot);
    double sin_theta = sin(theta);

    if (sin_theta < 1e-6) {
        // 接近平行，退化为 LERP
        if (error) *error = QUAT_OK;
        quat_t result = quat_lerp(a, b, t);
        return quat_normalize(result);
    }

    double s0 = sin((1.0 - t) * theta) / sin_theta;
    double s1 = sin(t * theta) / sin_theta;

    if (error) *error = QUAT_OK;
    return (quat_t){
        s0 * a.w + s1 * b.w,
        s0 * a.x + s1 * b.x,
        s0 * a.y + s1 * b.y,
        s0 * a.z + s1 * b.z
    };
}

quat_t quat_lerp(quat_t a, quat_t b, double t) {
    return (quat_t){
        a.w + t * (b.w - a.w),
        a.x + t * (b.x - a.x),
        a.y + t * (b.y - a.y),
        a.z + t * (b.z - a.z)
    };
}

/* =========================================================================
 * 轴角表示
 * ========================================================================= */

quat_t quat_from_axis_angle(vec3_t axis, double angle) {
    double half_angle = angle * 0.5;
    double s = sin(half_angle);

    // 归一化轴
    double len = sqrt(axis.x*axis.x + axis.y*axis.y + axis.z*axis.z);
    if (len > 1e-12) {
        s /= len;
    }

    return (quat_t){cos(half_angle), axis.x * s, axis.y * s, axis.z * s};
}

bool quat_to_axis_angle(quat_t q, vec3_t *axis, double *angle, quat_error_t *error) {
    if (!axis || !angle) {
        if (error) *error = QUAT_ERROR_NULL_PTR;
        return false;
    }

    q = quat_normalize(q);
    double s = sqrt(1.0 - q.w * q.w);  // sin(angle/2)

    if (s < 1e-10) {
        // 角度为 0，任意轴
        axis->x = 1; axis->y = 0; axis->z = 0;
        *angle = 0.0;
    } else {
        axis->x = q.x / s;
        axis->y = q.y / s;
        axis->z = q.z / s;
        *angle = 2.0 * acos(q.w);
    }

    if (error) *error = QUAT_OK;
    return true;
}

/* =========================================================================
 * 欧拉角转换（ZYX 内旋次序）
 * ========================================================================= */

quat_t quat_from_euler(euler_t euler) {
    double cr = cos(euler.roll * 0.5);
    double sr = sin(euler.roll * 0.5);
    double cp = cos(euler.pitch * 0.5);
    double sp = sin(euler.pitch * 0.5);
    double cy = cos(euler.yaw * 0.5);
    double sy = sin(euler.yaw * 0.5);

    return (quat_t){
        cr * cp * cy + sr * sp * sy,
        sr * cp * cy - cr * sp * sy,
        cr * sp * cy + sr * cp * sy,
        cr * cp * sy - sr * sp * cy
    };
}

bool quat_to_euler(quat_t q, euler_t *euler) {
    if (!euler) return false;

    q = quat_normalize(q);

    // 计算 roll (X), pitch (Y), yaw (Z) — ZYX 内旋
    double sinr_cosp = 2.0 * (q.w * q.x + q.y * q.z);
    double cosr_cosp = 1.0 - 2.0 * (q.x * q.x + q.y * q.y);
    euler->roll = atan2(sinr_cosp, cosr_cosp);

    double sinp = 2.0 * (q.w * q.y - q.z * q.x);
    if (fabs(sinp) >= 1.0) {
        euler->pitch = copysign(M_PI / 2.0, sinp);
    } else {
        euler->pitch = asin(sinp);
    }

    double siny_cosp = 2.0 * (q.w * q.z + q.x * q.y);
    double cosy_cosp = 1.0 - 2.0 * (q.y * q.y + q.z * q.z);
    euler->yaw = atan2(siny_cosp, cosy_cosp);

    return true;
}

/* =========================================================================
 * 旋转矩阵转换
 * ========================================================================= */

quat_t quat_from_matrix(mat4_t mat) {
    double trace = mat.m[0][0] + mat.m[1][1] + mat.m[2][2];
    quat_t q;

    if (trace > 0.0) {
        double s = 0.5 / sqrt(trace + 1.0);
        q.w = 0.25 / s;
        q.x = (mat.m[2][1] - mat.m[1][2]) * s;
        q.y = (mat.m[0][2] - mat.m[2][0]) * s;
        q.z = (mat.m[1][0] - mat.m[0][1]) * s;
    } else if (mat.m[0][0] > mat.m[1][1] && mat.m[0][0] > mat.m[2][2]) {
        double s = 2.0 * sqrt(1.0 + mat.m[0][0] - mat.m[1][1] - mat.m[2][2]);
        q.w = (mat.m[2][1] - mat.m[1][2]) / s;
        q.x = 0.25 * s;
        q.y = (mat.m[0][1] + mat.m[1][0]) / s;
        q.z = (mat.m[0][2] + mat.m[2][0]) / s;
    } else if (mat.m[1][1] > mat.m[2][2]) {
        double s = 2.0 * sqrt(1.0 + mat.m[1][1] - mat.m[0][0] - mat.m[2][2]);
        q.w = (mat.m[0][2] - mat.m[2][0]) / s;
        q.x = (mat.m[0][1] + mat.m[1][0]) / s;
        q.y = 0.25 * s;
        q.z = (mat.m[1][2] + mat.m[2][1]) / s;
    } else {
        double s = 2.0 * sqrt(1.0 + mat.m[2][2] - mat.m[0][0] - mat.m[1][1]);
        q.w = (mat.m[1][0] - mat.m[0][1]) / s;
        q.x = (mat.m[0][2] + mat.m[2][0]) / s;
        q.y = (mat.m[1][2] + mat.m[2][1]) / s;
        q.z = 0.25 * s;
    }

    return quat_normalize(q);
}

bool quat_to_matrix(quat_t q, mat4_t *mat) {
    if (!mat) return false;

    q = quat_normalize(q);
    double w = q.w, x = q.x, y = q.y, z = q.z;

    double xx = x*x, yy = y*y, zz = z*z;
    double xy = x*y, xz = x*z, yz = y*z;
    double wx = w*x, wy = w*y, wz = w*z;

    memset(mat, 0, sizeof(mat4_t));
    mat->m[0][0] = 1.0 - 2.0*(yy + zz);
    mat->m[0][1] = 2.0*(xy - wz);
    mat->m[0][2] = 2.0*(xz + wy);
    mat->m[1][0] = 2.0*(xy + wz);
    mat->m[1][1] = 1.0 - 2.0*(xx + zz);
    mat->m[1][2] = 2.0*(yz - wx);
    mat->m[2][0] = 2.0*(xz - wy);
    mat->m[2][1] = 2.0*(yz + wx);
    mat->m[2][2] = 1.0 - 2.0*(xx + yy);
    mat->m[3][3] = 1.0;

    return true;
}

/* =========================================================================
 * 向量旋转
 * ========================================================================= */

vec3_t quat_rotate_vector(quat_t q, vec3_t v) {
    q = quat_normalize(q);

    // q * v_quat * q_conj
    quat_t vq = {0, v.x, v.y, v.z};
    quat_t q_conj = quat_conjugate(q);
    quat_t result = quat_mul(quat_mul(q, vq), q_conj);

    return (vec3_t){result.x, result.y, result.z};
}

bool quat_rotate_point(quat_t q, double x, double y, double z,
                       double *x_out, double *y_out, double *z_out) {
    if (!x_out || !y_out || !z_out) return false;

    vec3_t rotated = quat_rotate_vector(q, (vec3_t){x, y, z});
    *x_out = rotated.x;
    *y_out = rotated.y;
    *z_out = rotated.z;
    return true;
}

/* =========================================================================
 * 有效性检查与比较
 * ========================================================================= */

bool quat_is_valid(quat_t q, const quat_config_t *config) {
    double eps = config ? config->epsilon : 1e-9;

    if (isnan(q.w) || isnan(q.x) || isnan(q.y) || isnan(q.z))
        return false;
    if (isinf(q.w) || isinf(q.x) || isinf(q.y) || isinf(q.z))
        return false;

    double len_sq = quat_length_squared(q);
    if (len_sq < eps)
        return false;

    return true;
}

bool quat_is_unit(quat_t q, double epsilon) {
    double len_sq = quat_length_squared(q);
    return fabs(len_sq - 1.0) < epsilon;
}

bool quat_equal(quat_t a, quat_t b, double epsilon) {
    return fabs(a.w - b.w) < epsilon &&
           fabs(a.x - b.x) < epsilon &&
           fabs(a.y - b.y) < epsilon &&
           fabs(a.z - b.z) < epsilon;
}

/* =========================================================================
 * 打印与错误信息
 * ========================================================================= */

void quat_print(quat_t q) {
    printf("(%.6f, %.6f, %.6f, %.6f)\n", q.w, q.x, q.y, q.z);
}

const char* quat_error_string(quat_error_t error) {
    switch (error) {
        case QUAT_OK:               return "Success";
        case QUAT_ERROR_NULL_PTR:   return "NULL pointer parameter";
        case QUAT_ERROR_INVALID_ARGS: return "Invalid arguments";
        case QUAT_ERROR_DIVISION_BY_ZERO: return "Division by zero";
        case QUAT_ERROR_NAN:        return "Not a number (NaN)";
        case QUAT_ERROR_INFINITY:   return "Infinity";
        case QUAT_ERROR_MAX:        return "Maximum error code";
        default:                    return "Unknown error";
    }
}
