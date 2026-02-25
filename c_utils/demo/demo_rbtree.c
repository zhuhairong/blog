/**
 * 红黑树演示程序
 */

#include <stdio.h>
#include <stdlib.h>
#include "../c_utils/rbtree.h"

static int int_compare(const void *a, const void *b) {
    int ia = *(int*)a;
    int ib = *(int*)b;
    return ia - ib;
}

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

    printf("树是否为空: %s\n", rbtree_is_empty(tree) ? "是" : "否");

    int key = 7;
    bool contains = rbtree_contains(tree, &key);
    printf("查找 7: %s\n", contains ? "找到" : "未找到");

    key = 100;
    contains = rbtree_contains(tree, &key);
    printf("查找 100: %s\n", contains ? "找到" : "未找到");
}

static void demo_search(void) {
    printf("\n=== 演示 2: 搜索 ===\n");

    rbtree_t *tree = rbtree_create(int_compare);

    int keys[] = {50, 30, 70, 20, 40, 60, 80};
    for (int i = 0; i < 7; i++) {
        rbtree_insert(tree, &keys[i], &keys[i]);
    }

    int search_keys[] = {20, 50, 80, 99};
    for (int i = 0; i < 4; i++) {
        bool found = rbtree_contains(tree, &search_keys[i]);
        printf("搜索 %d: %s\n", search_keys[i], found ? "找到" : "未找到");
    }
}

int main(void) {
    printf("========================================\n");
    printf("    红黑树演示\n");
    printf("========================================\n");

    demo_basic_operations();
    demo_search();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
