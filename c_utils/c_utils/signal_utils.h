#ifndef C_UTILS_SIGNAL_UTILS_H
#define C_UTILS_SIGNAL_UTILS_H

#include <stdbool.h>
#include <signal.h>
#include <stdint.h>

typedef enum {
    SIGNAL_OK = 0,
    SIGNAL_ERROR_INVALID_PARAMS = -1,
    SIGNAL_ERROR_REGISTRATION = -2,
    SIGNAL_ERROR_DISABLE = -3,
    SIGNAL_ERROR_HANDLER_NULL = -4,
    SIGNAL_ERROR_SIGNAL_INVALID = -5,
    SIGNAL_ERROR_MEMORY = -6
} signal_error_t;

typedef struct {
    bool catch_sigint;          // 捕获 SIGINT
    bool catch_sigterm;         // 捕获 SIGTERM
    bool catch_sigquit;         // 捕获 SIGQUIT
    bool catch_sighup;          // 捕获 SIGHUP
    bool catch_sigpipe;         // 捕获 SIGPIPE
    bool catch_sigsegv;         // 捕获 SIGSEGV (需要谨慎)
    bool use_async_safe;        // 使用异步安全的处理
    bool enable_nested_handlers; // 启用嵌套处理
    size_t max_handlers;        // 最大处理器数量
} signal_config_t;

typedef struct {
    signal_error_t last_error;     // 最后一次错误
    uint32_t signal_count;         // 信号计数
    bool exit_requested;           // 退出请求
    bool is_initialized;           // 是否初始化
    int pending_signal;            // 待处理信号
} signal_state_t;

/**
 * @brief 信号处理函数类型
 */
typedef void (*signal_handler_t)(int signum, siginfo_t *info, void *context);

/**
 * @brief 注册退出信号捕获 (SIGINT, SIGTERM)
 */
void signal_init_exit_handler(void);

/**
 * @brief 增强版信号初始化
 * @param config 配置选项
 * @param state 状态输出
 * @return 错误码
 */
signal_error_t signal_init_ex(const signal_config_t *config, signal_state_t *state);

/**
 * @brief 检查是否收到了退出请求
 * @return 是否收到退出请求
 */
bool signal_is_exit_requested(void);

/**
 * @brief 获取退出请求状态
 * @param state 状态输出
 * @return 错误码
 */
signal_error_t signal_get_exit_status(bool *exit_requested, signal_state_t *state);

/**
 * @brief 注册自定义信号处理器
 * @param signum 信号编号
 * @param handler 处理函数
 * @param state 状态输出
 * @return 错误码
 */
signal_error_t signal_register_handler(int signum, signal_handler_t handler, signal_state_t *state);

/**
 * @brief 移除信号处理器
 * @param signum 信号编号
 * @param state 状态输出
 * @return 错误码
 */
signal_error_t signal_remove_handler(int signum, signal_state_t *state);

/**
 * @brief 阻塞指定信号
 * @param signum 信号编号
 * @param state 状态输出
 * @return 错误码
 */
signal_error_t signal_block(int signum, signal_state_t *state);

/**
 * @brief 解除阻塞指定信号
 * @param signum 信号编号
 * @param state 状态输出
 * @return 错误码
 */
signal_error_t signal_unblock(int signum, signal_state_t *state);

/**
 * @brief 忽略指定信号
 * @param signum 信号编号
 * @param state 状态输出
 * @return 错误码
 */
signal_error_t signal_ignore(int signum, signal_state_t *state);

/**
 * @brief 恢复默认信号处理
 * @param signum 信号编号
 * @param state 状态输出
 * @return 错误码
 */
signal_error_t signal_restore_default(int signum, signal_state_t *state);

/**
 * @brief 发送信号到进程
 * @param pid 进程ID
 * @param signum 信号编号
 * @param state 状态输出
 * @return 错误码
 */
signal_error_t signal_send(pid_t pid, int signum, signal_state_t *state);

/**
 * @brief 获取信号名称
 * @param signum 信号编号
 * @return 信号名称字符串
 */
const char* signal_get_name(int signum);

/**
 * @brief 获取信号描述
 * @param signum 信号编号
 * @return 信号描述字符串
 */
const char* signal_get_description(int signum);

/**
 * @brief 重置信号状态
 * @param state 状态输出
 * @return 错误码
 */
signal_error_t signal_reset(signal_state_t *state);

/**
 * @brief 禁用信号处理
 * @param state 状态输出
 * @return 错误码
 */
signal_error_t signal_disable_all(signal_state_t *state);

/**
 * @brief 获取最后一次错误信息
 * @param state 状态
 * @return 错误描述字符串
 */
const char* signal_strerror(const signal_state_t *state);

/**
 * @brief 初始化配置为默认值
 * @param config 配置结构
 */
void signal_config_init(signal_config_t *config);

/**
 * @brief 初始化状态为默认值
 * @param state 状态结构
 */
void signal_state_init(signal_state_t *state);

#endif // C_UTILS_SIGNAL_UTILS_H
