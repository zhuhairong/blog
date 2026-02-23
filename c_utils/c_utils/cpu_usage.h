#ifndef C_UTILS_CPU_USAGE_H
#define C_UTILS_CPU_USAGE_H

#include <stdbool.h>
#include <stdint.h>

// CPU 使用率错误码
typedef enum {
    CPU_USAGE_OK = 0,
    CPU_USAGE_ERROR_INVALID_CORE,
    CPU_USAGE_ERROR_OPERATION_FAILED,
    CPU_USAGE_ERROR_PLATFORM_UNSUPPORTED,
    CPU_USAGE_ERROR_MEMORY_ALLOC,
    CPU_USAGE_ERROR_INSUFFICIENT_DATA
} cpu_usage_error_t;

// CPU 时间统计
typedef struct {
    double user;
    double system;
    double idle;
    double io_wait;
    double nice;
    double irq;
    double softirq;
    double steal;
} cpu_time_stats_t;

// CPU 使用率统计
typedef struct {
    double user;
    double system;
    double idle;
    double io_wait;
    double nice;
    double irq;
    double softirq;
    double steal;
    double total;
} cpu_usage_stats_t;

// 初始化 CPU 使用率统计
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool cpu_usage_init(cpu_usage_error_t *error);

// 获取当前进程 CPU 使用时间（秒）
// user_time: 用户时间（输出参数，可为 NULL）
// system_time: 系统时间（输出参数，可为 NULL）
// error: 错误码（输出参数，可为 NULL）
// 返回: 总 CPU 时间，失败返回 0.0
double cpu_usage_get_process_time(double *user_time, double *system_time, cpu_usage_error_t *error);

// 获取总体 CPU 使用率
// stats: CPU 使用率统计（输出参数）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool cpu_usage_get_overall(cpu_usage_stats_t *stats, cpu_usage_error_t *error);

// 获取指定核心的 CPU 使用率
// core_id: 核心 ID（0 表示总体）
// stats: CPU 使用率统计（输出参数）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool cpu_usage_get_core(int core_id, cpu_usage_stats_t *stats, cpu_usage_error_t *error);

// 获取 CPU 时间统计
// core_id: 核心 ID（0 表示总体）
// stats: CPU 时间统计（输出参数）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool cpu_usage_get_time_stats(int core_id, cpu_time_stats_t *stats, cpu_usage_error_t *error);

// 获取 CPU 核心数量
// 返回: CPU 核心数量，失败返回 0
int cpu_usage_get_core_count(void);

// 检查 CPU 核心是否有效
// core_id: 核心 ID
// 返回: 有效返回 true，无效返回 false
bool cpu_usage_is_core_valid(int core_id);

// 刷新 CPU 使用率数据
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool cpu_usage_refresh(cpu_usage_error_t *error);

// 获取错误信息
// error: 错误码
// 返回: 错误信息字符串
const char* cpu_usage_strerror(cpu_usage_error_t error);

#endif // C_UTILS_CPU_USAGE_H
