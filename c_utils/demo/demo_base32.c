/**
 * @file demo_base32.c
 * @brief Base32 编解码演示
 *
 * 本演示展示了 Base32 编解码的各种用法，包括：
 * - 基本编码和解码
 * - 内存分配版编解码
 * - 有效性验证
 * - 与 Base64 对比
 * - 实际应用场景
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/base32.h"

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
        "Hello, World!",
        "Base32 encoding test",
        "1234567890"
    };
    int num_inputs = sizeof(inputs) / sizeof(inputs[0]);

    for (int i = 0; i < num_inputs; i++) {
        const char* input = inputs[i];
        size_t input_len = strlen(input);

        size_t encode_size = base32_encode_size(input_len);
        char* encoded = malloc(encode_size);

        if (encoded) {
            base32_encode((const unsigned char*)input, input_len, encoded);
            printf("\"%s\" (%zu bytes) -> %s\n", input, input_len, encoded);
            free(encoded);
        }
    }
}

// 演示 2: 基本解码
static void demo_basic_decode(void) {
    printf("\n=== 演示 2: 基本解码 ===\n");

    const char* encoded_inputs[] = {
        "JBSWY3DP",                    // "Hello"
        "JBSWY3DPEB3W64TMMQ======",    // "Hello, World!"
        "IJQXGZJAGK4TU===",            // "test"
    };
    int num_inputs = sizeof(encoded_inputs) / sizeof(encoded_inputs[0]);

    for (int i = 0; i < num_inputs; i++) {
        const char* encoded = encoded_inputs[i];
        size_t encoded_len = strlen(encoded);

        unsigned char decoded[64];
        size_t decoded_len = base32_decode(encoded, encoded_len, decoded);
        decoded[decoded_len] = '\0';

        printf("%s -> \"%s\" (%zu bytes)\n", encoded, decoded, decoded_len);
    }
}

// 演示 3: 内存分配版编解码
static void demo_alloc_encode_decode(void) {
    printf("\n=== 演示 3: 内存分配版编解码 ===\n");

    const char* input = "Memory allocation version of Base32!";
    size_t input_len = strlen(input);

    printf("原始数据: \"%s\"\n", input);
    printf("原始长度: %zu 字节\n", input_len);

    // 编码
    char* encoded = base32_encode_alloc((const unsigned char*)input, input_len);
    if (encoded) {
        printf("Base32 编码: %s\n", encoded);
        printf("编码长度: %zu 字符\n", strlen(encoded));

        // 解码
        size_t decoded_len;
        unsigned char* decoded = base32_decode_alloc(encoded, strlen(encoded), &decoded_len);
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
    unsigned char binary_data[20];
    for (int i = 0; i < 20; i++) {
        binary_data[i] = (unsigned char)(i * 13);
    }

    print_binary_data("原始二进制数据", binary_data, 20);

    // 编码
    size_t encode_size = base32_encode_size(20);
    char* encoded = malloc(encode_size);
    if (encoded) {
        base32_encode(binary_data, 20, encoded);
        printf("Base32 编码: %s\n", encoded);

        // 解码
        unsigned char* decoded = malloc(20);
        if (decoded) {
            size_t decoded_len = base32_decode(encoded, strlen(encoded), decoded);
            print_binary_data("解码后数据", decoded, decoded_len);

            // 验证
            bool match = (decoded_len == 20) &&
                        (memcmp(binary_data, decoded, 20) == 0);
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
        "JBSWY3DP",                    // 有效
        "jbswy3dp",                    // 有效（小写）
        "JBSWY3DPEB3W64TMMQ======",    // 有效（带填充）
        "JBSWY3DP!",                   // 无效（包含 !）
        "JBSWY3D",                     // 有效
        "JBSWY3DPEB3W64TMMQ",          // 有效（无填充）
        "",                            // 有效（空字符串）
        "========",                    // 有效（只有填充）
    };
    int num_cases = sizeof(test_cases) / sizeof(test_cases[0]);

    printf("验证测试:\n");
    for (int i = 0; i < num_cases; i++) {
        bool valid = base32_is_valid(test_cases[i], strlen(test_cases[i]));
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

        size_t encode_size = base32_encode_size(len);
        char* encoded = malloc(encode_size);

        if (encoded) {
            base32_encode(data, len, encoded);
            printf("%d 字节 -> %s\n", len, encoded);
            free(encoded);
        }
    }
}

// 演示 7: 密钥编码（Base32 常见用途）
static void demo_key_encoding(void) {
    printf("\n=== 演示 7: 密钥编码（Base32 常见用途）===\n");

    // 模拟一个 20 字节的密钥（常用于 TOTP）
    unsigned char secret_key[20] = {
        0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
        0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36,
        0x37, 0x38, 0x39, 0x30
    };

    printf("原始密钥 (20 bytes):\n  ");
    for (int i = 0; i < 20; i++) {
        printf("%02X", secret_key[i]);
    }
    printf("\n");

    // 编码为 Base32（常用于 Google Authenticator 等）
    char* encoded_key = base32_encode_alloc(secret_key, 20);
    if (encoded_key) {
        printf("\nBase32 编码的密钥:\n  %s\n", encoded_key);

        // 解码验证
        size_t decoded_len;
        unsigned char* decoded = base32_decode_alloc(encoded_key, strlen(encoded_key), &decoded_len);
        if (decoded) {
            printf("\n解码验证 (%zu bytes):\n  ", decoded_len);
            for (size_t i = 0; i < decoded_len; i++) {
                printf("%02X", decoded[i]);
            }
            printf("\n");
            free(decoded);
        }

        free(encoded_key);
    }
}

// 演示 8: 大文件处理模拟
static void demo_large_data(void) {
    printf("\n=== 演示 8: 大数据处理 ===\n");

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
    size_t encoded_size = base32_encode_size(data_size);
    printf("编码后大小: %zu 字符\n", encoded_size - 1);  // 减去 '\0'

    // 编码
    char* encoded = malloc(encoded_size);
    if (encoded) {
        base32_encode(large_data, data_size, encoded);
        printf("编码完成，前 64 字符: %.64s...\n", encoded);

        // 解码
        unsigned char* decoded = malloc(data_size);
        if (decoded) {
            size_t decoded_len = base32_decode(encoded, strlen(encoded), decoded);
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

// 演示 9: 与 Base64 空间效率对比
static void demo_space_efficiency(void) {
    printf("\n=== 演示 9: Base32 vs Base64 空间效率 ===\n");

    const char* test_strings[] = {
        "A",
        "Hello",
        "Hello, World!",
        "The quick brown fox jumps over the lazy dog"
    };
    int num_tests = sizeof(test_strings) / sizeof(test_strings[0]);

    printf("数据对比:\n");
    printf("%-40s %10s %10s\n", "原始数据", "Base32", "Base64");
    printf("%-40s %10s %10s\n", "--------", "------", "------");

    for (int i = 0; i < num_tests; i++) {
        size_t len = strlen(test_strings[i]);
        size_t b32_size = base32_encode_size(len) - 1;  // 减去 '\0'
        size_t b64_size = ((len + 2) / 3) * 4;  // Base64 大小估算

        char display[41];
        strncpy(display, test_strings[i], 40);
        display[40] = '\0';
        if (len > 40) {
            display[37] = '.';
            display[38] = '.';
            display[39] = '.';
        }

        printf("%-40s %10zu %10zu\n", display, b32_size, b64_size);
    }

    printf("\n说明: Base32 比 Base64 大约 60%%, 但更适合人类阅读\n");
    printf("      且不使用容易混淆的字符 (如 0/O, 1/I/l)\n");
}

int main(void) {
    printf("========================================\n");
    printf("    Base32 编解码演示程序\n");
    printf("========================================\n");

    demo_basic_encode();
    demo_basic_decode();
    demo_alloc_encode_decode();
    demo_binary_data();
    demo_validation();
    demo_various_lengths();
    demo_key_encoding();
    demo_large_data();
    demo_space_efficiency();

    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("========================================\n");

    return 0;
}
