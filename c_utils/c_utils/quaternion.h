#ifndef C_UTILS_QUATERNION_H
#define C_UTILS_QUATERNION_H

#include <stddef.h>
#include <stdbool.h>
#include <math.h>

/**
 * @brief 四元数错误码
 */
typedef enum {
    QUAT_OK = 0,                  /**< 成功 */
    QUAT_ERROR_NULL_PTR,          /**< 空指针错误 */
    QUAT_ERROR_INVALID_ARGS,      /**< 无效参数 */
    QUAT_ERROR_DIVISION_BY_ZERO,  /**< 除以零错误 */
    QUAT_ERROR_NAN,               /**< 非数字错误 */
    QUAT_ERROR_INFINITY,          /**< 无穷大错误 */
    QUAT_ERROR_MAX                /**< 最大错误码 */
} quat_error_t;

/**
 * @brief 四元数配置
 */
typedef struct {
    bool normalize_result;         /**< 是否归一化结果 */
    bool check_validity;           /**< 是否检查有效性 */
    double epsilon;                /**< 精度阈值 */
} quat_config_t;

/**
 * @brief 四元数结构
 */
typedef struct { double w, x, y, z; } quat_t;

/**
 * @brief 向量结构
 */
typedef struct { double x, y, z; } vec3_t;

/**
 * @brief 欧拉角结构
 */
typedef struct {
    double roll;                   /**< 滚转角（X轴，弧度） */
    double pitch;                  /**< 俯仰角（Y轴，弧度） */
    double yaw;                    /**< 偏航角（Z轴，弧度） */
} euler_t;

/**
 * @brief 旋转矩阵结构
 */
typedef struct {
    double m[4][4];                /**< 4x4 旋转矩阵 */
} mat4_t;

/**
 * @brief 获取默认配置
 * @return 默认配置
 */
quat_config_t quat_default_config(void);

/**
 * @brief 创建四元数
 * @param w 实部
 * @param x 虚部 x
 * @param y 虚部 y
 * @param z 虚部 z
 * @return 四元数
 */
quat_t quat_create(double w, double x, double y, double z);

/**
 * @brief 四元数加法
 * @param a 第一个四元数
 * @param b 第二个四元数
 * @return 结果四元数
 */
quat_t quat_add(quat_t a, quat_t b);

/**
 * @brief 四元数减法
 * @param a 第一个四元数
 * @param b 第二个四元数
 * @return 结果四元数
 */
quat_t quat_sub(quat_t a, quat_t b);

/**
 * @brief 四元数乘法
 * @param a 第一个四元数
 * @param b 第二个四元数
 * @return 结果四元数
 */
quat_t quat_mul(quat_t a, quat_t b);

/**
 * @brief 四元数标量乘法
 * @param q 四元数
 * @param scalar 标量
 * @return 结果四元数
 */
quat_t quat_mul_scalar(quat_t q, double scalar);

/**
 * @brief 四元数标量除法
 * @param q 四元数
 * @param scalar 标量
 * @param error 错误码输出
 * @return 结果四元数
 */
quat_t quat_div_scalar(quat_t q, double scalar, quat_error_t *error);

/**
 * @brief 四元数归一化
 * @param q 四元数
 * @return 归一化后的四元数
 */
quat_t quat_normalize(quat_t q);

/**
 * @brief 四元数归一化（带错误处理）
 * @param q 四元数
 * @param config 配置选项
 * @param error 错误码输出
 * @return 归一化后的四元数
 */
quat_t quat_normalize_ex(quat_t q, const quat_config_t *config, quat_error_t *error);

/**
 * @brief 四元数共轭
 * @param q 四元数
 * @return 共轭四元数
 */
quat_t quat_conjugate(quat_t q);

/**
 * @brief 四元数逆
 * @param q 四元数
 * @param error 错误码输出
 * @return 逆四元数
 */
quat_t quat_inverse(quat_t q, quat_error_t *error);

/**
 * @brief 四元数长度
 * @param q 四元数
 * @return 长度
 */
