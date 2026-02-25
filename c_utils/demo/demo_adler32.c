/**
 * @file demo_adler32.c
 * @brief Adler-32 校验和算法演示
 *
 * 本演示展示了 Adler-32 校验和算法的各种用法，包括：
 * - 基本校验和计算
 * - 增量计算
 * - 批量计算
 * - 文件校验和计算
 * - 内存块校验和计算
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../c_utils/adler32.h"

// 打印错误信息
static void print_error(const char* operation, adler32_error_t error) {
    fprintf(stderr, "操作 '%s' 失败: %s\n", operation, adler32_strerror(error));
}

// 演示 1: 基本校验和计算
static void demo_basic_checksum(void) {
    printf("\n=== 演示 1: 基本校验和计算 ===\n");

    const char* data = "Hello, World!";
    uint32_t checksum = adler32_compute(data, strlen(data));
    printf("数据: \"%s\"\n", data);
    printf("Adler-32 校验和: 0x%08X\n", checksum);

    // 计算不同数据的校验和
    const char* data2 = "The quick brown fox jumps over the lazy dog";
    uint32_t checksum2 = adler32_compute(data2, strlen(data2));
    printf("\n数据: \"%s\"\n", data2);
    printf("Adler-32 校验和: 0x%08X\n", checksum2);
}

// 演示 2: 使用上下文的安全计算
static void demo_safe_compute(void) {
    printf("\n=== 演示 2: 使用上下文的安全计算 ===\n");

    adler32_ctx_t* ctx = NULL;
    adler32_config_t config = {
        .check_overflow = true,
        .use_large_window = false,
        .verify_result = true,
        .max_batch_size = 1024,
        .max_file_size = 1024 * 1024
    };

    adler32_error_t err = adler32_create(&ctx, &config);
    if (err != ADLER32_OK) {
        print_error("创建上下文", err);
        return;
    }

    const char* data = "Safe computing with Adler-32!";
    uint32_t checksum;
    err = adler32_compute_safe(ctx, data, strlen(data), &checksum);
    if (err != ADLER32_OK) {
        print_error("计算校验和", err);
        adler32_destroy(ctx);
        return;
    }

    printf("数据: \"%s\"\n", data);
    printf("安全计算的校验和: 0x%08X\n", checksum);

    adler32_destroy(ctx);
}

// 演示 3: 增量计算
static void demo_incremental(void) {
    printf("\n=== 演示 3: 增量计算 ===\n");

    // 模拟流式数据计算
    const char* chunks[] = {
        "Hello, ",
        "World! ",
        "This ",
        "is ",
        "incremental ",
        "computation."
    };
    size_t num_chunks = sizeof(chunks) / sizeof(chunks[0]);

    uint32_t adler = ADLER32_INIT;
    printf("增量计算过程:\n");
    for (size_t i = 0; i < num_chunks; i++) {
        adler = adler32_update(adler, chunks[i], strlen(chunks[i]));
        printf("  块 %zu: \"%s\" -> 当前校验和: 0x%08X\n", i + 1, chunks[i], adler);
    }

    // 验证与一次性计算结果相同
    char full_data[256] = {0};
    for (size_t i = 0; i < num_chunks; i++) {
        strcat(full_data, chunks[i]);
    }
    uint32_t full_checksum = adler32_compute(full_data, strlen(full_data));
    printf("\n完整数据校验和: 0x%08X\n", full_checksum);
    printf("增量计算结果:   0x%08X\n", adler);
    printf("结果一致: %s\n", (adler == full_checksum) ? "是" : "否");
}

// 演示 4: 批量计算
static void demo_batch_compute(void) {
    printf("\n=== 演示 4: 批量计算 ===\n");

    adler32_ctx_t* ctx = NULL;
    adler32_config_t config = {
        .check_overflow = true,
        .use_large_window = false,
        .verify_result = true,
        .max_batch_size = 1024,
        .max_file_size = 1024 * 1024
    };

    adler32_error_t err = adler32_create(&ctx, &config);
    if (err != ADLER32_OK) {
        print_error("创建上下文", err);
        return;
    }

    const char* data_items[] = {
        "First data block",
        "Second data block",
        "Third data block",
        "Fourth data block"
    };
    size_t num_items = sizeof(data_items) / sizeof(data_items[0]);

    const void* data_ptrs[4];
    size_t lengths[4];
    for (size_t i = 0; i < num_items; i++) {
        data_ptrs[i] = data_items[i];
        lengths[i] = strlen(data_items[i]);
        printf("数据块 %zu: \"%s\" (长度: %zu)\n", i + 1, data_items[i], lengths[i]);
    }

    uint32_t batch_checksums[4];
    err = adler32_compute_batch(ctx, data_ptrs, lengths, num_items, batch_checksums);
    if (err != ADLER32_OK) {
        print_error("批量计算", err);
        adler32_destroy(ctx);
        return;
    }

    printf("\n批量计算结果:\n");
    for (size_t i = 0; i < num_items; i++) {
        printf("  数据块 %zu 校验和: 0x%08X\n", i + 1, batch_checksums[i]);
    }
    adler32_destroy(ctx);
}

// 演示 5: 内存块校验和计算
static void demo_block_compute(void) {
    printf("\n=== 演示 5: 内存块校验和计算 ===\n");

    adler32_ctx_t* ctx = NULL;
    adler32_config_t config = {
        .check_overflow = true,
        .use_large_window = false,
        .verify_result = true,
        .max_batch_size = 16,  // 小批量以演示分块
        .max_file_size = 1024 * 1024
    };

    adler32_error_t err = adler32_create(&ctx, &config);
    if (err != ADLER32_OK) {
        print_error("创建上下文", err);
        return;
    }

    // 创建较大的测试数据
    size_t data_size = 100;
    unsigned char* data = malloc(data_size);
    if (!data) {
        fprintf(stderr, "内存分配失败\n");
        adler32_destroy(ctx);
        return;
    }

    for (size_t i = 0; i < data_size; i++) {
        data[i] = (unsigned char)(i % 256);
    }

    printf("数据大小: %zu 字节\n", data_size);
    printf("块大小: %zu 字节\n", config.max_batch_size);

    uint32_t block_checksum;
    err = adler32_compute_block(ctx, data, data_size, config.max_batch_size, &block_checksum);
    if (err != ADLER32_OK) {
        print_error("块计算", err);
        free(data);
        adler32_destroy(ctx);
        return;
    }

    printf("分块计算校验和: 0x%08X\n", block_checksum);

    // 对比一次性计算
    uint32_t full_checksum = adler32_compute(data, data_size);
    printf("完整计算校验和: 0x%08X\n", full_checksum);
    printf("结果一致: %s\n", (block_checksum == full_checksum) ? "是" : "否");

    free(data);
    adler32_destroy(ctx);
}

// 演示 6: 文件校验和计算
static void demo_file_checksum(void) {
    printf("\n=== 演示 6: 文件校验和计算 ===\n");

    // 创建一个测试文件
    const char* test_filename = "/tmp/adler32_test.txt";
    FILE* fp = fopen(test_filename, "w");
    if (!fp) {
        fprintf(stderr, "无法创建测试文件\n");
        return;
    }

    fprintf(fp, "This is a test file for Adler-32 checksum computation.\n");
    fprintf(fp, "It contains multiple lines of text.\n");
    fprintf(fp, "Line 3: Testing file checksum functionality.\n");
    fclose(fp);

    adler32_ctx_t* ctx = NULL;
    adler32_config_t config = {
        .check_overflow = true,
        .use_large_window = false,
        .verify_result = true,
        .max_batch_size = 1024,
        .max_file_size = 1024 * 1024
    };

    adler32_error_t err = adler32_create(&ctx, &config);
    if (err != ADLER32_OK) {
        print_error("创建上下文", err);
        remove(test_filename);
        return;
    }

    uint32_t file_checksum;
    err = adler32_compute_file(ctx, test_filename, &file_checksum);
    if (err != ADLER32_OK) {
        print_error("文件校验和计算", err);
        adler32_destroy(ctx);
        remove(test_filename);
        return;
    }

    printf("文件: %s\n", test_filename);
    printf("文件校验和: 0x%08X\n", file_checksum);

    // 清理
    adler32_destroy(ctx);
    remove(test_filename);
}

// 演示 7: 错误处理
static void demo_error_handling(void) {
    printf("\n=== 演示 7: 错误处理 ===\n");

    adler32_ctx_t* ctx = NULL;
    adler32_error_t err;

    // 测试无效参数
    err = adler32_create(&ctx, NULL);
    printf("使用 NULL 配置创建上下文: %s\n", adler32_strerror(err));

    // 创建有效上下文
    adler32_config_t config = {
        .check_overflow = true,
        .use_large_window = false,
        .verify_result = true,
        .max_batch_size = 1024,
        .max_file_size = 1024
    };

    err = adler32_create(&ctx, &config);
    if (err != ADLER32_OK) {
        print_error("创建上下文", err);
        return;
    }

    // 测试无效的文件
    uint32_t checksum;
    err = adler32_compute_file(ctx, "/nonexistent/file.txt", &checksum);
    printf("读取不存在的文件: %s\n", adler32_strerror(err));

    adler32_destroy(ctx);
}

int main(void) {
    printf("========================================\n");
    printf("    Adler-32 校验和算法演示程序\n");
    printf("========================================\n");

    demo_basic_checksum();
    demo_safe_compute();
    demo_incremental();
    demo_batch_compute();
    demo_block_compute();
    demo_file_checksum();
    demo_error_handling();

    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("========================================\n");

    return 0;
}
