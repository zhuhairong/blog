/**
 * 网络工具演示程序
 *
 * 功能：
 * - 网络初始化和清理
 * - 套接字创建和连接
 * - 数据发送和接收
 * - TCP 客户端-服务器通信示例
 * - 并发测试、大数据传输测试、超时测试
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdatomic.h>
#include "../c_utils/net.h"

#define DEFAULT_BUFFER_SIZE 1024
#define LARGE_DATA_SIZE_1KB 1024
#define LARGE_DATA_SIZE_1MB (1024 * 1024)
#define MAX_CONCURRENT_CLIENTS 5
#define SERVER_READY_TIMEOUT_MS 100
#define SERVER_READY_POLL_INTERVAL_US 10000
#define SHORT_WAIT_US 100000
#define MEDIUM_WAIT_US 500000
#define TEST_TIMEOUT_MS 1000

typedef struct {
    int port;
    atomic_int *server_ready;
    atomic_int *client_count;
    int max_clients;
} server_args_t;

typedef struct {
    int port;
    atomic_int *server_ready;
    size_t data_size;
    atomic_int success_count;
    atomic_int fail_count;
} data_server_args_t;

typedef struct {
    int port;
    int client_id;
    size_t data_size;
    atomic_int *success_count;
    atomic_int *fail_count;
} client_args_t;

static void *echo_server_thread(void *arg) {
    server_args_t *args = (server_args_t *)arg;
    
    net_init();
    
    socket_t server_fd = net_listen("0");
    if (server_fd == INVALID_SOCKET) {
        net_cleanup();
        return NULL;
    }
    
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    getsockname(server_fd, (struct sockaddr *)&addr, &addr_len);
    args->port = ntohs(addr.sin_port);
    
    atomic_store(args->server_ready, 1);
    
    int clients_served = 0;
    while (clients_served < args->max_clients) {
        struct timeval tv = {.tv_sec = 1, .tv_usec = 0};
        setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
        
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        socket_t client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        
        if (client_fd == INVALID_SOCKET) {
            continue;
        }
        
        if (args->client_count) {
            atomic_fetch_add(args->client_count, 1);
        }
        
        char buffer[DEFAULT_BUFFER_SIZE];
        int received = net_recv(client_fd, buffer, sizeof(buffer) - 1);
        if (received > 0) {
            buffer[received] = '\0';
            const char *response = "Server received: ";
            net_send(client_fd, response, strlen(response));
            net_send(client_fd, buffer, received);
        }
        
        net_close(client_fd);
        clients_served++;
    }
    
    net_close(server_fd);
    net_cleanup();
    return NULL;
}

static void *data_server_thread(void *arg) {
    data_server_args_t *args = (data_server_args_t *)arg;
    
    net_init();
    
    socket_t server_fd = net_listen("0");
    if (server_fd == INVALID_SOCKET) {
        net_cleanup();
        return NULL;
    }
    
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    getsockname(server_fd, (struct sockaddr *)&addr, &addr_len);
    args->port = ntohs(addr.sin_port);
    
    atomic_store(args->server_ready, 1);
    
    struct timeval tv = {.tv_sec = 5, .tv_usec = 0};
    setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    socket_t client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
    
    if (client_fd != INVALID_SOCKET) {
        char *buffer = malloc(args->data_size);
        if (buffer) {
            size_t total_received = 0;
            while (total_received < args->data_size) {
                int received = net_recv(client_fd, buffer + total_received, 
                                       args->data_size - total_received);
                if (received <= 0) break;
                total_received += received;
            }
            
            size_t total_sent = 0;
            while (total_sent < total_received) {
                int sent = net_send(client_fd, buffer + total_sent, 
                                   total_received - total_sent);
                if (sent <= 0) break;
                total_sent += sent;
            }
            
            free(buffer);
        }
        net_close(client_fd);
    }
    
    net_close(server_fd);
    net_cleanup();
    return NULL;
}

static void *client_thread(void *arg) {
    client_args_t *args = (client_args_t *)arg;
    
    net_init();
    
    char port_str[16];
    snprintf(port_str, sizeof(port_str), "%d", args->port);
    
    usleep(SHORT_WAIT_US);
    
    socket_t client_fd = net_connect("127.0.0.1", port_str);
    if (client_fd == INVALID_SOCKET) {
        atomic_fetch_add(args->fail_count, 1);
        net_cleanup();
        return NULL;
    }
    
    char *send_buffer = malloc(args->data_size);
    char *recv_buffer = malloc(args->data_size);
    
    if (!send_buffer || !recv_buffer) {
        if (send_buffer) free(send_buffer);
        if (recv_buffer) free(recv_buffer);
        net_close(client_fd);
        atomic_fetch_add(args->fail_count, 1);
        net_cleanup();
        return NULL;
    }
    
    for (size_t i = 0; i < args->data_size; i++) {
        send_buffer[i] = (char)('A' + (i % 26));
    }
    
    size_t total_sent = 0;
    while (total_sent < args->data_size) {
        int sent = net_send(client_fd, send_buffer + total_sent, 
                           args->data_size - total_sent);
        if (sent <= 0) break;
        total_sent += sent;
    }
    
    size_t total_received = 0;
    while (total_received < args->data_size) {
        int received = net_recv(client_fd, recv_buffer + total_received,
                               args->data_size - total_received);
        if (received <= 0) break;
        total_received += received;
    }
    
    int success = 0;
    if (total_received >= args->data_size) {
        success = 1;
    }
    
    if (success) {
        atomic_fetch_add(args->success_count, 1);
    } else {
        atomic_fetch_add(args->fail_count, 1);
    }
    
    free(send_buffer);
    free(recv_buffer);
    net_close(client_fd);
    net_cleanup();
    return NULL;
}

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

static void demo_tcp_communication(void) {
    printf("\n=== 演示 4: TCP 客户端-服务器通信 ===\n");
    
    atomic_int server_ready = 0;
    int port = 0;
    server_args_t args = {
        .port = port,
        .server_ready = &server_ready,
        .client_count = NULL,
        .max_clients = 1
    };
    
    pthread_t server_thread;
    pthread_create(&server_thread, NULL, echo_server_thread, &args);
    
    int timeout = SERVER_READY_TIMEOUT_MS;
    while (!atomic_load(&server_ready) && timeout-- > 0) {
        usleep(SERVER_READY_POLL_INTERVAL_US);
    }
    
    if (!atomic_load(&server_ready)) {
        printf("  服务器启动超时\n");
        pthread_cancel(server_thread);
        pthread_join(server_thread, NULL);
        return;
    }
    
    usleep(SHORT_WAIT_US);
    
    net_init();
    
    char port_str[16];
    snprintf(port_str, sizeof(port_str), "%d", args.port);
    
    printf("  客户端: 连接到 127.0.0.1:%s...\n", port_str);
    socket_t client_fd = net_connect("127.0.0.1", port_str);
    
    if (client_fd != INVALID_SOCKET) {
        printf("  客户端: 连接成功\n");
        
        const char *message = "Hello, Network!";
        printf("  客户端: 发送数据: %s\n", message);
        int sent = net_send(client_fd, message, strlen(message));
        printf("  客户端: 发送了 %d 字节\n", sent);
        
        char buffer[DEFAULT_BUFFER_SIZE];
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
    
    pthread_join(server_thread, NULL);
}

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

static void demo_config_and_features(void) {
    printf("\n=== 演示 9: 配置选项和高级功能测试 ===\n");
    
    net_init();
    
    printf("配置选项测试:\n");
    printf("  注意: 高级配置选项在当前实现中不可用\n");
    
    printf("\n基本功能测试:\n");
    socket_t server_fd = net_listen("0");
    if (server_fd != INVALID_SOCKET) {
        printf("  ✓ 服务器套接字创建成功\n");
        
        struct sockaddr_in addr;
        socklen_t addr_len = sizeof(addr);
        getsockname(server_fd, (struct sockaddr *)&addr, &addr_len);
        printf("  ✓ 监听端口: %d\n", ntohs(addr.sin_port));
        
        int optval = 0;
        socklen_t optlen = sizeof(optval);
        if (getsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, &optlen) == 0) {
            printf("  ✓ SO_REUSEADDR: %s\n", optval ? "启用" : "禁用");
        }
        
        net_close(server_fd);
    } else {
        printf("  ✗ 服务器套接字创建失败\n");
    }
    
    net_cleanup();
}

static void demo_error_and_boundary_cases(void) {
    printf("\n=== 演示 10: 错误和边界情况测试 ===\n");
    
    net_init();
    
    printf("无效参数测试:\n");
    
    printf("  空指针测试...\n");
    socket_t fd = net_listen(NULL);
    if (fd == INVALID_SOCKET) {
        printf("  ✓ 空指针测试通过 (返回 INVALID_SOCKET)\n");
    } else {
        printf("  ✗ 空指针测试失败\n");
        net_close(fd);
    }
    
    printf("  无效端口测试...\n");
    fd = net_connect("localhost", "99999");
    if (fd == INVALID_SOCKET) {
        printf("  ✓ 无效端口测试通过 (返回 INVALID_SOCKET)\n");
    } else {
        printf("  ✗ 无效端口测试失败\n");
        net_close(fd);
    }
    
    printf("  空地址测试...\n");
    fd = net_connect("", "80");
    if (fd == INVALID_SOCKET) {
        printf("  ✓ 空地址测试通过 (返回 INVALID_SOCKET)\n");
    } else {
        printf("  ✗ 空地址测试失败\n");
        net_close(fd);
    }
    
    printf("\n连接失败测试:\n");
    fd = net_connect("localhost", "9999");
    if (fd == INVALID_SOCKET) {
        printf("  ✓ 连接失败测试通过 (返回 INVALID_SOCKET)\n");
    } else {
        printf("  ✗ 连接失败测试失败\n");
        net_close(fd);
    }
    
    printf("\n无效套接字操作测试:\n");
    int sent = net_send(INVALID_SOCKET, "test", 4);
    if (sent == SOCKET_ERROR) {
        printf("  ✓ 发送失败测试通过 (返回 SOCKET_ERROR)\n");
    } else {
        printf("  ✗ 发送失败测试失败\n");
    }
    
    char buffer[DEFAULT_BUFFER_SIZE];
    int received = net_recv(INVALID_SOCKET, buffer, sizeof(buffer));
    if (received == SOCKET_ERROR) {
        printf("  ✓ 接收失败测试通过 (返回 SOCKET_ERROR)\n");
    } else {
        printf("  ✗ 接收失败测试失败\n");
    }
    
    net_cleanup();
}

static void demo_multiple_clients(void) {
    printf("\n=== 演示 11: 多客户端测试 ===\n");
    
    atomic_int server_ready = 0;
    atomic_int client_count = 0;
    server_args_t args = {
        .port = 0,
        .server_ready = &server_ready,
        .client_count = &client_count,
        .max_clients = 3
    };
    
    pthread_t server_thread;
    pthread_create(&server_thread, NULL, echo_server_thread, &args);
    
    int timeout = SERVER_READY_TIMEOUT_MS;
    while (!atomic_load(&server_ready) && timeout-- > 0) {
        usleep(SERVER_READY_POLL_INTERVAL_US);
    }
    
    if (!atomic_load(&server_ready)) {
        printf("  服务器启动超时\n");
        pthread_cancel(server_thread);
        pthread_join(server_thread, NULL);
        return;
    }
    
    usleep(SHORT_WAIT_US);
    
    char port_str[16];
    snprintf(port_str, sizeof(port_str), "%d", args.port);
    
    printf("  服务器监听端口: %s\n", port_str);
    printf("  测试多次连接尝试...\n");
    
    for (int i = 0; i < 3; i++) {
        net_init();
        socket_t client_fd = net_connect("127.0.0.1", port_str);
        if (client_fd != INVALID_SOCKET) {
            printf("  连接尝试 %d: 成功\n", i+1);
            
            char msg[32];
            snprintf(msg, sizeof(msg), "Client %d", i+1);
            net_send(client_fd, msg, strlen(msg));
            
            char buf[DEFAULT_BUFFER_SIZE];
            int received = net_recv(client_fd, buf, sizeof(buf) - 1);
            if (received > 0) {
                buf[received] = '\0';
                printf("    收到响应: %s\n", buf);
            }
            
            net_close(client_fd);
        } else {
            printf("  连接尝试 %d: 失败\n", i+1);
        }
        net_cleanup();
    }
    
    pthread_join(server_thread, NULL);
    printf("  总共服务了 %d 个客户端\n", atomic_load(&client_count));
}

static void demo_data_transfer(void) {
    printf("\n=== 演示 12: 数据传输测试 ===\n");
    
    struct {
        const char *name;
        size_t size;
    } test_cases[] = {
        {"空数据", 0},
        {"单字节", 1},
        {"小数据(10字节)", 10},
        {"中等数据(100字节)", 100},
        {"大数据(1KB)", LARGE_DATA_SIZE_1KB}
    };
    
    for (size_t t = 0; t < sizeof(test_cases) / sizeof(test_cases[0]); t++) {
        printf("\n测试 %s (%zu 字节):\n", test_cases[t].name, test_cases[t].size);
        
        atomic_int server_ready = 0;
        data_server_args_t server_args = {
            .port = 0,
            .server_ready = &server_ready,
            .data_size = test_cases[t].size
        };
        
        pthread_t server_thread;
        pthread_create(&server_thread, NULL, data_server_thread, &server_args);
        
        int timeout = SERVER_READY_TIMEOUT_MS;
        while (!atomic_load(&server_ready) && timeout-- > 0) {
            usleep(SERVER_READY_POLL_INTERVAL_US);
        }
        
        if (!atomic_load(&server_ready)) {
            printf("  服务器启动超时\n");
            pthread_cancel(server_thread);
            pthread_join(server_thread, NULL);
            continue;
        }
        
        usleep(SHORT_WAIT_US);
        
        net_init();
        
        char port_str[16];
        snprintf(port_str, sizeof(port_str), "%d", server_args.port);
        
        socket_t client_fd = net_connect("127.0.0.1", port_str);
        
        if (client_fd == INVALID_SOCKET) {
            printf("  ✗ 连接失败\n");
            net_cleanup();
            pthread_join(server_thread, NULL);
            continue;
        }
        
        printf("  ✓ 连接成功\n");
        
        if (test_cases[t].size > 0) {
            char *send_buffer = malloc(test_cases[t].size);
            char *recv_buffer = malloc(test_cases[t].size);
            
            if (!send_buffer || !recv_buffer) {
                printf("  ✗ 内存分配失败\n");
                if (send_buffer) free(send_buffer);
                if (recv_buffer) free(recv_buffer);
                net_close(client_fd);
                net_cleanup();
                pthread_join(server_thread, NULL);
                continue;
            }
            
            for (size_t i = 0; i < test_cases[t].size; i++) {
                send_buffer[i] = (char)('A' + (i % 26));
            }
            
            size_t total_sent = 0;
            while (total_sent < test_cases[t].size) {
                int sent = net_send(client_fd, send_buffer + total_sent,
                                   test_cases[t].size - total_sent);
                if (sent <= 0) break;
                total_sent += sent;
            }
            printf("  发送: %zu/%zu 字节\n", total_sent, test_cases[t].size);
            
            size_t total_received = 0;
            while (total_received < test_cases[t].size) {
                int received = net_recv(client_fd, recv_buffer + total_received,
                                       test_cases[t].size - total_received);
                if (received <= 0) break;
                total_received += received;
            }
            printf("  接收: %zu/%zu 字节\n", total_received, test_cases[t].size);
            
            int data_match = (total_sent == test_cases[t].size && 
                            total_received >= test_cases[t].size);
            if (data_match) {
                printf("  ✓ 数据传输测试通过\n");
            } else {
                printf("  ✗ 数据传输测试失败\n");
            }
            
            free(send_buffer);
            free(recv_buffer);
        } else {
            printf("  ✓ 空数据测试通过 (无数据传输)\n");
        }
        
        net_close(client_fd);
        net_cleanup();
        pthread_join(server_thread, NULL);
    }
}

static void demo_concurrent_clients(void) {
    printf("\n=== 演示 13: 并发客户端测试 ===\n");
    
    atomic_int server_ready = 0;
    server_args_t args = {
        .port = 0,
        .server_ready = &server_ready,
        .client_count = NULL,
        .max_clients = MAX_CONCURRENT_CLIENTS
    };
    
    pthread_t server_thread;
    pthread_create(&server_thread, NULL, echo_server_thread, &args);
    
    int timeout = SERVER_READY_TIMEOUT_MS;
    while (!atomic_load(&server_ready) && timeout-- > 0) {
        usleep(SERVER_READY_POLL_INTERVAL_US);
    }
    
    if (!atomic_load(&server_ready)) {
        printf("  服务器启动超时\n");
        pthread_cancel(server_thread);
        pthread_join(server_thread, NULL);
        return;
    }
    
    usleep(SHORT_WAIT_US);
    
    printf("  服务器监听端口: %d\n", args.port);
    printf("  启动 %d 个并发客户端...\n", MAX_CONCURRENT_CLIENTS);
    
    pthread_t client_threads[MAX_CONCURRENT_CLIENTS];
    client_args_t client_args[MAX_CONCURRENT_CLIENTS];
    atomic_int success_count = 0;
    atomic_int fail_count = 0;
    
    for (int i = 0; i < MAX_CONCURRENT_CLIENTS; i++) {
        client_args[i].port = args.port;
        client_args[i].client_id = i;
        client_args[i].data_size = 100;
        client_args[i].success_count = &success_count;
        client_args[i].fail_count = &fail_count;
        pthread_create(&client_threads[i], NULL, client_thread, &client_args[i]);
    }
    
    for (int i = 0; i < MAX_CONCURRENT_CLIENTS; i++) {
        pthread_join(client_threads[i], NULL);
    }
    
    pthread_join(server_thread, NULL);
    
    printf("  并发测试结果: 成功 %d, 失败 %d\n", 
           atomic_load(&success_count), atomic_load(&fail_count));
}

static void demo_large_data_transfer(void) {
    printf("\n=== 演示 14: 大数据传输测试 ===\n");
    
    size_t test_sizes[] = {LARGE_DATA_SIZE_1KB, LARGE_DATA_SIZE_1MB};
    const char *test_names[] = {"1KB", "1MB"};
    
    for (size_t t = 0; t < sizeof(test_sizes) / sizeof(test_sizes[0]); t++) {
        printf("\n测试 %s 数据传输:\n", test_names[t]);
        
        atomic_int server_ready = 0;
        data_server_args_t server_args = {
            .port = 0,
            .server_ready = &server_ready,
            .data_size = test_sizes[t]
        };
        
        pthread_t server_thread;
        pthread_create(&server_thread, NULL, data_server_thread, &server_args);
        
        int timeout = SERVER_READY_TIMEOUT_MS * 10;
        while (!atomic_load(&server_ready) && timeout-- > 0) {
            usleep(SERVER_READY_POLL_INTERVAL_US);
        }
        
        if (!atomic_load(&server_ready)) {
            printf("  服务器启动超时\n");
            pthread_cancel(server_thread);
            pthread_join(server_thread, NULL);
            continue;
        }
        
        usleep(SHORT_WAIT_US);
        
        net_init();
        
        char port_str[16];
        snprintf(port_str, sizeof(port_str), "%d", server_args.port);
        
        socket_t client_fd = net_connect("127.0.0.1", port_str);
        
        if (client_fd == INVALID_SOCKET) {
            printf("  ✗ 连接失败\n");
            net_cleanup();
            pthread_join(server_thread, NULL);
            continue;
        }
        
        printf("  ✓ 连接成功\n");
        
        char *send_buffer = malloc(test_sizes[t]);
        char *recv_buffer = malloc(test_sizes[t]);
        
        if (!send_buffer || !recv_buffer) {
            printf("  ✗ 内存分配失败\n");
            if (send_buffer) free(send_buffer);
            if (recv_buffer) free(recv_buffer);
            net_close(client_fd);
            net_cleanup();
            pthread_join(server_thread, NULL);
            continue;
        }
        
        for (size_t i = 0; i < test_sizes[t]; i++) {
            send_buffer[i] = (char)('A' + (i % 26));
        }
        
        printf("  发送数据中...\n");
        size_t total_sent = 0;
        while (total_sent < test_sizes[t]) {
            int sent = net_send(client_fd, send_buffer + total_sent,
                               test_sizes[t] - total_sent);
            if (sent <= 0) {
                printf("  发送中断\n");
                break;
            }
            total_sent += sent;
        }
        printf("  发送完成: %zu/%zu 字节\n", total_sent, test_sizes[t]);
        
        printf("  接收数据中...\n");
        size_t total_received = 0;
        while (total_received < test_sizes[t]) {
            int received = net_recv(client_fd, recv_buffer + total_received,
                                   test_sizes[t] - total_received);
            if (received <= 0) {
                printf("  接收中断\n");
                break;
            }
            total_received += received;
        }
        printf("  接收完成: %zu/%zu 字节\n", total_received, test_sizes[t]);
        
        int data_match = (total_sent == test_sizes[t] && 
                         total_received >= test_sizes[t]);
        if (data_match) {
            printf("  ✓ %s 数据传输测试通过\n", test_names[t]);
        } else {
            printf("  ✗ %s 数据传输测试失败\n", test_names[t]);
        }
        
        free(send_buffer);
        free(recv_buffer);
        net_close(client_fd);
        net_cleanup();
        pthread_join(server_thread, NULL);
    }
}

static void *timeout_server_thread(void *arg) {
    int *port = (int *)arg;
    
    net_init();
    
    socket_t server_fd = net_listen("0");
    if (server_fd == INVALID_SOCKET) {
        net_cleanup();
        return NULL;
    }
    
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    getsockname(server_fd, (struct sockaddr *)&addr, &addr_len);
    *port = ntohs(addr.sin_port);
    
    struct timeval tv = {.tv_sec = 10, .tv_usec = 0};
    setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    socket_t client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
    
    if (client_fd != INVALID_SOCKET) {
        sleep(5);
        net_close(client_fd);
    }
    
    net_close(server_fd);
    net_cleanup();
    return NULL;
}

static void demo_timeout(void) {
    printf("\n=== 演示 15: 超时测试 ===\n");
    
    printf("连接超时测试:\n");
    net_init();
    
    struct timeval start, end;
    gettimeofday(&start, NULL);
    
    socket_t client_fd = net_connect("10.255.255.1", "80");
    
    gettimeofday(&end, NULL);
    long elapsed_ms = (end.tv_sec - start.tv_sec) * 1000 + 
                     (end.tv_usec - start.tv_usec) / 1000;
    
    if (client_fd == INVALID_SOCKET) {
        printf("  ✓ 连接超时测试通过 (耗时: %ld ms)\n", elapsed_ms);
    } else {
        printf("  ✗ 连接超时测试失败 (应该超时)\n");
        net_close(client_fd);
    }
    
    printf("\n接收超时测试:\n");
    
    int port = 0;
    pthread_t server_thread;
    pthread_create(&server_thread, NULL, timeout_server_thread, &port);
    
    usleep(SHORT_WAIT_US * 2);
    
    char port_str[16];
    snprintf(port_str, sizeof(port_str), "%d", port);
    
    client_fd = net_connect("127.0.0.1", port_str);
    if (client_fd != INVALID_SOCKET) {
        printf("  ✓ 连接成功\n");
        
        char buffer[DEFAULT_BUFFER_SIZE];
        gettimeofday(&start, NULL);
        
        int received = net_recv(client_fd, buffer, sizeof(buffer));
        
        gettimeofday(&end, NULL);
        elapsed_ms = (end.tv_sec - start.tv_sec) * 1000 + 
                     (end.tv_usec - start.tv_usec) / 1000;
        
        printf("  接收操作耗时: %ld ms\n", elapsed_ms);
        
        if (received == SOCKET_ERROR || received == 0) {
            printf("  ✓ 接收超时或连接关闭\n");
        }
        
        net_close(client_fd);
    } else {
        printf("  ✗ 连接失败\n");
    }
    
    net_cleanup();
    pthread_join(server_thread, NULL);
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
    demo_config_and_features();
    demo_error_and_boundary_cases();
    demo_multiple_clients();
    demo_data_transfer();
    demo_concurrent_clients();
    demo_large_data_transfer();
    demo_timeout();
    
    printf("\n========================================\n");
    printf("演示完成!\n");
    
    return 0;
}
