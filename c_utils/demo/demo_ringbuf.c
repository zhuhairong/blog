/**
 * 环形缓冲区演示程序
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/ringbuf.h"

static void demo_basic(void) {
    printf("\n=== 演示 1: 基本创建和使用 ===\n");

    printf("创建环形缓冲区 (容量: 64 字节)...\n");
    ringbuf_t *rb = ringbuf_create(64);
    if (!rb) {
        printf("创建失败\n");
        return;
    }
    printf("创建成功!\n");

    printf("\n写入数据...\n");
    const char *data = "Hello, Ring Buffer!";
    size_t written = ringbuf_write(rb, (const uint8_t*)data, strlen(data));
    printf("  写入: \"%s\" (%zu 字节)\n", data, written);

    printf("\n读取数据...\n");
    uint8_t buffer[64];
    size_t read_len = ringbuf_read(rb, buffer, sizeof(buffer) - 1);
    buffer[read_len] = '\0';
    printf("  读取: \"%s\" (%zu 字节)\n", buffer, read_len);

    printf("\n状态查询:\n");
    printf("  大小: %zu\n", ringbuf_size(rb));
    printf("  可用: %zu\n", ringbuf_avail(rb));
    printf("  是否为空: %s\n", ringbuf_is_empty(rb) ? "是" : "否");
    printf("  是否已满: %s\n", ringbuf_is_full(rb) ? "是" : "否");

    ringbuf_free(rb);
    printf("\n缓冲区已释放\n");
}

static void demo_overflow(void) {
    printf("\n=== 演示 2: 写入超过容量 ===\n");

    printf("创建缓冲区 (容量: 16 字节)...\n");
    ringbuf_t *rb = ringbuf_create(16);
    if (!rb) {
        printf("创建失败\n");
        return;
    }

    printf("\n写入数据...\n");
    const char *data1 = "12345678";
    const char *data2 = "ABCDEFGH";

    size_t w1 = ringbuf_write(rb, (const uint8_t*)data1, strlen(data1));
    printf("  写入: \"%s\" (%zu 字节, 大小: %zu)\n", data1, w1, ringbuf_size(rb));

    size_t w2 = ringbuf_write(rb, (const uint8_t*)data2, strlen(data2));
    printf("  写入: \"%s\" (%zu 字节, 大小: %zu)\n", data2, w2, ringbuf_size(rb));

    printf("\n缓冲区已满: %s\n", ringbuf_is_full(rb) ? "是" : "否");

    printf("\n读取所有数据...\n");
    uint8_t buffer[32];
    size_t read_len = ringbuf_read(rb, buffer, sizeof(buffer) - 1);
    buffer[read_len] = '\0';
    printf("  读取: \"%s\" (%zu 字节)\n", buffer, read_len);

    ringbuf_free(rb);
}

static void demo_multiple_rw(void) {
    printf("\n=== 演示 3: 多次读写 ===\n");

    ringbuf_t *rb = ringbuf_create(32);
    if (!rb) {
        printf("创建失败\n");
        return;
    }

    printf("循环写入和读取:\n");
    for (int round = 1; round <= 3; round++) {
        char data[16];
        snprintf(data, sizeof(data), "Round%d", round);
        
        size_t written = ringbuf_write(rb, (uint8_t*)data, strlen(data));
        printf("  写入: \"%s\" (%zu 字节)\n", data, written);
        
        uint8_t buffer[16];
        size_t read_len = ringbuf_read(rb, buffer, sizeof(buffer) - 1);
        buffer[read_len] = '\0';
        printf("  读取: \"%s\" (%zu 字节)\n", buffer, read_len);
        printf("  大小: %zu\n\n", ringbuf_size(rb));
    }

    ringbuf_free(rb);
}

int main(void) {
    printf("========================================\n");
    printf("    环形缓冲区演示\n");
    printf("========================================\n");

    demo_basic();
    demo_overflow();
    demo_multiple_rw();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
