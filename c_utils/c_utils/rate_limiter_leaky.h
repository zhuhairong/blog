#ifndef C_UTILS_RATE_LIMITER_LEAKY_H
#define C_UTILS_RATE_LIMITER_LEAKY_H

#include <stdbool.h>
#include <time.h>
#include <stddef.h>

/**
 * @brief 漏桶限流器错误码
 */
typedef enum {
    LEAKY_BUCKET_OK = 0,                  /**< 成功 */
    LEAKY_BUCKET_ERROR_NULL_PTR,          /**< 空指针错误 */
    LEAKY_BUCKET_ERROR_INVALID_ARGS,      /**< 无效参数 */
    LEAKY_BUCKET_ERROR_CAPACITY_TOO_SMALL, /**< 容量过小 */
    LEAKY_BUCKET_ERROR_RATE_TOO_SLOW,     /**< 速率过慢 */
    LEAKY_BUCKET_ERROR_AMOUNT_TOO_LARGE,  /**< 消耗数量过大 */
    LEAKY_BUCKET_ERROR_MAX                /**< 最大错误码 */
} leaky_bucket_error_t;

/**
 * @brief 漏桶限流器配置
 */
typedef struct {
    double capacity;                   /**< 桶容量 */
    double rate;                       /**< 漏水速率（单位/秒） */
    double initial_water;              /**< 初始水量 */
    bool use_monotonic_time;           /**< 是否使用单调时间 */
    bool allow_burst;                  /**< 是否允许突发流量 */
} leaky_bucket_config_t;

/**
 * @brief 漏桶限流器状态
 */
typedef struct {
    double current_water;              /**< 当前水量 */
    double available_capacity;         /**< 可用容量 */
    double fill_level;                 /**< 填充水平 (0.0-1.0) */
    time_t last_update_time;           /**< 最后更新时间 */
    bool is_full;                      /**< 是否已满 */
    bool is_empty;                     /**< 是否为空 */
} leaky_bucket_state_t;

/**
 * @brief 漏桶限流器结构
 */
typedef struct {
    double capacity;                   /**< 桶容量 */
    double rate;                       /**< 漏水速率（单位/秒） */
    double water;                      /**< 当前水量 */
    time_t last_time;                  /**< 最后更新时间 */
    leaky_bucket_config_t config;      /**< 配置 */
} leaky_bucket_t;

/**
 * @brief 获取默认配置
 * @return 默认配置
 */
leaky_bucket_config_t leaky_bucket_default_config(void);

/**
 * @brief 初始化漏桶限流器
 * @param lb 漏桶限流器
 * @param capacity 桶容量
 * @param rate 漏水速率（单位/秒）
 */
void leaky_init(leaky_bucket_t *lb, double capacity, double rate);

/**
 * @brief 使用自定义配置初始化漏桶限流器
 * @param lb 漏桶限流器
 * @param config 配置选项
 * @param error 错误码输出
 * @return 是否成功
 */
bool leaky_init_ex(leaky_bucket_t *lb, const leaky_bucket_config_t *config, leaky_bucket_error_t *error);

/**
 * @brief 尝试消耗指定数量的令牌
 * @param lb 漏桶限流器
 * @param amount 消耗数量
 * @return 是否成功
 */
bool leaky_consume(leaky_bucket_t *lb, double amount);

/**
 * @brief 尝试消耗指定数量的令牌（带错误处理）
 * @param lb 漏桶限流器
 * @param amount 消耗数量
 * @param error 错误码输出
 * @return 是否成功
 */
bool leaky_consume_ex(leaky_bucket_t *lb, double amount, leaky_bucket_error_t *error);

/**
 * @brief 尝试消耗指定数量的令牌（带等待时间）
 * @param lb 漏桶限流器
 * @param amount 消耗数量
 * @param max_wait_ms 最大等待时间（毫秒）
 * @param actual_wait_ms 实际等待时间输出
 * @param error 错误码输出
 * @return 是否成功
 */
bool leaky_consume_with_wait(leaky_bucket_t *lb, double amount, int max_wait_ms, int *actual_wait_ms, leaky_bucket_error_t *error);

/**
 * @brief 获取漏桶限流器状态
 * @param lb 漏桶限流器
 * @param state 状态输出
 * @param error 错误码输出
 * @return 是否成功
 */
bool leaky_get_state(const leaky_bucket_t *lb, leaky_bucket_state_t *state, leaky_bucket_error_t *error);

/**
 * @brief 重置漏桶限流器
 * @param lb 漏桶限流器
 * @param error 错误码输出
 * @return 是否成功
 */
bool leaky_reset(leaky_bucket_t *lb, leaky_bucket_error_t *error);

/**
 * @brief 手动更新漏桶状态
 * @param lb 漏桶限流器
 * @param error 错误码输出
 * @return 是否成功
 */
bool leaky_update(leaky_bucket_t *lb, leaky_bucket_error_t *error);

/**
 * @brief 计算预计等待时间
 * @param lb 漏桶限流器
 * @param amount 消耗数量
 * @param wait_ms 等待时间输出（毫秒）
 * @param error 错误码输出
 * @return 是否成功
 */
bool leaky_calculate_wait_time(const leaky_bucket_t *lb, double amount, int *wait_ms, leaky_bucket_error_t *error);

/**
 * @brief 设置漏桶参数
 * @param lb 漏桶限流器
 * @param capacity 桶容量
 * @param rate 漏水速率（单位/秒）
 * @param error 错误码输出
 * @return 是否成功
 */
bool leaky_set_params(leaky_bucket_t *lb, double capacity, double rate, leaky_bucket_error_t *error);

/**
 * @brief 检查漏桶是否可以消耗指定数量
 * @param lb 漏桶限流器
 * @param amount 消耗数量
 * @param error 错误码输出
 * @return 是否可以消耗
 */
bool leaky_can_consume(const leaky_bucket_t *lb, double amount, leaky_bucket_error_t *error);

/**
 * @brief 获取当前可用容量
 * @param lb 漏桶限流器
 * @param error 错误码输出
 * @return 可用容量
 */
double leaky_get_available(const leaky_bucket_t *lb, leaky_bucket_error_t *error);

/**
 * @brief 获取错误信息
 * @param error 错误码
 * @return 错误信息字符串
 */
const char* leaky_bucket_error_string(leaky_bucket_error_t error);

#endif // C_UTILS_RATE_LIMITER_LEAKY_H
