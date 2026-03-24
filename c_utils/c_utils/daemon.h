#ifndef C_UTILS_DAEMON_H
#define C_UTILS_DAEMON_H

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef __cplusplus
extern "C" {
#endif

// 守护进程错误码
typedef enum {
    DAEMON_OK = 0,
    DAEMON_ERROR_FORK,
    DAEMON_ERROR_SETSID,
    DAEMON_ERROR_CHDIR,
    DAEMON_ERROR_OPEN,
    DAEMON_ERROR_DUP2,
    DAEMON_ERROR_PID_FILE,
    DAEMON_ERROR_ALREADY_RUNNING,
    DAEMON_ERROR_INVALID_PARAM,
    DAEMON_ERROR_SIGNAL,
    DAEMON_ERROR_MEMORY
} daemon_error_t;

// 守护进程信号处理回调
typedef void (*daemon_signal_handler_t)(int signum);

// 守护进程配置
typedef struct {
    const char *name;              // 守护进程名称
    const char *working_dir;       // 工作目录（NULL 表示根目录）
    const char *pid_file;          // PID 文件路径（NULL 表示不创建）
    const char *log_file;          // 日志文件路径（NULL 表示 /dev/null）
    const char *error_file;        // 错误文件路径（NULL 表示与 log_file 相同）
    mode_t umask_value;            // 文件创建掩码（默认 0）
    bool close_all_fds;            // 是否关闭所有文件描述符（默认 true）
    bool redirect_stdio;           // 是否重定向标准 IO（默认 true）
    daemon_signal_handler_t signal_handler; // 自定义信号处理函数
} daemon_config_t;

// 守护进程上下文
typedef struct {
    pid_t pid;                     // 守护进程 PID
    char pid_file_path[256];       // PID 文件路径
    char log_file_path[256];       // 日志文件路径
    bool running;                  // 是否正在运行
} daemon_context_t;

// 获取默认守护进程配置
// name: 守护进程名称
// 返回: 默认配置
daemon_config_t daemon_default_config(const char *name);

// 创建守护进程
// config: 配置（NULL 使用默认配置）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回守护进程上下文，失败返回 NULL
daemon_context_t* daemon_create(const daemon_config_t *config, daemon_error_t *error);

// 释放守护进程上下文
// ctx: 守护进程上下文
void daemon_free(daemon_context_t *ctx);

// 检查守护进程是否已运行
// pid_file: PID 文件路径
// 返回: 运行中返回进程 PID，否则返回 -1
pid_t daemon_is_running(const char *pid_file);

// 停止守护进程
// pid_file: PID 文件路径
// timeout_ms: 等待超时（毫秒，-1 表示无限等待）
// 返回: 成功返回 true，失败返回 false
bool daemon_stop(const char *pid_file, int timeout_ms);

// 重新加载守护进程配置
// pid_file: PID 文件路径
// 返回: 成功返回 true，失败返回 false
bool daemon_reload(const char *pid_file);

// 安装信号处理器
// handler: 信号处理函数
// 返回: 成功返回 true，失败返回 false
bool daemon_setup_signals(daemon_signal_handler_t handler);

// 等待守护进程退出
// ctx: 守护进程上下文
// timeout_ms: 等待超时（毫秒，-1 表示无限等待）
// 返回: 成功返回 true，超时返回 false
bool daemon_wait(daemon_context_t *ctx, int timeout_ms);

// 获取错误信息
// error: 错误码
// 返回: 错误信息字符串
const char* daemon_strerror(daemon_error_t error);

// 写入 PID 文件
// pid_file: PID 文件路径
// pid: 进程 ID
// 返回: 成功返回 true，失败返回 false
bool daemon_write_pid(const char *pid_file, pid_t pid);

// 删除 PID 文件
// pid_file: PID 文件路径
// 返回: 成功返回 true，失败返回 false
bool daemon_remove_pid(const char *pid_file);

#ifdef __cplusplus
}
#endif

#endif // C_UTILS_DAEMON_H
