#ifndef C_UTILS_STOPWATCH_H
#define C_UTILS_STOPWATCH_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct {
    uint64_t start_time;
    uint64_t elapsed;
    bool     running;
} stopwatch_t;

typedef enum {
    STOPWATCH_OK = 0,
    STOPWATCH_ERROR_INVALID_PARAMS = -1,
    STOPWATCH_ERROR_START = -2,
    STOPWATCH_ERROR_STOP = -3,
    STOPWATCH_ERROR_RESET = -4,
    STOPWATCH_ERROR_ELAPSED = -5,
    STOPWATCH_ERROR_MEMORY = -6,
    STOPWATCH_ERROR_NOT_RUNNING = -7,
    STOPWATCH_ERROR_ALREADY_RUNNING = -8,
    STOPWATCH_ERROR_LAP = -9,
    STOPWATCH_ERROR_FILE_OPEN = -10,
    STOPWATCH_ERROR_FILE_READ = -11,
    STOPWATCH_ERROR_FILE_WRITE = -12,
    STOPWATCH_ERROR_TIMEOUT = -13
} stopwatch_error_t;

typedef struct {
    bool enable_statistics;        // 启用统计
    bool enable_lap_timer;         // 启用lap计时
    bool use_high_resolution;      // 使用高分辨率计时器
    bool auto_reset;               // 自动重置
    bool enable_file_operations;   // 启用文件操作
    size_t max_laps;               // 最大lap数量
    size_t buffer_size;            // 缓冲区大小
    uint64_t timeout_ms;           // 超时时间（毫秒）
} stopwatch_config_t;

typedef struct {
    stopwatch_error_t last_error;     // 最后一次错误
    size_t starts;                    // 启动次数
    size_t stops;                     // 停止次数
    size_t resets;                    // 重置次数
    size_t laps;                      // lap次数
    bool is_initialized;              // 是否初始化
    uint64_t total_elapsed;           // 总运行时间
    uint64_t average_lap_time;        // 平均lap时间
} stopwatch_state_t;

/**
 * @brief 启动计时器
 * @param sw 计时器结构体
 */
void     stopwatch_start(stopwatch_t *sw);

/**
 * @brief 增强版启动计时器
 * @param sw 计时器结构体
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
stopwatch_error_t stopwatch_start_ex(stopwatch_t *sw,
                                   const stopwatch_config_t *config,
                                   stopwatch_state_t *state);

/**
 * @brief 停止计时器
 * @param sw 计时器结构体
 */
void     stopwatch_stop(stopwatch_t *sw);

/**
 * @brief 增强版停止计时器
 * @param sw 计时器结构体
 * @param state 状态输出
 * @return 错误码
 */
stopwatch_error_t stopwatch_stop_ex(stopwatch_t *sw,
                                  stopwatch_state_t *state);

/**
 * @brief 重置计时器
 * @param sw 计时器结构体
 */
void     stopwatch_reset(stopwatch_t *sw);

/**
 * @brief 增强版重置计时器
 * @param sw 计时器结构体
 * @param state 状态输出
 * @return 错误码
 */
stopwatch_error_t stopwatch_reset_ex(stopwatch_t *sw,
                                   stopwatch_state_t *state);

/**
 * @brief 获取已用时间 (毫秒)
 * @param sw 计时器结构体
 * @return 已用时间（毫秒）
 */
uint64_t stopwatch_elapsed_ms(const stopwatch_t *sw);

/**
 * @brief 增强版获取已用时间
 * @param sw 计时器结构体
 * @param elapsed 输出已用时间
 * @param unit 时间单位（0:毫秒, 1:微秒, 2:纳秒, 3:秒）
 * @param state 状态输出
 * @return 错误码
 */
stopwatch_error_t stopwatch_elapsed_ex(const stopwatch_t *sw,
                                      uint64_t *elapsed,
                                      int unit,
                                      stopwatch_state_t *state);

/**
 * @brief 暂停计时器
 * @param sw 计时器结构体
 * @param state 状态输出
 * @return 错误码
 */
