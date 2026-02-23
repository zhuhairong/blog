#ifndef C_UTILS_TIME_UTILS_H
#define C_UTILS_TIME_UTILS_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <time.h>

typedef enum {
    TIME_OK = 0,
    TIME_ERROR_INVALID_PARAMS = -1,
    TIME_ERROR_NOW = -2,
    TIME_ERROR_SLEEP = -3,
    TIME_ERROR_FORMAT = -4,
    TIME_ERROR_PARSE = -5,
    TIME_ERROR_MEMORY = -6,
    TIME_ERROR_TIMEZONE = -7,
    TIME_ERROR_CLOCK = -8,
    TIME_ERROR_FILE_OPEN = -9,
    TIME_ERROR_FILE_READ = -10,
    TIME_ERROR_FILE_WRITE = -11,
    TIME_ERROR_TIMER = -12,
    TIME_ERROR_DIFFERENCE = -13,
    TIME_ERROR_STATISTICS = -14
} time_error_t;

typedef struct {
    bool enable_statistics;        // 启用统计
    bool enable_high_resolution;   // 启用高分辨率
    bool enable_timezone;          // 启用时区支持
    bool enable_validation;        // 启用验证
    bool use_system_clock;         // 使用系统时钟
    size_t max_format_size;        // 最大格式大小
    size_t buffer_size;            // 缓冲区大小
    int timezone_offset;           // 时区偏移（分钟）
} time_config_t;

typedef struct {
    time_error_t last_error;         // 最后一次错误
    size_t now_calls;               // now调用次数
    size_t sleep_calls;             // sleep调用次数
    size_t format_calls;            // format调用次数
    size_t parse_calls;             // parse调用次数
    bool is_initialized;            // 是否初始化
    uint64_t total_sleep_time;      // 总睡眠时间
    double average_call_time;       // 平均调用时间
} time_state_t;

/**
 * @brief 获取当前时间（毫秒）
 * @return 当前时间戳（毫秒）
 */
uint64_t time_now_ms(void);

/**
 * @brief 获取当前时间（微秒）
 * @return 当前时间戳（微秒）
 */
uint64_t time_now_us(void);

/**
 * @brief 睡眠指定时间（毫秒）
 * @param ms 睡眠时间（毫秒）
 */
void     time_sleep_ms(uint32_t ms);

/**
 * @brief 格式化当前时间
 * @param buf 输出缓冲区
 * @param buf_size 缓冲区大小
 */
void     time_format_now(char *buf, size_t buf_size);

/**
 * @brief 增强版获取当前时间（毫秒）
 * @param config 配置选项
 * @param state 状态输出
 * @return 当前时间戳（毫秒），失败返回0
 */
uint64_t time_now_ms_ex(const time_config_t *config, time_state_t *state);

/**
 * @brief 增强版获取当前时间（微秒）
 * @param config 配置选项
 * @param state 状态输出
 * @return 当前时间戳（微秒），失败返回0
 */
uint64_t time_now_us_ex(const time_config_t *config, time_state_t *state);

/**
 * @brief 增强版睡眠指定时间
 * @param ms 睡眠时间（毫秒）
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
time_error_t time_sleep_ms_ex(uint32_t ms, const time_config_t *config, time_state_t *state);

/**
 * @brief 增强版格式化当前时间
 * @param buf 输出缓冲区
 * @param buf_size 缓冲区大小
 * @param format 格式字符串
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
time_error_t time_format_now_ex(char *buf, size_t buf_size, const char *format,
                              const time_config_t *config, time_state_t *state);

/**
 * @brief 格式化指定时间
 * @param timestamp 时间戳（毫秒）
 * @param buf 输出缓冲区
 * @param buf_size 缓冲区大小
 * @param format 格式字符串
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
time_error_t time_format(uint64_t timestamp, char *buf, size_t buf_size,
                       const char *format,
                       const time_config_t *config,
                       time_state_t *state);

/**
 * @brief 解析时间字符串
 * @param str 时间字符串
 * @param format 格式字符串
 * @param timestamp 输出时间戳（毫秒）
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
time_error_t time_parse(const char *str, const char *format,
                       uint64_t *timestamp,
                       const time_config_t *config,
                       time_state_t *state);

/**
 * @brief 计算时间差
 * @param start 开始时间戳
 * @param end 结束时间戳
 * @param diff_ms 输出时间差（毫秒）
 * @param state 状态输出
 * @return 错误码
 */
