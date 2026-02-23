#ifndef C_UTILS_NET_H
#define C_UTILS_NET_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    typedef SOCKET socket_t;
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    typedef int socket_t;
    #define INVALID_SOCKET (-1)
    #define SOCKET_ERROR (-1)
#endif

/**
 * @brief 网络错误码
 */
typedef enum {
    NET_OK = 0,                  /**< 成功 */
    NET_ERROR_NULL_PTR,          /**< 空指针错误 */
    NET_ERROR_INIT,              /**< 初始化失败 */
    NET_ERROR_SOCKET_CREATE,     /**< 套接字创建失败 */
    NET_ERROR_BIND,              /**< 绑定失败 */
    NET_ERROR_LISTEN,            /**< 监听失败 */
    NET_ERROR_ACCEPT,            /**< 接受连接失败 */
    NET_ERROR_CONNECT,           /**< 连接失败 */
    NET_ERROR_SEND,              /**< 发送失败 */
    NET_ERROR_RECV,              /**< 接收失败 */
    NET_ERROR_CLOSE,             /**< 关闭失败 */
    NET_ERROR_GETADDRINFO,       /**< 获取地址信息失败 */
    NET_ERROR_INVALID_ARGS,      /**< 无效参数 */
    NET_ERROR_TIMEOUT,           /**< 超时 */
    NET_ERROR_MAX                /**< 最大错误码 */
} net_error_t;

/**
 * @brief 网络配置
 */
typedef struct {
    bool non_blocking;           /**< 是否非阻塞 */
    int backlog;                 /**< 监听队列长度 */
    int send_timeout_ms;         /**< 发送超时 (毫秒) */
    int recv_timeout_ms;         /**< 接收超时 (毫秒) */
    bool reuse_addr;             /**< 是否重用地址 */
    bool reuse_port;             /**< 是否重用端口 */
    int buffer_size;             /**< 缓冲区大小 */
} net_config_t;

/**
 * @brief 网络地址信息
 */
typedef struct {
    char ip[INET6_ADDRSTRLEN];   /**< IP 地址字符串 */
    char port[6];                /**< 端口字符串 */
    int family;                  /**< 地址族 (AF_INET, AF_INET6) */
} net_addr_t;

/**
 * @brief 套接字状态
 */
typedef struct {
    bool is_valid;               /**< 是否有效 */
    bool is_connected;           /**< 是否已连接 */
    bool is_listening;           /**< 是否在监听 */
    bool is_non_blocking;        /**< 是否非阻塞 */
    net_addr_t local_addr;       /**< 本地地址 */
    net_addr_t peer_addr;        /**< 对端地址 */
} net_socket_state_t;

/**
 * @brief 获取默认配置
 * @return 默认配置
 */
net_config_t net_default_config(void);

/**
 * @brief 初始化网络库
 * @return 是否成功
 */
bool net_init(void);

/**
 * @brief 初始化网络库（带错误码）
 * @param error 错误码输出
 * @return 是否成功
 */
bool net_init_ex(net_error_t *error);

/**
 * @brief 清理网络库
 */
void net_cleanup(void);

/**
 * @brief 创建并监听 TCP 套接字
 * @param port 端口号
 * @return 套接字描述符，失败返回 INVALID_SOCKET
 */
socket_t net_listen(const char *port);

/**
 * @brief 使用自定义配置创建并监听 TCP 套接字
 * @param port 端口号
 * @param config 配置选项
 * @param error 错误码输出
 * @return 套接字描述符，失败返回 INVALID_SOCKET
 */
socket_t net_listen_ex(const char *port, const net_config_t *config, net_error_t *error);

/**
 * @brief 接受新的连接
 * @param server_fd 服务器套接字描述符
 * @return 新连接的套接字描述符，失败返回 INVALID_SOCKET
 */
socket_t net_accept(socket_t server_fd);

/**
 * @brief 接受新的连接（带地址信息）
 * @param server_fd 服务器套接字描述符
 * @param client_addr 客户端地址信息输出
 * @param error 错误码输出
 * @return 新连接的套接字描述符，失败返回 INVALID_SOCKET
 */
socket_t net_accept_ex(socket_t server_fd, net_addr_t *client_addr, net_error_t *error);

