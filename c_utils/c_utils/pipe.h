#ifndef C_UTILS_PIPE_H
#define C_UTILS_PIPE_H

#include <unistd.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef _WIN32
    #include <windows.h>
    typedef HANDLE pipe_handle_t;
#else
    typedef int pipe_handle_t;
#endif

/**
 * @brief 管道错误码
 */
typedef enum {
    PIPE_OK = 0,                  /**< 成功 */
    PIPE_ERROR_NULL_PTR,          /**< 空指针错误 */
    PIPE_ERROR_CREATE_FAILED,     /**< 管道创建失败 */
    PIPE_ERROR_OPEN_FAILED,       /**< 管道打开失败 */
    PIPE_ERROR_READ_FAILED,       /**< 读取失败 */
    PIPE_ERROR_WRITE_FAILED,      /**< 写入失败 */
    PIPE_ERROR_CLOSE_FAILED,      /**< 关闭失败 */
    PIPE_ERROR_INVALID_HANDLE,    /**< 无效句柄 */
    PIPE_ERROR_BUFFER_TOO_SMALL,  /**< 缓冲区太小 */
    PIPE_ERROR_TIMEOUT,           /**< 超时 */
    PIPE_ERROR_MAX                /**< 最大错误码 */
} pipe_error_t;

/**
 * @brief 管道类型
 */
typedef enum {
    PIPE_TYPE_ANONYMOUS = 0,      /**< 匿名管道 */
    PIPE_TYPE_NAMED,              /**< 命名管道 */
    PIPE_TYPE_MAX                 /**< 最大管道类型 */
} pipe_type_t;

/**
 * @brief 管道配置
 */
typedef struct {
    pipe_type_t type;             /**< 管道类型 */
    const char *name;             /**< 命名管道名称 (仅用于命名管道) */
    bool non_blocking;            /**< 是否非阻塞 */
    size_t buffer_size;           /**< 缓冲区大小 */
    int timeout_ms;               /**< 超时时间 (毫秒，0 表示无限制) */
} pipe_config_t;

/**
 * @brief 管道结构
 */
typedef struct {
    pipe_handle_t read_fd;        /**< 读取文件描述符/句柄 */
    pipe_handle_t write_fd;       /**< 写入文件描述符/句柄 */
    pipe_config_t config;         /**< 管道配置 */
    bool is_open;                 /**< 是否打开 */
} pipe_t;

/**
 * @brief 获取默认配置
 * @return 默认配置
 */
pipe_config_t pipe_default_config(void);

/**
 * @brief 创建匿名管道
 * @param p 管道结构指针
 * @return 是否成功
 */
bool pipe_create(pipe_t *p);

/**
 * @brief 使用自定义配置创建管道
 * @param p 管道结构指针
 * @param config 配置选项
 * @param error 错误码输出
 * @return 是否成功
 */
bool pipe_create_ex(pipe_t *p, const pipe_config_t *config, pipe_error_t *error);

/**
 * @brief 创建命名管道
 * @param p 管道结构指针
 * @param name 管道名称
 * @param error 错误码输出
 * @return 是否成功
 */
bool pipe_create_named(pipe_t *p, const char *name, pipe_error_t *error);

/**
 * @brief 关闭管道
 * @param p 管道结构指针
 */
void pipe_close(pipe_t *p);

/**
 * @brief 关闭管道（带错误码）
 * @param p 管道结构指针
 * @param error 错误码输出
 * @return 是否成功
 */
bool pipe_close_ex(pipe_t *p, pipe_error_t *error);

/**
 * @brief 从管道读取数据
 * @param p 管道结构指针
 * @param buf 缓冲区
 * @param len 缓冲区长度
 * @param bytes_read 读取的字节数输出
 * @param error 错误码输出
 * @return 是否成功
 */
bool pipe_read(pipe_t *p, void *buf, size_t len, size_t *bytes_read, pipe_error_t *error);

/**
 * @brief 向管道写入数据
 * @param p 管道结构指针
 * @param buf 缓冲区
 * @param len 数据长度
 * @param bytes_written 写入的字节数输出
 * @param error 错误码输出
 * @return 是否成功
 */
bool pipe_write(pipe_t *p, const void *buf, size_t len, size_t *bytes_written, pipe_error_t *error);

/**
 * @brief 从管道读取一行数据
 * @param p 管道结构指针
 * @param buf 缓冲区
 * @param len 缓冲区长度
 * @param bytes_read 读取的字节数输出
 * @param error 错误码输出
 * @return 是否成功
 */
bool pipe_read_line(pipe_t *p, char *buf, size_t len, size_t *bytes_read, pipe_error_t *error);

/**
 * @brief 向管道写入一行数据
 * @param p 管道结构指针
 * @param buf 缓冲区
 * @param error 错误码输出
 * @return 是否成功
 */
bool pipe_write_line(pipe_t *p, const char *buf, pipe_error_t *error);

/**
 * @brief 检查管道是否可读
 * @param p 管道结构指针
 * @param timeout_ms 超时时间 (毫秒)
 * @param error 错误码输出
 * @return 是否可读
 */
bool pipe_is_readable(pipe_t *p, int timeout_ms, pipe_error_t *error);

/**
 * @brief 检查管道是否可写
 * @param p 管道结构指针
 * @param timeout_ms 超时时间 (毫秒)
 * @param error 错误码输出
 * @return 是否可写
 */
bool pipe_is_writable(pipe_t *p, int timeout_ms, pipe_error_t *error);

/**
 * @brief 设置管道为非阻塞模式
 * @param p 管道结构指针
 * @param non_blocking 是否非阻塞
 * @param error 错误码输出
 * @return 是否成功
 */
bool pipe_set_non_blocking(pipe_t *p, bool non_blocking, pipe_error_t *error);

/**
 * @brief 获取管道状态
 * @param p 管道结构指针
 * @param is_open 是否打开输出
 * @param error 错误码输出
 * @return 是否成功
 */
bool pipe_get_state(const pipe_t *p, bool *is_open, pipe_error_t *error);

/**
 * @brief 复制管道结构
 * @param dest 目标管道结构
 * @param src 源管道结构
 * @param error 错误码输出
 * @return 是否成功
 */
bool pipe_copy(pipe_t *dest, const pipe_t *src, pipe_error_t *error);

/**
 * @brief 获取错误信息
 * @param error 错误码
 * @return 错误信息字符串
 */
const char* pipe_error_string(pipe_error_t error);

#endif // C_UTILS_PIPE_H
