/**
 * @file demo_bitset.c
 * @brief 位集合演示
 *
 * 本演示展示了位集合的各种操作，包括：
 * - 创建和销毁
 * - 基本位操作（设置、清除、测试、翻转）
 * - 批量操作
 * - 位运算
 * - 计数和查找
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/bitset.h"

// 打印位集合
static void print_bitset(const bitset_t* bs, const char* label) {
    size_t size = bitset_size(bs);
    printf("%s (%zu 位): ", label, size);

    for (size_t i = 0; i < size && i < 64; i++) {
        printf("%d", bitset_test(bs, i) ? 1 : 0);
        if ((i + 1) % 8 == 0 && i < size - 1) printf(" ");
    }

    if (size > 64) printf(" ...");
    printf("\n");
}

// 演示 1: 创建和基本操作
static void demo_create_and_basic(void) {
    printf("\n=== 演示 1: 创建和基本操作 ===\n");

    bitset_t* bs = bitset_create(32);
    if (!bs) {
        fprintf(stderr, "创建位集合失败\n");
        return;
    }

    printf("创建 32 位位集合\n");
    print_bitset(bs, "初始状态");

    // 设置一些位
    printf("\n设置位 0, 5, 10, 15, 20, 25, 30\n");
    bitset_set(bs, 0);
    bitset_set(bs, 5);
    bitset_set(bs, 10);
    bitset_set(bs, 15);
    bitset_set(bs, 20);
    bitset_set(bs, 25);
    bitset_set(bs, 30);
    print_bitset(bs, "设置后");

    // 测试位
    printf("\n位测试:\n");
    for (size_t i = 0; i <= 30; i += 5) {
        printf("  位 %zu: %s\n", i, bitset_test(bs, i) ? "已设置" : "未设置");
    }

    // 清除位
    printf("\n清除位 10 和 20\n");
    bitset_clear(bs, 10);
    bitset_clear(bs, 20);
    print_bitset(bs, "清除后");

    // 翻转位
    printf("\n翻转位 5 和 15\n");
    bitset_flip(bs, 5);
    bitset_flip(bs, 15);
    print_bitset(bs, "翻转后");

    bitset_free(bs);
}

// 演示 2: 批量操作
static void demo_batch_operations(void) {
    printf("\n=== 演示 2: 批量操作 ===\n");

    bitset_t* bs = bitset_create(32);
    if (!bs) return;

    printf("设置所有位\n");
    bitset_set_all(bs);
    print_bitset(bs, "结果");

    printf("\n清除所有位\n");
    bitset_clear_all(bs);
    print_bitset(bs, "结果");

    printf("\n设置范围 [8, 23]\n");
    bitset_set_range(bs, 8, 23);
    print_bitset(bs, "结果");

    printf("\n清除范围 [12, 19]\n");
    bitset_clear_range(bs, 12, 19);
    print_bitset(bs, "结果");

    printf("\n翻转所有位\n");
    bitset_flip_all(bs);
    print_bitset(bs, "结果");

    bitset_free(bs);
}

// 演示 3: 位运算
static void demo_bitwise_operations(void) {
    printf("\n=== 演示 3: 位运算 ===\n");

    bitset_t* a = bitset_create(16);
    bitset_t* b = bitset_create(16);

    if (!a || !b) {
        bitset_free(a);
        bitset_free(b);
        return;
    }

    // 设置 A: 位 0, 2, 4, 6, 8
    bitset_set(a, 0);
    bitset_set(a, 2);
    bitset_set(a, 4);
    bitset_set(a, 6);
    bitset_set(a, 8);

    // 设置 B: 位 4, 5, 6, 7, 8
    bitset_set(b, 4);
    bitset_set(b, 5);
    bitset_set(b, 6);
    bitset_set(b, 7);
    bitset_set(b, 8);

    print_bitset(a, "集合 A");
    print_bitset(b, "集合 B");

    // AND
    bitset_t* and_result = bitset_and(a, b);
    if (and_result) {
        print_bitset(and_result, "A AND B");
        bitset_free(and_result);
    }

    // OR
    bitset_t* or_result = bitset_or(a, b);
    if (or_result) {
        print_bitset(or_result, "A OR B");
        bitset_free(or_result);
    }

    // XOR
    bitset_t* xor_result = bitset_xor(a, b);
    if (xor_result) {
        print_bitset(xor_result, "A XOR B");
        bitset_free(xor_result);
    }

    // NOT A
    bitset_t* not_a = bitset_not(a);
    if (not_a) {
        print_bitset(not_a, "NOT A");
        bitset_free(not_a);
    }

    bitset_free(a);
    bitset_free(b);
}

// 演示 4: 计数和查找
static void demo_counting_and_finding(void) {
    printf("\n=== 演示 4: 计数和查找 ===\n");

    bitset_t* bs = bitset_create(64);
    if (!bs) return;

    // 设置一些位
    bitset_set(bs, 5);
    bitset_set(bs, 10);
    bitset_set(bs, 15);
    bitset_set(bs, 20);
    bitset_set(bs, 25);
    bitset_set(bs, 30);
    bitset_set(bs, 35);
    bitset_set(bs, 40);

    print_bitset(bs, "位集合");

    printf("\n统计信息:\n");
    printf("  总位数: %zu\n", bitset_size(bs));
    printf("  已设置位数: %zu\n", bitset_count(bs));
    printf("  未设置位数: %zu\n", bitset_size(bs) - bitset_count(bs));
    printf("  字节大小: %zu\n", bitset_byte_size(bs));

    printf("\n范围 [10, 30] 统计:\n");
    printf("  已设置位数: %zu\n", bitset_count_range(bs, 10, 30));

    printf("\n查找操作:\n");
    printf("  第一个已设置位: %zu\n", bitset_find_first_set(bs, 0));
    printf("  从位 15 开始第一个已设置位: %zu\n", bitset_find_first_set(bs, 15));
    printf("  第一个未设置位: %zu\n", bitset_find_first_clear(bs, 0));
    printf("  最后一个已设置位: %zu\n", bitset_find_last_set(bs));

    bitset_free(bs);
}

// 演示 5: 状态查询
static void demo_status_queries(void) {
    printf("\n=== 演示 5: 状态查询 ===\n");

    bitset_t* bs1 = bitset_create(16);
    bitset_t* bs2 = bitset_create(16);
    bitset_t* bs3 = bitset_create(16);

    if (!bs1 || !bs2 || !bs3) {
        bitset_free(bs1);
        bitset_free(bs2);
        bitset_free(bs3);
        return;
    }

    // bs1: 全空
    // bs2: 全满
    bitset_set_all(bs2);

    // bs3: 部分设置
    bitset_set(bs3, 5);
    bitset_set(bs3, 10);

    print_bitset(bs1, "集合 1 (空)");
    print_bitset(bs2, "集合 2 (满)");
    print_bitset(bs3, "集合 3 (部分)");

    printf("\n状态检查:\n");
    printf("  集合 1 是否为空: %s\n", bitset_is_empty(bs1) ? "是" : "否");
    printf("  集合 1 是否全满: %s\n", bitset_is_all_set(bs1) ? "是" : "否");
    printf("  集合 2 是否为空: %s\n", bitset_is_empty(bs2) ? "是" : "否");
    printf("  集合 2 是否全满: %s\n", bitset_is_all_set(bs2) ? "是" : "否");

    // 相等性测试
    bitset_t* bs4 = bitset_clone(bs3);
    if (bs4) {
        printf("\n相等性测试:\n");
        printf("  集合 3 == 集合 4 (克隆): %s\n",
               bitset_equals(bs3, bs4) ? "是" : "否");

        bitset_set(bs4, 7);
        printf("  修改集合 4 后 == 集合 3: %s\n",
               bitset_equals(bs3, bs4) ? "是" : "否");

        bitset_free(bs4);
    }

    bitset_free(bs1);
    bitset_free(bs2);
    bitset_free(bs3);
}

// 演示 6: 从字节创建
static void demo_from_bytes(void) {
    printf("\n=== 演示 6: 从字节创建 ===\n");

    uint8_t bytes[] = {0xAA, 0x55, 0xF0, 0x0F};
    size_t num_bytes = sizeof(bytes);
    size_t num_bits = num_bytes * 8;

    printf("源字节: ");
    for (size_t i = 0; i < num_bytes; i++) {
        printf("0x%02X ", bytes[i]);
    }
    printf("\n");

    bitset_t* bs = bitset_create_from_bytes(bytes, num_bytes, num_bits);
    if (bs) {
        print_bitset(bs, "创建的位集合");

        printf("\n验证:\n");
        printf("  字节 0 (0xAA): ");
        for (int i = 7; i >= 0; i--) {
            printf("%d", bitset_test(bs, i) ? 1 : 0);
        }
        printf("\n");

        printf("  字节 1 (0x55): ");
        for (int i = 15; i >= 8; i--) {
            printf("%d", bitset_test(bs, i) ? 1 : 0);
        }
        printf("\n");

        bitset_free(bs);
    }
}

// 演示 7: 克隆
static void demo_clone(void) {
    printf("\n=== 演示 7: 克隆 ===\n");

    bitset_t* original = bitset_create(32);
    if (!original) return;

    // 设置一些位
    bitset_set(original, 0);
    bitset_set(original, 8);
    bitset_set(original, 16);
    bitset_set(original, 24);

    print_bitset(original, "原始集合");

    bitset_t* clone = bitset_clone(original);
    if (clone) {
        print_bitset(clone, "克隆集合");

        // 修改克隆
        bitset_set(clone, 4);
        bitset_clear(clone, 8);

        printf("\n修改克隆后:\n");
        print_bitset(original, "原始集合 (未变)");
        print_bitset(clone, "克隆集合 (已变)");

        bitset_free(clone);
    }

    bitset_free(original);
}

// 演示 8: 实际应用 - 权限系统
static void demo_permissions(void) {
    printf("\n=== 演示 8: 实际应用 - 权限系统 ===\n");

    // 定义权限位
    enum {
        PERM_READ = 0,
        PERM_WRITE = 1,
        PERM_EXECUTE = 2,
        PERM_DELETE = 3,
        PERM_ADMIN = 4,
        PERM_CREATE = 5
    };

    bitset_t* user_perms = bitset_create(8);
    if (!user_perms) return;

    // 设置用户权限
    bitset_set(user_perms, PERM_READ);
    bitset_set(user_perms, PERM_WRITE);
    bitset_set(user_perms, PERM_CREATE);

    printf("用户权限:\n");
    printf("  读:   %s\n", bitset_test(user_perms, PERM_READ) ? "允许" : "拒绝");
    printf("  写:   %s\n", bitset_test(user_perms, PERM_WRITE) ? "允许" : "拒绝");
    printf("  执行: %s\n", bitset_test(user_perms, PERM_EXECUTE) ? "允许" : "拒绝");
    printf("  删除: %s\n", bitset_test(user_perms, PERM_DELETE) ? "允许" : "拒绝");
    printf("  管理: %s\n", bitset_test(user_perms, PERM_ADMIN) ? "允许" : "拒绝");
    printf("  创建: %s\n", bitset_test(user_perms, PERM_CREATE) ? "允许" : "拒绝");

    // 检查权限
    printf("\n权限检查:\n");
    printf("  可以读写: %s\n",
           (bitset_test(user_perms, PERM_READ) &&
            bitset_test(user_perms, PERM_WRITE)) ? "是" : "否");
    printf("  可以执行: %s\n",
           bitset_test(user_perms, PERM_EXECUTE) ? "是" : "否");

    bitset_free(user_perms);
}

// 演示 9: 实际应用 - 质数筛
static void demo_sieve_of_eratosthenes(void) {
    printf("\n=== 演示 9: 实际应用 - 质数筛 ===\n");

    const int n = 100;
    bitset_t* is_prime = bitset_create(n + 1);
    if (!is_prime) return;

    // 初始化：假设所有数都是质数
    bitset_set_all(is_prime);
    bitset_clear(is_prime, 0);
    bitset_clear(is_prime, 1);

    // 筛法
    for (int p = 2; p * p <= n; p++) {
        if (bitset_test(is_prime, p)) {
            for (int i = p * p; i <= n; i += p) {
                bitset_clear(is_prime, i);
            }
        }
    }

    printf("100 以内的质数:\n  ");
    int count = 0;
    for (int i = 2; i <= n; i++) {
        if (bitset_test(is_prime, i)) {
            printf("%d ", i);
            count++;
            if (count % 10 == 0) printf("\n  ");
        }
    }
    printf("\n\n共 %d 个质数\n", count);

    bitset_free(is_prime);
}

// 演示 10: 性能对比
static void demo_performance(void) {
    printf("\n=== 演示 10: 内存效率对比 ===\n");

    printf("位集合 vs 布尔数组内存占用:\n");
    printf("  位数    | 位集合(字节) | 布尔数组(字节)\n");
    printf("  --------|--------------|---------------\n");

    size_t sizes[] = {64, 128, 256, 512, 1024, 4096, 16384};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    for (int i = 0; i < num_sizes; i++) {
        size_t bits = sizes[i];
        size_t bitset_bytes = (bits + 7) / 8;
        size_t bool_bytes = bits * sizeof(_Bool);

        printf("  %7zu | %12zu | %13zu\n", bits, bitset_bytes, bool_bytes);
    }

    printf("\n说明: 位集合比布尔数组节省约 87.5%% 内存\n");
}

int main(void) {
    printf("========================================\n");
    printf("    位集合演示程序\n");
    printf("========================================\n");

    demo_create_and_basic();
    demo_batch_operations();
    demo_bitwise_operations();
    demo_counting_and_finding();
    demo_status_queries();
    demo_from_bytes();
    demo_clone();
    demo_permissions();
    demo_sieve_of_eratosthenes();
    demo_performance();

    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("========================================\n");

    return 0;
}
