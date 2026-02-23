#ifndef C_UTILS_SEM_H
#define C_UTILS_SEM_H

#include <semaphore.h>
#include <stdbool.h>

typedef sem_t* sem_handle_t;

typedef enum {
    SEM_OK = 0,
    SEM_ERROR_INVALID_PARAMS = -1,
    SEM_ERROR_CREATE = -2,
    SEM_ERROR_OPEN = -3,
    SEM_ERROR_WAIT = -4,
    SEM_ERROR_POST = -5,
    SEM_ERROR_TRYWAIT = -6,
    SEM_ERROR_TIMEDWAIT = -7,
    SEM_ERROR_GETVALUE = -8,
    SEM_ERROR_CLOSE = -9,
    SEM_ERROR_UNLINK = -10,
    SEM_ERROR_MEMORY = -11
} sem_error_t;

typedef struct {
    bool create_if_not_exists;  // 如果信号量不存在则创建
    bool exclusive;             // 独占模式
    bool unlink_on_close;       // 关闭时自动删除
    unsigned int max_value;     // 信号量最大值（0表示无限制）
} sem_config_t;

typedef struct {
    sem_error_t last_error;     // 最后一次错误
    int error_code;             // 系统错误码
    unsigned int current_value; // 当前信号量值
    bool is_initialized;        // 是否初始化
    bool is_named;              // 是否为命名信号量
} sem_state_t;

/**
 * @brief 创建信号量
 * @param name 信号量名称（NULL表示匿名信号量）
 * @param value 初始值
 * @return 信号量句柄，失败返回NULL
 */
sem_handle_t sem_create(const char *name, unsigned int value);

/**
 * @brief 增强版信号量创建
 * @param name 信号量名称（NULL表示匿名信号量）
 * @param value 初始值
 * @param config 配置选项
 * @param state 状态输出
 * @return 信号量句柄，失败返回NULL
 */
sem_handle_t sem_create_ex(const char *name, unsigned int value, const sem_config_t *config, sem_state_t *state);

/**
 * @brief 打开已存在的信号量
 * @param name 信号量名称
 * @param state 状态输出
 * @return 信号量句柄，失败返回NULL
 */
sem_handle_t sem_open_ex(const char *name, sem_state_t *state);

/**
 * @brief 关闭并删除信号量
 * @param sem 信号量句柄
 * @param name 信号量名称（命名信号量需要）
 */
void sem_close_delete(sem_handle_t sem, const char *name);

/**
 * @brief 增强版关闭并删除信号量
 * @param sem 信号量句柄
 * @param name 信号量名称（命名信号量需要）
 * @param state 状态输出
 * @return 错误码
 */
sem_error_t sem_close_delete_ex(sem_handle_t sem, const char *name, sem_state_t *state);

/**
 * @brief 等待信号量（阻塞）
 * @param sem 信号量句柄
 */
void sem_p(sem_handle_t sem);

/**
 * @brief 增强版等待信号量
 * @param sem 信号量句柄
 * @param state 状态输出
 * @return 错误码
 */
sem_error_t sem_p_ex(sem_handle_t sem, sem_state_t *state);

/**
 * @brief 尝试等待信号量（非阻塞）
 * @param sem 信号量句柄
 * @param state 状态输出
 * @return 错误码，SEM_OK表示成功获取
 */
sem_error_t sem_trywait_ex(sem_handle_t sem, sem_state_t *state);

/**
 * @brief 超时等待信号量
 * @param sem 信号量句柄
 * @param timeout_ms 超时时间（毫秒）
 * @param state 状态输出
 * @return 错误码，SEM_OK表示成功获取
 */
sem_error_t sem_timedwait_ex(sem_handle_t sem, unsigned int timeout_ms, sem_state_t *state);

/**
 * @brief 发布信号量
 * @param sem 信号量句柄
 */
void sem_v(sem_handle_t sem);

/**
 * @brief 增强版发布信号量
 * @param sem 信号量句柄
 * @param state 状态输出
 * @return 错误码
 */
sem_error_t sem_v_ex(sem_handle_t sem, sem_state_t *state);

/**
 * @brief 获取信号量当前值
 * @param sem 信号量句柄
 * @param value 输出值
 * @param state 状态输出
 * @return 错误码
 */
sem_error_t sem_get_value(sem_handle_t sem, int *value, sem_state_t *state);

/**
 * @brief 重置信号量值
 * @param sem 信号量句柄
 * @param value 新值
 * @param state 状态输出
 * @return 错误码
 */
sem_error_t sem_reset(sem_handle_t sem, unsigned int value, sem_state_t *state);

/**
 * @brief 销毁匿名信号量
 * @param sem 信号量句柄
 * @param state 状态输出
 * @return 错误码
 */
sem_error_t sem_destroy_ex(sem_handle_t sem, sem_state_t *state);

/**
 * @brief 获取最后一次错误信息
 * @param state 状态
 * @return 错误描述字符串
 */
const char* sem_strerror(const sem_state_t *state);

/**
 * @brief 初始化配置为默认值
 * @param config 配置结构
 */
void sem_config_init(sem_config_t *config);

/**
 * @brief 初始化状态为默认值
 * @param state 状态结构
 */
void sem_state_init(sem_state_t *state);

#endif // C_UTILS_SEM_H
