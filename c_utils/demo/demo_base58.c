/**
 * @file demo_base58.c
 * @brief Base58 编解码演示 (Bitcoin 风格)
 *
 * 本演示展示了 Base58 编解码的各种用法，包括：
 * - 基本编码和解码
 * - 内存分配版编解码
 * - 比特币地址编码
 * - 有效性验证
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/base58.h"

// 打印二进制数据
static void print_binary_data(const char* label, const unsigned char* data, size_t len) {
    printf("%s (%zu bytes): ", label, len);
    for (size_t i = 0; i < len && i < 32; i++) {
        printf("%02X", data[i]);
    }
    if (len > 32) printf("...");
    printf("\n");
}

// 演示 1: 基本编码
static void demo_basic_encode(void) {
    printf("\n=== 演示 1: 基本编码 ===\n");

    const char* inputs[] = {
        "Hello",
        "Hello, World!",
        "Base58 encoding",
        "1234567890"
    };
    int num_inputs = sizeof(inputs) / sizeof(inputs[0]);

    for (int i = 0; i < num_inputs; i++) {
        const char* input = inputs[i];
        size_t input_len = strlen(input);

        size_t encode_size = base58_encode_size(input_len);
        char* encoded = malloc(encode_size);
        size_t out_len;

        if (encoded && base58_encode((const unsigned char*)input, input_len, encoded, &out_len)) {
            printf("\"%s\" (%zu bytes) -> %s (%zu chars)\n", input, input_len, encoded, out_len);
        }
        free(encoded);
    }
}

// 演示 2: 基本解码
static void demo_basic_decode(void) {
    printf("\n=== 演示 2: 基本解码 ===\n");

    const char* encoded_inputs[] = {
        "9Ajdvzr",
        "2NEpo7TZRRrLZSi2U",
        "6Mx3vL2gL"
    };
    int num_inputs = sizeof(encoded_inputs) / sizeof(encoded_inputs[0]);

    for (int i = 0; i < num_inputs; i++) {
        const char* encoded = encoded_inputs[i];

        unsigned char decoded[64];
        size_t decoded_len = sizeof(decoded);

        if (base58_decode(encoded, decoded, &decoded_len)) {
            decoded[decoded_len] = '\0';
            printf("%s -> \"%s\" (%zu bytes)\n", encoded, decoded, decoded_len);
        } else {
            printf("%s -> 解码失败\n", encoded);
        }
    }
}

// 演示 3: 内存分配版编解码
static void demo_alloc_encode_decode(void) {
    printf("\n=== 演示 3: 内存分配版编解码 ===\n");

    const char* input = "Memory allocation version of Base58!";
    size_t input_len = strlen(input);

    printf("原始数据: \"%s\"\n", input);
    printf("原始长度: %zu 字节\n", input_len);

    // 编码
    size_t encoded_len;
    char* encoded = base58_encode_alloc((const unsigned char*)input, input_len, &encoded_len);
    if (encoded) {
        printf("Base58 编码: %s\n", encoded);
        printf("编码长度: %zu 字符\n", encoded_len);

        // 解码
        size_t decoded_len;
        unsigned char* decoded = base58_decode_alloc(encoded, &decoded_len);
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

    // 创建二进制数据（模拟比特币私钥）
    unsigned char binary_data[32];
    for (int i = 0; i < 32; i++) {
        binary_data[i] = (unsigned char)(0x80 + i * 3);
    }

    print_binary_data("原始二进制数据", binary_data, 32);

    // 编码
    size_t encoded_len;
    char* encoded = base58_encode_alloc(binary_data, 32, &encoded_len);
    if (encoded) {
        printf("Base58 编码: %s\n", encoded);

        // 解码
        size_t decoded_len;
        unsigned char* decoded = base58_decode_alloc(encoded, &decoded_len);
        if (decoded) {
            print_binary_data("解码后数据", decoded, decoded_len);

            // 验证
            bool match = (decoded_len == 32) &&
                        (memcmp(binary_data, decoded, 32) == 0);
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
        "9Ajdvzr",
        "2NEpo7TZRRrLZSi2U",
        "9Ajdvzr0",         // 包含 0
        "9AjdvzrO",         // 包含 O
        "9Ajdvzrl",         // 包含 l
        "9AjdvzrI",         // 包含 I
        "",
        "123"
    };
    int num_cases = sizeof(test_cases) / sizeof(test_cases[0]);

    printf("验证测试:\n");
    for (int i = 0; i < num_cases; i++) {
        bool valid = base58_is_valid(test_cases[i]);
        printf("  \"%s\" -> %s\n", test_cases[i],
               valid ? "有效" : "无效");
    }

    printf("\n说明: Base58 排除了 0, O, I, l 等容易混淆的字符\n");
}

// 演示 6: 比特币 WIF 格式模拟
static void demo_bitcoin_wif(void) {
    printf("\n=== 演示 6: 比特币 WIF 格式模拟 ===\n");

    // 模拟私钥 (32 字节)
    unsigned char private_key[32] = {
        0x0C, 0x28, 0xFC, 0xA3, 0x86, 0xC7, 0xA2, 0x27,
        0x60, 0x0B, 0x2F, 0xE5, 0x0B, 0x7C, 0xAE, 0x11,
        0xEC, 0x86, 0xD3, 0xBF, 0x1F, 0xBE, 0x47, 0x1B,
        0xE8, 0x98, 0x27, 0xE1, 0x9D, 0x72, 0xAA, 0x1D
    };

    printf("私钥 (hex): ");
    for (int i = 0; i < 32; i++) printf("%02X", private_key[i]);
    printf("\n");

    // WIF 格式: 版本字节(0x80) + 私钥 + 压缩标志(0x01，可选)
    unsigned char wif_data[34];
    wif_data[0] = 0x80;  // 主网版本字节
    memcpy(wif_data + 1, private_key, 32);
    wif_data[33] = 0x01; // 压缩格式

    // 编码为 WIF
    size_t wif_len;
    char* wif = base58_encode_alloc(wif_data, 34, &wif_len);
    if (wif) {
        printf("WIF 格式: %s\n", wif);

        // 解码验证
        size_t decoded_len;
        unsigned char* decoded = base58_decode_alloc(wif, &decoded_len);
        if (decoded && decoded_len == 34) {
            printf("解码验证: 版本=0x%02X, 压缩标志=0x%02X\n",
                   decoded[0], decoded[33]);
            free(decoded);
        }

        free(wif);
    }
}

// 演示 7: 大文件处理模拟
static void demo_large_data(void) {
    printf("\n=== 演示 7: 大数据处理 ===\n");

    // 模拟 256 字节数据
    size_t data_size = 256;
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
    size_t encoded_size = base58_encode_size(data_size);
    printf("编码缓冲区大小: %zu 字节\n", encoded_size);

    // 编码
    char* encoded = malloc(encoded_size);
    size_t encoded_len;
    if (encoded && base58_encode(large_data, data_size, encoded, &encoded_len)) {
        printf("编码完成，前 64 字符: %.64s...\n", encoded);
        printf("编码后长度: %zu 字符\n", encoded_len);

        // 解码
        unsigned char* decoded = malloc(data_size);
        size_t decoded_len = data_size;
        if (decoded && base58_decode(encoded, decoded, &decoded_len)) {
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

// 演示 8: 与其他编码对比
static void demo_comparison(void) {
    printf("\n=== 演示 8: Base58 vs Base64 对比 ===\n");

    const char* test_data = "Hello, World! This is a test.";
    size_t len = strlen(test_data);

    printf("原始数据: \"%s\"\n", test_data);
    printf("原始长度: %zu 字节\n\n", len);

    // Base58 编码
    size_t b58_len;
    char* b58 = base58_encode_alloc((const unsigned char*)test_data, len, &b58_len);
    if (b58) {
        printf("Base58 编码 (%zu 字符):\n  %s\n\n", b58_len, b58);
        free(b58);
    }

    printf("说明:\n");
    printf("  - Base58 比 Base64 更长，但更适合人工输入\n");
    printf("  - Base58 不含 0, O, I, l 等易混淆字符\n");
    printf("  - Base58 常用于加密货币地址\n");
}

int main(void) {
    printf("========================================\n");
    printf("    Base58 编解码演示程序\n");
    printf("    (Bitcoin 风格)\n");
    printf("========================================\n");

    demo_basic_encode();
    demo_basic_decode();
    demo_alloc_encode_decode();
    demo_binary_data();
    demo_validation();
    demo_bitcoin_wif();
    demo_large_data();
    demo_comparison();

    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("========================================\n");

    return 0;
}
