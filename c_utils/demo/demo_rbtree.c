/**
 * 红黑树演示程序
 *
 * 功能：
 * - 基本操作（插入、查找、删除）
 * - 遍历（中序、前序、后序）
 * - 迭代器
 * - 极值查找
 * - 树属性（高度、大小）
 */

#include <stdio.h>
#include <stdlib.h>
#include "../c_utils/rbtree.h"

static int int_compare(const void *a, const void *b) {
    int ia = *(int*)a;
    int ib = *(int*)b;
    return ia - ib;
}

// 遍历回调：打印键值
static void print_node(void *key, void *value, void *user_data) {
    (void)user_data;
    printf("  %d", *(int*)key);
}

// 遍历回调：收集键到数组中
static void collect_key(void *key, void *value, void *user_data) {
    (void)value;
    int **arr = (int**)user_data;
    **arr = *(int*)key;
    (*arr)++;
}

// 演示 1: 基本操作（插入、查找）
static void demo_basic_operations(void) {
    printf("\n=== 演示 1: 基本操作 ===\n");

    rbtree_t *tree = rbtree_create(int_compare);
    if (!tree) {
        printf("创建树失败\n");
        return;
    }

    printf("插入元素: 10, 5, 15, 3, 7, 12, 20\n");
    int keys[] = {10, 5, 15, 3, 7, 12, 20};
    for (int i = 0; i < 7; i++) {
        rbtree_insert(tree, &keys[i], &keys[i]);
    }

    printf("树大小: %zu\n", rbtree_size(tree));
    printf("树高度: %d\n", rbtree_height(tree));
    printf("树是否为空: %s\n", rbtree_is_empty(tree) ? "是" : "否");

    int key = 7;
    bool contains = rbtree_contains(tree, &key);
    printf("查找 7: %s\n", contains ? "找到" : "未找到");

    key = 100;
    contains = rbtree_contains(tree, &key);
    printf("查找 100: %s\n", contains ? "找到" : "未找到");

    // 通过 rbtree_get 获取值
    key = 15;
    int *val = (int*)rbtree_get(tree, &key);
    if (val) {
        printf("获取 key=15 的值: %d\n", *val);
    }

    rbtree_free(tree);
}

// 演示 2: 极值查找
static void demo_min_max(void) {
    printf("\n=== 演示 2: 极值查找 ===\n");

    rbtree_t *tree = rbtree_create(int_compare);

    int keys[] = {50, 30, 70, 20, 40, 60, 80};
    for (int i = 0; i < 7; i++) {
        rbtree_insert(tree, &keys[i], &keys[i]);
    }

    printf("插入: 50, 30, 70, 20, 40, 60, 80\n");

    int *min_val = (int*)rbtree_min(tree);
    int *max_val = (int*)rbtree_max(tree);

    printf("最小值: %d\n", min_val ? *min_val : -1);
    printf("最大值: %d\n", max_val ? *max_val : -1);

    rbtree_free(tree);
}

// 演示 3: 三种遍历方式
static void demo_traversals(void) {
    printf("\n=== 演示 3: 遍历 ===\n");

    rbtree_t *tree = rbtree_create(int_compare);

    int keys[] = {50, 30, 70, 20, 40, 60, 80};
    for (int i = 0; i < 7; i++) {
        rbtree_insert(tree, &keys[i], &keys[i]);
    }

    printf("中序遍历 (排序输出):");
    rbtree_inorder(tree, print_node, NULL);
    printf("\n");

    printf("前序遍历 (根-左-右):");
    rbtree_preorder(tree, print_node, NULL);
    printf("\n");

    printf("后序遍历 (左-右-根):");
    rbtree_postorder(tree, print_node, NULL);
    printf("\n");

    rbtree_free(tree);
}

// 演示 4: 迭代器
static void demo_iterator(void) {
    printf("\n=== 演示 4: 迭代器 ===\n");

    rbtree_t *tree = rbtree_create(int_compare);

    int keys[] = {100, 50, 150, 25, 75, 125, 175};
    for (int i = 0; i < 7; i++) {
        rbtree_insert(tree, &keys[i], &keys[i]);
    }

    printf("使用迭代器遍历:\n");
    rbtree_iter_t iter = rbtree_iter_begin(tree);
    int count = 0;
    while (rbtree_iter_valid(&iter)) {
        int *key = (int*)rbtree_iter_key(&iter);
        int *val = (int*)rbtree_iter_value(&iter);
        printf("  [%d] key=%d, value=%d\n", count++, *key, *val);
        rbtree_iter_next(&iter);
    }
    printf("共遍历 %d 个节点\n", count);
    rbtree_iter_free(&iter);

    rbtree_free(tree);
}

// 演示 5: 删除操作
static void demo_delete(void) {
    printf("\n=== 演示 5: 删除操作 ===\n");

    rbtree_t *tree = rbtree_create(int_compare);

    int keys[] = {50, 30, 70, 20, 40, 60, 80, 10, 90};
    for (int i = 0; i < 9; i++) {
        rbtree_insert(tree, &keys[i], &keys[i]);
    }

    printf("初始树 (中序):");
    rbtree_inorder(tree, print_node, NULL);
    printf("\n初始大小: %zu, 高度: %d\n\n", rbtree_size(tree), rbtree_height(tree));

    // 删除叶子节点
    int del_key = 10;
    printf("删除叶子节点 %d:\n", del_key);
    rbtree_delete(tree, &del_key);
    printf("  中序:");
    rbtree_inorder(tree, print_node, NULL);
    printf("\n  大小: %zu\n\n", rbtree_size(tree));

    // 删除有一个子节点的节点
    del_key = 20;
    printf("删除单子节点 %d:\n", del_key);
    rbtree_delete(tree, &del_key);
    printf("  中序:");
    rbtree_inorder(tree, print_node, NULL);
    printf("\n  大小: %zu\n\n", rbtree_size(tree));

    // 删除有两个子节点的节点
    del_key = 50;
    printf("删除双子节点(根) %d:\n", del_key);
    rbtree_delete(tree, &del_key);
    printf("  中序:");
    rbtree_inorder(tree, print_node, NULL);
    printf("\n  大小: %zu, 高度: %d\n\n", rbtree_size(tree), rbtree_height(tree));

    // 验证删除后树仍然有效（能找到剩余节点）
    printf("验证剩余节点:\n");
    int remaining[] = {30, 40, 60, 70, 80, 90};
    for (int i = 0; i < 6; i++) {
        printf("  查找 %d: %s\n", remaining[i],
               rbtree_contains(tree, &remaining[i]) ? "找到" : "未找到(错误!)");
    }

    rbtree_free(tree);
}

int main(void) {
    printf("========================================\n");
    printf("    红黑树演示\n");
    printf("========================================\n");

    demo_basic_operations();
    demo_min_max();
    demo_traversals();
    demo_iterator();
    demo_delete();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
