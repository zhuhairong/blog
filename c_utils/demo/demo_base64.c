/**
 * @file demo_base64.c
 * @brief Base64 编解码演示
 *
 * 本演示展示了 Base64 编解码的各种用法，包括：
 * - 基本编码和解码
 * - URL 安全编码
 * - 内存分配版编解码
 * - 有效性验证
 * - 实际应用场景
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/base64.h"

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
        "Base64 encoding test",
        "1234567890"
    };
    int num_inputs = sizeof(inputs) / sizeof(inputs[0]);

    for (int i = 0; i < num_inputs; i++) {
        const char* input = inputs[i];
        size_t input_len = strlen(input);

        size_t encode_size = base64_encode_size(input_len);
        char* encoded = malloc(encode_size);

        if (encoded) {
            base64_encode((const unsigned char*)input, input_len, encoded);
            printf("\"%s\" (%zu bytes) -> %s\n", input, input_len, encoded);
            free(encoded);
        }
    }
}

// 演示 2: 基本解码
static void demo_basic_decode(void) {
    printf("\n=== 演示 2: 基本解码 ===\n");

    const char* encoded_inputs[] = {
        "SGVsbG8=",
        "SGVsbG8sIFdvcmxkIQ==",
        "QmFzZTY0IGVuY29kaW5nIHRlc3Q=",
        "MTIzNDU2Nzg5MA=="
    };
    int num_inputs = sizeof(encoded_inputs) / sizeof(encoded_inputs[0]);

    for (int i = 0; i < num_inputs; i++) {
        const char* encoded = encoded_inputs[i];
        size_t encoded_len = strlen(encoded);

        unsigned char decoded[64];
        size_t decoded_len = base64_decode(encoded, encoded_len, decoded);
        decoded[decoded_len] = '\0';

        printf("%s -> \"%s\" (%zu bytes)\n", encoded, decoded, decoded_len);
    }
}

// 演示 3: URL 安全编码
static void demo_url_safe(void) {
    printf("\n=== 演示 3: URL 安全编码 ===\n");

    // 包含 + 和 / 的数据
    unsigned char binary_data[] = {
        0xFB, 0xFF, 0xFE, 0xFD, 0xFC, 0x00, 0x01, 0x02
    };
    size_t data_len = sizeof(binary_data);

    print_binary_data("原始二进制数据", binary_data, data_len);

    // 标准 Base64 编码
    char standard[32];
    base64_encode(binary_data, data_len, standard);
    printf("标准 Base64: %s\n", standard);

    // URL 安全编码
    char url_safe[32];
    base64_url_encode(binary_data, data_len, url_safe);
    printf("URL 安全:    %s\n", url_safe);

    printf("\n说明: URL 安全编码将 '+' 替换为 '-', '/' 替换为 '_'\n");
}

// 演示 4: 内存分配版编解码
static void demo_alloc_encode_decode(void) {
    printf("\n=== 演示 4: 内存分配版编解码 ===\n");

    const char* input = "Memory allocation version of Base64!";
    size_t input_len = strlen(input);

    printf("原始数据: \"%s\"\n", input);
    printf("原始长度: %zu 字节\n", input_len);

    // 标准编码
    char* encoded = base64_encode_alloc((const unsigned char*)input, input_len);
    if (encoded) {
        printf("Base64 编码: %s\n", encoded);

        size_t decoded_len;
        unsigned char* decoded = base64_decode_alloc(encoded, strlen(encoded), &decoded_len);
        if (decoded) {
            decoded[decoded_len] = '\0';
            printf("解码后数据: \"%s\"\n", decoded);
            printf("解码长度: %zu 字节\n", decoded_len);
            free(decoded);
        }

        free(encoded);
    }

    // URL 安全编码
    printf("\nURL 安全版本:\n");
    char* url_encoded = base64_url_encode_alloc((const unsigned char*)input, input_len);
    if (url_encoded) {
        printf("URL 编码: %s\n", url_encoded);

        size_t decoded_len;
        unsigned char* decoded = base64_url_decode_alloc(url_encoded, strlen(url_encoded), &decoded_len);
        if (decoded) {
            decoded[decoded_len] = '\0';
            printf("解码后: \"%s\"\n", decoded);
            free(decoded);
        }

        free(url_encoded);
    }
}

// 演示 5: 二进制数据处理
static void demo_binary_data(void) {
    printf("\n=== 演示 5: 二进制数据处理 ===\n");

    // 创建二进制数据
    unsigned char binary_data[32];
    for (int i = 0; i < 32; i++) {
        binary_data[i] = (unsigned char)(i * 8);
    }

    print_binary_data("原始二进制数据", binary_data, 32);

    // 编码
    size_t encode_size = base64_encode_size(32);
    char* encoded = malloc(encode_size);
    if (encoded) {
        base64_encode(binary_data, 32, encoded);
        printf("Base64 编码: %s\n", encoded);

        // 解码
        unsigned char* decoded = malloc(32);
        if (decoded) {
            size_t decoded_len = base64_decode(encoded, strlen(encoded), decoded);
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

// 演示 6: 有效性验证
static void demo_validation(void) {
    printf("\n=== 演示 6: 有效性验证 ===\n");

    const char* test_cases[] = {
        "SGVsbG8=",
        "SGVsbG8sIFdvcmxkIQ==",
        "SGVsbG8",              // 无填充
        "SGVsbG8=\n",           // 包含换行
        "SGVsbG8!",             // 无效字符
        "",
        "QQ==",
        "//8="
    };
    int num_cases = sizeof(test_cases) / sizeof(test_cases[0]);

    printf("标准 Base64 验证:\n");
    for (int i = 0; i < num_cases; i++) {
        bool valid = base64_is_valid(test_cases[i], strlen(test_cases[i]));
        printf("  \"%s\" -> %s\n", test_cases[i],
               valid ? "有效" : "无效");
    }
}

// 演示 7: 不同长度数据编码
static void demo_various_lengths(void) {
    printf("\n=== 演示 7: 不同长度数据编码 ===\n");

    printf("长度 | 原始 | 编码后 | 编码结果\n");
    printf("-----|------|--------|----------\n");

    // 测试 1-10 字节的数据
    for (int len = 1; len <= 10; len++) {
        unsigned char data[10];
        for (int i = 0; i < len; i++) {
            data[i] = 'A' + i;
        }

        size_t encode_size = base64_encode_size(len);
        char* encoded = malloc(encode_size);

        if (encoded) {
            base64_encode(data, len, encoded);
            printf("  %2d |  %2d  |   %2zu   | %s\n",
                   len, len, strlen(encoded), encoded);
            free(encoded);
        }
    }

    printf("\n说明: Base64 每 3 字节编码为 4 字符\n");
}

// 演示 8: 实际应用 - 图片数据模拟
static void demo_image_data(void) {
    printf("\n=== 演示 8: 图片数据模拟 (Data URL) ===\n");

    // 模拟一个小图片 (PNG 文件头)
    unsigned char png_header[] = {
        0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A,
        0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52,
        0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01,
        0x08, 0x02, 0x00, 0x00, 0x00, 0x90, 0x77, 0x53
    };

    print_binary_data("PNG 文件头", png_header, sizeof(png_header));

    // 编码为 Base64
    char* encoded = base64_encode_alloc(png_header, sizeof(png_header));
    if (encoded) {
        printf("Base64 编码:\n  %s\n\n", encoded);

        // 构造 Data URL
        printf("Data URL 格式:\n");
        printf("  data:image/png;base64,%s\n", encoded);

        free(encoded);
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
    size_t encoded_size = base64_encode_size(data_size);
    printf("编码后大小: %zu 字符\n", encoded_size - 1);

    // 编码
    char* encoded = malloc(encoded_size);
    if (encoded) {
        base64_encode(large_data, data_size, encoded);
        printf("编码完成，前 64 字符: %.64s...\n", encoded);

        // 解码
        unsigned char* decoded = malloc(data_size);
        if (decoded) {
            size_t decoded_len = base64_decode(encoded, strlen(encoded), decoded);
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

// 演示 10: 编码效率对比
static void demo_efficiency(void) {
    printf("\n=== 演示 10: 编码效率对比 ===\n");

    const char* test_strings[] = {
        "A",
        "Hello",
        "Hello, World!",
        "The quick brown fox jumps over the lazy dog",
        "Lorem ipsum dolor sit amet, consectetur adipiscing elit."
    };
    int num_tests = sizeof(test_strings) / sizeof(test_strings[0]);

    printf("数据长度对比:\n");
    printf("%-50s %8s %8s %8s\n", "原始数据", "原始", "Base64", "开销");
    printf("%-50s %8s %8s %8s\n", "--------", "----", "------", "----");

    for (int i = 0; i < num_tests; i++) {
        size_t len = strlen(test_strings[i]);
        size_t b64_size = base64_encode_size(len) - 1;
        double overhead = ((double)b64_size / len - 1) * 100;

        char display[51];
        strncpy(display, test_strings[i], 50);
        display[50] = '\0';
        if (len > 50) {
            display[47] = '.';
            display[48] = '.';
            display[49] = '.';
        }

        printf("%-50s %8zu %8zu %7.1f%%\n", display, len, b64_size, overhead);
    }

    printf("\n说明: Base64 编码开销约为 33%%\n");
}

int main(void) {
    printf("========================================\n");
    printf("    Base64 编解码演示程序\n");
    printf("========================================\n");

    demo_basic_encode();
    demo_basic_decode();
    demo_url_safe();
    demo_alloc_encode_decode();
    demo_binary_data();
    demo_validation();
    demo_various_lengths();
    demo_image_data();
    demo_large_data();
    demo_efficiency();

    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("========================================\n");

    return 0;
}
