#ifndef C_UTILS_LOG_ROTATE_H
#define C_UTILS_LOG_ROTATE_H

#include <stddef.h>
#include <stdbool.h>
#include <sys/types.h>

// 日志滚动错误码
typedef enum {
    LOG_ROTATE_OK = 0,
    LOG_ROTATE_INVALID_INPUT = -1,
    LOG_ROTATE_FILE_ERROR = -2,
    LOG_ROTATE_RENAME_ERROR = -3,
    LOG_ROTATE_STAT_ERROR = -4,
    LOG_ROTATE_OPEN_ERROR = -5,
    LOG_ROTATE_CLOSE_ERROR = -6,
    LOG_ROTATE_PERMISSION_DENIED = -7,
    LOG_ROTATE_SYSTEM_ERROR = -8
} log_rotate_error_t;

// 日志滚动配置
typedef struct {
    size_t max_size;
    int max_backups;
    bool compress;
    bool create_file;
    bool verbose;
    bool check_size;
    mode_t file_mode;
    int rotate_interval;
    bool force_rotate;
} log_rotate_config_t;

// 日志滚动触发类型
typedef enum {
    LOG_ROTATE_TRIGGER_SIZE = 0,
    LOG_ROTATE_TRIGGER_TIME = 1,
    LOG_ROTATE_TRIGGER_FORCE = 2,
    LOG_ROTATE_TRIGGER_SIGNAL = 3
} log_rotate_trigger_t;

// 极简日志滚动逻辑，当文件超过 max_size 时重命名为 .1, .2 等
void log_rotate(const char *path, size_t max_size, int max_backups);

// 带错误处理的日志滚动
log_rotate_error_t log_rotate_ex(const char *path, const log_rotate_config_t *config, 
                                log_rotate_trigger_t trigger, bool *rotated);

// 检查是否需要滚动
bool log_rotate_needs_rotation(const char *path, const log_rotate_config_t *config, 
                              log_rotate_error_t *error);

// 强制滚动日志
log_rotate_error_t log_rotate_force(const char *path, const log_rotate_config_t *config);

// 清理旧日志文件
log_rotate_error_t log_rotate_cleanup(const char *path, int max_backups, 
                                     log_rotate_error_t *error);

// 获取日志文件大小
log_rotate_error_t log_rotate_get_file_size(const char *path, size_t *size);

// 获取默认配置
void log_rotate_get_default_config(log_rotate_config_t *config);

#endif // C_UTILS_LOG_ROTATE_H