stopwatch_error_t stopwatch_pause(stopwatch_t *sw,
                                 stopwatch_state_t *state);

/**
 * @brief 恢复计时器
 * @param sw 计时器结构体
 * @param state 状态输出
 * @return 错误码
 */
stopwatch_error_t stopwatch_resume(stopwatch_t *sw,
                                  stopwatch_state_t *state);

/**
 * @brief 记录lap时间
 * @param sw 计时器结构体
 * @param lap_time 输出lap时间
 * @param state 状态输出
 * @return 错误码
 */
stopwatch_error_t stopwatch_lap(stopwatch_t *sw,
                               uint64_t *lap_time,
                               stopwatch_state_t *state);

/**
 * @brief 获取所有lap时间
 * @param sw 计时器结构体
 * @param laps lap时间数组
 * @param max_laps 最大lap数量
 * @param lap_count 输出lap数量
 * @param state 状态输出
 * @return 错误码
 */
stopwatch_error_t stopwatch_get_laps(const stopwatch_t *sw,
                                    uint64_t *laps,
                                    size_t max_laps,
                                    size_t *lap_count,
                                    stopwatch_state_t *state);

/**
 * @brief 检查计时器是否正在运行
 * @param sw 计时器结构体
 * @param running 输出运行状态
 * @return 错误码
 */
stopwatch_error_t stopwatch_is_running(const stopwatch_t *sw,
                                      bool *running);

/**
 * @brief 等待指定时间
 * @param milliseconds 等待时间（毫秒）
 * @param state 状态输出
 * @return 错误码
 */
stopwatch_error_t stopwatch_sleep(uint64_t milliseconds,
                                 stopwatch_state_t *state);

/**
 * @brief 测量函数执行时间
 * @param func 要测量的函数
 * @param args 函数参数
 * @param execution_time 输出执行时间
 * @param state 状态输出
 * @return 错误码
 */
stopwatch_error_t stopwatch_measure_function(void (*func)(void *),
                                           void *args,
                                           uint64_t *execution_time,
                                           stopwatch_state_t *state);

/**
 * @brief 从文件加载计时器状态
 * @param sw 计时器结构体
 * @param filename 文件名
 * @param state 状态输出
 * @return 错误码
 */
stopwatch_error_t stopwatch_load_from_file(stopwatch_t *sw,
                                          const char *filename,
                                          stopwatch_state_t *state);

/**
 * @brief 保存计时器状态到文件
 * @param sw 计时器结构体
 * @param filename 文件名
 * @param state 状态输出
 * @return 错误码
 */
stopwatch_error_t stopwatch_save_to_file(const stopwatch_t *sw,
                                         const char *filename,
                                         stopwatch_state_t *state);

/**
 * @brief 比较两个计时器
 * @param sw1 第一个计时器
 * @param sw2 第二个计时器
 * @param difference 输出时间差
 * @param state 状态输出
 * @return 错误码
 */
stopwatch_error_t stopwatch_compare(const stopwatch_t *sw1,
                                   const stopwatch_t *sw2,
                                   int64_t *difference,
                                   stopwatch_state_t *state);

/**
 * @brief 获取计时器统计信息
 * @param state 状态结构
 * @return 错误码
 */
stopwatch_error_t stopwatch_get_statistics(stopwatch_state_t *state);

/**
 * @brief 重置计时器状态
 * @param state 状态结构
 */
void stopwatch_reset_state(stopwatch_state_t *state);

/**
 * @brief 获取最后一次错误信息
 * @param state 状态
 * @return 错误描述字符串
 */
const char* stopwatch_strerror(const stopwatch_state_t *state);

/**
 * @brief 初始化配置为默认值
 * @param config 配置结构
 */
void stopwatch_config_init(stopwatch_config_t *config);

/**
 * @brief 初始化状态为默认值
 * @param state 状态结构
 */
void stopwatch_state_init(stopwatch_state_t *state);

#endif // C_UTILS_STOPWATCH_H
