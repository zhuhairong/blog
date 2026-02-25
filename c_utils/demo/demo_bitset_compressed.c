/**
 * @file demo_bitset_compressed.c
 * @brief 压缩位集合 (RLE) 演示
 *
 * 本演示展示了压缩位集合的各种操作，包括：
 * - 创建和销毁
 * - 基本位操作
 * - 压缩效果对比
 * - 序列化
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/bitset_compressed.h"

// 打印压缩位集合的统计信息
static void print_compressed_stats(const bitset_compressed_t* bc, const char* label) {
    printf("%s:\n", label);
    printf("  总位数: %zu\n", bitset_comp_size(bc));
    printf("  运行段数: %zu\n", bitset_comp_run_count(bc));
    printf("  内存占用: %zu 字节\n", bitset_comp_byte_size(bc));
    printf("  压缩率: %.2f%%\n",
           100.0 * bitset_comp_byte_size(bc) / ((bitset_comp_size(bc) + 7) / 8));
}

// 打印位集合内容（前 n 位）
static void print_bits(const bitset_compressed_t* bc, size_t n) {
    printf("位内容 (前 %zu 位): ", n);
    for (size_t i = 0; i < n && i < bitset_comp_size(bc); i++) {
        printf("%d", bitset_comp_test(bc, i) ? 1 : 0);
        if ((i + 1) % 8 == 0) printf(" ");
    }
    printf("\n");
}

// 演示 1: 创建和基本操作
static void demo_create_and_basic(void) {
    printf("\n=== 演示 1: 创建和基本操作 ===\n");

    bitset_compressed_t* bc = bitset_comp_create(1000);
    if (!bc) {
        fprintf(stderr, "创建失败\n");
        return;
    }

    printf("创建 1000 位压缩位集合\n");
    print_compressed_stats(bc, "初始状态");

    // 设置一些位
    printf("\n设置位 0, 100, 200, 300, 400, 500\n");
    bitset_comp_set(bc, 0);
    bitset_comp_set(bc, 100);
    bitset_comp_set(bc, 200);
    bitset_comp_set(bc, 300);
    bitset_comp_set(bc, 400);
    bitset_comp_set(bc, 500);

    print_bits(bc, 32);
    print_compressed_stats(bc, "设置后");

    // 测试位
    printf("\n位测试:\n");
    for (size_t i = 0; i <= 500; i += 100) {
        printf("  位 %zu: %s\n", i, bitset_comp_test(bc, i) ? "已设置" : "未设置");
    }

    bitset_comp_free(bc);
}

// 演示 2: 范围操作
static void demo_range_operations(void) {
    printf("\n=== 演示 2: 范围操作 ===\n");

    bitset_compressed_t* bc = bitset_comp_create(256);
    if (!bc) return;

    printf("设置范围 [50, 100] 为 1\n");
    bitset_comp_set_range(bc, 50, 100, true);
    print_bits(bc, 128);
    print_compressed_stats(bc, "状态");

    printf("\n设置范围 [150, 200] 为 1\n");
    bitset_comp_set_range(bc, 150, 200, true);
    print_compressed_stats(bc, "状态");

    printf("\n清除范围 [60, 90]\n");
    bitset_comp_set_range(bc, 60, 90, false);
    print_bits(bc, 128);
    print_compressed_stats(bc, "状态");

    printf("\n范围统计:\n");
    printf("  范围 [0, 255] 中 1 的个数: %zu\n",
           bitset_comp_count_range(bc, 0, 255, true));
    printf("  范围 [0, 255] 中 0 的个数: %zu\n",
           bitset_comp_count_range(bc, 0, 255, false));

    bitset_comp_free(bc);
}

// 演示 3: 压缩效果对比
static void demo_compression_comparison(void) {
    printf("\n=== 演示 3: 压缩效果对比 ===\n");

    const size_t sizes[] = {100, 1000, 10000};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    for (int i = 0; i < num_sizes; i++) {
        size_t n = sizes[i];
        printf("\n--- 大小 %zu 位 ---\n", n);

        // 稀疏位集合（少量 1）
        bitset_compressed_t* sparse = bitset_comp_create(n);
        for (size_t j = 0; j < n; j += 10) {
            bitset_comp_set(sparse, j);
        }

        // 密集位集合（大量 1）
        bitset_compressed_t* dense = bitset_comp_create(n);
        for (size_t j = 0; j < n; j++) {
            if (j % 3 == 0) bitset_comp_set(dense, j);
        }

        // 交替位集合
        bitset_compressed_t* alternating = bitset_comp_create(n);
        for (size_t j = 0; j < n; j++) {
            bitset_comp_set(alternating, j);
        }
        for (size_t j = 0; j < n; j += 2) {
            bitset_comp_clear(alternating, j);
        }

        size_t normal_size = (n + 7) / 8;

        printf("稀疏位集合 (每 10 位一个 1):\n");
        printf("  普通位集合: %zu 字节\n", normal_size);
        printf("  压缩位集合: %zu 字节 (%.1f%%)\n",
               bitset_comp_byte_size(sparse),
               100.0 * bitset_comp_byte_size(sparse) / normal_size);

        printf("密集位集合 (每 3 位一个 1):\n");
        printf("  普通位集合: %zu 字节\n", normal_size);
        printf("  压缩位集合: %zu 字节 (%.1f%%)\n",
               bitset_comp_byte_size(dense),
               100.0 * bitset_comp_byte_size(dense) / normal_size);

        printf("交替位集合:\n");
        printf("  普通位集合: %zu 字节\n", normal_size);
        printf("  压缩位集合: %zu 字节 (%.1f%%)\n",
               bitset_comp_byte_size(alternating),
               100.0 * bitset_comp_byte_size(alternating) / normal_size);

        bitset_comp_free(sparse);
        bitset_comp_free(dense);
        bitset_comp_free(alternating);
    }
}

// 演示 4: 查找操作
static void demo_finding(void) {
    printf("\n=== 演示 4: 查找操作 ===\n");

    bitset_compressed_t* bc = bitset_comp_create(200);
    if (!bc) return;

    // 设置一些范围
    bitset_comp_set_range(bc, 10, 20, true);
    bitset_comp_set_range(bc, 50, 60, true);
    bitset_comp_set_range(bc, 100, 110, true);

    print_bits(bc, 128);

    printf("\n查找操作:\n");
    printf("  第一个 1 (从 0): %zu\n", bitset_comp_find_first(bc, true, 0));
    printf("  第一个 1 (从 30): %zu\n", bitset_comp_find_first(bc, true, 30));
    printf("  第一个 0 (从 0): %zu\n", bitset_comp_find_first(bc, false, 0));
    printf("  最后一个 1: %zu\n", bitset_comp_find_last(bc, true));
    printf("  最后一个 0: %zu\n", bitset_comp_find_last(bc, false));

    bitset_comp_free(bc);
}

// 演示 5: 克隆
static void demo_clone(void) {
    printf("\n=== 演示 5: 克隆 ===\n");

    bitset_compressed_t* original = bitset_comp_create(100);
    if (!original) return;

    bitset_comp_set_range(original, 20, 40, true);
    bitset_comp_set_range(original, 60, 80, true);

    print_bits(original, 100);
    print_compressed_stats(original, "原始");

    bitset_compressed_t* clone = bitset_comp_clone(original);
    if (clone) {
        print_compressed_stats(clone, "克隆");

        // 修改克隆
        bitset_comp_set_range(clone, 45, 55, true);

        printf("\n修改克隆后:\n");
        print_bits(original, 100);
        print_bits(clone, 100);

        bitset_comp_free(clone);
    }

    bitset_comp_free(original);
}

// 演示 6: 序列化
static void demo_serialization(void) {
    printf("\n=== 演示 6: 序列化 ===\n");

    bitset_compressed_t* bc = bitset_comp_create(256);
    if (!bc) return;

    // 设置一些位
    bitset_comp_set_range(bc, 10, 50, true);
    bitset_comp_set_range(bc, 100, 150, true);

    print_compressed_stats(bc, "原始数据");

    // 序列化
    size_t buf_size = bitset_comp_byte_size(bc);
    uint8_t* buffer = malloc(buf_size);
    if (buffer) {
        if (bitset_comp_to_bytes(bc, buffer, buf_size)) {
            printf("\n序列化成功，%zu 字节\n", buf_size);

            // 反序列化
            bitset_compressed_t* restored = bitset_comp_from_bytes(buffer, buf_size);
            if (restored) {
                print_compressed_stats(restored, "恢复后");

                // 验证
                bool match = (bitset_comp_size(bc) == bitset_comp_size(restored)) &&
                            (bitset_comp_count(bc, true) == bitset_comp_count(restored, true));
                printf("\n数据一致性: %s\n", match ? "通过" : "失败");

                bitset_comp_free(restored);
            }
        }
        free(buffer);
    }

    bitset_comp_free(bc);
}

// 演示 7: 实际应用 - 时间槽管理
static void demo_timeslot_management(void) {
    printf("\n=== 演示 7: 实际应用 - 时间槽管理 ===\n");

    // 模拟 24 小时，每小时一个槽
    bitset_compressed_t* slots = bitset_comp_create(24);
    if (!slots) return;

    printf("会议室预订系统 (24 小时制)\n\n");

    // 预订一些时间段
    printf("预订 9:00-12:00\n");
    bitset_comp_set_range(slots, 9, 12, true);

    printf("预订 14:00-16:00\n");
    bitset_comp_set_range(slots, 14, 16, true);

    printf("预订 19:00-21:00\n");
    bitset_comp_set_range(slots, 19, 21, true);

    printf("\n预订状态:\n");
    for (int hour = 0; hour < 24; hour++) {
        printf("  %02d:00-%02d:00 %s\n",
               hour, hour + 1,
               bitset_comp_test(slots, hour) ? "[已预订]" : "[空闲]");
    }

    printf("\n统计:\n");
    printf("  已预订: %zu 小时\n", bitset_comp_count(slots, true));
    printf("  空闲: %zu 小时\n", bitset_comp_count(slots, false));

    printf("\n查找第一个空闲时段 (从 8:00): %zu:00\n",
           bitset_comp_find_first(slots, false, 8));

    bitset_comp_free(slots);
}

// 演示 8: 批量添加运行段
static void demo_add_runs(void) {
    printf("\n=== 演示 8: 批量添加运行段 ===\n");

    bitset_compressed_t bc;
    bitset_comp_init(&bc);

    printf("添加运行段:\n");
    printf("  添加 [10, 20] 长度 11\n");
    bitset_comp_add_run(&bc, 10, 11);

    printf("  添加 [50, 60] 长度 11\n");
    bitset_comp_add_run(&bc, 50, 11);

    printf("  添加 [100, 120] 长度 21\n");
    bitset_comp_add_run(&bc, 100, 21);

    print_bits(&bc, 128);
    print_compressed_stats(&bc, "状态");

    bitset_comp_free(&bc);
}

// 演示 9: 翻转操作
static void demo_flip(void) {
    printf("\n=== 演示 9: 翻转操作 ===\n");

    bitset_compressed_t* bc = bitset_comp_create(64);
    if (!bc) return;

    // 初始状态：设置偶数位
    for (size_t i = 0; i < 64; i += 2) {
        bitset_comp_set(bc, i);
    }

    printf("初始 (偶数位为 1):\n");
    print_bits(bc, 32);

    printf("\n翻转位 0, 1, 2:\n");
    bitset_comp_flip(bc, 0);
    bitset_comp_flip(bc, 1);
    bitset_comp_flip(bc, 2);
    print_bits(bc, 32);

    printf("\n位 0: %s\n", bitset_comp_test(bc, 0) ? "1" : "0");
    printf("位 1: %s\n", bitset_comp_test(bc, 1) ? "1" : "0");
    printf("位 2: %s\n", bitset_comp_test(bc, 2) ? "1" : "0");

    bitset_comp_free(bc);
}

// 演示 10: 性能对比
static void demo_performance(void) {
    printf("\n=== 演示 10: 性能对比 ===\n");

    printf("适用场景:\n");
    printf("  压缩位集合适用于:\n");
    printf("    - 稀疏数据（大量连续的 0 或 1）\n");
    printf("    - 内存受限的环境\n");
    printf("    - 需要序列化的场景\n\n");

    printf("  普通位集合适用于:\n");
    printf("    - 随机访问频繁的场景\n");
    printf("    - 位运算密集的操作\n");
    printf("    - 数据分布均匀的情况\n\n");

    printf("内存使用对比 (10000 位):\n");
    printf("  普通位集合: %zu 字节\n", (size_t)(10000 + 7) / 8);

    bitset_compressed_t* sparse = bitset_comp_create(10000);
    bitset_comp_set(sparse, 100);
    bitset_comp_set(sparse, 5000);
    bitset_comp_set(sparse, 9999);

    printf("  稀疏压缩位集合: %zu 字节\n", bitset_comp_byte_size(sparse));
    printf("  节省: %.1f%%\n",
           100.0 * ((10000 + 7) / 8 - bitset_comp_byte_size(sparse)) / ((10000 + 7) / 8));

    bitset_comp_free(sparse);
}

int main(void) {
    printf("========================================\n");
    printf("    压缩位集合 (RLE) 演示程序\n");
    printf("========================================\n");

    demo_create_and_basic();
    demo_range_operations();
    demo_compression_comparison();
    demo_finding();
    demo_clone();
    demo_serialization();
    demo_timeslot_management();
    demo_add_runs();
    demo_flip();
    demo_performance();

    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("========================================\n");

    return 0;
}
