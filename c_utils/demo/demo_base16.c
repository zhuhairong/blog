/**
 * @file demo_base16.c
 * @brief Base16 (十六进制) 编解码演示
 *
 * 本演示展示了 Base16 编解码的各种用法，包括：
 * - 基本编码和解码
 * - 大小写编码
 * - 内存分配版编解码
 * - 有效性验证
 * - 实际应用场景
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/base16.h"

// 打印十六进制数据
static void print_hex_data(const char* label, const unsigned char* data, size_t len) {
    printf("%s (%zu bytes): ", label, len);
    for (size_t i = 0; i < len; i++) {
        printf("%02X", data[i]);
        if ((i + 1) % 16 == 0 && i < len - 1) printf(" ");
    }
    printf("\n");
}

// 打印字符串数据
static void print_string_data(const char* label, const char* data) {
    printf("%s: \"%s\"\n", label, data);
}

// 演示 1: 基本编码
static void demo_basic_encode(void) {
    printf("\n=== 演示 1: 基本编码 ===\n");

    const char* input = "Hello, World!";
    size_t input_len = strlen(input);

    printf("原始数据: \"%s\"\n", input);
    printf("原始长度: %zu 字节\n", input_len);

    // 计算编码后大小
    size_t encode_size = base16_encode_size(input_len);
    printf("编码后大小: %zu 字节\n", encode_size);

    // 编码（大写）
    char encoded_upper[64];
    base16_encode((const unsigned char*)input, input_len, encoded_upper, true);
    printf("Base16 编码 (大写): %s\n", encoded_upper);

    // 编码（小写）
    char encoded_lower[64];
    base16_encode((const unsigned char*)input, input_len, encoded_lower, false);
    printf("Base16 编码 (小写): %s\n", encoded_lower);
}

// 演示 2: 基本解码
static void demo_basic_decode(void) {
    printf("\n=== 演示 2: 基本解码 ===\n");

    const char* encoded = "48656C6C6F2C20576F726C6421";
    size_t encoded_len = strlen(encoded);

    printf("Base16 编码: %s\n", encoded);
    printf("编码长度: %zu 字符\n", encoded_len);

    // 解码
    unsigned char decoded[64];
    size_t decoded_len = base16_decode(encoded, encoded_len, decoded);
    decoded[decoded_len] = '\0';

    printf("解码后数据: \"%s\"\n", decoded);
    printf("解码长度: %zu 字节\n", decoded_len);
}

// 演示 3: 内存分配版编解码
static void demo_alloc_encode_decode(void) {
    printf("\n=== 演示 3: 内存分配版编解码 ===\n");

    const char* input = "Memory allocation version test!";
    size_t input_len = strlen(input);

    printf("原始数据: \"%s\"\n", input);

    // 编码（自动分配内存）
    char* encoded = base16_encode_alloc((const unsigned char*)input, input_len, true);
    if (encoded) {
        printf("Base16 编码: %s\n", encoded);

        // 解码（自动分配内存）
        size_t decoded_len;
        unsigned char* decoded = base16_decode_alloc(encoded, strlen(encoded), &decoded_len);
        if (decoded) {
            printf("解码后数据: \"%.*s\"\n", (int)decoded_len, decoded);
            printf("解码长度: %zu 字节\n", decoded_len);
            free(decoded);
        }

        free(encoded);
    }
}

// 演示 4: 二进制数据处理
static void demo_binary_data(void) {
    printf("\n=== 演示 4: 二进制数据处理 ===\n");

    // 创建二进制数据
    unsigned char binary_data[32];
    for (int i = 0; i < 32; i++) {
        binary_data[i] = (unsigned char)(i * 8);
    }

    print_hex_data("原始二进制数据", binary_data, 32);

    // 编码
    char encoded[128];
    base16_encode(binary_data, 32, encoded, true);
    printf("Base16 编码:\n  %s\n", encoded);

    // 解码
    unsigned char decoded[64];
    size_t decoded_len = base16_decode(encoded, strlen(encoded), decoded);
    print_hex_data("解码后数据", decoded, decoded_len);
}

// 演示 5: 有效性验证
static void demo_validation(void) {
    printf("\n=== 演示 5: 有效性验证 ===\n");

    const char* test_cases[] = {
        "48656C6C6F",           // 有效
        "48656c6c6f",           // 有效（小写）
        "48656C6C6F2",          // 无效（奇数长度）
        "48656C6G6F",           // 无效（包含 G）
        "48 65 6C 6C 6F",       // 无效（包含空格）
        "",                     // 有效（空字符串）
        "00",                   // 有效
        "FF",                   // 有效
    };
    int num_cases = sizeof(test_cases) / sizeof(test_cases[0]);

    printf("验证测试:\n");
    for (int i = 0; i < num_cases; i++) {
        bool valid = base16_is_valid(test_cases[i], strlen(test_cases[i]));
        printf("  \"%s\" -> %s\n", test_cases[i],
               valid ? "有效" : "无效");
    }
}

// 演示 6: 大小写混合处理
static void demo_mixed_case(void) {
    printf("\n=== 演示 6: 大小写混合处理 ===\n");

    const char* mixed_case = "48656c6C6f2C20576f726c6421";
    printf("混合大小写编码: %s\n", mixed_case);

    // 验证
    bool valid = base16_is_valid(mixed_case, strlen(mixed_case));
    printf("有效性: %s\n", valid ? "有效" : "无效");

    // 解码
    unsigned char decoded[64];
    size_t decoded_len = base16_decode(mixed_case, strlen(mixed_case), decoded);
    decoded[decoded_len] = '\0';
    printf("解码结果: \"%s\"\n", decoded);
}

// 演示 7: 实际应用 - 校验和显示
static void demo_checksum_display(void) {
    printf("\n=== 演示 7: 校验和显示 ===\n");

    // 模拟计算出的校验和
    unsigned char checksum[16] = {
        0x5F, 0x4D, 0xCC, 0x3B, 0x5A, 0xA7, 0x65, 0xD6,
        0x1D, 0x83, 0x27, 0xDE, 0xB8, 0x82, 0xCF, 0x99
    };

    printf("MD5 校验和 (二进制):\n");
    for (int i = 0; i < 16; i++) {
        printf("%02X", checksum[i]);
    }
    printf("\n");

    // 转换为 Base16 字符串
    char hex_checksum[64];
    base16_encode(checksum, 16, hex_checksum, false);
    printf("MD5 校验和 (Base16): %s\n", hex_checksum);
}

// 演示 8: 数据序列化
static void demo_serialization(void) {
    printf("\n=== 演示 8: 数据序列化 ===\n");

    // 定义一个结构体
    typedef struct {
        int id;
        float value;
        char flags;
    } data_packet_t;

    data_packet_t packet = {
        .id = 0x12345678,
        .value = 3.14159f,
        .flags = 0xAB
    };

    printf("原始数据包:\n");
    printf("  id: 0x%08X\n", packet.id);
    printf("  value: %f\n", packet.value);
    printf("  flags: 0x%02X\n", packet.flags);

    // 序列化为 Base16
    char serialized[128];
    base16_encode((const unsigned char*)&packet, sizeof(packet), serialized, true);
    printf("\n序列化 (Base16): %s\n", serialized);

    // 反序列化
    data_packet_t restored;
    size_t decoded_len = base16_decode(serialized, strlen(serialized),
                                        (unsigned char*)&restored);

    if (decoded_len == sizeof(packet)) {
        printf("\n反序列化后:\n");
        printf("  id: 0x%08X\n", restored.id);
        printf("  value: %f\n", restored.value);
        printf("  flags: 0x%02X\n", restored.flags);
    }
}

// 演示 9: 大文件处理模拟
static void demo_large_data(void) {
    printf("\n=== 演示 9: 大数据处理 ===\n");

    // 模拟 1KB 数据
    size_t data_size = 1024;
    unsigned char* large_data = malloc(data_size);
    if (!large_data) {
        fprintf(stderr, "内存分配失败\n");
        return;
    }

    // 填充数据
    for (size_t i = 0; i < data_size; i++) {
        large_data[i] = (unsigned char)(i & 0xFF);
    }

    printf("原始数据大小: %zu 字节\n", data_size);

    // 计算编码后大小
    size_t encoded_size = base16_encode_size(data_size);
    printf("编码后大小: %zu 字节\n", encoded_size);

    // 编码
    char* encoded = malloc(encoded_size);
    if (encoded) {
        base16_encode(large_data, data_size, encoded, true);
        printf("编码完成，前 64 字符: %.64s...\n", encoded);

        // 解码
        unsigned char* decoded = malloc(data_size);
        if (decoded) {
            size_t decoded_len = base16_decode(encoded, strlen(encoded), decoded);
            printf("解码完成，大小: %zu 字节\n", decoded_len);

            // 验证
            bool match = (decoded_len == data_size) &&
                        (memcmp(large_data, decoded, data_size) == 0);
            printf("数据一致性: %s\n", match ? "通过" : "失败");

            free(decoded);
        }
        free(encoded);
    }

    free(large_data);
}

int main(void) {
    printf("========================================\n");
    printf("    Base16 (十六进制) 编解码演示程序\n");
    printf("========================================\n");

    demo_basic_encode();
    demo_basic_decode();
    demo_alloc_encode_decode();
    demo_binary_data();
    demo_validation();
    demo_mixed_case();
    demo_checksum_display();
    demo_serialization();
    demo_large_data();

    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("========================================\n");

    return 0;
}
