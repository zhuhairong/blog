/**
 * @file demo_varint.c
 * @brief 可变长度整数编码演示
 *
 * 本演示展示了可变长度整数编码的基本操作，包括：
 * - 无符号整数编码和解码
 * - 有符号整数编码和解码（使用ZigZag编码）
 * - 实际应用场景
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../c_utils/varint.h"

// 打印二进制数据
static void print_hex(const char *label, const uint8_t *data, size_t len) {
    printf("%s: ", label);
    for (size_t i = 0; i < len; i++) {
        printf("%02X ", data[i]);
    }
    printf("\n");
}

// ZigZag 编码：将有符号整数转换为无符号整数
static uint64_t zigzag_encode(int64_t val) {
    return (val < 0) ? ((uint64_t)(-val) * 2 - 1) : ((uint64_t)val * 2);
}

// ZigZag 解码：将无符号整数转换为有符号整数
static int64_t zigzag_decode(uint64_t val) {
    return (val % 2 == 0) ? (int64_t)(val / 2) : (int64_t)-((val + 1) / 2);
}

// 演示 1: 无符号整数编码
static void demo_uint_encode(void) {
    printf("\n=== 演示 1: 无符号整数编码 ===\n");

    uint64_t values[] = {
        0,
        1,
        127,
        128,
        255,
        256,
        16383,
        16384,
        2097151,
        2097152,
        268435455,
        268435456
    };
    int num_values = sizeof(values) / sizeof(values[0]);

    for (int i = 0; i < num_values; i++) {
        uint8_t buffer[10];
        size_t encoded_len = varint_encode(values[i], buffer);
        printf("值: %lu (0x%lX)\n", values[i], values[i]);
        print_hex("编码", buffer, encoded_len);
        printf("编码长度: %zu 字节\n\n", encoded_len);
    }
}

// 演示 2: 无符号整数解码
static void demo_uint_decode(void) {
    printf("\n=== 演示 2: 无符号整数解码 ===\n");

    // 测试数据: 编码后的 varint 值
    uint8_t test_cases[][10] = {
        {0x00},                          // 0
        {0x01},                          // 1
        {0x7F},                          // 127
        {0x80, 0x01},                     // 128
        {0xFF, 0x01},                     // 255
        {0x80, 0x02},                     // 256
        {0xFF, 0x7F},                     // 16383
        {0x80, 0x80, 0x01},               // 16384
        {0xFF, 0xFF, 0x7F},               // 2097151
        {0x80, 0x80, 0x80, 0x01},         // 2097152
        {0xFF, 0xFF, 0xFF, 0x7F},         // 268435455
        {0x80, 0x80, 0x80, 0x80, 0x01}   // 268435456
    };
    int num_tests = sizeof(test_cases) / sizeof(test_cases[0]);

    for (int i = 0; i < num_tests; i++) {
        uint64_t value;
        size_t decoded_len = varint_decode(test_cases[i], &value);
        printf("编码数据: ");
        for (size_t j = 0; j < decoded_len; j++) {
            printf("%02X ", test_cases[i][j]);
        }
        printf("\n解码值: %lu (0x%lX)\n", value, value);
        printf("解码长度: %zu 字节\n\n", decoded_len);
    }
}

// 演示 3: 有符号整数编码（使用ZigZag）
static void demo_sint_encode(void) {
    printf("\n=== 演示 3: 有符号整数编码 ===\n");

    int64_t values[] = {
        0,
        1,
        -1,
        63,
        -63,
        64,
        -64,
        8191,
        -8191,
        8192,
        -8192
    };
    int num_values = sizeof(values) / sizeof(values[0]);

    for (int i = 0; i < num_values; i++) {
        // 先进行 ZigZag 编码
        uint64_t zigzag = zigzag_encode(values[i]);
        uint8_t buffer[10];
        size_t encoded_len = varint_encode(zigzag, buffer);
        printf("值: %ld (ZigZag: %lu)\n", values[i], zigzag);
        print_hex("编码", buffer, encoded_len);
        printf("编码长度: %zu 字节\n\n", encoded_len);
    }
}

// 演示 4: 有符号整数解码（使用ZigZag）
static void demo_sint_decode(void) {
    printf("\n=== 演示 4: 有符号整数解码 ===\n");

    // 测试数据: 编码后的 varint 值（已进行 ZigZag 编码）
    uint8_t test_cases[][10] = {
        {0x00},                          // 0
        {0x02},                          // 1
        {0x01},                          // -1
        {0x7E},                          // 63
        {0x7D},                          // -63
        {0x80, 0x01},                     // 64
        {0x81, 0x01},                     // -64
        {0xFE, 0x7F},                     // 8191
        {0xFF, 0x7F},                     // -8191
        {0x80, 0x80, 0x01},               // 8192
        {0x81, 0x80, 0x01}               // -8192
    };
    int num_tests = sizeof(test_cases) / sizeof(test_cases[0]);

    for (int i = 0; i < num_tests; i++) {
        uint64_t zigzag_value;
        size_t decoded_len = varint_decode(test_cases[i], &zigzag_value);
        int64_t value = zigzag_decode(zigzag_value);
        printf("编码数据: ");
        for (size_t j = 0; j < decoded_len; j++) {
            printf("%02X ", test_cases[i][j]);
        }
        printf("\n解码值: %ld (ZigZag: %lu)\n", value, zigzag_value);
        printf("解码长度: %zu 字节\n\n", decoded_len);
    }
}

// 演示 5: 实际应用场景
static void demo_applications(void) {
    printf("\n=== 演示 5: 实际应用场景 ===\n");

    printf("1. 序列化格式:\n");
    printf("   - Protocol Buffers\n");
    printf("   - MessagePack\n");
    printf("   - BSON\n");
    printf("   - FlatBuffers\n");

    printf("\n2. 网络协议:\n");
    printf("   - gRPC\n");
    printf("   - WebSocket\n");
    printf("   - MQTT\n");

    printf("\n3. 存储格式:\n");
    printf("   - 列存储数据库\n");
    printf("   - 日志格式\n");
    printf("   - 配置文件\n");

    printf("\n4. 游戏开发:\n");
    printf("   - 网络同步\n");
    printf("   - 状态序列化\n");
    printf("   - 资源打包\n");

    printf("\n5. 嵌入式系统:\n");
    printf("   - 传感器数据传输\n");
    printf("   - 低带宽通信\n");
    printf("   - 存储优化\n");
}

// 演示 6: 性能对比
static void demo_performance(void) {
    printf("\n=== 演示 6: 性能对比 ===\n");

    printf("Varint 编码 vs 固定长度编码:\n");
    printf("\n值范围         | Varint 字节 | 固定 64 位 | 节省空间\n");
    printf("----------------|------------|------------|--------\n");
    printf("0-127          | 1          | 8          | 87.5%%\n");
    printf("128-16383      | 2          | 8          | 75%%\n");
    printf("16384-2097151  | 3          | 8          | 62.5%%\n");
    printf("2097152-268435455 | 4        | 8          | 50%%\n");
    printf("268435456-34359738367 | 5       | 8          | 37.5%%\n");
    printf("34359738368+    | 6-9       | 8          | 0-25%%\n");
}

int main(void) {
    printf("========================================\n");
    printf("    可变长度整数编码演示程序\n");
    printf("========================================\n");

    demo_uint_encode();
    demo_uint_decode();
    demo_sint_encode();
    demo_sint_decode();
    demo_applications();
    demo_performance();

    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("========================================\n");

    return 0;
}