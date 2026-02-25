/**
 * 页分配器演示程序
 *
 * 功能：
 * - 页对齐内存分配
 * - 配置选项
 * - 统计信息
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/page_allocator.h"

// 演示 1: 基本分配
static void demo_basic(void) {
    printf("\n=== 演示 1: 基本页分配 ===\n");

    printf("系统页大小: %zu 字节\n", page_alloc_get_page_size());

    printf("\n分配 4096 字节...\n");
    void *ptr = page_alloc(4096);

    if (ptr) {
        printf("分配成功!\n");
        printf("  地址: %p\n", ptr);

        // 检查对齐
        size_t page_size = page_alloc_get_page_size();
        if ((size_t)ptr % page_size == 0) {
            printf("  对齐检查: 已页对齐\n");
        }

        // 使用内存
        memset(ptr, 0xAB, 4096);
        printf("  内存写入测试: 成功\n");

        page_free(ptr);
        printf("  已释放\n");
    } else {
        printf("分配失败\n");
    }
}

// 演示 2: 配置选项
static void demo_config(void) {
    printf("\n=== 演示 2: 配置选项 ===\n");

    page_alloc_config_t config = page_alloc_default_config();

    printf("默认配置:\n");
    printf("  对齐要求: %zu\n", config.alignment);
    printf("  零初始化: %s\n", config.zero_initialize ? "是" : "否");
    printf("  最小大小: %zu\n", config.min_size);
    printf("  最大大小: %zu (0=无限制)\n", config.max_size);

    printf("\n自定义配置:\n");
    config.zero_initialize = true;
    printf("  启用零初始化\n");

    page_alloc_error_t error;
    void *ptr = page_alloc_ex(8192, &config, &error);

    if (ptr) {
        printf("  分配成功\n");

        // 验证零初始化
        bool all_zero = true;
        unsigned char *p = (unsigned char*)ptr;
        for (int i = 0; i < 100 && all_zero; i++) {
            if (p[i] != 0) all_zero = false;
        }
        printf("  零初始化检查: %s\n", all_zero ? "通过" : "未通过");

        page_free(ptr);
    }
}

// 演示 3: 批量分配
static void demo_batch(void) {
    printf("\n=== 演示 3: 批量分配 ===\n");

    const int count = 5;
    void *ptrs[count];

    printf("批量分配 %d 个页对齐内存块...\n", count);

    page_alloc_error_t error;
    if (page_alloc_array(count, 4096, ptrs, &error)) {
        printf("分配成功!\n");

        for (int i = 0; i < count; i++) {
            printf("  块 %d: %p\n", i + 1, ptrs[i]);
        }

        printf("批量释放...\n");
        page_free_array(count, ptrs, &error);
        printf("释放完成\n");
    } else {
        printf("分配失败: %d\n", error);
    }
}

// 演示 4: 统计信息
static void demo_stats(void) {
    printf("\n=== 演示 4: 统计信息 ===\n");

    page_alloc_stats_t stats;

    printf("页分配器统计:\n");
    printf("  系统页大小: %zu 字节\n", page_alloc_get_page_size());

    printf("\n应用场景统计:\n");
    printf("  总分配内存: 跟踪累计分配\n");
    printf("  当前分配: 跟踪未释放内存\n");
    printf("  分配次数: 跟踪分配调用次数\n");
    printf("  释放次数: 跟踪释放调用次数\n");
}

// 演示 5: 错误处理
static void demo_errors(void) {
    printf("\n=== 演示 5: 错误处理 ===\n");

    printf("可能的错误码:\n");
    printf("  PAGE_ALLOC_OK - 成功\n");
    printf("  PAGE_ALLOC_ERROR_NULL_PTR - 空指针\n");
    printf("  PAGE_ALLOC_ERROR_INVALID_SIZE - 无效大小\n");
    printf("  PAGE_ALLOC_ERROR_ALLOCATION_FAILED - 分配失败\n");
    printf("  PAGE_ALLOC_ERROR_INVALID_ALIGNMENT - 无效对齐\n");

    printf("\n测试无效大小:\n");
    page_alloc_error_t error;
    void *ptr = page_alloc_ex(0, NULL, &error);
    if (!ptr) {
        printf("  预期错误: %d\n", error);
    }
}

// 演示 6: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 6: 应用场景 ===\n");

    printf("1. DMA 缓冲区\n");
    printf("   - 硬件需要页对齐内存\n");
    printf("   - 直接内存访问\n");
    printf("   - 设备驱动程序\n\n");

    printf("2. 大页内存\n");
    printf("   - 减少 TLB miss\n");
    printf("   - 提高内存访问性能\n");
    printf("   - 数据库缓冲区\n\n");

    printf("3. 共享内存\n");
    printf("   - 进程间通信\n");
    printf("   - 页对齐便于映射\n\n");

    printf("4. 内存映射 I/O\n");
    printf("   - 设备寄存器映射\n");
    printf("   - 需要页对齐地址\n\n");

    printf("5. JIT 编译\n");
    printf("   - 可执行内存页\n");
    printf("   - 代码生成\n");
}

// 演示 7: 与普通 malloc 对比
static void demo_comparison(void) {
    printf("\n=== 演示 7: 与普通 malloc 对比 ===\n");

    printf("页分配器:\n");
    printf("  - 保证页对齐\n");
    printf("  - 适合特殊用途\n");
    printf("  - 可能内部碎片\n");
    printf("  - 系统调用开销\n\n");

    printf("malloc:\n");
    printf("  - 通用内存分配\n");
    printf("  - 更细粒度管理\n");
    printf("  - 更好的内存利用率\n");
    printf("  - 用户空间实现\n\n");

    printf("选择建议:\n");
    printf("  - 需要页对齐: 页分配器\n");
    printf("  - 一般用途: malloc\n");
    printf("  - 大内存块: 页分配器\n");
}

int main(void) {
    printf("========================================\n");
    printf("    页分配器演示\n");
    printf("========================================\n");

    demo_basic();
    demo_config();
    demo_batch();
    demo_stats();
    demo_errors();
    demo_applications();
    demo_comparison();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
