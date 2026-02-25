/**
 * 计数布隆过滤器演示程序
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/bloom_filter_counting.h"

static void demo_basic_usage(void) {
    printf("\n=== 演示 1: 基本使用 ===\n");

    bloom_counting_t *bf = bloom_counting_create(1000, 3, 255);
    if (!bf) {
        printf("创建失败\n");
        return;
    }
    printf("创建计数布隆过滤器: 大小=1000, 哈希函数=3\n");

    const char *items[] = {"apple", "banana", "cherry", "date", "elderberry"};
    int n = sizeof(items) / sizeof(items[0]);

    printf("\n添加 %d 个元素:\n", n);
    for (int i = 0; i < n; i++) {
        bloom_counting_add(bf, items[i], strlen(items[i]));
        printf("  添加: %s\n", items[i]);
    }

    printf("\n检查元素是否存在:\n");
    for (int i = 0; i < n; i++) {
        bool exists = bloom_counting_check(bf, items[i], strlen(items[i]));
        printf("  %s: %s\n", items[i], exists ? "可能存在" : "不存在");
    }

    printf("\n检查未添加的元素:\n");
    const char *not_added[] = {"grape", "kiwi", "mango"};
    for (int i = 0; i < 3; i++) {
        bool exists = bloom_counting_check(bf, not_added[i], strlen(not_added[i]));
        printf("  %s: %s\n", not_added[i], exists ? "可能存在 (误报)" : "不存在");
    }

    bloom_counting_free(bf);
    printf("\n过滤器已释放\n");
}

static void demo_counting_feature(void) {
    printf("\n=== 演示 2: 计数特性 (支持删除) ===\n");

    bloom_counting_t *bf = bloom_counting_create(500, 3, 255);

    const char *item = "test_item";
    printf("测试项目: %s\n\n", item);

    printf("添加 3 次:\n");
    for (int i = 0; i < 3; i++) {
        bloom_counting_add(bf, item, strlen(item));
        printf("  第 %d 次添加\n", i + 1);
    }

    bool exists = bloom_counting_check(bf, item, strlen(item));
    printf("\n检查: %s\n", exists ? "存在" : "不存在");

    printf("\n删除 1 次:\n");
    bloom_counting_remove(bf, item, strlen(item));
    exists = bloom_counting_check(bf, item, strlen(item));
    printf("检查: %s\n", exists ? "存在" : "不存在");

    printf("\n再删除 2 次:\n");
    bloom_counting_remove(bf, item, strlen(item));
    bloom_counting_remove(bf, item, strlen(item));
    exists = bloom_counting_check(bf, item, strlen(item));
    printf("检查: %s (应该不存在)\n", exists ? "存在" : "不存在");

    bloom_counting_free(bf);
}

static void demo_false_positive(void) {
    printf("\n=== 演示 3: 误报率测试 ===\n");

    size_t size = 10000;
    int num_hashes = 5;
    bloom_counting_t *bf = bloom_counting_create(size, num_hashes, 255);

    printf("过滤器大小: %zu, 哈希函数: %d\n", size, num_hashes);

    int num_items = 1000;
    printf("添加 %d 个元素...\n", num_items);

    for (int i = 0; i < num_items; i++) {
        char key[32];
        sprintf(key, "item_%d", i);
        bloom_counting_add(bf, key, strlen(key));
    }

    int false_positives = 0;
    int test_count = 1000;
    printf("测试 %d 个未添加的元素...\n", test_count);

    for (int i = num_items; i < num_items + test_count; i++) {
        char key[32];
        sprintf(key, "item_%d", i);
        if (bloom_counting_check(bf, key, strlen(key))) {
            false_positives++;
        }
    }

    printf("误报数: %d\n", false_positives);
    printf("误报率: %.2f%%\n", (false_positives * 100.0) / test_count);

    bloom_counting_free(bf);
}

int main(void) {
    printf("========================================\n");
    printf("    计数布隆过滤器演示\n");
    printf("========================================\n");

    demo_basic_usage();
    demo_counting_feature();
    demo_false_positive();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
