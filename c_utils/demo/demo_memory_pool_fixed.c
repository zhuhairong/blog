/**
 * 固定大小内存池演示程序
 *
 * 功能：
 * - 内存池初始化和销毁
 * - 内存分配和释放
 * - 性能对比
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../c_utils/memory_pool_fixed.h"

// 演示 1: 基本操作
static void demo_basic(void) {
    printf("\n=== 演示 1: 基本操作 ===\n");

    printf("创建内存池:\n");
    printf("  块大小: 64 字节\n");
    printf("  块数量: 10\n\n");

    mem_pool_fixed_t pool;
    mem_pool_fixed_error_t error = mem_pool_fixed_init(&pool, 64, 10);

    if (error == MEM_POOL_FIXED_OK) {
        printf("内存池创建成功!\n\n");

        printf("分配 5 个块:\n");
        void *blocks[5];
        for (int i = 0; i < 5; i++) {
            blocks[i] = mem_pool_fixed_alloc(&pool);
            printf("  块 %d: %p\n", i + 1, blocks[i]);
        }

        printf("\n释放第 3 个块...\n");
        mem_pool_fixed_free(&pool, blocks[2]);

        printf("再次分配 (应该复用刚释放的块):\n");
        void *new_block = mem_pool_fixed_alloc(&pool);
        printf("  新块: %p\n", new_block);

        // 清理
        for (int i = 0; i < 5; i++) {
            if (i != 2) {
                mem_pool_fixed_free(&pool, blocks[i]);
            }
        }
        mem_pool_fixed_free(&pool, new_block);

        mem_pool_fixed_destroy(&pool);
        printf("\n内存池已销毁\n");
    } else {
        printf("创建失败: %d\n", error);
    }
}

// 演示 2: 内存池统计
static void demo_stats(void) {
    printf("\n=== 演示 2: 内存池统计 ===\n");

    mem_pool_fixed_t pool;
    mem_pool_fixed_init(&pool, 32, 5);

    mem_pool_fixed_stats_t stats;
    mem_pool_fixed_get_stats(&pool, &stats);

    printf("初始状态:\n");
    printf("  总块数: %zu\n", stats.total_blocks);
    printf("  已用块: %zu\n", stats.used_blocks);
    printf("  空闲块: %zu\n", stats.free_blocks);
    printf("  块大小: %zu 字节\n", stats.block_size);
    printf("  总内存: %zu 字节\n\n", stats.total_memory);

    printf("分配 3 个块...\n");
    void *b1 = mem_pool_fixed_alloc(&pool);
    void *b2 = mem_pool_fixed_alloc(&pool);
    void *b3 = mem_pool_fixed_alloc(&pool);

    mem_pool_fixed_get_stats(&pool, &stats);
    printf("  已用块: %zu\n", stats.used_blocks);
    printf("  空闲块: %zu\n\n", stats.free_blocks);

    printf("释放所有块...\n");
    mem_pool_fixed_free(&pool, b1);
    mem_pool_fixed_free(&pool, b2);
    mem_pool_fixed_free(&pool, b3);

    mem_pool_fixed_get_stats(&pool, &stats);
    printf("  已用块: %zu\n", stats.used_blocks);
    printf("  空闲块: %zu\n", stats.free_blocks);

    mem_pool_fixed_destroy(&pool);
}

// 演示 3: 配置选项
static void demo_config(void) {
    printf("\n=== 演示 3: 配置选项 ===\n");

    mem_pool_fixed_config_t config = mem_pool_fixed_default_config();

    printf("默认配置:\n");
    printf("  线程安全: %s\n", config.thread_safe ? "是" : "否");
    printf("  零初始化: %s\n", config.zero_initialize ? "是" : "否");
    printf("  对齐要求: %zu\n", config.alignment);

    printf("\n使用自定义配置:\n");
    config.zero_initialize = true;
    printf("  启用零初始化\n");

    mem_pool_fixed_t pool;
    mem_pool_fixed_init_ex(&pool, 64, 5, &config);

    void *block = mem_pool_fixed_alloc(&pool);
    if (block) {
        // 检查是否为零初始化
        unsigned char *p = (unsigned char*)block;
        bool all_zero = true;
        for (int i = 0; i < 64; i++) {
            if (p[i] != 0) {
                all_zero = false;
                break;
            }
        }
        printf("  块是否零初始化: %s\n", all_zero ? "是" : "否");

        mem_pool_fixed_free(&pool, block);
    }

    mem_pool_fixed_destroy(&pool);
}

// 演示 4: 性能对比
static void demo_performance(void) {
    printf("\n=== 演示 4: 性能对比 ===\n");

    const int iterations = 100000;
    const int block_size = 64;

    printf("测试 %d 次分配/释放操作:\n\n", iterations);

    // 内存池测试
    mem_pool_fixed_t pool;
    mem_pool_fixed_init(&pool, block_size, 100);

    clock_t start = clock();
    for (int i = 0; i < iterations; i++) {
        void *p = mem_pool_fixed_alloc(&pool);
        mem_pool_fixed_free(&pool, p);
    }
    clock_t pool_time = clock() - start;

    printf("内存池:\n");
    printf("  时间: %.3f ms\n", (double)pool_time * 1000 / CLOCKS_PER_SEC);

    // malloc/free 测试
    start = clock();
    for (int i = 0; i < iterations; i++) {
        void *p = malloc(block_size);
        free(p);
    }
    clock_t malloc_time = clock() - start;

    printf("\nmalloc/free:\n");
    printf("  时间: %.3f ms\n", (double)malloc_time * 1000 / CLOCKS_PER_SEC);

    if (malloc_time > 0) {
        printf("\n内存池速度提升: %.1fx\n",
               (double)malloc_time / pool_time);
    }

    mem_pool_fixed_destroy(&pool);
}

// 演示 5: 内存碎片对比
static void demo_fragmentation(void) {
    printf("\n=== 演示 5: 内存碎片 ===\n");

    printf("内存池特点:\n");
    printf("  - 预分配连续内存块\n");
    printf("  - 无外部碎片\n");
    printf("  - 可预测的内存使用\n\n");

    printf("malloc/free 问题:\n");
    printf("  - 可能产生内存碎片\n");
    printf("  - 频繁的分配/释放导致碎片化\n");
    printf("  - 长时间运行后性能下降\n\n");

    printf("内存池优势场景:\n");
    printf("  - 固定大小的对象分配\n");
    printf("  - 高频分配/释放\n");
    printf("  - 实时系统\n");
}

// 演示 6: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 6: 应用场景 ===\n");

    printf("1. 网络服务器\n");
    printf("   - 连接对象池\n");
    printf("   - 请求缓冲区池\n");
    printf("\n");

    printf("2. 游戏开发\n");
    printf("   - 游戏对象池\n");
    printf("   - 粒子系统\n");
    printf("   - 音频缓冲区\n");
    printf("\n");

    printf("3. 嵌入式系统\n");
    printf("   - 有限的堆内存\n");
    printf("   - 避免内存碎片\n");
    printf("   - 确定性分配时间\n");
    printf("\n");

    printf("4. 数据库系统\n");
    printf("   - 页缓冲区池\n");
    printf("   - 事务对象池\n");
    printf("   - 连接池\n");
}

int main(void) {
    printf("========================================\n");
    printf("    固定大小内存池演示\n");
    printf("========================================\n");

    demo_basic();
    demo_stats();
    demo_config();
    demo_performance();
    demo_fragmentation();
    demo_applications();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
