#ifndef C_UTILS_PID_CONTROLLER_H
#define C_UTILS_PID_CONTROLLER_H

#include <stddef.h>
#include <stdbool.h>

/**
 * @brief PID 控制器错误码
 */
typedef enum {
    PID_OK = 0,                  /**< 成功 */
    PID_ERROR_NULL_PTR,          /**< 空指针错误 */
    PID_ERROR_INVALID_ARGS,      /**< 无效参数 */
    PID_ERROR_DT_TOO_SMALL,      /**< 时间步长过小 */
    PID_ERROR_SATURATION,        /**< 饱和错误 */
    PID_ERROR_MAX                /**< 最大错误码 */
} pid_error_t;

/**
 * @brief PID 控制器模式
 */
typedef enum {
    PID_MODE_AUTO = 0,           /**< 自动模式 */
    PID_MODE_MANUAL,             /**< 手动模式 */
    PID_MODE_OFF                 /**< 关闭模式 */
} pid_mode_t;

/**
 * @brief PID 控制器配置
 */
typedef struct {
    double kp;                   /**< 比例增益 */
    double ki;                   /**< 积分增益 */
    double kd;                   /**< 微分增益 */
    double setpoint;             /**< 设定值 */
    double output_min;           /**< 输出最小值 */
    double output_max;           /**< 输出最大值 */
    double integral_min;         /**< 积分最小值 */
    double integral_max;         /**< 积分最大值 */
    double derivative_filter;    /**< 微分滤波器系数 (0-1) */
    bool anti_windup;            /**< 是否启用抗积分饱和 */
    bool derivative_on_measurement; /**< 是否对测量值进行微分 */
} pid_config_t;

/**
 * @brief PID 控制器状态
 */
typedef struct {
    double setpoint;             /**< 当前设定值 */
    double measured;             /**< 当前测量值 */
    double error;                /**< 当前误差 */
    double prev_error;           /**< 上一次误差 */
    double integral;             /**< 积分值 */
    double derivative;           /**< 微分值 */
    double output;               /**< 输出值 */
    double proportional_term;    /**< 比例项 */
    double integral_term;        /**< 积分项 */
    double derivative_term;      /**< 微分项 */
    pid_mode_t mode;             /**< 控制器模式 */
    bool saturated;              /**< 是否饱和 */
} pid_state_t;

/**
 * @brief PID 控制器结构
 */
typedef struct {
    pid_config_t config;         /**< 配置 */
    pid_state_t state;           /**< 状态 */
    double prev_measured;        /**< 上一次测量值 */
    double filtered_derivative;  /**< 滤波后的微分值 */
} pid_controller_t;

/**
 * @brief 获取默认配置
 * @return 默认配置
 */
pid_config_t pid_default_config(void);

/**
 * @brief 初始化 PID 控制器
 * @param pid PID 控制器指针
 * @param kp 比例增益
 * @param ki 积分增益
 * @param kd 微分增益
 */
void pid_init(pid_controller_t *pid, double kp, double ki, double kd);

/**
 * @brief 使用自定义配置初始化 PID 控制器
 * @param pid PID 控制器指针
 * @param config 配置选项
 * @param error 错误码输出
 * @return 是否成功
 */
bool pid_init_ex(pid_controller_t *pid, const pid_config_t *config, pid_error_t *error);

/**
 * @brief 更新 PID 控制器
 * @param pid PID 控制器指针
 * @param setpoint 设定值
 * @param measured 测量值
 * @param dt 时间步长
 * @return 输出值
 */
double pid_update(pid_controller_t *pid, double setpoint, double measured, double dt);

/**
 * @brief 更新 PID 控制器（带错误码）
 * @param pid PID 控制器指针
 * @param setpoint 设定值
 * @param measured 测量值
 * @param dt 时间步长
 * @param error 错误码输出
 * @return 输出值
 */
double pid_update_ex(pid_controller_t *pid, double setpoint, double measured, double dt, pid_error_t *error);

/**
 * @brief 设置 PID 控制器模式
 * @param pid PID 控制器指针
 * @param mode 模式
 * @param error 错误码输出
 * @return 是否成功
 */
bool pid_set_mode(pid_controller_t *pid, pid_mode_t mode, pid_error_t *error);

/**
 * @brief 设置 PID 控制器参数
 * @param pid PID 控制器指针
 * @param kp 比例增益
 * @param ki 积分增益
 * @param kd 微分增益
 * @param error 错误码输出
 * @return 是否成功
 */
bool pid_set_tunings(pid_controller_t *pid, double kp, double ki, double kd, pid_error_t *error);

/**
 * @brief 设置 PID 控制器输出范围
 * @param pid PID 控制器指针
 * @param min 最小值
 * @param max 最大值
 * @param error 错误码输出
 * @return 是否成功
 */
bool pid_set_output_limits(pid_controller_t *pid, double min, double max, pid_error_t *error);

/**
 * @brief 设置 PID 控制器积分范围
 * @param pid PID 控制器指针
 * @param min 最小值
 * @param max 最大值
 * @param error 错误码输出
 * @return 是否成功
 */
bool pid_set_integral_limits(pid_controller_t *pid, double min, double max, pid_error_t *error);

/**
 * @brief 重置 PID 控制器状态
 * @param pid PID 控制器指针
 * @param error 错误码输出
 * @return 是否成功
 */
bool pid_reset(pid_controller_t *pid, pid_error_t *error);

/**
 * @brief 获取 PID 控制器状态
 * @param pid PID 控制器指针
 * @param state 状态输出
 * @param error 错误码输出
 * @return 是否成功
 */
bool pid_get_state(const pid_controller_t *pid, pid_state_t *state, pid_error_t *error);

/**
 * @brief 获取 PID 控制器配置
 * @param pid PID 控制器指针
 * @param config 配置输出
 * @param error 错误码输出
 * @return 是否成功
 */
bool pid_get_config(const pid_controller_t *pid, pid_config_t *config, pid_error_t *error);

/**
 * @brief 计算 PID 控制器的闭环响应
 * @param pid PID 控制器指针
 * @param setpoint 设定值
 * @param measured 测量值
 * @param dt 时间步长
 * @param response 响应输出
 * @param error 错误码输出
 * @return 是否成功
 */
bool pid_calculate_response(pid_controller_t *pid, double setpoint, double measured, double dt, double *response, pid_error_t *error);

/**
 * @brief 获取错误信息
 * @param error 错误码
 * @return 错误信息字符串
 */
const char* pid_error_string(pid_error_t error);

#endif // C_UTILS_PID_CONTROLLER_H
