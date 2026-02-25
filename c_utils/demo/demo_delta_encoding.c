/**
 * @file demo_delta_encoding.c
 * @brief 增量编码演示
 *
 * 本演示展示了增量编码算法的应用，包括：
 * - 基本增量编码/解码
 * - 不同数据类型的处理
 * - 变长编码
 * - 压缩效果对比
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include "../c_utils/delta_encoding.h"

// 打印 int32 数组
static void print_int32_array(const char *label, int32_t *arr, size_t n) {
    printf("%s: [", label);
    for (size_t i = 0; i < n && i < 10; i++) {
        if (i > 0) printf(", ");
        printf("%d", arr[i]);
    }
    if (n > 10) printf(", ... (%zu more)", n - 10);
    printf("]\n");
}

// 演示 1: 基本增量编码
static void demo_basic(void) {
    printf("\n=== 演示 1: 基本增量编码 ===\n");

    // 原始数据（递增序列，适合增量编码）
    int32_t original[] = {100, 102, 105, 107, 110, 112, 115, 118, 120, 123};
    size_t n = sizeof(original) / sizeof(original[0]);
    
    print_int32_array("原始数据", original, n);

    // 增量编码
    int32_t encoded[10];
    delta_encoding_error_t error;
    
    if (delta_encode(original, encoded, n, DELTA_TYPE_INT32, &error)) {
        print_int32_array("增量编码", encoded, n);
        
        // 解码验证
        int32_t decoded[10];
        if (delta_decode(encoded, decoded, n, DELTA_TYPE_INT32, &error)) {
            print_int32_array("解码结果", decoded, n);
            
            // 验证
            bool match = true;
            for (size_t i = 0; i < n; i++) {
                if (original[i] != decoded[i]) {
                    match = false;
                    break;
                }
            }
            printf("数据验证: %s\n", match ? "✓ 通过" : "✗ 失败");
        }
    } else {
        printf("编码失败，错误码: %d\n", error);
    }
}

// 演示 2: 不同数据类型
static void demo_types(void) {
    printf("\n=== 演示 2: 不同数据类型 ===\n");

    delta_encoding_error_t error;

    // int8 类型
    printf("\n1. int8 类型:\n");
    int8_t data8[] = {10, 12, 15, 18, 20};
    int8_t encoded8[5], decoded8[5];
    if (delta_encode(data8, encoded8, 5, DELTA_TYPE_INT8, &error)) {
        printf("  原始: [");
        for (int i = 0; i < 5; i++) {
            if (i > 0) printf(", ");
            printf("%d", data8[i]);
        }
        printf("]\n");
        printf("  编码: [");
        for (int i = 0; i < 5; i++) {
            if (i > 0) printf(", ");
            printf("%d", encoded8[i]);
        }
        printf("]\n");
    }

    // int16 类型
    printf("\n2. int16 类型:\n");
    int16_t data16[] = {1000, 1005, 1010, 1015, 1020};
    int16_t encoded16[5], decoded16[5];
    if (delta_encode(data16, encoded16, 5, DELTA_TYPE_INT16, &error)) {
        printf("  原始: [");
        for (int i = 0; i < 5; i++) {
            if (i > 0) printf(", ");
            printf("%d", data16[i]);
        }
        printf("]\n");
        printf("  编码: [");
        for (int i = 0; i < 5; i++) {
            if (i > 0) printf(", ");
            printf("%d", encoded16[i]);
        }
        printf("]\n");
    }

    // int64 类型
    printf("\n3. int64 类型:\n");
    int64_t data64[] = {1000000, 1000001, 1000003, 1000006, 1000010};
    int64_t encoded64[5], decoded64[5];
    if (delta_encode(data64, encoded64, 5, DELTA_TYPE_INT64, &error)) {
        printf("  原始: [");
        for (int i = 0; i < 5; i++) {
            if (i > 0) printf(", ");
            printf("%ld", (long)data64[i]);
        }
        printf("]\n");
        printf("  编码: [");
        for (int i = 0; i < 5; i++) {
            if (i > 0) printf(", ");
            printf("%ld", (long)encoded64[i]);
        }
        printf("]\n");
    }
}

// 演示 3: 压缩效果对比
static void demo_compression(void) {
    printf("\n=== 演示 3: 压缩效果对比 ===\n");

    delta_encoding_error_t error;

    // 测试 1: 高度可压缩数据（连续整数）
    printf("\n1. 高度可压缩数据（连续整数）:\n");
    int32_t data1[100];
    int32_t encoded1[100];
    for (int i = 0; i < 100; i++) {
        data1[i] = i * 10;
    }
    
    delta_encoding_config_t config = delta_encoding_default_config(DELTA_TYPE_INT32);
    config.use_variable_length = true;
    
    size_t encoded_size;
    if (delta_encode_ex(data1, encoded1, 100, &config, &encoded_size, &error)) {
        printf("  原始数据大小: %zu 字节\n", 100 * sizeof(int32_t));
        printf("  编码后大小: %zu 字节\n", encoded_size);
        printf("  压缩率: %.1f%%\n", 100.0 * (100 * sizeof(int32_t) - encoded_size) / (100 * sizeof(int32_t)));
    }

    // 测试 2: 中度可压缩数据
    printf("\n2. 中度可压缩数据（小范围随机）:\n");
    int32_t data2[100];
    int32_t encoded2[100];
    data2[0] = 1000;
    for (int i = 1; i < 100; i++) {
        data2[i] = data2[i-1] + (rand() % 10 - 5);
    }
    
    if (delta_encode_ex(data2, encoded2, 100, &config, &encoded_size, &error)) {
        printf("  原始数据大小: %zu 字节\n", 100 * sizeof(int32_t));
        printf("  编码后大小: %zu 字节\n", encoded_size);
        printf("  压缩率: %.1f%%\n", 100.0 * (100 * sizeof(int32_t) - encoded_size) / (100 * sizeof(int32_t)));
    }

    // 测试 3: 不可压缩数据（完全随机）
    printf("\n3. 不可压缩数据（完全随机）:\n");
    int32_t data3[100];
    int32_t encoded3[100];
    for (int i = 0; i < 100; i++) {
        data3[i] = rand();
    }
    
    if (delta_encode_ex(data3, encoded3, 100, &config, &encoded_size, &error)) {
        printf("  原始数据大小: %zu 字节\n", 100 * sizeof(int32_t));
        printf("  编码后大小: %zu 字节\n", encoded_size);
        printf("  压缩率: %.1f%%\n", 100.0 * (100 * sizeof(int32_t) - encoded_size) / (100 * sizeof(int32_t)));
    }
}

// 演示 4: 错误处理
static void demo_errors(void) {
    printf("\n=== 演示 4: 错误处理 ===\n");

    printf("错误码说明:\n");
    printf("  DELTA_ENCODING_OK (%d): 成功\n", DELTA_ENCODING_OK);
    printf("  DELTA_ENCODING_ERROR_INVALID_PARAM (%d): 无效参数\n", DELTA_ENCODING_ERROR_INVALID_PARAM);
    printf("  DELTA_ENCODING_ERROR_INSUFFICIENT_DATA (%d): 数据不足\n", DELTA_ENCODING_ERROR_INSUFFICIENT_DATA);
    printf("  DELTA_ENCODING_ERROR_BUFFER_TOO_SMALL (%d): 缓冲区太小\n", DELTA_ENCODING_ERROR_BUFFER_TOO_SMALL);
    printf("  DELTA_ENCODING_ERROR_MEMORY_ALLOC (%d): 内存分配失败\n", DELTA_ENCODING_ERROR_MEMORY_ALLOC);
    printf("  DELTA_ENCODING_ERROR_UNSUPPORTED_TYPE (%d): 不支持的类型\n", DELTA_ENCODING_ERROR_UNSUPPORTED_TYPE);

    printf("\n错误场景测试:\n");
    
    delta_encoding_error_t error;
    int32_t data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int32_t output[10];

    // NULL 输入
    bool result = delta_encode(NULL, output, 10, DELTA_TYPE_INT32, &error);
    printf("  NULL 输入: %s, 错误码: %d\n", result ? "成功" : "失败", error);

    // 零长度
    result = delta_encode(data, output, 0, DELTA_TYPE_INT32, &error);
    printf("  零长度: %s, 错误码: %d\n", result ? "成功" : "失败", error);

    // 错误信息
    printf("\n错误信息: %s\n", delta_encoding_strerror(DELTA_ENCODING_ERROR_INVALID_PARAM));
}

// 演示 5: 应用场景
static void demo_use_cases(void) {
    printf("\n=== 演示 5: 应用场景 ===\n");

    printf("增量编码的应用场景:\n\n");

    printf("1. 时间序列数据:\n");
    printf("   - 传感器数据\n");
    printf("   - 股票行情\n");
    printf("   - 日志时间戳\n\n");

    printf("2. 音频/图像处理:\n");
    printf("   - PCM 音频差分编码\n");
    printf("   - 图像行差分\n");
    printf("   - 视频帧间差分\n\n");

    printf("3. 数据压缩:\n");
    printf("   - 数据库压缩\n");
    printf("   - 网络传输优化\n");
    printf("   - 存储空间节省\n\n");

    printf("4. 信号处理:\n");
    printf("   - 滤波器设计\n");
    printf("   - 预测编码\n");
    printf("   - 数据平滑\n");
}

// 演示 6: 性能测试
static void demo_performance(void) {
    printf("\n=== 演示 6: 性能测试 ===\n");

    const size_t data_size = 1000000;
    int32_t *data = malloc(data_size * sizeof(int32_t));
    int32_t *encoded = malloc(data_size * sizeof(int32_t));
    int32_t *decoded = malloc(data_size * sizeof(int32_t));
    
    if (!data || !encoded || !decoded) {
        printf("内存分配失败\n");
        free(data);
        free(encoded);
        free(decoded);
        return;
    }

    // 生成测试数据
    data[0] = 1000;
    for (size_t i = 1; i < data_size; i++) {
        data[i] = data[i-1] + (rand() % 10);
    }

    delta_encoding_error_t error;
    clock_t start, end;

    // 编码性能
    start = clock();
    delta_encode(data, encoded, data_size, DELTA_TYPE_INT32, &error);
    end = clock();
    double encode_time = (double)(end - start) * 1000 / CLOCKS_PER_SEC;

    // 解码性能
    start = clock();
    delta_decode(encoded, decoded, data_size, DELTA_TYPE_INT32, &error);
    end = clock();
    double decode_time = (double)(end - start) * 1000 / CLOCKS_PER_SEC;

    printf("数据量: %zu 个 int32\n", data_size);
    printf("编码时间: %.3f ms (%.2f MB/s)\n", 
           encode_time, 
           (data_size * sizeof(int32_t)) / (encode_time * 1000));
    printf("解码时间: %.3f ms (%.2f MB/s)\n", 
           decode_time, 
           (data_size * sizeof(int32_t)) / (decode_time * 1000));

    free(data);
    free(encoded);
    free(decoded);
}

// 演示 7: 与其他算法比较
static void demo_comparison(void) {
    printf("\n=== 演示 7: 与其他算法比较 ===\n");

    printf("压缩算法比较:\n\n");

    printf("特性          增量编码    RLE         Huffman     LZ77\n");
    printf("-----------------------------------------------------\n");
    printf("速度          极快        快          中等        慢\n");
    printf("压缩率        中          中-高       高          高\n");
    printf("内存占用      极低        低          中          高\n");
    printf("适用数据      时间序列    重复数据    文本        通用\n");
    printf("复杂度        O(n)        O(n)        O(n)        O(n^2)\n\n");

    printf("增量编码优势:\n");
    printf("  - 计算简单，速度极快\n");
    printf("  - 内存占用小\n");
    printf("  - 适合流式处理\n");
    printf("  - 可与其他算法组合使用\n");
}

// 演示 8: 最佳实践
static void demo_best_practices(void) {
    printf("\n=== 演示 8: 最佳实践 ===\n");

    printf("增量编码最佳实践:\n\n");

    printf("1. 数据选择:\n");
    printf("   - 适合连续变化的数据\n");
    printf("   - 避免完全随机数据\n");
    printf("   - 考虑数据预处理\n\n");

    printf("2. 类型选择:\n");
    printf("   - 使用最小足够类型\n");
    printf("   - 考虑符号扩展\n");
    printf("   - 注意溢出处理\n\n");

    printf("3. 组合策略:\n");
    printf("   - 增量 + RLE\n");
    printf("   - 增量 + 变长编码\n");
    printf("   - 增量 + 字典编码\n\n");

    printf("4. 错误处理:\n");
    printf("   - 验证输入数据\n");
    printf("   - 检查缓冲区大小\n");
    printf("   - 处理边界情况\n");
}

int main(void) {
    printf("========================================\n");
    printf("    增量编码演示程序\n");
    printf("========================================\n");

    demo_basic();
    demo_types();
    demo_compression();
    demo_errors();
    demo_use_cases();
    demo_performance();
    demo_comparison();
    demo_best_practices();

    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("========================================\n");

    return 0;
}
