/**
 * @file demo_base32_hex.c
 * @brief Base32 Hex 变体编解码演示
 *
 * Base32 Hex 是 Base32 的十六进制变体，使用 0-9, A-V 字符集
 * 相比标准 Base32，它更适合大小写不敏感的环境
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/base32_hex.h"

// 打印二进制数据
static void print_binary_data(const char* label, const unsigned char* data, size_t len) {
    printf("%s (%zu bytes): ", label, len);
    for (size_t i = 0; i < len && i < 20; i++) {
        printf("%02X", data[i]);
    }
    if (len > 20) printf("...");
    printf("\n");
}

// 演示 1: 基本编码
static void demo_basic_encode(void) {
    printf("\n=== 演示 1: 基本编码 ===\n");

    const char* inputs[] = {
        "Hello",
        "Test123",
        "Base32 Hex",
        "Binary Data"
    };
    int num_inputs = sizeof(inputs) / sizeof(inputs[0]);

    for (int i = 0; i < num_inputs; i++) {
        const char* input = inputs[i];
        size_t input_len = strlen(input);

        size_t encode_size = base32_hex_encode_size(input_len);
        char* encoded = malloc(encode_size);

        if (encoded) {
            base32_hex_encode((const unsigned char*)input, input_len, encoded);
            printf("\"%s\" (%zu bytes) -> %s\n", input, input_len, encoded);
            free(encoded);
        }
    }
}

// 演示 2: 基本解码
static void demo_basic_decode(void) {
    printf("\n=== 演示 2: 基本解码 ===\n");

    const char* encoded_inputs[] = {
        "91IMOR3F",                    // "Hello"
        "EHK6AP9P",                    // "Test123"
        "8196IM8BC5RG====",            // "Base32 Hex"
    };
    int num_inputs = sizeof(encoded_inputs) / sizeof(encoded_inputs[0]);

    for (int i = 0; i < num_inputs; i++) {
        const char* encoded = encoded_inputs[i];
        size_t encoded_len = strlen(encoded);

        unsigned char decoded[64];
        size_t decoded_len = base32_hex_decode(encoded, encoded_len, decoded);
        decoded[decoded_len] = '\0';

        printf("%s -> \"%s\" (%zu bytes)\n", encoded, decoded, decoded_len);
    }
}

// 演示 3: 内存分配版编解码
static void demo_alloc_encode_decode(void) {
    printf("\n=== 演示 3: 内存分配版编解码 ===\n");

    const char* input = "Base32 Hex encoding with dynamic allocation!";
    size_t input_len = strlen(input);

    printf("原始数据: \"%s\"\n", input);
    printf("原始长度: %zu 字节\n", input_len);

    // 编码
    char* encoded = base32_hex_encode_alloc((const unsigned char*)input, input_len);
    if (encoded) {
        printf("Base32 Hex 编码: %s\n", encoded);
        printf("编码长度: %zu 字符\n", strlen(encoded));

        // 解码
        size_t decoded_len;
        unsigned char* decoded = base32_hex_decode_alloc(encoded, strlen(encoded), &decoded_len);
        if (decoded) {
            decoded[decoded_len] = '\0';
            printf("解码后数据: \"%s\"\n", decoded);
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
    unsigned char binary_data[16];
    for (int i = 0; i < 16; i++) {
        binary_data[i] = (unsigned char)(0xA0 + i);
    }

    print_binary_data("原始二进制数据", binary_data, 16);

    // 编码
    size_t encode_size = base32_hex_encode_size(16);
    char* encoded = malloc(encode_size);
    if (encoded) {
        base32_hex_encode(binary_data, 16, encoded);
        printf("Base32 Hex 编码: %s\n", encoded);

        // 解码
        unsigned char* decoded = malloc(16);
        if (decoded) {
            size_t decoded_len = base32_hex_decode(encoded, strlen(encoded), decoded);
            print_binary_data("解码后数据", decoded, decoded_len);

            // 验证
            bool match = (decoded_len == 16) &&
                        (memcmp(binary_data, decoded, 16) == 0);
            printf("数据一致性: %s\n", match ? "通过" : "失败");

            free(decoded);
        }
        free(encoded);
    }
}

// 演示 5: 有效性验证
static void demo_validation(void) {
    printf("\n=== 演示 5: 有效性验证 ===\n");

    const char* test_cases[] = {
        "91IMOR3F",                    // 有效
        "91imor3f",                    // 有效（小写）
        "91IMOR3F!",                   // 无效（包含 !）
        "91IMOR3",                     // 有效
        "",                            // 有效（空字符串）
        "VVVVVVVV",                    // 有效
        "WWWWWWWW",                    // 无效（W 不在字符集中）
    };
    int num_cases = sizeof(test_cases) / sizeof(test_cases[0]);

    printf("验证测试:\n");
    for (int i = 0; i < num_cases; i++) {
        bool valid = base32_hex_is_valid(test_cases[i], strlen(test_cases[i]));
        printf("  \"%s\" -> %s\n", test_cases[i],
               valid ? "有效" : "无效");
    }
}

// 演示 6: 不同长度数据编码
static void demo_various_lengths(void) {
    printf("\n=== 演示 6: 不同长度数据编码 ===\n");

    // 测试 1-10 字节的数据
    for (int len = 1; len <= 10; len++) {
        unsigned char data[10];
        for (int i = 0; i < len; i++) {
            data[i] = 'A' + i;
        }

        size_t encode_size = base32_hex_encode_size(len);
        char* encoded = malloc(encode_size);

        if (encoded) {
            base32_hex_encode(data, len, encoded);
            printf("%d 字节 -> %s\n", len, encoded);
            free(encoded);
        }
    }
}

// 演示 7: Base32 与 Base32 Hex 对比
static void demo_comparison(void) {
    printf("\n=== 演示 7: Base32 与 Base32 Hex 对比 ===\n");

    const char* test_strings[] = {
        "A",
        "Hi",
        "Test",
        "Hello World"
    };
    int num_tests = sizeof(test_strings) / sizeof(test_strings[0]);

    printf("对比:\n");
    printf("%-20s %-30s %-30s\n", "原始数据", "标准 Base32", "Base32 Hex");
    printf("%-20s %-30s %-30s\n", "--------", "-----------", "------------");

    for (int i = 0; i < num_tests; i++) {
        size_t len = strlen(test_strings[i]);

        // 这里只显示 Base32 Hex 结果
        size_t encode_size = base32_hex_encode_size(len);
        char* encoded_hex = malloc(encode_size);
        base32_hex_encode((const unsigned char*)test_strings[i], len, encoded_hex);

        printf("%-20s %-30s %s\n", test_strings[i], "(see demo_base32)", encoded_hex);

        free(encoded_hex);
    }

    printf("\n说明:\n");
    printf("  - 标准 Base32: 使用 A-Z, 2-7 (适合人类阅读)\n");
    printf("  - Base32 Hex: 使用 0-9, A-V (适合大小写不敏感环境)\n");
}

// 演示 8: 大文件处理模拟
static void demo_large_data(void) {
    printf("\n=== 演示 8: 大数据处理 ===\n");

    // 模拟 512 字节数据
    size_t data_size = 512;
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
    size_t encoded_size = base32_hex_encode_size(data_size);
    printf("编码后大小: %zu 字符\n", encoded_size - 1);

    // 编码
    char* encoded = malloc(encoded_size);
    if (encoded) {
        base32_hex_encode(large_data, data_size, encoded);
        printf("编码完成，前 64 字符: %.64s...\n", encoded);

        // 解码
        unsigned char* decoded = malloc(data_size);
        if (decoded) {
            size_t decoded_len = base32_hex_decode(encoded, strlen(encoded), decoded);
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
    printf("    Base32 Hex 编解码演示程序\n");
    printf("========================================\n");

    demo_basic_encode();
    demo_basic_decode();
    demo_alloc_encode_decode();
    demo_binary_data();
    demo_validation();
    demo_various_lengths();
    demo_comparison();
    demo_large_data();

    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("========================================\n");

    return 0;
}