/**
 * @brief 连接到 TCP 服务器
 * @param host 主机名或 IP 地址
 * @param port 端口号
 * @return 套接字描述符，失败返回 INVALID_SOCKET
 */
socket_t net_connect(const char *host, const char *port);

/**
 * @brief 使用自定义配置连接到 TCP 服务器
 * @param host 主机名或 IP 地址
 * @param port 端口号
 * @param config 配置选项
 * @param error 错误码输出
 * @return 套接字描述符，失败返回 INVALID_SOCKET
 */
socket_t net_connect_ex(const char *host, const char *port, const net_config_t *config, net_error_t *error);

/**
 * @brief 发送数据
 * @param fd 套接字描述符
 * @param data 数据指针
 * @param len 数据长度
 * @return 发送的字节数，失败返回 SOCKET_ERROR
 */
int net_send(socket_t fd, const void *data, size_t len);

/**
 * @brief 发送数据（带错误码）
 * @param fd 套接字描述符
 * @param data 数据指针
 * @param len 数据长度
 * @param error 错误码输出
 * @return 发送的字节数，失败返回 SOCKET_ERROR
 */
int net_send_ex(socket_t fd, const void *data, size_t len, net_error_t *error);

/**
 * @brief 接收数据
 * @param fd 套接字描述符
 * @param buf 缓冲区指针
 * @param len 缓冲区长度
 * @return 接收的字节数，失败返回 SOCKET_ERROR
 */
int net_recv(socket_t fd, void *buf, size_t len);

/**
 * @brief 接收数据（带错误码）
 * @param fd 套接字描述符
 * @param buf 缓冲区指针
 * @param len 缓冲区长度
 * @param error 错误码输出
 * @return 接收的字节数，失败返回 SOCKET_ERROR
 */
int net_recv_ex(socket_t fd, void *buf, size_t len, net_error_t *error);

/**
 * @brief 关闭套接字
 * @param fd 套接字描述符
 */
void net_close(socket_t fd);

/**
 * @brief 关闭套接字（带错误码）
 * @param fd 套接字描述符
 * @param error 错误码输出
 * @return 是否成功
 */
bool net_close_ex(socket_t fd, net_error_t *error);

/**
 * @brief 设置套接字为非阻塞模式
 * @param fd 套接字描述符
 * @param non_blocking 是否非阻塞
 * @param error 错误码输出
 * @return 是否成功
 */
bool net_set_non_blocking(socket_t fd, bool non_blocking, net_error_t *error);

/**
 * @brief 设置套接字超时
 * @param fd 套接字描述符
 * @param send_timeout_ms 发送超时 (毫秒)
 * @param recv_timeout_ms 接收超时 (毫秒)
 * @param error 错误码输出
 * @return 是否成功
 */
bool net_set_timeout(socket_t fd, int send_timeout_ms, int recv_timeout_ms, net_error_t *error);

/**
 * @brief 获取套接字地址信息
 * @param fd 套接字描述符
 * @param local 本地地址信息输出
 * @param peer 对端地址信息输出
 * @param error 错误码输出
 * @return 是否成功
 */
bool net_get_addrinfo(socket_t fd, net_addr_t *local, net_addr_t *peer, net_error_t *error);

/**
 * @brief 获取套接字状态
 * @param fd 套接字描述符
 * @param state 状态输出
 * @param error 错误码输出
 * @return 是否成功
 */
bool net_get_socket_state(socket_t fd, net_socket_state_t *state, net_error_t *error);

/**
 * @brief 解析 IP 地址字符串
 * @param ip_str IP 地址字符串
 * @param addr 地址结构体输出
 * @param error 错误码输出
 * @return 是否成功
 */
bool net_parse_ip(const char *ip_str, struct sockaddr_storage *addr, net_error_t *error);

/**
 * @brief 将地址结构体转换为字符串
 * @param addr 地址结构体
 * @param ip_str IP 地址字符串输出
 * @param ip_str_len 字符串长度
 * @param port 端口号输出
 * @param error 错误码输出
 * @return 是否成功
 */
bool net_addr_to_string(const struct sockaddr_storage *addr, char *ip_str, size_t ip_str_len, uint16_t *port, net_error_t *error);

/**
 * @brief 获取错误信息
 * @param error 错误码
 * @return 错误信息字符串
 */
const char* net_error_string(net_error_t error);

#endif // C_UTILS_NET_H
