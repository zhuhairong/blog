/**
 * 游程长度受限编码演示程序
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/run_length_limited.h"

static void print_hex(const char *label, const unsigned char *data, size_t len) {
    printf("%s: ", label);
    for (size_t i = 0; i < len && i < 20; i++) {
        printf("%02X ", data[i]);
    }
    if (len > 20) printf("...");
    printf("\n");
}

static void demo_basic_encode_decode(void) {
    printf("\n=== 演示 1: 基本编解码 ===\n");

    unsigned char input[] = {
        'A', 'A', 'A', 'A', 'A',
        'B', 'B', 'B',
        'C',
        'D', 'D', 'D', 'D', 'D', 'D',
        'E'
    };
    size_t input_len = sizeof(input);

    printf("原始数据:\n");
    print_hex("  输入", input, input_len);
    printf("  长度: %zu 字节\n", input_len);

    unsigned char encoded[256];
    rll_config_t config = rll_default_config();
    rll_result_t result;
    rll_error_t error;
    size_t encoded_len = rll_encode(input, input_len, encoded, 256, &config, &result, &error);

    printf("\n编码后:\n");
    print_hex("  输出", encoded, encoded_len);
    printf("  长度: %zu 字节\n", encoded_len);
    printf("  压缩率: %.1f%%\n", (1.0 - result.compression_ratio) * 100);

    unsigned char decoded[256];
    rll_result_t decode_result;
    size_t decoded_len = rll_decode(encoded, encoded_len, decoded, 256, &config, &decode_result, &error);

    printf("\n解码后:\n");
    print_hex("  输出", decoded, decoded_len);
    printf("  长度: %zu 字节\n", decoded_len);

    bool success = (decoded_len == input_len) &&
                   (memcmp(input, decoded, input_len) == 0);
    printf("\n验证: %s\n", success ? "成功" : "失败");
}

static void demo_rll_1_7(void) {
    printf("\n=== 演示 2: RLL (1,7) 编码 ===\n");

    unsigned char input[] = {
        0x00, 0x00, 0x00, 0x00,
        0xFF, 0xFF, 0xFF,
        0x55, 0x55,
        0xAA, 0xAA, 0xAA
    };
    size_t input_len = sizeof(input);

    printf("原始数据:\n");
    print_hex("  输入", input, input_len);

    unsigned char encoded[256];
    rll_config_t config = rll_1_7_default_config();
    rll_result_t result;
    rll_error_t error;
    size_t encoded_len = rll_encode(input, input_len, encoded, 256, &config, &result, &error);

    printf("\nRLL (1,7) 编码:\n");
    print_hex("  输出", encoded, encoded_len);
    printf("  长度: %zu 字节\n", encoded_len);

    unsigned char decoded[256];
    rll_result_t decode_result;
    size_t decoded_len = rll_decode(encoded, encoded_len, decoded, 256, &config, &decode_result, &error);

    printf("\n解码验证: %s\n",
           (decoded_len == input_len && memcmp(input, decoded, input_len) == 0)
           ? "成功" : "失败");
}

static void demo_compression_ratio(void) {
    printf("\n=== 演示 3: 压缩率分析 ===\n");

    struct {
        const char *desc;
        unsigned char data[32];
        size_t len;
    } tests[] = {
        {"全相同", {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
                   0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA}, 16},
        {"交替模式", {0xAB, 0xCD, 0xAB, 0xCD, 0xAB, 0xCD, 0xAB, 0xCD,
                     0xAB, 0xCD, 0xAB, 0xCD, 0xAB, 0xCD, 0xAB, 0xCD}, 16},
        {"随机数据", {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0,
                     0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88}, 16},
        {"短游程", {0x00, 0x00, 0x11, 0x11, 0x22, 0x22, 0x33, 0x33,
                   0x44, 0x44, 0x55, 0x55, 0x66, 0x66, 0x77, 0x77}, 16}
    };

    rll_config_t config = rll_default_config();

    printf("%-12s %10s %10s %10s\n", "类型", "原始", "编码后", "压缩率");
    printf("----------------------------------------------\n");

    for (int i = 0; i < 4; i++) {
        unsigned char encoded[64];
        rll_result_t result;
        rll_error_t error;
        size_t encoded_len = rll_encode(tests[i].data, tests[i].len, encoded, 64, &config, &result, &error);

        double ratio = (1.0 - result.compression_ratio) * 100;
        printf("%-12s %10zu %10zu %9.1f%%\n",
               tests[i].desc, tests[i].len, encoded_len, ratio);
    }
}

static void demo_limits(void) {
    printf("\n=== 演示 4: 游程限制 ===\n");

    rll_config_t config = rll_default_config();
    printf("默认配置:\n");
    printf("  最小游程长度: %zu\n", config.min_run_length);
    printf("  最大游程长度: %zu\n", config.max_run_length);

    unsigned char long_run[150];
    memset(long_run, 'X', 150);

    unsigned char encoded[256];
    rll_result_t result;
    rll_error_t error;
    size_t encoded_len = rll_encode(long_run, 150, encoded, 256, &config, &result, &error);

    printf("\n150 个相同字符的编码:\n");
    printf("  编码长度: %zu 字节\n", encoded_len);
    printf("  原始长度: 150 字节\n");
    printf("  压缩率: %.1f%%\n", (1.0 - result.compression_ratio) * 100);

    unsigned char decoded[256];
    rll_result_t decode_result;
    size_t decoded_len = rll_decode(encoded, encoded_len, decoded, 256, &config, &decode_result, &error);
    printf("  解码验证: %s\n",
           (decoded_len == 150 && memcmp(long_run, decoded, 150) == 0)
           ? "成功" : "失败");
}

int main(void) {
    printf("========================================\n");
    printf("    游程长度受限编码演示\n");
    printf("========================================\n");

    demo_basic_encode_decode();
    demo_rll_1_7();
    demo_compression_ratio();
    demo_limits();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
