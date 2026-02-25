/**
 * @file demo_cobs.c
 * @brief COBS (Consistent Overhead Byte Stuffing) 演示
 *
 * 本演示展示了 COBS 编码的各种用法，包括：
 * - 基本编码和解码
 * - 帧分隔
 * - 实际应用场景
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include "../c_utils/cobs.h"

// 打印十六进制数据
static void print_hex(const char* label, const unsigned char* data, size_t len) {
    printf("%s (%zu bytes): ", label, len);
    for (size_t i = 0; i < len && i < 32; i++) {
        printf("%02X ", data[i]);
    }
    if (len > 32) printf("...");
    printf("\n");
}

// 打印字符串（可打印字符）
static void print_string(const char* label, const unsigned char* data, size_t len) {
    printf("%s: \"", label);
    for (size_t i = 0; i < len; i++) {
        if (data[i] >= 32 && data[i] < 127) {
            printf("%c", data[i]);
        } else {
            printf("\\x%02X", data[i]);
        }
    }
    printf("\"\n");
}

// 演示 1: 基本编码解码
static void demo_basic_encode_decode(void) {
    printf("\n=== 演示 1: 基本编码解码 ===\n");

    const char* messages[] = {
        "Hello",
        "Hello\x00World",
        "Test\x00\x00Data",
        "NoNullHere"
    };
    int num_messages = sizeof(messages) / sizeof(messages[0]);

    for (int i = 0; i < num_messages; i++) {
        size_t len = strlen(messages[i]) + 1; // 包含结尾的 \0

        size_t max_encoded = cobs_max_encoded_size(len);
        unsigned char* encoded = malloc(max_encoded);
        unsigned char* decoded = malloc(len);

        if (!encoded || !decoded) {
            free(encoded);
            free(decoded);
            continue;
        }

        printf("\n消息 %d:\n", i + 1);
        print_string("  原始", (const unsigned char*)messages[i], len);

        // 编码
        size_t encoded_len;
        cobs_error_t err = cobs_encode((const unsigned char*)messages[i], len,
                                       encoded, max_encoded, &encoded_len);

        if (err == COBS_OK) {
            print_hex("  编码", encoded, encoded_len);
            printf("  编码后长度: %zu (开销: %zu)\n",
                   encoded_len, encoded_len - len);

            // 解码
            size_t decoded_len;
            err = cobs_decode(encoded, encoded_len, decoded, len, &decoded_len);

            if (err == COBS_OK) {
                print_string("  解码", decoded, decoded_len);

                bool match = (len == decoded_len) &&
                           (memcmp(messages[i], decoded, len) == 0);
                printf("  验证: %s\n", match ? "成功" : "失败");
            }
        }

        free(encoded);
        free(decoded);
    }
}

// 演示 2: 包含多个 0 的数据
static void demo_multiple_zeros(void) {
    printf("\n=== 演示 2: 包含多个 0 的数据 ===\n");

    // 构造包含多个 0 的数据
    unsigned char data[32];
    memset(data, 0xAA, sizeof(data));
    data[5] = 0;
    data[10] = 0;
    data[15] = 0;
    data[20] = 0;
    data[25] = 0;

    printf("原始数据: 32 字节，包含 5 个 0\n");
    print_hex("数据", data, 32);

    size_t max_encoded = cobs_max_encoded_size(32);
    unsigned char* encoded = malloc(max_encoded);
    unsigned char* decoded = malloc(32);

    if (encoded && decoded) {
        size_t encoded_len;
        cobs_encode(data, 32, encoded, max_encoded, &encoded_len);

        print_hex("编码", encoded, encoded_len);
        printf("编码后长度: %zu (开销: %zu 字节)\n", encoded_len, encoded_len - 32);

        // 验证编码中无 0
        int zeros = 0;
        for (size_t i = 0; i < encoded_len; i++) {
            if (encoded[i] == 0) zeros++;
        }
        printf("编码中 0 的数量: %d (应为 0)\n", zeros);

        // 解码验证
        size_t decoded_len;
        cobs_decode(encoded, encoded_len, decoded, 32, &decoded_len);

        bool match = (memcmp(data, decoded, 32) == 0);
        printf("解码验证: %s\n", match ? "成功" : "失败");
    }

    free(encoded);
    free(decoded);
}

// 演示 3: 帧分隔
static void demo_frame_delimiting(void) {
    printf("\n=== 演示 3: 帧分隔 ===\n");

    // 模拟串行通信中的帧
    const char* frames[] = {
        "Frame1",
        "Frame2Data",
        "Frame3MoreData"
    };
    int num_frames = sizeof(frames) / sizeof(frames[0]);

    printf("COBS 帧分隔原理:\n");
    printf("  - 使用 0x00 作为帧分隔符\n");
    printf("  - COBS 编码确保数据中无 0x00\n");
    printf("  - 接收方通过检测 0x00 识别帧边界\n\n");

    // 编码并连接帧
    unsigned char stream[256];
    size_t stream_pos = 0;

    printf("发送 %d 个帧:\n", num_frames);
    for (int i = 0; i < num_frames; i++) {
        size_t len = strlen(frames[i]);
        size_t max_encoded = cobs_max_encoded_size(len);
        unsigned char* encoded = malloc(max_encoded);

        if (encoded) {
            size_t encoded_len;
            cobs_encode((const unsigned char*)frames[i], len,
                       encoded, max_encoded, &encoded_len);

            printf("  帧 %d: \"%s\" -> %zu bytes\n", i + 1, frames[i], encoded_len);

            // 添加到流，并添加帧分隔符
            memcpy(stream + stream_pos, encoded, encoded_len);
            stream_pos += encoded_len;
            stream[stream_pos++] = 0; // 帧分隔符

            free(encoded);
        }
    }

    printf("\n传输流 (%zu bytes):\n", stream_pos);
    print_hex("流", stream, stream_pos);

    // 解码帧
    printf("\n接收并解码帧:\n");
    size_t pos = 0;
    int frame_count = 0;

    while (pos < stream_pos) {
        // 找到帧结束位置
        size_t frame_end = pos;
        while (frame_end < stream_pos && stream[frame_end] != 0) {
            frame_end++;
        }

        size_t frame_len = frame_end - pos;
        if (frame_len > 0) {
            unsigned char decoded[64];
            size_t decoded_len;
            cobs_decode(stream + pos, frame_len, decoded, sizeof(decoded), &decoded_len);

            printf("  帧 %d: \"", ++frame_count);
            for (size_t i = 0; i < decoded_len; i++) {
                printf("%c", decoded[i]);
            }
            printf("\"\n");
        }

        pos = frame_end + 1; // 跳过分隔符
    }
}

// 演示 4: 缓冲区大小计算
static void demo_buffer_size(void) {
    printf("\n=== 演示 4: 缓冲区大小计算 ===\n");

    size_t input_sizes[] = {10, 100, 254, 255, 256, 1000};
    int num_sizes = sizeof(input_sizes) / sizeof(input_sizes[0]);

    printf("COBS 编码缓冲区需求:\n\n");
    printf("输入大小 | 最大编码大小 | 开销\n");
    printf("---------|--------------|------\n");

    for (int i = 0; i < num_sizes; i++) {
        size_t input = input_sizes[i];
        size_t max_encoded = cobs_max_encoded_size(input);
        size_t overhead = max_encoded - input;

        printf("%8zu | %12zu | %4zu\n", input, max_encoded, overhead);
    }

    printf("\n说明:\n");
    printf("  - COBS 开销最大为 (n/254) + 1 字节\n");
    printf("  - 对于小于 254 字节的数据，开销最多 2 字节\n");
}

// 演示 5: 实际应用 - 串行通信
static void demo_serial_communication(void) {
    printf("\n=== 演示 5: 实际应用 - 串行通信 ===\n");

    // 模拟传感器数据帧
    typedef struct {
        uint8_t sensor_id;
        uint16_t temperature;
        uint16_t humidity;
        uint32_t timestamp;
    } sensor_packet_t;

    sensor_packet_t packet = {
        .sensor_id = 0x01,
        .temperature = 250,  // 25.0 度
        .humidity = 600,     // 60.0%
        .timestamp = 12345678
    };

    printf("传感器数据包:\n");
    printf("  传感器 ID: %d\n", packet.sensor_id);
    printf("  温度: %.1f°C\n", packet.temperature / 10.0);
    printf("  湿度: %.1f%%\n", packet.humidity / 10.0);
    printf("  时间戳: %u\n", packet.timestamp);

    // 编码
    size_t max_encoded = cobs_max_encoded_size(sizeof(packet));
    unsigned char* encoded = malloc(max_encoded + 1); // +1 for delimiter

    if (encoded) {
        size_t encoded_len;
        cobs_encode((const unsigned char*)&packet, sizeof(packet),
                   encoded, max_encoded, &encoded_len);

        // 添加帧分隔符
        encoded[encoded_len] = 0;

        printf("\n编码后 (%zu bytes):\n", encoded_len + 1);
        print_hex("数据", encoded, encoded_len + 1);

        // 模拟传输...
        printf("\n通过串口传输...\n");

        // 解码
        sensor_packet_t decoded_packet;
        size_t decoded_len;
        cobs_decode(encoded, encoded_len, (unsigned char*)&decoded_packet,
                   sizeof(decoded_packet), &decoded_len);

        printf("\n解码后:\n");
        printf("  传感器 ID: %d\n", decoded_packet.sensor_id);
        printf("  温度: %.1f°C\n", decoded_packet.temperature / 10.0);
        printf("  湿度: %.1f%%\n", decoded_packet.humidity / 10.0);
        printf("  时间戳: %u\n", decoded_packet.timestamp);

        free(encoded);
    }
}

// 演示 6: 有效性验证
static void demo_validation(void) {
    printf("\n=== 演示 6: 有效性验证 ===\n");

    // 有效的 COBS 编码
    unsigned char valid[] = {0x03, 0x11, 0x22, 0x02, 0x33};

    // 无效的 COBS 编码（包含 0）
    unsigned char invalid[] = {0x03, 0x11, 0x00, 0x22};

    printf("COBS 编码验证:\n\n");

    printf("有效编码: ");
    print_hex("", valid, sizeof(valid));
    printf("  验证结果: %s\n",
           cobs_validate(valid, sizeof(valid)) ? "有效" : "无效");

    printf("\n无效编码: ");
    print_hex("", invalid, sizeof(invalid));
    printf("  验证结果: %s\n",
           cobs_validate(invalid, sizeof(invalid)) ? "有效" : "无效");

    printf("\n说明: 有效的 COBS 编码不应包含 0x00\n");
}

// 演示 7: 错误处理
static void demo_error_handling(void) {
    printf("\n=== 演示 7: 错误处理 ===\n");

    unsigned char data[100] = {0x11, 0x22, 0x33};
    unsigned char encoded[200];
    unsigned char decoded[100];
    size_t len;

    printf("错误码测试:\n\n");

    // 正常编码
    cobs_error_t err = cobs_encode(data, 10, encoded, sizeof(encoded), &len);
    printf("正常编码: %s (错误码 %d)\n",
           err == COBS_OK ? "成功" : "失败", err);

    // 缓冲区太小
    err = cobs_encode(data, 100, encoded, 10, &len);
    printf("缓冲区太小: %s (错误码 %d - COBS_ERROR_BUFFER_TOO_SMALL)\n",
           err != COBS_OK ? "预期失败" : "意外成功", err);

    printf("\n错误码说明:\n");
    printf("  COBS_OK = 0: 成功\n");
    printf("  COBS_ERROR_INVALID_INPUT = 1: 无效输入\n");
    printf("  COBS_ERROR_INVALID_ENCODED_DATA = 2: 无效编码数据\n");
    printf("  COBS_ERROR_BUFFER_TOO_SMALL = 3: 缓冲区太小\n");
    printf("  COBS_ERROR_UNTERMINATED_PACKET = 4: 未终止的数据包\n");
}

// 演示 8: 大数据编码
static void demo_large_data(void) {
    printf("\n=== 演示 8: 大数据编码 ===\n");

    size_t data_size = 1000;
    unsigned char* data = malloc(data_size);
    unsigned char* encoded = malloc(cobs_max_encoded_size(data_size));
    unsigned char* decoded = malloc(data_size);

    if (!data || !encoded || !decoded) {
        free(data);
        free(encoded);
        free(decoded);
        return;
    }

    // 填充数据
    for (size_t i = 0; i < data_size; i++) {
        data[i] = (unsigned char)(i % 256);
    }

    printf("大数据编码测试 (%zu bytes):\n", data_size);

    // 编码
    size_t encoded_len;
    cobs_error_t err = cobs_encode(data, data_size, encoded,
                                   cobs_max_encoded_size(data_size), &encoded_len);

    if (err == COBS_OK) {
        printf("  编码后大小: %zu bytes\n", encoded_len);
        printf("  开销: %zu bytes (%.2f%%)\n",
               encoded_len - data_size,
               100.0 * (encoded_len - data_size) / data_size);

        // 解码
        size_t decoded_len;
        err = cobs_decode(encoded, encoded_len, decoded, data_size, &decoded_len);

        if (err == COBS_OK) {
            bool match = (data_size == decoded_len) &&
                       (memcmp(data, decoded, data_size) == 0);
            printf("  解码验证: %s\n", match ? "成功" : "失败");
        }
    }

    free(data);
    free(encoded);
    free(decoded);
}

// 演示 9: 与 SLIP 对比
static void demo_comparison(void) {
    printf("\n=== 演示 9: COBS vs SLIP ===\n");

    printf("协议对比:\n\n");

    printf("COBS (Consistent Overhead Byte Stuffing):\n");
    printf("  + 开销确定，最多 (n/254) + 1 字节\n");
    printf("  + 编解码简单高效\n");
    printf("  + 适合硬件实现\n");
    printf("  + 广泛应用于现代系统\n\n");

    printf("SLIP (Serial Line Internet Protocol):\n");
    printf("  + 实现简单\n");
    printf("  - 开销不确定，最坏情况 2x\n");
    printf("  - 需要转义多个字符\n");
    printf("  - 已较少使用\n\n");

    printf("推荐使用 COBS 的场景:\n");
    printf("  - 带宽受限的串行通信\n");
    printf("  - 实时性要求高的系统\n");
    printf("  - 嵌入式系统\n");
    printf("  - 需要确定性的开销\n");
}

// 演示 10: 性能测试
static void demo_performance(void) {
    printf("\n=== 演示 10: 性能测试 ===\n");

    const size_t sizes[] = {100, 1000, 10000};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    printf("编码性能测试:\n\n");
    printf("数据大小 | 编码时间 | 解码时间\n");
    printf("---------|----------|----------\n");

    for (int i = 0; i < num_sizes; i++) {
        size_t size = sizes[i];
        unsigned char* data = malloc(size);
        unsigned char* encoded = malloc(cobs_max_encoded_size(size));
        unsigned char* decoded = malloc(size);

        if (!data || !encoded || !decoded) {
            free(data);
            free(encoded);
            free(decoded);
            continue;
        }

        // 填充数据
        for (size_t j = 0; j < size; j++) {
            data[j] = (unsigned char)(rand() % 256);
        }

        // 编码计时
        clock_t start = clock();
        size_t encoded_len;
        for (int iter = 0; iter < 1000; iter++) {
            cobs_encode(data, size, encoded, cobs_max_encoded_size(size), &encoded_len);
        }
        clock_t encode_end = clock();

        // 解码计时
        for (int iter = 0; iter < 1000; iter++) {
            size_t decoded_len;
            cobs_decode(encoded, encoded_len, decoded, size, &decoded_len);
        }
        clock_t decode_end = clock();

        double encode_time = (double)(encode_end - start) * 1000 / CLOCKS_PER_SEC;
        double decode_time = (double)(decode_end - encode_end) * 1000 / CLOCKS_PER_SEC;

        printf("%8zu | %8.3f | %8.3f\n", size, encode_time, decode_time);

        free(data);
        free(encoded);
        free(decoded);
    }

    printf("\n(时间单位: ms, 1000 次迭代)\n");
}

int main(void) {
    printf("========================================\n");
    printf("    COBS 编码演示程序\n");
    printf("    (Consistent Overhead Byte Stuffing)\n");
    printf("========================================\n");

    demo_basic_encode_decode();
    demo_multiple_zeros();
    demo_frame_delimiting();
    demo_buffer_size();
    demo_serial_communication();
    demo_validation();
    demo_error_handling();
    demo_large_data();
    demo_comparison();
    demo_performance();

    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("========================================\n");

    return 0;
}
