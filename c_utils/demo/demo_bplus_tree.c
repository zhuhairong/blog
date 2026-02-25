/**
 * @file demo_bplus_tree.c
 * @brief B+ 树演示
 *
 * 本演示展示了 B+ 树的各种操作，包括：
 * - 插入和查找
 * - 迭代遍历
 * - 范围查询
 * - 性能测试
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../c_utils/bplus_tree.h"

// 整数比较函数
static int int_compare(const void* a, const void* b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return (ia > ib) - (ia < ib);
}

// 演示 1: 基本操作
static void demo_basic_operations(void) {
    printf("\n=== 演示 1: 基本操作 ===\n");

    bplus_tree_t* tree = bplus_tree_create(int_compare);
    if (!tree) {
        printf("创建 B+ 树失败\n");
        return;
    }

    printf("插入键值对:\n");

    // 插入数据
    for (int i = 1; i <= 10; i++) {
        int* key = malloc(sizeof(int));
        int* value = malloc(sizeof(int));
        *key = i * 10;
        *value = i * 100;

        if (bplus_tree_insert(tree, key, value)) {
            printf("  插入: key=%d, value=%d\n", *key, *value);
        }
    }

    printf("\n查找键值对:\n");

    // 查找数据
    for (int i = 1; i <= 15; i++) {
        int search_key = i * 10;
        int* value = (int*)bplus_tree_get(tree, &search_key);
        if (value) {
            printf("  找到 key=%d: value=%d\n", search_key, *value);
        } else {
            printf("  未找到 key=%d\n", search_key);
        }
    }

    printf("\n树状态:\n");
    printf("  大小: %zu\n", bplus_tree_size(tree));
    printf("  是否为空: %s\n", bplus_tree_is_empty(tree) ? "是" : "否");

    // 清理
    bplus_iterator_t* iter = bplus_iterator_create(tree);
    const void* key;
    void* value;
    while (bplus_iterator_next(iter, &key, &value)) {
        free((void*)key);
        free(value);
    }
    bplus_iterator_free(iter);
    bplus_tree_free(tree);
}

// 演示 2: 迭代遍历
static void demo_iterator(void) {
    printf("\n=== 演示 2: 迭代遍历 ===\n");

    bplus_tree_t* tree = bplus_tree_create(int_compare);
    if (!tree) return;

    // 插入数据
    int keys[] = {50, 30, 70, 20, 40, 60, 80, 10, 25, 35, 45, 55, 65, 75, 85};
    int num_keys = sizeof(keys) / sizeof(keys[0]);

    for (int i = 0; i < num_keys; i++) {
        int* key = malloc(sizeof(int));
        int* value = malloc(sizeof(int));
        *key = keys[i];
        *value = keys[i] * 10;
        bplus_tree_insert(tree, key, value);
    }

    printf("正向遍历:\n  ");
    bplus_iterator_t* iter = bplus_iterator_create(tree);
    const void* key;
    void* value;
    int count = 0;
    while (bplus_iterator_next(iter, &key, &value)) {
        printf("%d ", *(int*)key);
        if (++count % 10 == 0) printf("\n  ");
    }
    printf("\n");
    bplus_iterator_free(iter);

    // 重置迭代器
    printf("\n重置迭代器后再次遍历:\n  ");
    iter = bplus_iterator_create(tree);
    count = 0;
    while (bplus_iterator_next(iter, &key, &value)) {
        printf("%d ", *(int*)key);
        if (++count % 10 == 0) printf("\n  ");
    }
    printf("\n");
    bplus_iterator_free(iter);

    // 清理
    iter = bplus_iterator_create(tree);
    while (bplus_iterator_next(iter, &key, &value)) {
        free((void*)key);
        free(value);
    }
    bplus_iterator_free(iter);
    bplus_tree_free(tree);
}

// 演示 3: 范围查询回调
static bool range_callback(const void* key, void* value, void* user_data) {
    (void)value;
    int* count = (int*)user_data;
    printf("  在范围内: key=%d\n", *(int*)key);
    (*count)++;
    return true;  // 继续遍历
}

// 演示 3: 范围查询
static void demo_range_query(void) {
    printf("\n=== 演示 3: 范围查询 ===\n");

    bplus_tree_t* tree = bplus_tree_create(int_compare);
    if (!tree) return;

    // 插入有序数据
    for (int i = 1; i <= 20; i++) {
        int* key = malloc(sizeof(int));
        int* value = malloc(sizeof(int));
        *key = i * 5;
        *value = i * 50;
        bplus_tree_insert(tree, key, value);
    }

    printf("数据范围: 5 - 100\n\n");

    // 范围查询 [30, 70]
    int start = 30, end = 70;
    printf("查询范围 [%d, %d]:\n", start, end);
    int count = 0;
    size_t found = bplus_tree_range_query(tree, &start, &end, range_callback, &count);
    printf("共找到 %zu 个键\n", found);

    // 范围查询 [80, 120]
    start = 80; end = 120;
    printf("\n查询范围 [%d, %d]:\n", start, end);
    count = 0;
    found = bplus_tree_range_query(tree, &start, &end, range_callback, &count);
    printf("共找到 %zu 个键\n", found);

    // 清理
    bplus_iterator_t* iter = bplus_iterator_create(tree);
    const void* key;
    void* value;
    while (bplus_iterator_next(iter, &key, &value)) {
        free((void*)key);
        free(value);
    }
    bplus_iterator_free(iter);
    bplus_tree_free(tree);
}

// 演示 4: 边界值测试
static void demo_edge_cases(void) {
    printf("\n=== 演示 4: 边界值测试 ===\n");

    bplus_tree_t* tree = bplus_tree_create(int_compare);
    if (!tree) return;

    printf("空树测试:\n");
    printf("  是否为空: %s\n", bplus_tree_is_empty(tree) ? "是" : "否");
    printf("  大小: %zu\n", bplus_tree_size(tree));

    int search_key = 100;
    void* value = bplus_tree_get(tree, &search_key);
    printf("  查找不存在的键: %s\n", value ? "找到" : "未找到");

    printf("\n单元素树:\n");
    int* key = malloc(sizeof(int));
    int* val = malloc(sizeof(int));
    *key = 42;
    *val = 420;
    bplus_tree_insert(tree, key, val);

    printf("  是否为空: %s\n", bplus_tree_is_empty(tree) ? "是" : "否");
    printf("  大小: %zu\n", bplus_tree_size(tree));

    search_key = 42;
    value = bplus_tree_get(tree, &search_key);
    if (value) {
        printf("  查找 key=42: value=%d\n", *(int*)value);
    }

    // 清理
    bplus_iterator_t* iter = bplus_iterator_create(tree);
    const void* k;
    void* v;
    while (bplus_iterator_next(iter, &k, &v)) {
        free((void*)k);
        free(v);
    }
    bplus_iterator_free(iter);
    bplus_tree_free(tree);
}

// 演示 5: 性能测试
static void demo_performance(void) {
    printf("\n=== 演示 5: 性能测试 ===\n");

    bplus_tree_t* tree = bplus_tree_create(int_compare);
    if (!tree) return;

    const int n = 10000;

    printf("插入 %d 个随机键...\n", n);

    clock_t start = clock();

    srand(42);  // 固定种子以便复现
    for (int i = 0; i < n; i++) {
        int* key = malloc(sizeof(int));
        int* value = malloc(sizeof(int));
        *key = rand() % (n * 10);
        *value = i;
        bplus_tree_insert(tree, key, value);
    }

    clock_t end = clock();
    double insert_time = (double)(end - start) * 1000 / CLOCKS_PER_SEC;

    printf("  插入时间: %.2f ms\n", insert_time);
    printf("  平均每次: %.4f ms\n", insert_time / n);
    printf("  树大小: %zu\n", bplus_tree_size(tree));

    printf("\n查找 %d 个键...\n", n);

    start = clock();

    int found = 0;
    for (int i = 0; i < n; i++) {
        int key = rand() % (n * 10);
        if (bplus_tree_get(tree, &key)) {
            found++;
        }
    }

    end = clock();
    double search_time = (double)(end - start) * 1000 / CLOCKS_PER_SEC;

    printf("  查找时间: %.2f ms\n", search_time);
    printf("  平均每次: %.4f ms\n", search_time / n);
    printf("  找到: %d/%d\n", found, n);

    // 清理
    bplus_iterator_t* iter = bplus_iterator_create(tree);
    const void* key;
    void* value;
    while (bplus_iterator_next(iter, &key, &value)) {
        free((void*)key);
        free(value);
    }
    bplus_iterator_free(iter);
    bplus_tree_free(tree);
}

// 演示 6: 字符串键
static int str_compare(const void* a, const void* b) {
    return strcmp((const char*)a, (const char*)b);
}

static void demo_string_keys(void) {
    printf("\n=== 演示 6: 字符串键 ===\n");

    bplus_tree_t* tree = bplus_tree_create(str_compare);
    if (!tree) return;

    printf("插入字符串键值对:\n");

    const char* names[] = {"Alice", "Bob", "Charlie", "David", "Eve"};
    int scores[] = {85, 92, 78, 88, 95};

    for (int i = 0; i < 5; i++) {
        char* key = strdup(names[i]);
        int* value = malloc(sizeof(int));
        *value = scores[i];
        bplus_tree_insert(tree, key, value);
        printf("  插入: %s -> %d\n", names[i], scores[i]);
    }

    printf("\n查找:\n");

    const char* search_names[] = {"Bob", "Frank", "Alice", "Grace"};
    for (int i = 0; i < 4; i++) {
        int* score = (int*)bplus_tree_get(tree, search_names[i]);
        if (score) {
            printf("  %s: %d分\n", search_names[i], *score);
        } else {
            printf("  %s: 未找到\n", search_names[i]);
        }
    }

    printf("\n遍历所有键值对:\n");
    bplus_iterator_t* iter = bplus_iterator_create(tree);
    const void* key;
    void* value;
    while (bplus_iterator_next(iter, &key, &value)) {
        printf("  %s: %d分\n", (char*)key, *(int*)value);
    }
    bplus_iterator_free(iter);

    // 清理
    iter = bplus_iterator_create(tree);
    while (bplus_iterator_next(iter, &key, &value)) {
        free((void*)key);
        free(value);
    }
    bplus_iterator_free(iter);
    bplus_tree_free(tree);
}

// 演示 7: 最小/最大值
static void demo_min_max(void) {
    printf("\n=== 演示 7: 最小/最大值 ===\n");

    bplus_tree_t* tree = bplus_tree_create(int_compare);
    if (!tree) return;

    // 插入随机数据
    srand(123);
    for (int i = 0; i < 20; i++) {
        int* key = malloc(sizeof(int));
        int* value = malloc(sizeof(int));
        *key = rand() % 1000;
        *value = *key * 10;
        bplus_tree_insert(tree, key, value);
    }

    printf("插入 20 个随机键\n\n");

    const void* min_key;
    void* min_value;
    if (bplus_tree_min(tree, &min_key, &min_value)) {
        printf("最小键: %d, 值: %d\n", *(int*)min_key, *(int*)min_value);
    }

    const void* max_key;
    void* max_value;
    if (bplus_tree_max(tree, &max_key, &max_value)) {
        printf("最大键: %d, 值: %d\n", *(int*)max_key, *(int*)max_value);
    }

    printf("\n所有键（升序）:\n  ");
    bplus_iterator_t* iter = bplus_iterator_create(tree);
    const void* key;
    void* value;
    int count = 0;
    while (bplus_iterator_next(iter, &key, &value)) {
        printf("%d ", *(int*)key);
        if (++count % 10 == 0) printf("\n  ");
    }
    printf("\n");
    bplus_iterator_free(iter);

    // 清理
    iter = bplus_iterator_create(tree);
    while (bplus_iterator_next(iter, &key, &value)) {
        free((void*)key);
        free(value);
    }
    bplus_iterator_free(iter);
    bplus_tree_free(tree);
}

// 演示 8: 应用场景 - 索引
static void demo_indexing(void) {
    printf("\n=== 演示 8: 应用场景 - 数据库索引 ===\n");

    bplus_tree_t* index = bplus_tree_create(int_compare);
    if (!index) return;

    printf("模拟数据库索引:\n\n");

    // 模拟用户数据，以用户ID为键
    typedef struct {
        int user_id;
        char name[32];
        char email[64];
    } user_t;

    user_t users[] = {
        {1001, "Alice", "alice@example.com"},
        {1002, "Bob", "bob@example.com"},
        {1003, "Charlie", "charlie@example.com"},
        {1004, "David", "david@example.com"},
        {1005, "Eve", "eve@example.com"}
    };

    printf("创建用户ID索引:\n");
    for (int i = 0; i < 5; i++) {
        int* key = malloc(sizeof(int));
        *key = users[i].user_id;
        user_t* value = malloc(sizeof(user_t));
        *value = users[i];
        bplus_tree_insert(index, key, value);
        printf("  索引: user_id=%d -> %s\n", users[i].user_id, users[i].name);
    }

    printf("\n通过ID查找用户:\n");
    int search_ids[] = {1003, 1001, 9999};
    for (int i = 0; i < 3; i++) {
        user_t* user = (user_t*)bplus_tree_get(index, &search_ids[i]);
        if (user) {
            printf("  ID=%d: %s (%s)\n", search_ids[i], user->name, user->email);
        } else {
            printf("  ID=%d: 用户不存在\n", search_ids[i]);
        }
    }

    printf("\n范围查询 (ID 1002-1004):\n");
    int start = 1002, end = 1004;
    bplus_iterator_t* iter = bplus_iterator_create(index);
    const void* key;
    void* value;
    while (bplus_iterator_next(iter, &key, &value)) {
        int k = *(int*)key;
        if (k >= start && k <= end) {
            user_t* user = (user_t*)value;
            printf("  ID=%d: %s\n", k, user->name);
        }
    }
    bplus_iterator_free(iter);

    // 清理
    iter = bplus_iterator_create(index);
    while (bplus_iterator_next(iter, &key, &value)) {
        free((void*)key);
        free(value);
    }
    bplus_iterator_free(iter);
    bplus_tree_free(index);
}

// 演示 9: 有序性保证
static void demo_ordering(void) {
    printf("\n=== 演示 9: 有序性保证 ===\n");

    bplus_tree_t* tree = bplus_tree_create(int_compare);
    if (!tree) return;

    printf("插入乱序数据:\n  ");
    int keys[] = {50, 10, 90, 30, 70, 20, 80, 40, 60};
    int num_keys = sizeof(keys) / sizeof(keys[0]);

    for (int i = 0; i < num_keys; i++) {
        int* key = malloc(sizeof(int));
        int* value = malloc(sizeof(int));
        *key = keys[i];
        *value = keys[i];
        bplus_tree_insert(tree, key, value);
        printf("%d ", keys[i]);
    }
    printf("\n");

    printf("\n遍历结果（自动排序）:\n  ");
    bplus_iterator_t* iter = bplus_iterator_create(tree);
    const void* key;
    void* value;
    while (bplus_iterator_next(iter, &key, &value)) {
        printf("%d ", *(int*)key);
    }
    printf("\n");
    bplus_iterator_free(iter);

    // 清理
    iter = bplus_iterator_create(tree);
    while (bplus_iterator_next(iter, &key, &value)) {
        free((void*)key);
        free(value);
    }
    bplus_iterator_free(iter);
    bplus_tree_free(tree);
}

// 演示 10: 特点总结
static void demo_features(void) {
    printf("\n=== 演示 10: B+ 树特点 ===\n");

    printf("B+ 树的优势:\n\n");

    printf("1. 磁盘友好:\n");
    printf("   - 节点大小通常与磁盘页对齐\n");
    printf("   - 减少磁盘 I/O 次数\n");
    printf("   - 适合大规模数据存储\n\n");

    printf("2. 范围查询高效:\n");
    printf("   - 叶子节点形成有序链表\n");
    printf("   - 顺序遍历无需回溯\n");
    printf("   - 适合区间查询\n\n");

    printf("3. 查询稳定:\n");
    printf("   - 所有查询都到达叶子节点\n");
    printf("   - 时间复杂度稳定为 O(log n)\n\n");

    printf("4. 应用场景:\n");
    printf("   - 数据库索引\n");
    printf("   - 文件系统\n");
    printf("   - 键值存储\n");
    printf("   - 需要范围查询的场景\n");
}

int main(void) {
    printf("========================================\n");
    printf("    B+ 树演示程序\n");
    printf("========================================\n");

    demo_basic_operations();
    demo_iterator();
    demo_range_query();
    demo_edge_cases();
    demo_performance();
    demo_string_keys();
    demo_min_max();
    demo_indexing();
    demo_ordering();
    demo_features();

    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("========================================\n");

    return 0;
}
