#ifndef C_UTILS_RETRY_H
#define C_UTILS_RETRY_H

#include <stdbool.h>
#include <stddef.h>
#include <time.h>

/**
 * @brief 重试策略错误码
 */
typedef enum {
    RETRY_OK = 0,                  /**< 成功 */
    RETRY_ERROR_NULL_PTR,          /**< 空指针错误 */
    RETRY_ERROR_INVALID_ARGS,      /**< 无效参数 */
    RETRY_ERROR_MAX_ATTEMPTS_REACHED, /**< 达到最大尝试次数 */
    RETRY_ERROR_DELAY_TOO_LONG,    /**< 延迟过长 */
    RETRY_ERROR_TIMEOUT,           /**< 超时 */
    RETRY_ERROR_MAX                /**< 最大错误码 */
} retry_error_t;

/**
 * @brief 重试延迟策略
 */
typedef enum {
    RETRY_DELAY_CONSTANT = 0,       /**< 恒定延迟 */
    RETRY_DELAY_LINEAR,            /**< 线性延迟 */
    RETRY_DELAY_EXPONENTIAL,       /**< 指数延迟 */
    RETRY_DELAY_JITTER,            /**< 抖动延迟 */
    RETRY_DELAY_MAX                /**< 最大延迟策略 */
} retry_delay_t;

/**
 * @brief 重试策略配置
 */
typedef struct {
    int max_attempts;              /**< 最大尝试次数 */
    int base_delay_ms;             /**< 基础延迟（毫秒） */
    int max_delay_ms;              /**< 最大延迟（毫秒） */
    int max_total_time_ms;         /**< 最大总时间（毫秒） */
    retry_delay_t delay_strategy;  /**< 延迟策略 */
    double jitter_factor;          /**< 抖动因子 (0.0-1.0) */
    double backoff_factor;         /**< 退避因子 */
    bool exponential_backoff;      /**< 是否指数退避 */
    bool use_monotonic_time;       /**< 是否使用单调时间 */
} retry_config_t;

/**
 * @brief 重试策略状态
 */
typedef struct {
    int current_attempt;           /**< 当前尝试次数 */
    int last_delay_ms;             /**< 上次延迟（毫秒） */
    int next_delay_ms;             /**< 下次延迟（毫秒） */
    int total_delay_ms;            /**< 总延迟（毫秒） */
    time_t start_time;             /**< 开始时间 */
    bool exhausted;                /**< 是否已耗尽 */
    bool timed_out;                /**< 是否已超时 */
} retry_state_t;

/**
 * @brief 重试策略结构
 */
typedef struct {
    retry_config_t config;         /**< 配置 */
    retry_state_t state;           /**< 状态 */
    time_t start_time;             /**< 开始时间 */
} retry_policy_t;

/**
 * @brief 重试回调函数类型
 */
typedef bool (*retry_callback_t)(void *user_data, int attempt, retry_error_t *error);

/**
 * @brief 获取默认配置
 * @return 默认配置
 */
retry_config_t retry_default_config(void);

/**
 * @brief 初始化重试策略
 * @param policy 重试策略
 * @param max_attempts 最大尝试次数
 * @param base_delay_ms 基础延迟（毫秒）
 */
void retry_init(retry_policy_t *policy, int max_attempts, int base_delay_ms);

/**
 * @brief 使用自定义配置初始化重试策略
 * @param policy 重试策略
 * @param config 配置选项
 * @param error 错误码输出
 * @return 是否成功
 */
bool retry_init_ex(retry_policy_t *policy, const retry_config_t *config, retry_error_t *error);

/**
 * @brief 检查是否应该继续重试
 * @param policy 重试策略
 * @return 如果返回 true，则自动进行 sleep 等待
 */
bool retry_next(retry_policy_t *policy);

/**
 * @brief 检查是否应该继续重试（带错误处理）
 * @param policy 重试策略
 * @param error 错误码输出
 * @return 如果返回 true，则自动进行 sleep 等待
 */
bool retry_next_ex(retry_policy_t *policy, retry_error_t *error);

/**
 * @brief 执行带重试的操作
 * @param policy 重试策略
 * @param callback 回调函数
 * @param user_data 用户数据
 * @param error 错误码输出
 * @return 是否成功
 */
bool retry_execute(retry_policy_t *policy, retry_callback_t callback, void *user_data, retry_error_t *error);

/**
 * @brief 重置重试策略
 * @param policy 重试策略
 * @param error 错误码输出
 * @return 是否成功
 */
bool retry_reset(retry_policy_t *policy, retry_error_t *error);

/**
 * @brief 获取重试策略状态
 * @param policy 重试策略
 * @param state 状态输出
 * @param error 错误码输出
 * @return 是否成功
 */
bool retry_get_state(const retry_policy_t *policy, retry_state_t *state, retry_error_t *error);

/**
 * @brief 计算下次延迟时间
 * @param policy 重试策略
 * @param next_delay_ms 下次延迟输出（毫秒）
 * @param error 错误码输出
 * @return 是否成功
 */
bool retry_calculate_next_delay(const retry_policy_t *policy, int *next_delay_ms, retry_error_t *error);

/**
 * @brief 检查是否达到最大尝试次数
 * @param policy 重试策略
 * @return 是否已达到最大尝试次数
 */
bool retry_is_exhausted(const retry_policy_t *policy);

/**
 * @brief 检查是否超时
 * @param policy 重试策略
 * @return 是否已超时
 */
bool retry_is_timed_out(const retry_policy_t *policy);

/**
 * @brief 设置重试策略参数
 * @param policy 重试策略
 * @param max_attempts 最大尝试次数
 * @param base_delay_ms 基础延迟（毫秒）
 * @param error 错误码输出
 * @return 是否成功
 */
bool retry_set_params(retry_policy_t *policy, int max_attempts, int base_delay_ms, retry_error_t *error);

/**
 * @brief 获取错误信息
 * @param error 错误码
 * @return 错误信息字符串
 */
const char* retry_error_string(retry_error_t error);

#endif // C_UTILS_RETRY_H
