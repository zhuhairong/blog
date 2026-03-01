/**
 * ZigZag编码演示程序
 *
 * 功能：
 * - 基本ZigZag编码/解码
 * - 批量编码/解码
 * - 错误处理
 * - 配置管理
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/zigzag.h"

// 演示 1: 基本ZigZag编码/解码 (int64_t)
static void demo_basic_zigzag_int64(void) {
    printf("\n=== 演示 1: 基本ZigZag编码/解码 (int64_t) ===\n");

    zigzag_ctx_t *ctx = NULL;
    zigzag_config_t config = {true, true, 100};
    zigzag_create(&ctx, &config);

    int64_t test_values[] = {0, 1, -1, 2, -2, 127, -127, 128, -128, 2147483647, -2147483648};
    size_t count = sizeof(test_values) / sizeof(test_values[0]);

    for (size_t i = 0; i < count; i++) {
        int64_t value = test_values[i];
        uint64_t encoded;
        int64_t decoded;

        // 编码
        zigzag_error_t error = zigzag_encode_int64(ctx, value, &encoded);
        if (error == ZIGZAG_OK) {
            // 解码
            error = zigzag_decode_int64(ctx, encoded, &decoded);
            if (error == ZIGZAG_OK) {
                printf("%12lld -> %12llu -> %12lld %s\n", 
                       value, encoded, decoded, 
                       (value == decoded) ? "✓" : "✗");
            } else {
                printf("解码错误: %s\n", zigzag_strerror(error));
            }
        } else {
            printf("编码错误: %s\n", zigzag_strerror(error));
        }
    }

    zigzag_destroy(ctx);
}

// 演示 2: 基本ZigZag编码/解码 (int32_t)
static void demo_basic_zigzag_int32(void) {
    printf("\n=== 演示 2: 基本ZigZag编码/解码 (int32_t) ===\n");

    zigzag_ctx_t *ctx = NULL;
    zigzag_config_t config = {true, true, 100};
    zigzag_create(&ctx, &config);

    int32_t test_values[] = {0, 1, -1, 2, -2, 127, -127, 128, -128, 32767, -32768};
    size_t count = sizeof(test_values) / sizeof(test_values[0]);

    for (size_t i = 0; i < count; i++) {
        int32_t value = test_values[i];
        uint32_t encoded;
        int32_t decoded;

        // 编码
        zigzag_error_t error = zigzag_encode_int32(ctx, value, &encoded);
        if (error == ZIGZAG_OK) {
            // 解码
            error = zigzag_decode_int32(ctx, encoded, &decoded);
            if (error == ZIGZAG_OK) {
                printf("%8d -> %8u -> %8d %s\n", 
                       value, encoded, decoded, 
                       (value == decoded) ? "✓" : "✗");
            } else {
                printf("解码错误: %s\n", zigzag_strerror(error));
            }
        } else {
            printf("编码错误: %s\n", zigzag_strerror(error));
        }
    }

    zigzag_destroy(ctx);
}

// 演示 3: 批量编码/解码 (int64_t)
static void demo_batch_zigzag_int64(void) {
    printf("\n=== 演示 3: 批量编码/解码 (int64_t) ===\n");

    zigzag_ctx_t *ctx = NULL;
    zigzag_config_t config = {true, true, 100};
    zigzag_create(&ctx, &config);

    int64_t input[] = {1, -1, 2, -2, 100, -100};
    size_t count = sizeof(input) / sizeof(input[0]);
    uint64_t encoded[count];
    int64_t decoded[count];

    // 批量编码
    zigzag_error_t error = zigzag_encode_batch_int64(ctx, input, encoded, count);
    if (error == ZIGZAG_OK) {
        printf("批量编码结果: ");
        for (size_t i = 0; i < count; i++) {
            printf("%llu ", encoded[i]);
        }
        printf("\n");

        // 批量解码
        error = zigzag_decode_batch_int64(ctx, encoded, decoded, count);
        if (error == ZIGZAG_OK) {
            printf("批量解码结果: ");
            for (size_t i = 0; i < count; i++) {
                printf("%lld ", decoded[i]);
            }
            printf("\n");

            // 验证结果
            bool all_correct = true;
            for (size_t i = 0; i < count; i++) {
                if (input[i] != decoded[i]) {
                    all_correct = false;
                    break;
                }
            }
            printf("验证结果: %s\n", all_correct ? "✓ 全部正确" : "✗ 有错误");
        } else {
            printf("批量解码错误: %s\n", zigzag_strerror(error));
        }
    } else {
        printf("批量编码错误: %s\n", zigzag_strerror(error));
    }

    zigzag_destroy(ctx);
}

// 演示 4: 批量编码/解码 (int32_t)
static void demo_batch_zigzag_int32(void) {
    printf("\n=== 演示 4: 批量编码/解码 (int32_t) ===\n");

    zigzag_ctx_t *ctx = NULL;
    zigzag_config_t config = {true, true, 100};
    zigzag_create(&ctx, &config);

    int32_t input[] = {1, -1, 2, -2, 100, -100};
    size_t count = sizeof(input) / sizeof(input[0]);
    uint32_t encoded[count];
    int32_t decoded[count];

    // 批量编码
    zigzag_error_t error = zigzag_encode_batch_int32(ctx, input, encoded, count);
    if (error == ZIGZAG_OK) {
        printf("批量编码结果: ");
        for (size_t i = 0; i < count; i++) {
            printf("%u ", encoded[i]);
        }
        printf("\n");

        // 批量解码
        error = zigzag_decode_batch_int32(ctx, encoded, decoded, count);
        if (error == ZIGZAG_OK) {
            printf("批量解码结果: ");
            for (size_t i = 0; i < count; i++) {
                printf("%d ", decoded[i]);
            }
            printf("\n");

            // 验证结果
            bool all_correct = true;
            for (size_t i = 0; i < count; i++) {
                if (input[i] != decoded[i]) {
                    all_correct = false;
                    break;
                }
            }
            printf("验证结果: %s\n", all_correct ? "✓ 全部正确" : "✗ 有错误");
        } else {
            printf("批量解码错误: %s\n", zigzag_strerror(error));
        }
    } else {
        printf("批量编码错误: %s\n", zigzag_strerror(error));
    }

    zigzag_destroy(ctx);
}

// 演示 5: 传统ZigZag函数
static void demo_traditional_zigzag(void) {
    printf("\n=== 演示 5: 传统ZigZag函数 ===\n");

    int64_t test_values[] = {0, 1, -1, 2, -2, 100, -100};
    size_t count = sizeof(test_values) / sizeof(test_values[0]);

    for (size_t i = 0; i < count; i++) {
        int64_t value = test_values[i];
        uint64_t encoded = zigzag_encode(value);
        int64_t decoded = zigzag_decode(encoded);

        printf("%lld -> %llu -> %lld %s\n", 
               value, encoded, decoded, 
               (value == decoded) ? "✓" : "✗");
    }
}

// 演示 6: 错误处理
static void demo_error_handling(void) {
    printf("\n=== 演示 6: 错误处理 ===\n");

    zigzag_ctx_t *ctx = NULL;
    zigzag_config_t config = {true, true, 100};
    zigzag_create(&ctx, &config);

    // 测试空指针错误
    int64_t value = 100;
    uint64_t encoded;
    zigzag_error_t error = zigzag_encode_int64(NULL, value, &encoded);
    if (error != ZIGZAG_OK) {
        printf("测试空指针错误: %s\n", zigzag_strerror(error));
    }

    // 测试空输出指针错误
    error = zigzag_encode_int64(ctx, value, NULL);
    if (error != ZIGZAG_OK) {
        printf("测试空输出指针错误: %s\n", zigzag_strerror(error));
    }

    zigzag_destroy(ctx);
}

// 演示 7: 配置管理
static void demo_config_management(void) {
    printf("\n=== 演示 7: 配置管理 ===\n");

    // 测试不同配置
    zigzag_config_t config1 = {true, true, 100};  // 检查溢出，使用有符号
    zigzag_config_t config2 = {false, false, 50}; // 不检查溢出，不使用有符号

    zigzag_ctx_t *ctx1 = NULL;
    zigzag_ctx_t *ctx2 = NULL;

    zigzag_create(&ctx1, &config1);
    zigzag_create(&ctx2, &config2);

    int64_t value = 1000;
    uint64_t encoded1, encoded2;
    int64_t decoded1, decoded2;

    // 使用配置1编码/解码
    zigzag_encode_int64(ctx1, value, &encoded1);
    zigzag_decode_int64(ctx1, encoded1, &decoded1);

    // 使用配置2编码/解码
    zigzag_encode_int64(ctx2, value, &encoded2);
    zigzag_decode_int64(ctx2, encoded2, &decoded2);

    printf("配置1 (检查溢出): %lld -> %llu -> %lld\n", value, encoded1, decoded1);
    printf("配置2 (不检查溢出): %lld -> %llu -> %lld\n", value, encoded2, decoded2);

    // 检查统计信息
    printf("配置1 编码次数: %zu, 解码次数: %zu\n", ctx1->encode_count, ctx1->decode_count);
    printf("配置2 编码次数: %zu, 解码次数: %zu\n", ctx2->encode_count, ctx2->decode_count);

    zigzag_destroy(ctx1);
    zigzag_destroy(ctx2);
}

// 演示 8: ZigZag编码原理
static void demo_zigzag_principle(void) {
    printf("\n=== 演示 8: ZigZag编码原理 ===\n");

    printf("ZigZag编码将有符号整数映射到无符号整数:\n");
    printf("- 0 → 0\n");
    printf("- 1 → 1\n");
    printf("- -1 → 2\n");
    printf("- 2 → 3\n");
    printf("- -2 → 4\n");
    printf("- 3 → 5\n");
    printf("- -3 → 6\n");
    printf("...\n");

    printf("\n编码公式: (n << 1) ^ (n >> 63)  (对于int64_t)\n");
    printf("解码公式: (n >> 1) ^ -(n & 1)     (对于int64_t)\n");

    // 示例计算
    int64_t n = -3;
    uint64_t encoded = (n << 1) ^ (n >> 63);
    int64_t decoded = (encoded >> 1) ^ -(encoded & 1);

    printf("\n示例计算:\n");
    printf("n = %lld\n", n);
    printf("编码: (%lld << 1) ^ (%lld >> 63) = %llu\n", n, n, encoded);
    printf("解码: (%llu >> 1) ^ -(%llu & 1) = %lld\n", encoded, encoded, decoded);
}

int main(void) {
    printf("========================================\n");
    printf("    ZigZag编码演示\n");
    printf("========================================\n");

    demo_basic_zigzag_int64();
    demo_basic_zigzag_int32();
    demo_batch_zigzag_int64();
    demo_batch_zigzag_int32();
    demo_traditional_zigzag();
    demo_error_handling();
    demo_config_management();
    demo_zigzag_principle();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
