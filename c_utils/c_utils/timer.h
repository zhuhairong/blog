#ifndef C_UTILS_TIMER_H
#define C_UTILS_TIMER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef void (*timer_callback_t)(void *data);

typedef struct {
    uint64_t target_ms;
    timer_callback_t cb;
    void *data;
    bool active;
} cutils_timer_t;

typedef enum {
    TIMER_OK = 0,
    TIMER_ERROR_INVALID_PARAMS = -1,
    TIMER_ERROR_SET = -2,
    TIMER_ERROR_UPDATE = -3,
    TIMER_ERROR_CANCEL = -4,
    TIMER_ERROR_DELETE = -5,
    TIMER_ERROR_MEMORY = -6,
    TIMER_ERROR_CALLBACK_NULL = -7,
    TIMER_ERROR_TIMER_NULL = -8,
    TIMER_ERROR_DELAY_ZERO = -9,
    TIMER_ERROR_NOT_ACTIVE = -10,
    TIMER_ERROR_ALREADY_ACTIVE = -11,
    TIMER_ERROR_FILE_OPEN = -12,
    TIMER_ERROR_FILE_READ = -13,
    TIMER_ERROR_FILE_WRITE = -14,
    TIMER_ERROR_STATISTICS = -15,
    TIMER_ERROR_BATCH_OPERATION = -16
} timer_error_t;

typedef struct {
    bool enable_statistics;        // 启用统计
    bool enable_validation;        // 启用验证
    bool enable_thread_safety;     // 启用线程安全
    bool auto_cleanup;             // 自动清理
    size_t max_timers;             // 最大定时器数量
    size_t buffer_size;            // 缓冲区大小
    uint64_t max_delay;            // 最大延迟时间
    bool use_high_resolution;      // 使用高分辨率
} timer_config_t;

typedef struct {
    timer_error_t last_error;      // 最后一次错误
    size_t timer_sets;             // 定时器设置次数
    size_t timer_triggers;         // 定时器触发次数
    size_t timer_cancels;          // 定时器取消次数
    bool is_initialized;           // 是否初始化
    size_t active_timers;           // 活跃定时器数量
    uint64_t average_delay;         // 平均延迟时间
} timer_state_t;

/**
 * @brief 增强定时器结构体
 */
typedef struct {
    uint64_t target_ms;
    uint64_t start_ms;
    uint32_t delay_ms;
    timer_callback_t cb;
    void *data;
    bool active;
    bool repeating;
    uint32_t repeat_interval;
    size_t repeat_count;
    size_t max_repeats;
    timer_config_t config;
    timer_state_t state;
} timer_extended_t;

/**
 * @brief 创建定时器 (delay 毫秒后执行)
 * @param t 定时器结构体
 * @param delay_ms 延迟时间
 * @param cb 回调函数
 * @param data 回调数据
 */
void timer_set(cutils_timer_t *t, uint32_t delay_ms, timer_callback_t cb, void *data);

/**
 * @brief 增强版定时器设置
 * @param t 定时器结构体
 * @param delay_ms 延迟时间
 * @param cb 回调函数
 * @param data 回调数据
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
timer_error_t timer_set_ex(cutils_timer_t *t, uint32_t delay_ms, timer_callback_t cb, void *data,
                         const timer_config_t *config,
                         timer_state_t *state);

/**
 * @brief 检查并触发 (需在主循环调用)
 * @param t 定时器结构体
 */
void timer_update(cutils_timer_t *t);

/**
 * @brief 增强版定时器更新
 * @param t 定时器结构体
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
timer_error_t timer_update_ex(cutils_timer_t *t,
                           const timer_config_t *config,
                           timer_state_t *state);

/**
 * @brief 取消定时器
 * @param t 定时器结构体
 * @param state 状态输出
 * @return 错误码
 */
timer_error_t timer_cancel(cutils_timer_t *t, timer_state_t *state);

/**
 * @brief 检查定时器是否活跃
 * @param t 定时器结构体
 * @param active 输出活跃状态
 * @return 错误码
 */
timer_error_t timer_is_active(const cutils_timer_t *t, bool *active);

/**
 * @brief 获取定时器剩余时间
 * @param t 定时器结构体
 * @param remaining_ms 输出剩余时间
 * @param state 状态输出
 * @return 错误码
 */