double quat_length(quat_t q);

/**
 * @brief 四元数长度平方
 * @param q 四元数
 * @return 长度平方
 */
double quat_length_squared(quat_t q);

/**
 * @brief 四元数点积
 * @param a 第一个四元数
 * @param b 第二个四元数
 * @return 点积
 */
double quat_dot(quat_t a, quat_t b);

/**
 * @brief 四元数距离
 * @param a 第一个四元数
 * @param b 第二个四元数
 * @return 距离
 */
double quat_distance(quat_t a, quat_t b);

/**
 * @brief 四元数球面线性插值
 * @param a 第一个四元数
 * @param b 第二个四元数
 * @param t 插值参数 (0-1)
 * @param error 错误码输出
 * @return 插值后的四元数
 */
quat_t quat_slerp(quat_t a, quat_t b, double t, quat_error_t *error);

/**
 * @brief 四元数线性插值
 * @param a 第一个四元数
 * @param b 第二个四元数
 * @param t 插值参数 (0-1)
 * @return 插值后的四元数
 */
quat_t quat_lerp(quat_t a, quat_t b, double t);

/**
 * @brief 从轴角创建四元数
 * @param axis 旋转轴
 * @param angle 旋转角度（弧度）
 * @return 四元数
 */
quat_t quat_from_axis_angle(vec3_t axis, double angle);

/**
 * @brief 从欧拉角创建四元数
 * @param euler 欧拉角
 * @return 四元数
 */
quat_t quat_from_euler(euler_t euler);

/**
 * @brief 从旋转矩阵创建四元数
 * @param mat 旋转矩阵
 * @return 四元数
 */
quat_t quat_from_matrix(mat4_t mat);

/**
 * @brief 四元数转换为轴角
 * @param q 四元数
 * @param axis 旋转轴输出
 * @param angle 旋转角度输出（弧度）
 * @param error 错误码输出
 * @return 是否成功
 */
bool quat_to_axis_angle(quat_t q, vec3_t *axis, double *angle, quat_error_t *error);

/**
 * @brief 四元数转换为欧拉角
 * @param q 四元数
 * @param euler 欧拉角输出
 * @return 是否成功
 */
bool quat_to_euler(quat_t q, euler_t *euler);

/**
 * @brief 四元数转换为旋转矩阵
 * @param q 四元数
 * @param mat 旋转矩阵输出
 * @return 是否成功
 */
bool quat_to_matrix(quat_t q, mat4_t *mat);

/**
 * @brief 四元数旋转向量
 * @param q 四元数
 * @param v 向量
 * @return 旋转后的向量
 */
vec3_t quat_rotate_vector(quat_t q, vec3_t v);

/**
 * @brief 四元数旋转点
 * @param q 四元数
 * @param x x坐标
 * @param y y坐标
 * @param z z坐标
 * @param x_out x坐标输出
 * @param y_out y坐标输出
 * @param z_out z坐标输出
 * @return 是否成功
 */
bool quat_rotate_point(quat_t q, double x, double y, double z, double *x_out, double *y_out, double *z_out);

/**
 * @brief 检查四元数是否有效
 * @param q 四元数
 * @param config 配置选项
 * @return 是否有效
 */
bool quat_is_valid(quat_t q, const quat_config_t *config);

/**
 * @brief 检查四元数是否为单位四元数
 * @param q 四元数
 * @param epsilon 精度阈值
 * @return 是否为单位四元数
 */
bool quat_is_unit(quat_t q, double epsilon);

/**
 * @brief 四元数比较
 * @param a 第一个四元数
 * @param b 第二个四元数
 * @param epsilon 精度阈值
 * @return 是否相等
 */
bool quat_equal(quat_t a, quat_t b, double epsilon);

/**
 * @brief 四元数打印
 * @param q 四元数
 */
void quat_print(quat_t q);

/**
 * @brief 获取错误信息
 * @param error 错误码
 * @return 错误信息字符串
 */
const char* quat_error_string(quat_error_t error);

#endif // C_UTILS_QUATERNION_H
