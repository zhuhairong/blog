#ifndef C_UTILS_LOG_H
#define C_UTILS_LOG_H

#include <stdio.h>
#include <stdarg.h>

typedef enum {
    LOG_LEVEL_TRACE = 0,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL
} log_level_t;

// 兼容性宏 - 用于 log_set_level 等函数
#define LOG_TRACE_LEVEL LOG_LEVEL_TRACE
#define LOG_DEBUG_LEVEL LOG_LEVEL_DEBUG
#define LOG_INFO_LEVEL  LOG_LEVEL_INFO
#define LOG_WARN_LEVEL  LOG_LEVEL_WARN
#define LOG_ERROR_LEVEL LOG_LEVEL_ERROR
#define LOG_FATAL_LEVEL LOG_LEVEL_FATAL

void log_set_level(log_level_t level);
void log_set_quiet(int enable);
int  log_add_fp(FILE *fp, log_level_t level);

// 日志宏 - 用于输出日志
#define LOGT(...) log_log(LOG_LEVEL_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define LOGD(...) log_log(LOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define LOGI(...)  log_log(LOG_LEVEL_INFO,  __FILE__, __LINE__, __VA_ARGS__)
#define LOGW(...)  log_log(LOG_LEVEL_WARN,  __FILE__, __LINE__, __VA_ARGS__)
#define LOGE(...) log_log(LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define LOGF(...) log_log(LOG_LEVEL_FATAL, __FILE__, __LINE__, __VA_ARGS__)

// 保留旧名称的宏，但避免冲突
#ifndef LOG_TRACE
#define LOG_TRACE(...) LOGT(__VA_ARGS__)
#endif
#ifndef LOG_DEBUG
#define LOG_DEBUG(...) LOGD(__VA_ARGS__)
#endif
#ifndef LOG_INFO
#define LOG_INFO(...)  LOGI(__VA_ARGS__)
#endif
#ifndef LOG_WARN
#define LOG_WARN(...)  LOGW(__VA_ARGS__)
#endif
#ifndef LOG_ERROR
#define LOG_ERROR(...) LOGE(__VA_ARGS__)
#endif
#ifndef LOG_FATAL
#define LOG_FATAL(...) LOGF(__VA_ARGS__)
#endif

void log_log(log_level_t level, const char *file, int line, const char *fmt, ...);

// 便捷函数
static inline void log_info(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    log_log(LOG_LEVEL_INFO, __FILE__, __LINE__, fmt, ap);
    va_end(ap);
}
static inline void log_error(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    log_log(LOG_LEVEL_ERROR, __FILE__, __LINE__, fmt, ap);
    va_end(ap);
}
static inline void log_debug(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    log_log(LOG_LEVEL_DEBUG, __FILE__, __LINE__, fmt, ap);
    va_end(ap);
}

#endif // C_UTILS_LOG_H