timer_error_t timer_get_remaining(const cutils_timer_t *t, uint32_t *remaining_ms,
                               timer_state_t *state);

/**
 * @brief 创建重复定时器
 * @param t 扩展定时器结构体
 * @param delay_ms 初始延迟
 * @param interval_ms 重复间隔
 * @param max_repeats 最大重复次数
 * @param cb 回调函数
 * @param data 回调数据
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
timer_error_t timer_set_repeating(timer_extended_t *t, uint32_t delay_ms,
                                uint32_t interval_ms, size_t max_repeats,
                                timer_callback_t cb, void *data,
                                const timer_config_t *config,
                                timer_state_t *state);

/**
 * @brief 更新扩展定时器
 * @param t 扩展定时器结构体
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
timer_error_t timer_update_extended(timer_extended_t *t,
                                 const timer_config_t *config,
                                 timer_state_t *state);

/**
 * @brief 取消扩展定时器
 * @param t 扩展定时器结构体
 * @param state 状态输出
 * @return 错误码
 */
timer_error_t timer_cancel_extended(timer_extended_t *t,
                                 timer_state_t *state);

/**
 * @brief 批量设置定时器
 * @param timers 定时器数组
 * @param count 定时器数量
 * @param delay_ms 延迟时间数组
 * @param cbs 回调函数数组
 * @param datas 回调数据数组
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
timer_error_t timer_batch_set(cutils_timer_t *timers, size_t count,
                            const uint32_t *delay_ms,
                            const timer_callback_t *cbs,
                            void **datas,
                            const timer_config_t *config,
                            timer_state_t *state);

/**
 * @brief 批量更新定时器
 * @param timers 定时器数组
 * @param count 定时器数量
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
timer_error_t timer_batch_update(cutils_timer_t *timers, size_t count,
                               const timer_config_t *config,
                               timer_state_t *state);

/**
 * @brief 批量取消定时器
 * @param timers 定时器数组
 * @param count 定时器数量
 * @param state 状态输出
 * @return 错误码
 */
timer_error_t timer_batch_cancel(cutils_timer_t *timers, size_t count,
                               timer_state_t *state);

/**
 * @brief 从文件加载定时器配置
 * @param filename 文件名
 * @param config 输出配置
 * @param state 状态输出
 * @return 错误码
 */
timer_error_t timer_load_config(const char *filename,
                              timer_config_t *config,
                              timer_state_t *state);

/**
 * @brief 保存定时器配置到文件
 * @param filename 文件名
 * @param config 配置
 * @param state 状态输出
 * @return 错误码
 */
timer_error_t timer_save_config(const char *filename,
                              const timer_config_t *config,
                              timer_state_t *state);

/**
 * @brief 保存定时器状态到文件
 * @param filename 文件名
 * @param t 定时器结构体
 * @param state 状态输出
 * @return 错误码
 */
timer_error_t timer_save_state(const char *filename, const cutils_timer_t *t,
                             timer_state_t *state);

/**
 * @brief 从文件加载定时器状态
 * @param filename 文件名
 * @param t 定时器结构体
 * @param state 状态输出
 * @return 错误码
 */
timer_error_t timer_load_state(const char *filename, cutils_timer_t *t,
                             timer_state_t *state);

/**
 * @brief 验证定时器
 * @param t 定时器结构体
 * @param valid 输出验证结果
 * @param state 状态输出
 * @return 错误码
 */
timer_error_t timer_validate(const cutils_timer_t *t, bool *valid,
                           timer_state_t *state);

/**
 * @brief 获取定时器统计信息
 * @param state 状态
 * @return 错误码
 */
timer_error_t timer_get_statistics(timer_state_t *state);

/**
 * @brief 重置定时器状态
 * @param state 状态
 */
void timer_reset_state(timer_state_t *state);

/**
 * @brief 获取最后一次错误信息
 * @param state 状态
 * @return 错误描述字符串
 */
const char* timer_strerror(const timer_state_t *state);

/**
 * @brief 初始化配置为默认值
 * @param config 配置
 */
void timer_config_init(timer_config_t *config);

/**
 * @brief 初始化状态为默认值
 * @param state 状态
 */
void timer_state_init(timer_state_t *state);

#endif // C_UTILS_TIMER_H
