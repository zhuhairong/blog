/**
 * 网络工具演示程序
 *
 * 功能：
 * - 网络初始化和清理
 * - 套接字创建和连接
 * - 数据发送和接收
 * - TCP 客户端-服务器通信示例
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../c_utils/net.h"

// 演示 1: 网络初始化和清理
static void demo_init_cleanup(void) {
    printf("\n=== 演示 1: 网络初始化和清理 ===\n");

    printf("初始化网络库...\n");
    if (net_init()) {
        printf("  ✓ 网络库初始化成功\n");
        
        printf("执行网络操作...\n");
        
        printf("清理网络库...\n");
        net_cleanup();
        printf("  ✓ 网络库清理完成\n");
    } else {
        printf("  ✗ 网络库初始化失败\n");
    }
}

// 演示 2: 错误码说明
static void demo_errors(void) {
    printf("\n=== 演示 2: 网络错误码 ===\n");
    
    printf("错误码定义:\n");
    printf("  NET_OK = 0                  - 成功\n");
    printf("  NET_ERROR_NULL_PTR = 1      - 空指针错误\n");
    printf("  NET_ERROR_INIT = 2          - 初始化失败\n");
    printf("  NET_ERROR_SOCKET_CREATE = 3 - 套接字创建失败\n");
    printf("  NET_ERROR_BIND = 4          - 绑定失败\n");
    printf("  NET_ERROR_LISTEN = 5        - 监听失败\n");
    printf("  NET_ERROR_ACCEPT = 6        - 接受连接失败\n");
    printf("  NET_ERROR_CONNECT = 7       - 连接失败\n");
    printf("  NET_ERROR_SEND = 8          - 发送失败\n");
    printf("  NET_ERROR_RECV = 9          - 接收失败\n");
    printf("  NET_ERROR_CLOSE = 10        - 关闭失败\n");
    printf("  NET_ERROR_GETADDRINFO = 11  - 地址解析失败\n");
    printf("  NET_ERROR_INVALID_ARGS = 12 - 无效参数\n");
    printf("  NET_ERROR_TIMEOUT = 13      - 超时\n");
}

// 演示 3: 套接字类型和常量
static void demo_socket_types(void) {
    printf("\n=== 演示 3: 套接字类型和常量 ===\n");
    
    printf("跨平台套接字类型:\n");
    printf("  socket_t - 统一的套接字描述符类型\n");
    printf("    - Windows: SOCKET (UINT_PTR)\n");
    printf("    - Linux/Unix: int\n\n");
    
    printf("常量定义:\n");
    printf("  INVALID_SOCKET = %d\n", (int)INVALID_SOCKET);
    printf("  SOCKET_ERROR = %d\n", SOCKET_ERROR);
}

// 服务器线程参数
typedef struct {
    int port;
    int *server_ready;
} server_args_t;

// 简单的 Echo 服务器线程 (使用 POSIX API 接受连接)
static void *echo_server_thread(void *arg) {
    server_args_t *args = (server_args_t *)arg;
    
    net_init();
    
    // 创建监听套接字
    socket_t server_fd = net_listen("0");  // 让系统分配端口
    if (server_fd == INVALID_SOCKET) {
        printf("  服务器: 监听失败\n");
        net_cleanup();
        return NULL;
    }
    
    // 获取实际分配的端口
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    getsockname(server_fd, (struct sockaddr *)&addr, &addr_len);
    int actual_port = ntohs(addr.sin_port);
    args->port = actual_port;
    
    // 通知主线程服务器已就绪
    *args->server_ready = 1;
    
    printf("  服务器: 正在端口 %d 上监听...\n", actual_port);
    
    // 接受连接 (使用 POSIX accept)
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    socket_t client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
    
    if (client_fd != INVALID_SOCKET) {
        printf("  服务器: 客户端已连接 (%s:%d)\n", 
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        
        // 接收数据
        char buffer[1024];
        int received = net_recv(client_fd, buffer, sizeof(buffer) - 1);
        if (received > 0) {
            buffer[received] = '\0';
            printf("  服务器: 收到数据: %s\n", buffer);
            
            // 发送响应
            const char *response = "Server received: ";
            net_send(client_fd, response, strlen(response));
            net_send(client_fd, buffer, received);
            printf("  服务器: 已发送响应\n");
        }
        
        net_close(client_fd);
        printf("  服务器: 客户端连接已关闭\n");
    } else {
        printf("  服务器: 接受连接失败\n");
    }
    
    net_close(server_fd);
    net_cleanup();
    return NULL;
}

// 演示 4: TCP 客户端-服务器通信
static void demo_tcp_communication(void) {
    printf("\n=== 演示 4: TCP 客户端-服务器通信 ===\n");
    
    int server_ready = 0;
    int port = 0;  // 将由服务器分配
    server_args_t args = {
        .port = port,
        .server_ready = &server_ready
    };
    
    // 启动服务器线程
    pthread_t server_thread;
    pthread_create(&server_thread, NULL, echo_server_thread, &args);
    
    // 等待服务器就绪
    int timeout = 100;
    while (!server_ready && timeout-- > 0) {
        usleep(10000); // 10ms
    }
    
    if (!server_ready) {
        printf("  服务器启动超时\n");
        pthread_cancel(server_thread);
        return;
    }
    
    // 给服务器一点时间开始监听
    usleep(100000); // 100ms
    
    // 客户端连接
    net_init();
    
    char port_str[16];
    snprintf(port_str, sizeof(port_str), "%d", args.port);
    
    printf("  客户端: 连接到 127.0.0.1:%s...\n", port_str);
    socket_t client_fd = net_connect("127.0.0.1", port_str);
    
    if (client_fd != INVALID_SOCKET) {
        printf("  客户端: 连接成功\n");
        
        // 发送数据
        const char *message = "Hello, Network!";
        printf("  客户端: 发送数据: %s\n", message);
        int sent = net_send(client_fd, message, strlen(message));
        printf("  客户端: 发送了 %d 字节\n", sent);
        
        // 接收响应
        char buffer[1024];
        int received = net_recv(client_fd, buffer, sizeof(buffer) - 1);
        if (received > 0) {
            buffer[received] = '\0';
            printf("  客户端: 收到响应: %s\n", buffer);
        } else if (received == 0) {
            printf("  客户端: 连接已关闭\n");
        } else {
            printf("  客户端: 接收失败\n");
        }
        
        net_close(client_fd);
        printf("  客户端: 连接已关闭\n");
    } else {
        printf("  客户端: 连接失败\n");
    }
    
    net_cleanup();
    
    // 等待服务器线程结束
    pthread_join(server_thread, NULL);
}

// 演示 5: 网络编程模式
static void demo_patterns(void) {
    printf("\n=== 演示 5: 网络编程模式 ===\n");
    
    printf("TCP 服务器模式:\n");
    printf("  1. net_init()              - 初始化网络库\n");
    printf("  2. net_listen(port)        - 创建监听套接字\n");
    printf("  3. accept()                - 接受客户端连接 (POSIX)\n");
    printf("  4. net_recv(fd, buf, len)  - 接收数据\n");
    printf("  5. net_send(fd, data, len) - 发送数据\n");
    printf("  6. net_close(fd)           - 关闭客户端连接\n");
    printf("  7. net_close(server_fd)    - 关闭监听套接字\n");
    printf("  8. net_cleanup()           - 清理网络库\n\n");
    
    printf("TCP 客户端模式:\n");
    printf("  1. net_init()                     - 初始化网络库\n");
    printf("  2. net_connect(host, port)        - 连接服务器\n");
    printf("  3. net_send(fd, data, len)        - 发送数据\n");
    printf("  4. net_recv(fd, buf, len)         - 接收数据\n");
    printf("  5. net_close(fd)                  - 关闭连接\n");
    printf("  6. net_cleanup()                  - 清理网络库\n");
}

// 演示 6: API 函数说明
static void demo_api_reference(void) {
    printf("\n=== 演示 6: API 函数参考 ===\n");
    
    printf("核心函数:\n\n");
    
    printf("bool net_init(void)\n");
    printf("  初始化网络库\n");
    printf("  返回: 成功返回 true，失败返回 false\n\n");
    
    printf("void net_cleanup(void)\n");
    printf("  清理网络库资源\n\n");
    
    printf("socket_t net_listen(const char *port)\n");
    printf("  创建 TCP 监听套接字\n");
    printf("  参数: port - 端口号字符串 (如 \"8080\", \"0\" 表示自动分配)\n");
    printf("  返回: 套接字描述符，失败返回 INVALID_SOCKET\n\n");
    
    printf("socket_t net_connect(const char *host, const char *port)\n");
    printf("  连接到 TCP 服务器\n");
    printf("  参数: host - 主机名或 IP 地址\n");
    printf("       port - 端口号字符串\n");
    printf("  返回: 套接字描述符，失败返回 INVALID_SOCKET\n\n");
    
    printf("int net_send(socket_t fd, const void *data, size_t len)\n");
    printf("  发送数据\n");
    printf("  参数: fd - 套接字描述符\n");
    printf("       data - 数据指针\n");
    printf("       len - 数据长度\n");
    printf("  返回: 发送的字节数，失败返回 SOCKET_ERROR\n\n");
    
    printf("int net_recv(socket_t fd, void *buf, size_t len)\n");
    printf("  接收数据\n");
    printf("  参数: fd - 套接字描述符\n");
    printf("       buf - 缓冲区指针\n");
    printf("       len - 缓冲区长度\n");
    printf("  返回: 接收的字节数，失败返回 SOCKET_ERROR，连接关闭返回 0\n\n");
    
    printf("void net_close(socket_t fd)\n");
    printf("  关闭套接字\n");
    printf("  参数: fd - 套接字描述符\n");
}

// 演示 7: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 7: 应用场景 ===\n");
    
    printf("1. Web 服务器\n");
    printf("   - 使用 net_listen 创建 HTTP 服务\n");
    printf("   - 使用 accept 处理客户端请求\n");
    printf("   - 使用 net_send 发送 HTTP 响应\n\n");
    
    printf("2. 即时通讯\n");
    printf("   - 使用 net_connect 连接到消息服务器\n");
    printf("   - 使用 net_send/net_recv 收发消息\n\n");
    
    printf("3. 文件传输\n");
    printf("   - 使用 net_listen 等待文件请求\n");
    printf("   - 使用 net_send 发送文件数据\n\n");
    
    printf("4. 物联网网关\n");
    printf("   - 使用 net_listen 接收设备连接\n");
    printf("   - 使用 net_recv 接收传感器数据\n\n");
    
    printf("5. 代理服务\n");
    printf("   - 使用 accept 接收客户端请求\n");
    printf("   - 使用 net_connect 连接目标服务器\n");
    printf("   - 转发数据 between 两端\n");
}

// 演示 8: 跨平台支持
static void demo_cross_platform(void) {
    printf("\n=== 演示 8: 跨平台支持 ===\n");
    
    printf("支持的系统:\n");
    printf("  - Linux / Unix (POSIX)\n");
    printf("  - Windows (Winsock2)\n\n");
    
    printf("平台差异处理:\n");
    printf("  - Windows 需要 WSAStartup/WSACleanup\n");
    printf("  - 套接字类型不同 (SOCKET vs int)\n");
    printf("  - 关闭套接字 (closesocket vs close)\n\n");
    
    printf("统一接口:\n");
    printf("  - socket_t 类型 - 跨平台套接字描述符\n");
    printf("  - INVALID_SOCKET - 统一的无效套接字值\n");
    printf("  - SOCKET_ERROR - 统一的错误返回值\n");
    printf("  - 一致的 API 行为\n");
}

int main(void) {
    printf("========================================\n");
    printf("    网络工具演示\n");
    printf("========================================\n");
    
    demo_init_cleanup();
    demo_errors();
    demo_socket_types();
    demo_tcp_communication();
    demo_patterns();
    demo_api_reference();
    demo_applications();
    demo_cross_platform();
    
    printf("\n========================================\n");
    printf("演示完成!\n");
    
    return 0;
}
