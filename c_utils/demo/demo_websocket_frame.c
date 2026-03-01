/**
 * WebSocket帧处理演示程序
 *
 * 功能：
 * - WebSocket帧头编码
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/websocket_frame.h"

// 演示 1: 基本帧头编码
static void demo_basic_encode(void) {
    printf("\n=== 演示 1: 基本帧头编码 ===\n");

    // 准备数据
    const char *message = "Hello, WebSocket!";
    size_t message_len = strlen(message);

    // 创建帧头
    ws_frame_hdr_t hdr = {
        .fin = 1,                  // 最后一帧
        .opcode = 1,               // 文本帧
        .masked = 1,               // 客户端发送的帧需要掩码
        .payload_len = message_len
    };

    // 生成随机掩码
    uint8_t mask[4] = {0x12, 0x34, 0x56, 0x78}; // 手动设置掩码
    memcpy(hdr.mask, mask, 4);

    // 分配缓冲区
    uint8_t buffer[128];

    // 编码帧头
    size_t hdr_size = ws_frame_encode_hdr(&hdr, buffer);
    printf("帧头编码大小: %zu 字节\n", hdr_size);

    // 显示帧头内容
    printf("帧头内容: ");
    for (size_t i = 0; i < hdr_size; i++) {
        printf("%02X ", buffer[i]);
    }
    printf("\n");
}

// 演示 2: 不同负载长度的帧头
static void demo_header_lengths(void) {
    printf("\n=== 演示 2: 不同负载长度的帧头 ===\n");

    // 测试不同长度的负载
    uint64_t payload_lengths[] = {125, 126, 65536};
    size_t lengths_count = sizeof(payload_lengths) / sizeof(payload_lengths[0]);

    for (size_t i = 0; i < lengths_count; i++) {
        uint64_t len = payload_lengths[i];

        // 创建帧头
        ws_frame_hdr_t hdr = {
            .fin = 1,
            .opcode = 1,
            .masked = 1,
            .payload_len = len
        };

        // 生成随机掩码
        uint8_t mask[4] = {0x12, 0x34, 0x56, 0x78};
        memcpy(hdr.mask, mask, 4);

        // 分配缓冲区
        uint8_t buffer[128];

        // 编码帧头
        size_t hdr_size = ws_frame_encode_hdr(&hdr, buffer);
        printf("负载长度 %llu: 帧头大小 %zu 字节\n", len, hdr_size);

        // 显示帧头内容
        printf("  帧头内容: ");
        for (size_t j = 0; j < hdr_size; j++) {
            printf("%02X ", buffer[j]);
        }
        printf("\n");
    }
}

// 演示 3: 不同类型的帧
static void demo_frame_types(void) {
    printf("\n=== 演示 3: 不同类型的帧 ===\n");

    // 帧类型定义
    struct {
        uint8_t opcode;
        const char *name;
    } frame_types[] = {
        {0x00, "继续帧"},
        {0x01, "文本帧"},
        {0x02, "二进制帧"},
        {0x08, "关闭帧"},
        {0x09, " Ping 帧"},
        {0x0A, " Pong 帧"}
    };

    size_t types_count = sizeof(frame_types) / sizeof(frame_types[0]);

    for (size_t i = 0; i < types_count; i++) {
        uint8_t opcode = frame_types[i].opcode;
        const char *name = frame_types[i].name;

        // 创建帧头
        ws_frame_hdr_t hdr = {
            .fin = 1,
            .opcode = opcode,
            .masked = 1,
            .payload_len = 0
        };

        // 生成随机掩码
        uint8_t mask[4] = {0x12, 0x34, 0x56, 0x78};
        memcpy(hdr.mask, mask, 4);

        // 分配缓冲区
        uint8_t buffer[128];

        // 编码帧头
        size_t hdr_size = ws_frame_encode_hdr(&hdr, buffer);
        printf("%s (opcode: 0x%02X): 帧头大小 %zu 字节\n", name, opcode, hdr_size);
    }
}

int main(void) {
    printf("========================================\n");
    printf("    WebSocket帧处理演示\n");
    printf("========================================\n");

    demo_basic_encode();
    demo_header_lengths();
    demo_frame_types();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
