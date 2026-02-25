/**
 * 内存映射文件演示程序
 *
 * 功能：
 * - 文件内存映射
 * - 映射配置选项
 * - 同步操作
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/mmap.h"

// 演示 1: 基本映射
static void demo_basic(void) {
    printf("\n=== 演示 1: 基本文件映射 ===\n");

    const char *filename = "/tmp/demo_mmap.txt";

    // 创建测试文件
    FILE *fp = fopen(filename, "w");
    if (fp) {
        fprintf(fp, "Hello, Memory Mapping!\n");
        fprintf(fp, "This is a test file for mmap demonstration.\n");
        fclose(fp);

        printf("创建测试文件: %s\n", filename);

        size_t size;
        void *addr = mmap_file(filename, &size);

        if (addr) {
            printf("映射成功!\n");
            printf("  映射地址: %p\n", addr);
            printf("  文件大小: %zu 字节\n", size);
            printf("  内容预览:\n");

            // 显示前 50 字节
            char preview[51];
            size_t preview_len = size < 50 ? size : 50;
            memcpy(preview, addr, preview_len);
            preview[preview_len] = '\0';
            printf("    %s\n", preview);

            mmap_free(addr, size);
            printf("  已解除映射\n");
        } else {
            printf("映射失败\n");
        }

        remove(filename);
    }
}

// 演示 2: 配置选项
static void demo_config(void) {
    printf("\n=== 演示 2: 映射配置选项 ===\n");

    mmap_config_t config = mmap_default_config();

    printf("默认配置:\n");
    printf("  只读: %s\n", config.read_only ? "是" : "否");
    printf("  私有: %s\n", config.private ? "是" : "否");
    printf("  预加载: %s\n", config.populate ? "是" : "否");
    printf("  偏移量: %zu\n", config.offset);
    printf("  长度: %zu (0 表示整个文件)\n", config.length);

    printf("\n配置说明:\n");
    printf("  - 只读: 映射区域不可写入\n");
    printf("  - 私有: 写时复制，不影响原文件\n");
    printf("  - 预加载: 映射时预读文件内容到内存\n");
}

// 演示 3: 映射信息
static void demo_info(void) {
    printf("\n=== 演示 3: 获取映射信息 ===\n");

    const char *filename = "/tmp/demo_mmap2.txt";

    FILE *fp = fopen(filename, "w");
    if (fp) {
        fprintf(fp, "Test content for mmap info.\n");
        fclose(fp);

        mmap_config_t config = mmap_default_config();
        mmap_info_t info;
        mmap_error_t error;

        if (mmap_file_info(filename, &config, &info, &error)) {
            printf("映射信息:\n");
            printf("  地址: %p\n", info.addr);
            printf("  映射大小: %zu\n", info.size);
            printf("  文件大小: %zu\n", info.file_size);
            printf("  只读: %s\n", info.read_only ? "是" : "否");
            printf("  私有: %s\n", info.private ? "是" : "否");
            printf("  文件名: %s\n", info.filename);

            mmap_free(info.addr, info.size);
        }

        remove(filename);
    }
}

// 演示 4: 错误处理
static void demo_errors(void) {
    printf("\n=== 演示 4: 错误处理 ===\n");

    printf("可能的错误码:\n");
    printf("  MMAP_OK: 成功\n");
    printf("  MMAP_ERROR_NULL_PTR: 空指针\n");
    printf("  MMAP_ERROR_OPEN_FILE: 文件打开失败\n");
    printf("  MMAP_ERROR_FILE_SIZE: 获取文件大小失败\n");
    printf("  MMAP_ERROR_MMAP: 映射失败\n");
    printf("  MMAP_ERROR_INVALID_ARGS: 无效参数\n");
    printf("  MMAP_ERROR_UNMAP: 解除映射失败\n");

    printf("\n测试不存在的文件:\n");
    mmap_error_t error;
    size_t size;
    void *addr = mmap_file_ex("/nonexistent/file.txt", &size, NULL, &error);

    if (!addr) {
        printf("  预期错误: %d\n", error);
    }
}

// 演示 5: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 5: 应用场景 ===\n");

    printf("1. 大文件处理\n");
    printf("   - 无需一次性读入内存\n");
    printf("   - 按需加载数据\n");
    printf("   - 减少内存占用\n\n");

    printf("2. 进程间通信\n");
    printf("   - 共享内存映射\n");
    printf("   - 高效数据交换\n\n");

    printf("3. 数据库系统\n");
    printf("   - 数据文件映射\n");
    printf("   - 快速访问\n");
    printf("   - 利用操作系统缓存\n\n");

    printf("4. 配置文件读取\n");
    printf("   - 快速访问配置\n");
    printf("   - 无需解析加载\n\n");

    printf("5. 日志系统\n");
    printf("   - 高效写入\n");
    printf("   - 持久化保证\n");
}

// 演示 6: 性能优势
static void demo_performance(void) {
    printf("\n=== 演示 6: 性能优势 ===\n");

    printf("内存映射 vs 传统 I/O:\n\n");

    printf("内存映射优势:\n");
    printf("  - 减少数据拷贝 (零拷贝)\n");
    printf("  - 利用操作系统页缓存\n");
    printf("  - 按需加载 (懒加载)\n");
    printf("  - 简化编程模型\n\n");

    printf("传统 I/O 特点:\n");
    printf("  - 显式读写调用\n");
    printf("  - 用户缓冲区管理\n");
    printf("  - 更多系统调用开销\n\n");

    printf("适用场景:\n");
    printf("  - 大文件随机访问\n");
    printf("  - 频繁读写操作\n");
    printf("  - 进程间共享数据\n");
}

int main(void) {
    printf("========================================\n");
    printf("    内存映射文件演示\n");
    printf("========================================\n");

    demo_basic();
    demo_config();
    demo_info();
    demo_errors();
    demo_applications();
    demo_performance();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
