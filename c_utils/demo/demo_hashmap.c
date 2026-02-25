/**
 * 哈希表 (HashMap) 演示程序
 *
 * 功能：
 * - 基本增删改查操作
 * - 遍历操作
 * - 实际应用场景
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/hashmap.h"

// 演示 1: 基本操作
static void demo_basic(void) {
    printf("\n=== 演示 1: 基本操作 ===\n");

    printf("创建哈希表...\n");
    hashmap_t *map = hashmap_create();
    if (!map) {
        printf("创建失败\n");
        return;
    }

    printf("\n添加键值对:\n");
    // 存储一些数据
    int value1 = 100;
    int value2 = 200;
    int value3 = 300;

    hashmap_set(map, "apple", &value1);
    printf("  添加: apple -> %d\n", value1);

    hashmap_set(map, "banana", &value2);
    printf("  添加: banana -> %d\n", value2);

    hashmap_set(map, "cherry", &value3);
    printf("  添加: cherry -> %d\n", value3);

    printf("\n查询键值对:\n");
    int *p1 = (int*)hashmap_get(map, "apple");
    int *p2 = (int*)hashmap_get(map, "banana");
    int *p3 = (int*)hashmap_get(map, "cherry");

    if (p1) printf("  apple = %d\n", *p1);
    if (p2) printf("  banana = %d\n", *p2);
    if (p3) printf("  cherry = %d\n", *p3);

    printf("\n更新键值对:\n");
    value1 = 150;
    hashmap_set(map, "apple", &value1);
    p1 = (int*)hashmap_get(map, "apple");
    printf("  apple 更新为 %d\n", *p1);

    printf("\n删除键值对:\n");
    hashmap_remove(map, "banana");
    printf("  删除 banana\n");
    p2 = (int*)hashmap_get(map, "banana");
    printf("  banana 查询结果: %s\n", p2 ? "存在" : "不存在");

    printf("\n当前大小: %zu\n", hashmap_size(map));

    hashmap_free(map);
}

// 演示 2: 遍历操作
static void demo_iteration(void) {
    printf("\n=== 演示 2: 遍历操作 ===\n");

    hashmap_t *map = hashmap_create();

    // 添加数据
    int values[] = {10, 20, 30, 40, 50};
    const char *keys[] = {"one", "two", "three", "four", "five"};

    for (int i = 0; i < 5; i++) {
        hashmap_set(map, keys[i], &values[i]);
    }

    printf("使用迭代器遍历:\n");
    hashmap_iter_t iter = hashmap_iter_begin(map);
    int count = 0;
    while (hashmap_iter_valid(&iter)) {
        // 注意：实际实现可能需要调整以获取键值
        printf("  条目 %d\n", ++count);
        hashmap_iter_next(&iter);
    }
    printf("  总共 %d 个条目\n", count);

    printf("\n当前大小: %zu\n", hashmap_size(map));

    hashmap_free(map);
}

// 演示 3: 字符串值存储
static void demo_strings(void) {
    printf("\n=== 演示 3: 字符串值存储 ===\n");

    hashmap_t *map = hashmap_create();

    printf("存储字符串值:\n");
    hashmap_set(map, "name", "Alice");
    hashmap_set(map, "city", "Beijing");
    hashmap_set(map, "country", "China");

    printf("  name = %s\n", (char*)hashmap_get(map, "name"));
    printf("  city = %s\n", (char*)hashmap_get(map, "city"));
    printf("  country = %s\n", (char*)hashmap_get(map, "country"));

    printf("\n更新字符串值:\n");
    hashmap_set(map, "city", "Shanghai");
    printf("  city 更新为 %s\n", (char*)hashmap_get(map, "city"));

    hashmap_free(map);
}

// 演示 4: 清空操作
static void demo_clear(void) {
    printf("\n=== 演示 4: 清空操作 ===\n");

    hashmap_t *map = hashmap_create();

    int values[] = {1, 2, 3};
    hashmap_set(map, "a", &values[0]);
    hashmap_set(map, "b", &values[1]);
    hashmap_set(map, "c", &values[2]);

    printf("清空前大小: %zu\n", hashmap_size(map));

    hashmap_clear(map);
    printf("清空后大小: %zu\n", hashmap_size(map));

    hashmap_free(map);
}

// 演示 5: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 5: 应用场景 ===\n");

    printf("1. 配置管理\n");
    printf("   key=value 存储配置项\n");
    printf("   快速查找配置值\n");
    printf("\n");

    printf("2. 缓存实现\n");
    printf("   key -> 缓存数据\n");
    printf("   O(1) 时间复杂度查找\n");
    printf("\n");

    printf("3. 符号表\n");
    printf("   变量名 -> 变量信息\n");
    printf("   编译器/解释器中使用\n");
    printf("\n");

    printf("4. 数据库索引\n");
    printf("   主键 -> 记录位置\n");
    printf("   快速数据定位\n");
    printf("\n");

    printf("5. 计数器\n");
    printf("   单词 -> 出现次数\n");
    printf("   词频统计\n");
}

// 演示 6: 性能特点
static void demo_performance(void) {
    printf("\n=== 演示 6: 性能特点 ===\n");

    printf("哈希表性能:\n\n");

    printf("时间复杂度:\n");
    printf("  插入: 平均 O(1), 最坏 O(n)\n");
    printf("  查找: 平均 O(1), 最坏 O(n)\n");
    printf("  删除: 平均 O(1), 最坏 O(n)\n");
    printf("\n");

    printf("空间复杂度:\n");
    printf("  O(n) - 与元素数量成正比\n");
    printf("\n");

    printf("影响因素:\n");
    printf("  - 哈希函数质量\n");
    printf("  - 负载因子 (元素数/桶数)\n");
    printf("  - 冲突解决策略\n");
}

int main(void) {
    printf("========================================\n");
    printf("    哈希表 (HashMap) 演示\n");
    printf("========================================\n");

    demo_basic();
    demo_iteration();
    demo_strings();
    demo_clear();
    demo_applications();
    demo_performance();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
