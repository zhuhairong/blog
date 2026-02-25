/**
 * 网络工具演示程序
 *
 * 功能：
 * - 网络初始化和清理
 * - 套接字操作
 * - 地址解析
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/net.h"

// 演示 1: 网络初始化
static void demo_init(void) {
    printf("\n=== 演示 1: 网络库初始化 ===\n");

    printf("初始化网络库...\n");

    if (net_init()) {
        printf("初始化成功!\n");

        printf("\n执行网络操作...\n");

        printf("清理网络库...\n");
        net_cleanup();
        printf("清理完成\n");
    } else {
        printf("初始化失败\n");
    }
}

// 演示 2: 错误码
static void demo_errors(void) {
    printf("\n=== 演示 2: 网络错误码 ===\n");

    printf("可能的错误:\n\n");

    printf("初始化错误:\n");
    printf("  NET_ERROR_INIT - 网络库初始化失败\n\n");

    printf("套接字错误:\n");
    printf("  NET_ERROR_SOCKET_CREATE - 创建失败\n");
    printf("  NET_ERROR_BIND - 绑定失败\n");
    printf("  NET_ERROR_LISTEN - 监听失败\n");
    printf("  NET_ERROR_ACCEPT - 接受连接失败\n\n");

    printf("连接错误:\n");
    printf("  NET_ERROR_CONNECT - 连接失败\n");
    printf("  NET_ERROR_SEND - 发送失败\n");
    printf("  NET_ERROR_RECV - 接收失败\n");
    printf("  NET_ERROR_TIMEOUT - 超时\n\n");

    printf("其他错误:\n");
    printf("  NET_ERROR_GETADDRINFO - 地址解析失败\n");
    printf("  NET_ERROR_INVALID_ARGS - 无效参数\n");
}

// 演示 3: 套接字状态
static void demo_socket_state(void) {
    printf("\n=== 演示 3: 套接字状态 ===\n");

    printf("套接字状态信息:\n\n");

    printf("基本状态:\n");
    printf("  is_valid - 是否有效\n");
    printf("  is_connected - 是否已连接\n");
    printf("  is_listening - 是否在监听\n");
    printf("  is_non_blocking - 是否非阻塞\n\n");

    printf("地址信息:\n");
    printf("  local_addr - 本地地址\n");
    printf("  peer_addr - 对端地址\n\n");

    printf("地址结构:\n");
    printf("  ip - IP 地址字符串\n");
    printf("  port - 端口号\n");
    printf("  family - 地址族 (IPv4/IPv6)\n");
}

// 演示 4: 网络编程模式
static void demo_patterns(void) {
    printf("\n=== 演示 4: 网络编程模式 ===\n");

    printf("TCP 服务器模式:\n");
    printf("  1. 初始化网络库 (net_init)\n");
    printf("  2. 创建监听套接字 (net_listen)\n");
    printf("  3. 接受连接 (net_accept)\n");
    printf("  4. 收发数据\n");
    printf("  5. 关闭连接\n");
    printf("  6. 清理网络库 (net_cleanup)\n\n");

    printf("TCP 客户端模式:\n");
    printf("  1. 初始化网络库 (net_init)\n");
    printf("  2. 连接服务器 (net_connect)\n");
    printf("  3. 收发数据\n");
    printf("  4. 关闭连接\n");
    printf("  5. 清理网络库 (net_cleanup)\n\n");

    printf("UDP 模式:\n");
    printf("  1. 创建 UDP 套接字\n");
    printf("  2. 直接收发数据\n");
}

// 演示 5: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 5: 应用场景 ===\n");

    printf("1. Web 服务器\n");
    printf("   - HTTP 服务\n");
    printf("   - 静态文件服务\n");
    printf("   - API 服务\n\n");

    printf("2. 即时通讯\n");
    printf("   - 聊天服务器\n");
    printf("   - 消息推送\n");
    printf("   - 在线游戏\n\n");

    printf("3. 文件传输\n");
    printf("   - FTP 服务\n");
    printf("   - 文件同步\n");
    printf("   - 远程备份\n\n");

    printf("4. 物联网网关\n");
    printf("   - 设备接入\n");
    printf("   - 协议转换\n");
    printf("   - 数据转发\n\n");

    printf("5. 代理服务\n");
    printf("   - HTTP 代理\n");
    printf("   - SOCKS 代理\n");
    printf("   - 反向代理\n");
}

// 演示 6: 跨平台支持
static void demo_cross_platform(void) {
    printf("\n=== 演示 6: 跨平台支持 ===\n");

    printf("支持的系统:\n");
    printf("  - Linux\n");
    printf("  - Windows (Winsock2)\n");
    printf("  - macOS\n");
    printf("  - 其他 POSIX 系统\n\n");

    printf("平台差异处理:\n");
    printf("  - Windows 需要 WSAStartup\n");
    printf("  - 套接字类型不同 (SOCKET vs int)\n");
    printf("  - 错误码处理方式不同\n\n");

    printf("统一接口:\n");
    printf("  - socket_t 类型\n");
    printf("  - 统一的错误码\n");
    printf("  - 一致的 API 行为\n");
}

// 演示 7: 配置选项说明
static void demo_config_info(void) {
    printf("\n=== 演示 7: 网络配置选项 ===\n");

    printf("网络配置结构体:\n\n");

    printf("基本选项:\n");
    printf("  non_blocking - 非阻塞模式\n");
    printf("  backlog - 监听队列长度\n");
    printf("  send_timeout_ms - 发送超时\n");
    printf("  recv_timeout_ms - 接收超时\n\n");

    printf("高级选项:\n");
    printf("  reuse_addr - 地址重用\n");
    printf("  reuse_port - 端口重用\n");
    printf("  buffer_size - 缓冲区大小\n\n");

    printf("配置说明:\n");
    printf("  - 非阻塞: 操作立即返回，不等待\n");
    printf("  - 监听队列: 待处理连接的最大数量\n");
    printf("  - 超时: 防止操作无限等待\n");
    printf("  - 重用: 快速重启服务时重用地址\n");
}

int main(void) {
    printf("========================================\n");
    printf("    网络工具演示\n");
    printf("========================================\n");

    demo_init();
    demo_errors();
    demo_socket_state();
    demo_patterns();
    demo_applications();
    demo_cross_platform();
    demo_config_info();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
