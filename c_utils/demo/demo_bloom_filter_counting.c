/**
 * @file demo_bloom_filter_counting.c
 * @brief 计数布隆过滤器演示
 *
 * 本演示展示了计数布隆过滤器的各种用法，包括：
 * - 创建和销毁
 * - 添加、删除和检查元素
 * - 计数功能
 * - 与标准布隆过滤器的对比
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/bloom_filter_counting.h"

// 演示 1: 基本操作
static void demo_basic_operations(void) {
    printf("\n=== 演示 1: 基本操作 ===\n");

    // 创建计数布隆过滤器
    bloom_counting_t* bf = bloom_counting_create(10000, 4, 15);
    if (!bf) {
        fprintf(stderr, "创建失败\n");
        return;
    }

    printf("创建计数布隆过滤器:\n");
    printf("  大小: 10000 位\n");
    printf("  哈希函数数: 4\n");
    printf("  最大计数值: 15\n");

    // 添加元素
    const char* items[] = {"apple", "banana", "cherry"};
    int num_items = sizeof(items) / sizeof(items[0]);

    printf("\n添加元素:\n");
    for (int i = 0; i < num_items; i++) {
        if (bloom_counting_add(bf, items[i], strlen(items[i]))) {
            printf("  + \"%s\"\n", items[i]);
        }
    }

    // 检查元素
    printf("\n检查元素:\n");
    for (int i = 0; i < num_items; i++) {
        bool exists = bloom_counting_check(bf, items[i], strlen(items[i]));
        printf("  \"%s\": %s\n", items[i], exists ? "可能存在" : "肯定不存在");
    }

    bloom_counting_free(bf);
}

// 演示 2: 删除操作
static void demo_deletion(void) {
    printf("\n=== 演示 2: 删除操作 ===\n");

    bloom_counting_t* bf = bloom_counting_create(1000, 4, 15);
    if (!bf) return;

    const char* item = "test_item";

    printf("测试删除操作:\n\n");

    // 添加元素
    printf("1. 添加 \"%s\"\n", item);
    bloom_counting_add(bf, item, strlen(item));
    printf("   存在: %s\n",
           bloom_counting_check(bf, item, strlen(item)) ? "是" : "否");

    // 再次添加（计数增加）
    printf("\n2. 再次添加 \"%s\"\n", item);
    bloom_counting_add(bf, item, strlen(item));
    printf("   存在: %s\n",
           bloom_counting_check(bf, item, strlen(item)) ? "是" : "否");

    // 删除一次
    printf("\n3. 删除一次 \"%s\"\n", item);
    bloom_counting_remove(bf, item, strlen(item));
    printf("   存在: %s\n",
           bloom_counting_check(bf, item, strlen(item)) ? "是" : "否");

    // 再次删除
    printf("\n4. 再次删除 \"%s\"\n", item);
    bloom_counting_remove(bf, item, strlen(item));
    printf("   存在: %s\n",
           bloom_counting_check(bf, item, strlen(item)) ? "是" : "否");

    bloom_counting_free(bf);
}

// 演示 3: 计数功能
static void demo_counting(void) {
    printf("\n=== 演示 3: 计数功能 ===\n");

    bloom_counting_t* bf = bloom_counting_create(1000, 4, 15);
    if (!bf) return;

    const char* item = "countable_item";

    printf("测试计数功能:\n\n");

    for (int i = 0; i <= 5; i++) {
        uint8_t count = bloom_counting_estimate(bf, item, strlen(item));
        printf("添加 %d 次后，估算计数: %u\n", i, count);

        if (i < 5) {
            bloom_counting_add(bf, item, strlen(item));
        }
    }

    printf("\n说明: 计数是估算值，可能高于实际值\n");

    bloom_counting_free(bf);
}

// 演示 4: 多元素管理
static void demo_multi_item_management(void) {
    printf("\n=== 演示 4: 多元素管理 ===\n");

    bloom_counting_t* bf = bloom_counting_create(5000, 4, 15);
    if (!bf) return;

    // 添加多个元素，每个多次
    const char* fruits[] = {"apple", "banana", "cherry", "date"};
    int counts[] = {3, 5, 2, 4};
    int num_fruits = sizeof(fruits) / sizeof(fruits[0]);

    printf("添加元素:\n");
    for (int i = 0; i < num_fruits; i++) {
        for (int j = 0; j < counts[i]; j++) {
            bloom_counting_add(bf, fruits[i], strlen(fruits[i]));
        }
        printf("  \"%s\" 添加 %d 次\n", fruits[i], counts[i]);
    }

    printf("\n计数查询:\n");
    for (int i = 0; i < num_fruits; i++) {
        uint8_t count = bloom_counting_estimate(bf, fruits[i], strlen(fruits[i]));
        printf("  \"%s\": 估算计数 = %u (实际 %d)\n", fruits[i], count, counts[i]);
    }

    // 删除一些
    printf("\n删除操作:\n");
    printf("  删除 \"banana\" 2 次\n");
    bloom_counting_remove(bf, "banana", 6);
    bloom_counting_remove(bf, "banana", 6);

    printf("  删除后计数:\n");
    for (int i = 0; i < num_fruits; i++) {
        uint8_t count = bloom_counting_estimate(bf, fruits[i], strlen(fruits[i]));
        printf("    \"%s\": %u\n", fruits[i], count);
    }

    bloom_counting_free(bf);
}

// 演示 5: 与标准布隆过滤器对比
static void demo_comparison_with_standard(void) {
    printf("\n=== 演示 5: 与标准布隆过滤器对比 ===\n");

    printf("特性对比:\n\n");

    printf("标准布隆过滤器:\n");
    printf("  + 内存效率高\n");
    printf("  + 实现简单\n");
    printf("  - 不支持删除\n");
    printf("  - 无计数功能\n\n");

    printf("计数布隆过滤器:\n");
    printf("  + 支持删除操作\n");
    printf("  + 支持计数估算\n");
    printf("  - 内存开销更大 (每个计数器需要多位)\n");
    printf("  - 计数器可能溢出\n");

    printf("\n适用场景:\n");
    printf("  标准版: 只增不删的场景 (如缓存、URL 去重)\n");
    printf("  计数版: 需要删除或计数的场景 (如流量统计)\n");
}

// 演示 6: 实际应用 - 流量统计
static void demo_traffic_statistics(void) {
    printf("\n=== 演示 6: 实际应用 - 流量统计 ===\n");

    bloom_counting_t* bf = bloom_counting_create(10000, 4, 15);
    if (!bf) return;

    // 模拟用户访问
    const char* users[] = {
        "user_001", "user_002", "user_003", "user_004", "user_005"
    };
    int num_users = sizeof(users) / sizeof(users[0]);

    // 模拟访问记录
    printf("模拟用户访问:\n");
    const char* accesses[] = {
        "user_001", "user_002", "user_001", "user_003",
        "user_001", "user_002", "user_004", "user_001",
        "user_005", "user_002", "user_003", "user_001"
    };
    int num_accesses = sizeof(accesses) / sizeof(accesses[0]);

    for (int i = 0; i < num_accesses; i++) {
        bloom_counting_add(bf, accesses[i], strlen(accesses[i]));
    }

    // 显示每个用户的访问次数
    printf("\n用户访问统计:\n");
    for (int i = 0; i < num_users; i++) {
        uint8_t count = bloom_counting_estimate(bf, users[i], strlen(users[i]));
        printf("  %s: %u 次访问\n", users[i], count);
    }

    bloom_counting_free(bf);
}

// 演示 7: 实际应用 - 商品库存管理
static void demo_inventory_management(void) {
    printf("\n=== 演示 7: 实际应用 - 商品库存管理 ===\n");

    bloom_counting_t* bf = bloom_counting_create(5000, 4, 15);
    if (!bf) return;

    // 商品入库
    printf("商品入库:\n");
    struct {
        const char* name;
        int quantity;
    } inventory[] = {
        {"SKU001", 100},
        {"SKU002", 50},
        {"SKU003", 200},
        {"SKU004", 75}
    };
    int num_items = sizeof(inventory) / sizeof(inventory[0]);

    for (int i = 0; i < num_items; i++) {
        for (int j = 0; j < inventory[i].quantity; j++) {
            bloom_counting_add(bf, inventory[i].name, strlen(inventory[i].name));
        }
        printf("  %s 入库 %d 件\n", inventory[i].name, inventory[i].quantity);
    }

    // 查询库存
    printf("\n当前库存:\n");
    for (int i = 0; i < num_items; i++) {
        uint8_t count = bloom_counting_estimate(bf, inventory[i].name, strlen(inventory[i].name));
        printf("  %s: 约 %u 件\n", inventory[i].name, count);
    }

    // 商品出库
    printf("\n商品出库:\n");
    printf("  SKU001 出库 30 件\n");
    for (int i = 0; i < 30; i++) {
        bloom_counting_remove(bf, "SKU001", 6);
    }

    printf("  SKU002 出库 20 件\n");
    for (int i = 0; i < 20; i++) {
        bloom_counting_remove(bf, "SKU002", 6);
    }

    // 查询更新后的库存
    printf("\n更新后的库存:\n");
    for (int i = 0; i < num_items; i++) {
        uint8_t count = bloom_counting_estimate(bf, inventory[i].name, strlen(inventory[i].name));
        printf("  %s: 约 %u 件\n", inventory[i].name, count);
    }

    bloom_counting_free(bf);
}

// 演示 8: 计数器溢出测试
static void demo_counter_overflow(void) {
    printf("\n=== 演示 8: 计数器溢出测试 ===\n");

    // 使用小计数器 (最大值 3)
    bloom_counting_t* bf = bloom_counting_create(100, 4, 3);
    if (!bf) return;

    const char* item = "overflow_test";

    printf("使用最大计数值为 3 的计数器:\n\n");

    for (int i = 0; i <= 6; i++) {
        uint8_t count = bloom_counting_estimate(bf, item, strlen(item));
        printf("添加 %d 次后，估算计数: %u\n", i, count);

        if (i < 6) {
            bloom_counting_add(bf, item, strlen(item));
        }
    }

    printf("\n说明: 计数器溢出后计数不再准确\n");

    bloom_counting_free(bf);
}

// 演示 9: 重置功能
static void demo_reset(void) {
    printf("\n=== 演示 9: 重置功能 ===\n");

    bloom_counting_t* bf = bloom_counting_create(1000, 4, 15);
    if (!bf) return;

    // 添加元素
    bloom_counting_add(bf, "item1", 5);
    bloom_counting_add(bf, "item2", 5);
    bloom_counting_add(bf, "item3", 5);

    printf("添加元素后:\n");
    printf("  item1: %s\n",
           bloom_counting_check(bf, "item1", 5) ? "存在" : "不存在");
    printf("  item2: %s\n",
           bloom_counting_check(bf, "item2", 5) ? "存在" : "不存在");

    // 重置
    printf("\n重置过滤器...\n");
    bloom_counting_reset(bf);

    printf("\n重置后:\n");
    printf("  item1: %s\n",
           bloom_counting_check(bf, "item1", 5) ? "存在" : "不存在");
    printf("  item2: %s\n",
           bloom_counting_check(bf, "item2", 5) ? "存在" : "不存在");

    bloom_counting_free(bf);
}

// 演示 10: 性能与内存分析
static void demo_performance_analysis(void) {
    printf("\n=== 演示 10: 性能与内存分析 ===\n");

    printf("计数器位数与内存使用:\n\n");

    struct {
        uint8_t max_count;
        int bits_needed;
    } configs[] = {
        {1, 1},    // 标准布隆过滤器
        {3, 2},
        {7, 3},
        {15, 4},
        {31, 5},
        {255, 8}
    };
    int num_configs = sizeof(configs) / sizeof(configs[0]);

    size_t base_size = 10000;

    printf("%-12s %-15s %-20s %-20s\n",
           "最大计数值", "位数/计数器", "内存使用(字节)", "相对标准版");
    printf("%-12s %-15s %-20s %-20s\n",
           "----------", "-----------", "--------------", "----------");

    for (int i = 0; i < num_configs; i++) {
        size_t bytes = (base_size * configs[i].bits_needed + 7) / 8;
        double relative = configs[i].bits_needed;

        printf("%-12d %-15d %-20zu %-20.0fx\n",
               configs[i].max_count, configs[i].bits_needed, bytes, relative);
    }

    printf("\n建议:\n");
    printf("  - 如果只需要删除功能，使用 2-4 位计数器\n");
    printf("  - 如果需要精确计数，考虑使用其他数据结构\n");
    printf("  - 标准布隆过滤器 (1位) 内存效率最高\n");
}

int main(void) {
    printf("========================================\n");
    printf("    计数布隆过滤器演示程序\n");
    printf("========================================\n");

    demo_basic_operations();
    demo_deletion();
    demo_counting();
    demo_multi_item_management();
    demo_comparison_with_standard();
    demo_traffic_statistics();
    demo_inventory_management();
    demo_counter_overflow();
    demo_reset();
    demo_performance_analysis();

    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("========================================\n");

    return 0;
}
