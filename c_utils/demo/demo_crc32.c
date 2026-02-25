/**
 * @file demo_crc32.c
 * @brief CRC32 校验演示
 *
 * 本演示展示了 CRC32 校验算法的应用，包括：
 * - 不同 CRC32 变体的计算
 * - 文件校验
 * - 增量计算
 * - 数据完整性验证
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../c_utils/crc32.h"

// 演示 1: 基本 CRC32 计算
static void demo_basic(void) {
    printf("\n=== 演示 1: 基本 CRC32 计算 ===\n");

    const char *test_strings[] = {
        "Hello, World!",
        "1234567890",
        "The quick brown fox jumps over the lazy dog",
        "",
        "A"
    };
    int num_strings = sizeof(test_strings) / sizeof(test_strings[0]);

    printf("标准 CRC32 计算:\n");
    for (int i = 0; i < num_strings; i++) {
        crc32_error_t error;
        uint32_t crc = crc32_compute(test_strings[i], strlen(test_strings[i]), CRC32_STANDARD, &error);
        printf("  \"%s\": 0x%08X\n", 
               strlen(test_strings[i]) > 20 ? "...(truncated)" : test_strings[i], 
               crc);
    }
}

// 演示 2: CRC32 变体比较
static void demo_variants(void) {
    printf("\n=== 演示 2: CRC32 变体比较 ===\n");

    const char *test_data = "123456789";
    size_t len = strlen(test_data);

    printf("测试数据: \"%s\"\n\n", test_data);

    crc32_variant_t variants[] = {
        CRC32_STANDARD,
        CRC32_C,
        CRC32_K,
        CRC32_Q
    };
    const char *variant_names[] = {
        "CRC32_STANDARD",
        "CRC32_C",
        "CRC32_K",
        "CRC32_Q"
    };

    for (int i = 0; i < 4; i++) {
        crc32_error_t error;
        uint32_t crc = crc32_compute(test_data, len, variants[i], &error);
        printf("  %s: 0x%08X\n", variant_names[i], crc);
    }
}

// 演示 3: 增量计算
static void demo_incremental(void) {
    printf("\n=== 演示 3: 增量计算 ===\n");

    crc32_context_t ctx;
    crc32_error_t error;

    printf("初始化 CRC32 上下文...\n");
    if (!crc32_init(&ctx, CRC32_STANDARD, &error)) {
        printf("初始化失败\n");
        return;
    }

    // 分块更新
    const char *chunks[] = {
        "Hello, ",
        "World",
        "!",
        " This is a test."
    };
    int num_chunks = sizeof(chunks) / sizeof(chunks[0]);

    printf("\n增量计算:\n");
    for (int i = 0; i < num_chunks; i++) {
        crc32_update(&ctx, chunks[i], strlen(chunks[i]));
        printf("  添加 \"%s\"\n", chunks[i]);
    }

    uint32_t final_crc = crc32_final(&ctx);
    printf("\n最终 CRC32: 0x%08X\n", final_crc);

    // 验证与一次性计算结果相同
    char full_string[256] = "";
    for (int i = 0; i < num_chunks; i++) {
        strcat(full_string, chunks[i]);
    }
    uint32_t direct_crc = crc32_compute(full_string, strlen(full_string), CRC32_STANDARD, &error);
    printf("直接计算: 0x%08X\n", direct_crc);
    printf("结果一致: %s\n", final_crc == direct_crc ? "是" : "否");
}

// 演示 4: 文件校验
static void demo_file_checksum(void) {
    printf("\n=== 演示 4: 文件校验 ===\n");

    // 创建测试文件
    const char *test_file = "/tmp/crc32_test.txt";
    FILE *fp = fopen(test_file, "w");
    if (!fp) {
        printf("无法创建测试文件\n");
        return;
    }

    fprintf(fp, "This is a test file for CRC32 checksum.\n");
    fprintf(fp, "Line 2: Some more text here.\n");
    fprintf(fp, "Line 3: End of file.\n");
    fclose(fp);

    printf("测试文件: %s\n", test_file);

    // 计算文件 CRC32
    crc32_error_t error;
    uint32_t file_crc = crc32_compute_file(test_file, CRC32_STANDARD, &error);
    if (file_crc != 0) {
        printf("文件 CRC32: 0x%08X\n", file_crc);
    } else {
        printf("文件计算失败\n");
    }

    // 清理
    remove(test_file);
}

// 演示 5: 数据完整性验证
static void demo_data_integrity(void) {
    printf("\n=== 演示 5: 数据完整性验证 ===\n");

    // 原始数据
    const char *original_data = "Important data that needs integrity check";
    size_t len = strlen(original_data);

    // 计算 CRC32
    crc32_error_t error;
    uint32_t original_crc = crc32_compute(original_data, len, CRC32_STANDARD, &error);
    
    printf("原始数据: \"%s\"\n", original_data);
    printf("原始 CRC32: 0x%08X\n", original_crc);

    // 模拟传输后的数据（未损坏）
    printf("\n场景 1: 数据未损坏\n");
    const char *received_data = original_data;
    uint32_t received_crc = crc32_compute(received_data, len, CRC32_STANDARD, &error);
    printf("  接收 CRC32: 0x%08X\n", received_crc);
    printf("  数据完整: %s\n", original_crc == received_crc ? "是" : "否");

    // 模拟传输后的数据（已损坏）
    printf("\n场景 2: 数据已损坏\n");
    char corrupted_data[256];
    strcpy(corrupted_data, original_data);
    corrupted_data[10] = 'X';  // 修改一个字符
    uint32_t corrupted_crc = crc32_compute(corrupted_data, len, CRC32_STANDARD, &error);
    printf("  损坏数据: \"...%s...\"\n", &corrupted_data[8]);
    printf("  接收 CRC32: 0x%08X\n", corrupted_crc);
    printf("  数据完整: %s\n", original_crc == corrupted_crc ? "是" : "否");
}

// 演示 6: 错误处理
static void demo_error_handling(void) {
    printf("\n=== 演示 6: 错误处理 ===\n");

    printf("错误码说明:\n");
    printf("  CRC32_OK (%d): 成功\n", CRC32_OK);
    printf("  CRC32_ERROR_INVALID_PARAM (%d): 无效参数\n", CRC32_ERROR_INVALID_PARAM);
    printf("  CRC32_ERROR_FILE_OPEN (%d): 文件打开失败\n", CRC32_ERROR_FILE_OPEN);
    printf("  CRC32_ERROR_FILE_READ (%d): 文件读取失败\n", CRC32_ERROR_FILE_READ);
    printf("  CRC32_ERROR_UNSUPPORTED_VARIANT (%d): 不支持的变体\n", CRC32_ERROR_UNSUPPORTED_VARIANT);

    // 测试无效参数
    printf("\n测试无效参数:\n");
    crc32_error_t error;
    crc32_context_t ctx;
    
    bool result = crc32_init(NULL, CRC32_STANDARD, &error);
    printf("  NULL 上下文: %s, 错误码: %d\n", result ? "成功" : "失败", error);
}

// 演示 7: 性能测试
static void demo_performance(void) {
    printf("\n=== 演示 7: 性能测试 ===\n");

    size_t sizes[] = {1024, 10240, 102400, 1048576};  // 1KB, 10KB, 100KB, 1MB
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    printf("CRC32 计算性能:\n\n");
    printf("%-10s %-15s %-15s\n", "数据大小", "时间(ms)", "速度(MB/s)");
    printf("----------------------------------------\n");

    for (int i = 0; i < num_sizes; i++) {
        size_t size = sizes[i];
        char *data = malloc(size);
        if (!data) continue;

        // 填充随机数据
        for (size_t j = 0; j < size; j++) {
            data[j] = (char)(j % 256);
        }

        // 计时
        clock_t start = clock();
        crc32_error_t error;
        uint32_t crc = crc32_compute(data, size, CRC32_STANDARD, &error);
        clock_t end = clock();

        double time_ms = (double)(end - start) * 1000 / CLOCKS_PER_SEC;
        double speed = (size / (1024.0 * 1024.0)) / (time_ms / 1000.0);

        if (size < 1024) {
            printf("%-10zu %-15.3f %-15.2f\n", size, time_ms, speed);
        } else if (size < 1024 * 1024) {
            printf("%-10.1fKB %-15.3f %-15.2f\n", size / 1024.0, time_ms, speed);
        } else {
            printf("%-10.1fMB %-15.3f %-15.2f\n", size / (1024.0 * 1024.0), time_ms, speed);
        }

        free(data);
    }
}

// 演示 8: 应用场景
static void demo_use_cases(void) {
    printf("\n=== 演示 8: 应用场景 ===\n");

    printf("CRC32 的应用场景:\n\n");

    printf("1. 数据存储:\n");
    printf("   - 文件系统完整性检查\n");
    printf("   - 数据库记录校验\n");
    printf("   - 归档文件验证\n\n");

    printf("2. 网络通信:\n");
    printf("   - 数据包完整性验证\n");
    printf("   - 协议帧校验\n");
    printf("   - 传输错误检测\n\n");

    printf("3. 压缩算法:\n");
    printf("   - ZIP 文件校验\n");
    printf("   - GZIP 数据验证\n");
    printf("   - PNG 图像校验\n\n");

    printf("4. 嵌入式系统:\n");
    printf("   - 固件完整性检查\n");
    printf("   - EEPROM 数据验证\n");
    printf("   - 通信协议校验\n");
}

// 演示 9: 与其他校验算法比较
static void demo_comparison(void) {
    printf("\n=== 演示 9: 与其他校验算法比较 ===\n");

    printf("校验算法比较:\n\n");

    printf("特性          CRC32      MD5        SHA-256\n");
    printf("-------------------------------------------\n");
    printf("输出长度      32位       128位      256位\n");
    printf("计算速度      快         中等       慢\n");
    printf("安全性        无         低         高\n");
    printf("用途          错误检测   完整性     安全性\n");
    printf("碰撞概率      较高       中等       极低\n\n");

    printf("CRC32 适用场景:\n");
    printf("  - 快速错误检测\n");
    printf("  - 非安全性的完整性验证\n");
    printf("  - 资源受限的环境\n");
    printf("  - 实时数据流处理\n");
}

// 演示 10: 最佳实践
static void demo_best_practices(void) {
    printf("\n=== 演示 10: 最佳实践 ===\n");

    printf("CRC32 使用最佳实践:\n\n");

    printf("1. 变体选择:\n");
    printf("   - 通用用途: CRC32_STANDARD\n");
    printf("   - 存储系统: CRC32_C (Castagnoli)\n");
    printf("   - 通信协议: 根据协议规范选择\n\n");

    printf("2. 性能优化:\n");
    printf("   - 使用查表法加速\n");
    printf("   - 大块数据处理更高效\n");
    printf("   - 考虑硬件加速\n\n");

    printf("3. 安全注意:\n");
    printf("   - CRC32 不是加密算法\n");
    printf("   - 不能用于安全校验\n");
    printf("   - 恶意攻击者可伪造 CRC\n\n");

    printf("4. 错误处理:\n");
    printf("   - 检查初始化返回值\n");
    printf("   - 验证文件读取结果\n");
    printf("   - 记录校验失败日志\n");
}

int main(void) {
    printf("========================================\n");
    printf("    CRC32 校验演示程序\n");
    printf("========================================\n");

    demo_basic();
    demo_variants();
    demo_incremental();
    demo_file_checksum();
    demo_data_integrity();
    demo_error_handling();
    demo_performance();
    demo_use_cases();
    demo_comparison();
    demo_best_practices();

    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("========================================\n");

    return 0;
}