time_error_t time_diff(uint64_t start, uint64_t end, uint64_t *diff_ms,
                      time_state_t *state);

/**
 * @brief 计算时间差（秒）
 * @param start 开始时间戳
 * @param end 结束时间戳
 * @param diff_sec 输出时间差（秒）
 * @param state 状态输出
 * @return 错误码
 */
time_error_t time_diff_sec(uint64_t start, uint64_t end, double *diff_sec,
                         time_state_t *state);

/**
 * @brief 获取时间组件
 * @param timestamp 时间戳（毫秒）
 * @param year 输出年
 * @param month 输出月
 * @param day 输出日
 * @param hour 输出时
 * @param minute 输出分
 * @param second 输出秒
 * @param ms 输出毫秒
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
time_error_t time_get_components(uint64_t timestamp,
                               int *year, int *month, int *day,
                               int *hour, int *minute, int *second, int *ms,
                               const time_config_t *config,
                               time_state_t *state);

/**
 * @brief 设置时间组件
 * @param year 年
 * @param month 月
 * @param day 日
 * @param hour 时
 * @param minute 分
 * @param second 秒
 * @param ms 毫秒
 * @param timestamp 输出时间戳（毫秒）
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
time_error_t time_set_components(int year, int month, int day,
                               int hour, int minute, int second, int ms,
                               uint64_t *timestamp,
                               const time_config_t *config,
                               time_state_t *state);

/**
 * @brief 获取时区信息
 * @param timezone 输出时区偏移（分钟）
 * @param dst 输出是否夏令时
 * @param state 状态输出
 * @return 错误码
 */
time_error_t time_get_timezone(int *timezone, bool *dst, time_state_t *state);

/**
 * @brief 设置时区
 * @param timezone_offset 时区偏移（分钟）
 * @param state 状态输出
 * @return 错误码
 */
time_error_t time_set_timezone(int timezone_offset, time_state_t *state);

/**
 * @brief 从文件加载时间配置
 * @param filename 文件名
 * @param config 输出配置
 * @param state 状态输出
 * @return 错误码
 */
time_error_t time_load_config(const char *filename,
                             time_config_t *config,
                             time_state_t *state);

/**
 * @brief 保存时间配置到文件
 * @param filename 文件名
 * @param config 配置
 * @param state 状态输出
 * @return 错误码
 */
time_error_t time_save_config(const char *filename,
                             const time_config_t *config,
                             time_state_t *state);

/**
 * @brief 保存时间戳到文件
 * @param filename 文件名
 * @param timestamp 时间戳
 * @param state 状态输出
 * @return 错误码
 */
time_error_t time_save_timestamp(const char *filename, uint64_t timestamp,
                               time_state_t *state);

/**
 * @brief 从文件加载时间戳
 * @param filename 文件名
 * @param timestamp 输出时间戳
 * @param state 状态输出
 * @return 错误码
 */
time_error_t time_load_timestamp(const char *filename,
                               uint64_t *timestamp,
                               time_state_t *state);

/**
 * @brief 验证时间戳
 * @param timestamp 时间戳
 * @param valid 输出验证结果
 * @param state 状态输出
 * @return 错误码
 */
time_error_t time_validate_timestamp(uint64_t timestamp, bool *valid,
                                   time_state_t *state);

/**
 * @brief 获取时间统计信息
 * @param state 状态
 * @return 错误码
 */
time_error_t time_get_statistics(time_state_t *state);

/**
 * @brief 重置时间状态
 * @param state 状态
 */
void time_reset_state(time_state_t *state);

/**
 * @brief 获取最后一次错误信息
 * @param state 状态
 * @return 错误描述字符串
 */
const char* time_strerror(const time_state_t *state);

/**
 * @brief 初始化配置为默认值
 * @param config 配置
 */
void time_config_init(time_config_t *config);

/**
 * @brief 初始化状态为默认值
 * @param state 状态
 */
void time_state_init(time_state_t *state);

#endif // C_UTILS_TIME_UTILS_H
