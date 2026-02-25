/**
 * @file demo_avl.c
 * @brief AVL 树演示
 *
 * 本演示展示了 AVL 自平衡二叉搜索树的各种用法，包括：
 * - 基本插入、查找、删除
 * - 遍历操作（中序、前序、后序）
 * - 极值查找
 * - 迭代器使用
 * - 自定义数据类型
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/avl.h"

// 整数比较函数
static int int_compare(const void* a, const void* b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return (ia > ib) - (ia < ib);
}

// 字符串比较函数
static int str_compare(const void* a, const void* b) {
    return strcmp((const char*)a, (const char*)b);
}

// 演示 1: 基本操作
static void demo_basic_operations(void) {
    printf("\n=== 演示 1: 基本操作 ===\n");

    avl_t* tree = avl_create(int_compare);
    if (!tree) {
        fprintf(stderr, "创建 AVL 树失败\n");
        return;
    }

    // 插入一些整数
    int values[] = {50, 30, 70, 20, 40, 60, 80, 10, 25, 35, 45};
    int n = sizeof(values) / sizeof(values[0]);

    printf("插入 %d 个整数:\n", n);
    for (int i = 0; i < n; i++) {
        int* key = malloc(sizeof(int));
        int* val = malloc(sizeof(int));
        *key = values[i];
        *val = values[i] * 10;  // 值是键的 10 倍
        avl_insert(tree, key, val);
        printf("  插入: key=%d, value=%d\n", *key, *val);
    }

    printf("\n树的大小: %zu\n", avl_size(tree));
    printf("树的高度: %d\n", avl_height(tree));
    printf("是否为空: %s\n", avl_is_empty(tree) ? "是" : "否");

    // 查找
    printf("\n查找操作:\n");
    int search_keys[] = {30, 100, 50};
    for (int i = 0; i < 3; i++) {
        int* result = avl_get(tree, &search_keys[i]);
        if (result) {
            printf("  查找 %d: 找到, value=%d\n", search_keys[i], *result);
        } else {
            printf("  查找 %d: 未找到\n", search_keys[i]);
        }
    }

    // 检查包含
    printf("\n包含检查:\n");
    for (int i = 0; i < 3; i++) {
        printf("  包含 %d: %s\n", search_keys[i],
               avl_contains(tree, &search_keys[i]) ? "是" : "否");
    }

    // 删除
    printf("\n删除操作:\n");
    int del_key = 30;
    printf("  删除 %d\n", del_key);
    avl_delete(tree, &del_key);
    printf("  删除后大小: %zu\n", avl_size(tree));
    printf("  包含 %d: %s\n", del_key,
           avl_contains(tree, &del_key) ? "是" : "否");

    avl_free(tree);
    printf("\nAVL 树已释放\n");
}

// 演示 2: 遍历操作
static void demo_traversal(void) {
    printf("\n=== 演示 2: 遍历操作 ===\n");

    avl_t* tree = avl_create(int_compare);
    if (!tree) {
        fprintf(stderr, "创建 AVL 树失败\n");
        return;
    }

    // 插入数据
    int values[] = {50, 30, 70, 20, 40, 60, 80};
    int n = sizeof(values) / sizeof(values[0]);
    for (int i = 0; i < n; i++) {
        int* key = malloc(sizeof(int));
        *key = values[i];
        avl_insert(tree, key, key);  // 键值相同
    }

    // 中序遍历（升序）
    printf("中序遍历（升序）:\n");
    avl_inorder(tree, (avl_visit_fn)printf, NULL);
    printf("\n");

    // 使用回调函数打印
    printf("\n中序遍历（带格式）:\n  ");
    avl_inorder(tree, (avl_visit_fn)printf, NULL);
    printf("\n");

    avl_free(tree);
}

// 演示 3: 极值查找
static void demo_min_max(void) {
    printf("\n=== 演示 3: 极值查找 ===\n");

    avl_t* tree = avl_create(int_compare);
    if (!tree) {
        fprintf(stderr, "创建 AVL 树失败\n");
        return;
    }

    int values[] = {45, 23, 78, 12, 89, 34, 67};
    int n = sizeof(values) / sizeof(values[0]);

    printf("插入的值: ");
    for (int i = 0; i < n; i++) {
        int* key = malloc(sizeof(int));
        *key = values[i];
        avl_insert(tree, key, key);
        printf("%d ", values[i]);
    }
    printf("\n");

    int* min_val = avl_min(tree);
    int* max_val = avl_max(tree);

    printf("最小值: %d\n", min_val ? *min_val : -1);
    printf("最大值: %d\n", max_val ? *max_val : -1);

    avl_free(tree);
}

// 演示 4: 字符串键
static void demo_string_keys(void) {
    printf("\n=== 演示 4: 字符串键 ===\n");

    avl_t* tree = avl_create(str_compare);
    if (!tree) {
        fprintf(stderr, "创建 AVL 树失败\n");
        return;
    }

    // 插入一些键值对
    typedef struct {
        char* key;
        char* value;
    } kv_pair_t;

    kv_pair_t pairs[] = {
        {"apple", "A fruit that is red or green"},
        {"banana", "A long yellow fruit"},
        {"cherry", "A small red fruit"},
        {"date", "A sweet brown fruit"},
        {"elderberry", "A small dark purple fruit"}
    };
    int n = sizeof(pairs) / sizeof(pairs[0]);

    printf("插入 %d 个字符串键值对:\n", n);
    for (int i = 0; i < n; i++) {
        char* key = strdup(pairs[i].key);
        char* value = strdup(pairs[i].value);
        avl_insert(tree, key, value);
        printf("  %s -> %s\n", key, value);
    }

    printf("\n查找操作:\n");
    char* search_keys[] = {"banana", "grape", "apple"};
    for (int i = 0; i < 3; i++) {
        char* result = avl_get(tree, search_keys[i]);
        if (result) {
            printf("  %s: %s\n", search_keys[i], result);
        } else {
            printf("  %s: 未找到\n", search_keys[i]);
        }
    }

    // 使用带释放函数的 free
    avl_free_with_data(tree, free, free);
    printf("\nAVL 树已释放（包括键和值）\n");
}

// 演示 5: 迭代器使用
static void demo_iterator(void) {
    printf("\n=== 演示 5: 迭代器使用 ===\n");

    avl_t* tree = avl_create(int_compare);
    if (!tree) {
        fprintf(stderr, "创建 AVL 树失败\n");
        return;
    }

    // 插入数据
    for (int i = 0; i < 10; i++) {
        int* key = malloc(sizeof(int));
        *key = (i * 7) % 23;  // 一些随机值
        avl_insert(tree, key, key);
    }

    printf("使用迭代器遍历（中序）:\n  ");
    avl_iter_t iter = avl_iter_begin(tree);
    while (avl_iter_valid(&iter)) {
        int* key = avl_iter_key(&iter);
        int* value = avl_iter_value(&iter);
        printf("[%d:%d] ", *key, *value);
        avl_iter_next(&iter);
    }
    avl_iter_free(&iter);
    printf("\n");

    avl_free(tree);
}

// 演示 6: 自定义数据结构
static void demo_custom_struct(void) {
    printf("\n=== 演示 6: 自定义数据结构 ===\n");

    typedef struct {
        int id;
        char name[32];
        double score;
    } student_t;

    // 学生 ID 比较函数
    int student_compare(const void* a, const void* b) {
        const student_t* sa = (const student_t*)a;
        const student_t* sb = (const student_t*)b;
        return (sa->id > sb->id) - (sa->id < sb->id);
    }

    avl_t* tree = avl_create(student_compare);
    if (!tree) {
        fprintf(stderr, "创建 AVL 树失败\n");
        return;
    }

    // 创建学生数据
    student_t students[] = {
        {1003, "Alice", 85.5},
        {1001, "Bob", 92.0},
        {1005, "Charlie", 78.5},
        {1002, "Diana", 95.5},
        {1004, "Eve", 88.0}
    };
    int n = sizeof(students) / sizeof(students[0]);

    printf("插入 %d 个学生记录（按 ID 排序）:\n", n);
    for (int i = 0; i < n; i++) {
        student_t* key = malloc(sizeof(student_t));
        student_t* value = malloc(sizeof(student_t));
        *key = students[i];
        *value = students[i];
        avl_insert(tree, key, value);
        printf("  ID=%d, Name=%s, Score=%.1f\n",
               students[i].id, students[i].name, students[i].score);
    }

    // 查找
    student_t search_key = {1003, "", 0};
    student_t* result = avl_get(tree, &search_key);
    if (result) {
        printf("\n查找 ID=1003:\n");
        printf("  Name=%s, Score=%.1f\n", result->name, result->score);
    }

    avl_free_with_data(tree, free, free);
}

// 演示 7: 大量数据测试
static void demo_large_dataset(void) {
    printf("\n=== 演示 7: 大量数据测试 ===\n");

    avl_t* tree = avl_create(int_compare);
    if (!tree) {
        fprintf(stderr, "创建 AVL 树失败\n");
        return;
    }

    const int N = 10000;
    printf("插入 %d 个随机整数...\n", N);

    for (int i = 0; i < N; i++) {
        int* key = malloc(sizeof(int));
        *key = rand() % 100000;
        int* val = malloc(sizeof(int));
        *val = *key;
        avl_insert(tree, key, val);
    }

    printf("插入完成\n");
    printf("树的大小: %zu\n", avl_size(tree));
    printf("树的高度: %d\n", avl_height(tree));

    // 查找测试
    int search_key = 50000;
    int* result = avl_get(tree, &search_key);
    printf("查找 %d: %s\n", search_key, result ? "找到" : "未找到");

    // 极值
    int* min_val = avl_min(tree);
    int* max_val = avl_max(tree);
    printf("最小值: %d\n", min_val ? *min_val : -1);
    printf("最大值: %d\n", max_val ? *max_val : -1);

    avl_free_with_data(tree, free, free);
}

int main(void) {
    printf("========================================\n");
    printf("    AVL 自平衡二叉搜索树演示程序\n");
    printf("========================================\n");

    demo_basic_operations();
    demo_traversal();
    demo_min_max();
    demo_string_keys();
    demo_iterator();
    demo_custom_struct();
    demo_large_dataset();

    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("========================================\n");

    return 0;
}
