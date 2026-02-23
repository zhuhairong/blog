#include "pid_controller.h"
#include <string.h>

pid_config_t pid_default_config(void) {
    pid_config_t config = {
        .kp = 1.0,
        .ki = 0.0,
        .kd = 0.0,
        .setpoint = 0.0,
        .output_min = -1000.0,
        .output_max = 1000.0,
        .integral_min = -1000.0,
        .integral_max = 1000.0,
        .derivative_filter = 0.1,
        .anti_windup = true,
        .derivative_on_measurement = false
    };
    return config;
}

void pid_init(pid_controller_t *pid, double kp, double ki, double kd) {
    memset(pid, 0, sizeof(pid_controller_t));
    pid->config.kp = kp;
    pid->config.ki = ki;
    pid->config.kd = kd;
    pid->config.output_min = -1000.0;
    pid->config.output_max = 1000.0;
    pid->config.integral_min = -1000.0;
    pid->config.integral_max = 1000.0;
    pid->config.anti_windup = true;
    pid->state.mode = PID_MODE_AUTO;
}

bool pid_init_ex(pid_controller_t *pid, const pid_config_t *config, pid_error_t *error) {
    if (!pid || !config) {
        if (error) *error = PID_ERROR_NULL_PTR;
        return false;
    }
    memset(pid, 0, sizeof(pid_controller_t));
    pid->config = *config;
    pid->state.mode = PID_MODE_AUTO;
    if (error) *error = PID_OK;
    return true;
}

double pid_update(pid_controller_t *pid, double setpoint, double measured, double dt) {
    if (!pid || dt <= 0) return 0.0;
    
    double error = setpoint - measured;
    pid->state.error = error;
    pid->state.setpoint = setpoint;
    pid->state.measured = measured;
    
    // 比例项
    pid->state.proportional_term = pid->config.kp * error;
    
    // 积分项
    pid->state.integral += error * dt;
    
    // 抗积分饱和
    if (pid->config.anti_windup) {
        if (pid->state.integral > pid->config.integral_max)
            pid->state.integral = pid->config.integral_max;
        else if (pid->state.integral < pid->config.integral_min)
            pid->state.integral = pid->config.integral_min;
    }
    pid->state.integral_term = pid->config.ki * pid->state.integral;
    
    // 微分项
    if (pid->config.derivative_on_measurement) {
        pid->state.derivative = (measured - pid->prev_measured) / dt;
    } else {
        pid->state.derivative = (error - pid->state.prev_error) / dt;
    }
    
    // 微分滤波
    pid->filtered_derivative = pid->config.derivative_filter * pid->state.derivative + 
                               (1.0 - pid->config.derivative_filter) * pid->filtered_derivative;
    pid->state.derivative_term = pid->config.kd * pid->filtered_derivative;
    
    // 计算输出
    double output = pid->state.proportional_term + pid->state.integral_term + pid->state.derivative_term;
    
    // 输出限幅
    if (output > pid->config.output_max) {
        output = pid->config.output_max;
        pid->state.saturated = true;
    } else if (output < pid->config.output_min) {
        output = pid->config.output_min;
        pid->state.saturated = true;
    } else {
        pid->state.saturated = false;
    }
    
    pid->state.output = output;
    pid->state.prev_error = error;
    pid->prev_measured = measured;
    
    return output;
}

double pid_update_ex(pid_controller_t *pid, double setpoint, double measured, double dt, pid_error_t *error) {
    if (!pid) {
        if (error) *error = PID_ERROR_NULL_PTR;
        return 0.0;
    }
    if (dt <= 0) {
        if (error) *error = PID_ERROR_DT_TOO_SMALL;
        return 0.0;
    }
    if (error) *error = PID_OK;
    return pid_update(pid, setpoint, measured, dt);
}

bool pid_set_mode(pid_controller_t *pid, pid_mode_t mode, pid_error_t *error) {
    if (!pid) {
        if (error) *error = PID_ERROR_NULL_PTR;
        return false;
    }
    pid->state.mode = mode;
    if (error) *error = PID_OK;
    return true;
}

pid_mode_t pid_get_mode(const pid_controller_t *pid) {
    return pid ? pid->state.mode : PID_MODE_OFF;
}

bool pid_reset(pid_controller_t *pid, pid_error_t *error) {
    if (!pid) {
        if (error) *error = PID_ERROR_NULL_PTR;
        return false;
    }
    pid->state.integral = 0.0;
    pid->state.prev_error = 0.0;
    pid->prev_measured = 0.0;
    pid->filtered_derivative = 0.0;
    pid->state.saturated = false;
    if (error) *error = PID_OK;
    return true;
}

bool pid_set_tunings(pid_controller_t *pid, double kp, double ki, double kd, pid_error_t *error) {
    if (!pid) {
        if (error) *error = PID_ERROR_NULL_PTR;
        return false;
    }
    pid->config.kp = kp;
    pid->config.ki = ki;
    pid->config.kd = kd;
    if (error) *error = PID_OK;
    return true;
}

void pid_set_setpoint(pid_controller_t *pid, double setpoint) {
    if (pid) pid->config.setpoint = setpoint;
}

bool pid_set_output_limits(pid_controller_t *pid, double min, double max, pid_error_t *error) {
    if (!pid) {
        if (error) *error = PID_ERROR_NULL_PTR;
        return false;
    }
    pid->config.output_min = min;
    pid->config.output_max = max;
    if (error) *error = PID_OK;
    return true;
}

const char* pid_strerror(pid_error_t error) {
    switch (error) {
        case PID_OK: return "Success";
        case PID_ERROR_NULL_PTR: return "Null pointer";
        case PID_ERROR_INVALID_ARGS: return "Invalid arguments";
        case PID_ERROR_DT_TOO_SMALL: return "Time step too small";
        case PID_ERROR_SATURATION: return "Saturation";
        default: return "Unknown error";
    }
}
