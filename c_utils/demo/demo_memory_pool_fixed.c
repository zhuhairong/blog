/**
 * 固定大小内存池演示程序
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/memory_pool_fixed.h"

static void demo_basic_usage(void) {
    printf("\n=== 演示 1: 基本使用 ===\n");

    mem_pool_fixed_t pool;
    size_t block_size = 64;
    size_t num_blocks = 10;

    printf("创建内存池: 块大小=%zu, 块数量=%zu\n", block_size, num_blocks);

    mem_pool_fixed_error_t err = mem_pool_fixed_init(&pool, block_size, num_blocks);
    if (err != MEM_POOL_FIXED_OK) {
        printf("初始化失败: %d\n", err);
        return;
    }
    printf("内存池创建成功!\n");

    printf("\n分配 5 个块:\n");
    void *blocks[5];
    for (int i = 0; i < 5; i++) {
        blocks[i] = mem_pool_fixed_alloc(&pool);
        if (blocks[i]) {
            sprintf((char*)blocks[i], "Block-%d", i);
            printf("  分配块 %d: %s\n", i, (char*)blocks[i]);
        }
    }

    mem_pool_fixed_stats_t stats;
    mem_pool_fixed_get_stats(&pool, &stats);
    printf("\n已用块数: %zu\n", stats.used_blocks);
    printf("可用块数: %zu\n", stats.free_blocks);

    printf("\n释放块:\n");
    for (int i = 0; i < 5; i++) {
        if (blocks[i]) {
            mem_pool_fixed_free(&pool, blocks[i]);
            printf("  释放块 %d\n", i);
        }
    }

    mem_pool_fixed_destroy(&pool);
    printf("\n内存池已销毁\n");
}

static void demo_config_options(void) {
    printf("\n=== 演示 2: 配置选项 ===\n");

    mem_pool_fixed_t pool;
    mem_pool_fixed_config_t config = mem_pool_fixed_default_config();
    config.zero_initialize = true;
    config.alignment = 16;

    printf("配置: 零初始化=true, 对齐=16\n");

    mem_pool_fixed_error_t err = mem_pool_fixed_init_ex(&pool, 128, 5, &config);
    if (err != MEM_POOL_FIXED_OK) {
        printf("初始化失败: %d\n", err);
        return;
    }

    void *block = mem_pool_fixed_alloc(&pool);
    if (block) {
        printf("分配成功，检查是否零初始化:\n");
        unsigned char *p = (unsigned char *)block;
        bool all_zero = true;
        for (int i = 0; i < 128; i++) {
            if (p[i] != 0) {
                all_zero = false;
                break;
            }
        }
        printf("  零初始化: %s\n", all_zero ? "是" : "否");
        mem_pool_fixed_free(&pool, block);
    }

    mem_pool_fixed_destroy(&pool);
}

static void demo_exhaustion(void) {
    printf("\n=== 演示 3: 内存耗尽 ===\n");

    mem_pool_fixed_t pool;
    mem_pool_fixed_init(&pool, 32, 3);

    printf("创建内存池: 3 个块\n");

    void *b1 = mem_pool_fixed_alloc(&pool);
    void *b2 = mem_pool_fixed_alloc(&pool);
    void *b3 = mem_pool_fixed_alloc(&pool);
    void *b4 = mem_pool_fixed_alloc(&pool);

    printf("分配结果:\n");
    printf("  块 1: %s\n", b1 ? "成功" : "失败");
    printf("  块 2: %s\n", b2 ? "成功" : "失败");
    printf("  块 3: %s\n", b3 ? "成功" : "失败");
    printf("  块 4: %s (预期失败)\n", b4 ? "成功" : "失败");

    if (b1) mem_pool_fixed_free(&pool, b1);
    if (b2) mem_pool_fixed_free(&pool, b2);
    if (b3) mem_pool_fixed_free(&pool, b3);

    mem_pool_fixed_destroy(&pool);
}

static void demo_stats(void) {
    printf("\n=== 演示 4: 统计信息 ===\n");

    mem_pool_fixed_t pool;
    size_t block_size = 256;
    size_t num_blocks = 20;

    mem_pool_fixed_init(&pool, block_size, num_blocks);

    mem_pool_fixed_stats_t stats;
    mem_pool_fixed_get_stats(&pool, &stats);

    printf("初始状态:\n");
    printf("  总块数: %zu\n", stats.total_blocks);
    printf("  已用块数: %zu\n", stats.used_blocks);
    printf("  空闲块数: %zu\n", stats.free_blocks);
    printf("  块大小: %zu\n", stats.block_size);
    printf("  总内存: %zu\n", stats.total_memory);

    void *blocks[10];
    for (int i = 0; i < 10; i++) {
        blocks[i] = mem_pool_fixed_alloc(&pool);
    }

    mem_pool_fixed_get_stats(&pool, &stats);
    printf("\n分配 10 块后:\n");
    printf("  已用块数: %zu\n", stats.used_blocks);
    printf("  空闲块数: %zu\n", stats.free_blocks);

    for (int i = 0; i < 10; i++) {
        if (blocks[i]) mem_pool_fixed_free(&pool, blocks[i]);
    }

    mem_pool_fixed_destroy(&pool);
}

int main(void) {
    printf("========================================\n");
    printf("    固定大小内存池演示\n");
    printf("========================================\n");

    demo_basic_usage();
    demo_config_options();
    demo_exhaustion();
    demo_stats();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
