/**
 * 环形缓冲区(增强版)演示程序
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/ringbuffer.h"

static void demo_basic(void) {
    printf("\n=== 演示 1: 基本创建和使用 ===\n");

    printf("创建环形缓冲区 (大小: 64 字节)...\n");
    ringbuffer_t *rb = ringbuffer_create(64);
    if (!rb) {
        printf("创建失败\n");
        return;
    }
    printf("创建成功!\n");

    printf("\n写入数据...\n");
    const char *data = "Hello, Ringbuffer!";
    size_t written = ringbuffer_write(rb, data, strlen(data));
    printf("  写入: \"%s\" (%zu 字节)\n", data, written);

    printf("\n状态查询:\n");
    printf("  可读: %zu\n", ringbuffer_available_read(rb));
    printf("  可写: %zu\n", ringbuffer_available_write(rb));
    printf("  是否为空: %s\n", ringbuffer_is_empty(rb) ? "是" : "否");
    printf("  是否已满: %s\n", ringbuffer_is_full(rb) ? "是" : "否");

    printf("\n读取数据...\n");
    char buffer[64];
    size_t read_len = ringbuffer_read(rb, buffer, sizeof(buffer) - 1);
    buffer[read_len] = '\0';
    printf("  读取: \"%s\" (%zu 字节)\n", buffer, read_len);

    ringbuffer_free(rb);
    printf("\n缓冲区已释放\n");
}

static void demo_overflow(void) {
    printf("\n=== 演示 2: 写入超过容量 ===\n");

    printf("创建缓冲区 (大小: 16 字节)...\n");
    ringbuffer_t *rb = ringbuffer_create(16);
    if (!rb) {
        printf("创建失败\n");
        return;
    }

    printf("\n写入数据...\n");
    const char *data1 = "12345678";
    const char *data2 = "ABCDEFGH";

    size_t w1 = ringbuffer_write(rb, data1, strlen(data1));
    printf("  写入: \"%s\" (%zu 字节)\n", data1, w1);
    printf("  可读: %zu, 可写: %zu\n", ringbuffer_available_read(rb), ringbuffer_available_write(rb));

    size_t w2 = ringbuffer_write(rb, data2, strlen(data2));
    printf("  写入: \"%s\" (%zu 字节)\n", data2, w2);
    printf("  可读: %zu, 可写: %zu\n", ringbuffer_available_read(rb), ringbuffer_available_write(rb));

    printf("\n缓冲区已满: %s\n", ringbuffer_is_full(rb) ? "是" : "否");

    printf("\n读取所有数据...\n");
    char buffer[32];
    size_t read_len = ringbuffer_read(rb, buffer, sizeof(buffer) - 1);
    buffer[read_len] = '\0';
    printf("  读取: \"%s\" (%zu 字节)\n", buffer, read_len);

    ringbuffer_free(rb);
}

static void demo_multiple_rw(void) {
    printf("\n=== 演示 3: 多次读写 ===\n");

    ringbuffer_t *rb = ringbuffer_create(32);
    if (!rb) {
        printf("创建失败\n");
        return;
    }

    printf("循环写入和读取:\n");
    for (int round = 1; round <= 3; round++) {
        char data[16];
        snprintf(data, sizeof(data), "Data%d", round);
        
        size_t written = ringbuffer_write(rb, data, strlen(data));
        printf("  写入: \"%s\" (%zu 字节)\n", data, written);
        
        char buffer[16];
        size_t read_len = ringbuffer_read(rb, buffer, sizeof(buffer) - 1);
        buffer[read_len] = '\0';
        printf("  读取: \"%s\" (%zu 字节)\n", buffer, read_len);
        printf("  可读: %zu\n\n", ringbuffer_available_read(rb));
    }

    ringbuffer_free(rb);
}

int main(void) {
    printf("========================================\n");
    printf("    环形缓冲区(增强版)演示\n");
    printf("========================================\n");

    demo_basic();
    demo_overflow();
    demo_multiple_rw();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
