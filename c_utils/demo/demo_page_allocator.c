/**
 * 页分配器演示程序
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "../c_utils/page_allocator.h"

static void demo_basic_usage(void) {
    printf("\n=== 演示 1: 基本使用 ===\n");

    size_t page_size = page_alloc_get_page_size();
    printf("系统页大小: %zu 字节\n", page_size);

    size_t alloc_size = page_size * 2;
    printf("分配 %zu 字节 (2 页)\n", alloc_size);

    void *ptr = page_alloc(alloc_size);
    if (ptr) {
        printf("分配成功!\n");
        printf("地址: %p\n", ptr);
        printf("页对齐: %s\n", page_is_aligned(ptr, 0) ? "是" : "否");

        memset(ptr, 0xAB, alloc_size);
        printf("写入测试数据完成\n");

        page_free(ptr);
        printf("释放成功\n");
    } else {
        printf("分配失败\n");
    }
}

static void demo_config_options(void) {
    printf("\n=== 演示 2: 配置选项 ===\n");

    page_alloc_config_t config = page_alloc_default_config();
    config.zero_initialize = true;
    config.alignment = 4096;

    printf("配置: 零初始化=true, 对齐=4096\n");

    page_alloc_error_t error;
    void *ptr = page_alloc_ex(8192, &config, &error);

    if (ptr) {
        printf("分配成功\n");

        unsigned char *p = (unsigned char *)ptr;
        bool all_zero = true;
        for (size_t i = 0; i < 8192; i++) {
            if (p[i] != 0) {
                all_zero = false;
                break;
            }
        }
        printf("零初始化验证: %s\n", all_zero ? "通过" : "失败");

        page_free(ptr);
    } else {
        printf("分配失败: %s\n", page_alloc_error_string(error));
    }
}

static void demo_stats(void) {
    printf("\n=== 演示 3: 统计信息 ===\n");

    page_alloc_error_t error;
    page_alloc_stats_t stats;

    page_alloc_get_stats(&stats, &error);
    printf("初始状态:\n");
    printf("  总分配: %zu 字节\n", stats.total_allocated);
    printf("  当前分配: %zu 字节\n", stats.current_allocated);
    printf("  分配次数: %zu\n", stats.allocation_count);
    printf("  释放次数: %zu\n", stats.free_count);

    void *p1 = page_alloc(4096);
    void *p2 = page_alloc(8192);

    page_alloc_get_stats(&stats, &error);
    printf("\n分配 2 块后:\n");
    printf("  当前分配: %zu 字节\n", stats.current_allocated);
    printf("  分配次数: %zu\n", stats.allocation_count);

    if (p1) page_free(p1);
    if (p2) page_free(p2);

    page_alloc_get_stats(&stats, &error);
    printf("\n释放后:\n");
    printf("  当前分配: %zu 字节\n", stats.current_allocated);
    printf("  释放次数: %zu\n", stats.free_count);
}

static void demo_array_allocation(void) {
    printf("\n=== 演示 4: 数组分配 ===\n");

    void *ptrs[5];
    page_alloc_error_t error;

    printf("分配 5 个页对齐内存块...\n");

    bool success = page_alloc_array(5, 4096, ptrs, &error);
    if (success) {
        printf("分配成功!\n");
        for (int i = 0; i < 5; i++) {
            printf("  块 %d: %p (对齐: %s)\n", i, ptrs[i],
                   page_is_aligned(ptrs[i], 0) ? "是" : "否");
        }

        page_free_array(5, ptrs, &error);
        printf("全部释放成功\n");
    } else {
        printf("分配失败: %s\n", page_alloc_error_string(error));
    }
}

static void demo_realloc(void) {
    printf("\n=== 演示 5: 重分配 ===\n");

    page_alloc_error_t error;
    size_t old_size = 4096;
    size_t new_size = 8192;

    void *ptr = page_alloc(old_size);
    if (ptr) {
        printf("初始分配: %zu 字节\n", old_size);

        memset(ptr, 0x55, old_size);

        void *new_ptr = page_realloc(ptr, old_size, new_size, &error);
        if (new_ptr) {
            printf("重分配成功: %zu 字节\n", new_size);

            unsigned char *p = (unsigned char *)new_ptr;
            bool data_ok = true;
            for (size_t i = 0; i < old_size; i++) {
                if (p[i] != 0x55) {
                    data_ok = false;
                    break;
                }
            }
            printf("数据保留验证: %s\n", data_ok ? "通过" : "失败");

            page_free(new_ptr);
        } else {
            printf("重分配失败: %s\n", page_alloc_error_string(error));
            page_free(ptr);
        }
    }
}

int main(void) {
    printf("========================================\n");
    printf("    页分配器演示\n");
    printf("========================================\n");

    demo_basic_usage();
    demo_config_options();
    demo_stats();
    demo_array_allocation();
    demo_realloc();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
