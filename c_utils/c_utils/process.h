#ifndef C_UTILS_PROCESS_H
#define C_UTILS_PROCESS_H

#include <stddef.h>
#include <stdbool.h>
#include <sys/types.h>

/**
 * @brief 进程错误码
 */
typedef enum {
    PROCESS_OK = 0,                  /**< 成功 */
    PROCESS_ERROR_NULL_PTR,          /**< 空指针错误 */
    PROCESS_ERROR_FORK_FAILED,       /**< 进程创建失败 */
    PROCESS_ERROR_PIPE_FAILED,       /**< 管道创建失败 */
    PROCESS_ERROR_EXEC_FAILED,       /**< 命令执行失败 */
    PROCESS_ERROR_WAIT_FAILED,       /**< 等待进程失败 */
    PROCESS_ERROR_MEMORY_ALLOC,      /**< 内存分配失败 */
    PROCESS_ERROR_TIMEOUT,           /**< 超时 */
    PROCESS_ERROR_MAX                /**< 最大错误码 */
} process_error_t;

/**
 * @brief 进程配置
 */
typedef struct {
    bool capture_stdout;             /**< 是否捕获标准输出 */
    bool capture_stderr;             /**< 是否捕获标准错误 */
    bool merge_stderr;               /**< 是否将标准错误合并到标准输出 */
    int timeout_ms;                  /**< 超时时间 (毫秒，0 表示无限制) */
    size_t max_output_size;          /**< 最大输出大小 (0 表示无限制) */
    const char *working_dir;         /**< 工作目录 */
    char *const *envp;               /**< 环境变量 */
} process_config_t;

/**
 * @brief 进程执行结果
 */
typedef struct {
    int exit_code;                   /**< 退出码 */
    char *stdout;                    /**< 标准输出 */
    size_t stdout_len;               /**< 标准输出长度 */
    char *stderr;                    /**< 标准错误 */
    size_t stderr_len;               /**< 标准错误长度 */
    bool timed_out;                  /**< 是否超时 */
    bool succeeded;                  /**< 是否成功 */
} process_result_t;

/**
 * @brief 获取默认配置
 * @return 默认配置
 */
process_config_t process_default_config(void);

/**
 * @brief 执行命令并获取输出
 * @param cmd 命令字符串
 * @param exit_code 退出码输出
 * @return 命令输出，调用者需 free 返回的字符串
 */
char* process_exec(const char *cmd, int *exit_code);

/**
 * @brief 执行命令并获取详细结果
 * @param cmd 命令字符串
 * @param config 配置选项
 * @param result 结果输出
 * @param error 错误码输出
 * @return 是否成功
 */
bool process_exec_ex(const char *cmd, const process_config_t *config, process_result_t *result, process_error_t *error);

/**
 * @brief 简单的系统命令执行
 * @param cmd 命令字符串
 * @return 退出码
 */
int process_run(const char *cmd);

/**
 * @brief 执行命令（带配置）
 * @param cmd 命令字符串
 * @param config 配置选项
 * @param error 错误码输出
 * @return 退出码
 */
int process_run_ex(const char *cmd, const process_config_t *config, process_error_t *error);

/**
 * @brief 执行命令数组
 * @param argv 命令参数数组
 * @param config 配置选项
 * @param result 结果输出
 * @param error 错误码输出
 * @return 是否成功
 */
bool process_exec_argv(char *const argv[], const process_config_t *config, process_result_t *result, process_error_t *error);

/**
 * @brief 释放进程结果
 * @param result 结果结构
 */
void process_free_result(process_result_t *result);

/**
 * @brief 检查命令是否存在
 * @param cmd 命令名称
 * @return 是否存在
 */
bool process_command_exists(const char *cmd);

/**
 * @brief 获取命令的完整路径
 * @param cmd 命令名称
 * @param error 错误码输出
 * @return 命令路径，调用者需 free 返回的字符串
 */
char* process_which(const char *cmd, process_error_t *error);

/**
 * @brief 获取当前进程 ID
 * @return 进程 ID
 */
pid_t process_get_pid(void);

/**
 * @brief 获取父进程 ID
 * @return 父进程 ID
 */
pid_t process_get_ppid(void);

/**
 * @brief 获取当前工作目录
 * @param error 错误码输出
 * @return 当前工作目录，调用者需 free 返回的字符串
 */
char* process_get_cwd(process_error_t *error);

/**
 * @brief 设置工作目录
 * @param path 目录路径
 * @param error 错误码输出
 * @return 是否成功
 */
bool process_set_cwd(const char *path, process_error_t *error);

/**
 * @brief 获取错误信息
 * @param error 错误码
 * @return 错误信息字符串
 */
const char* process_error_string(process_error_t error);

#endif // C_UTILS_PROCESS_H
