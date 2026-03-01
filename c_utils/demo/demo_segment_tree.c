/**
 * 线段树演示程序
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/segment_tree.h"

static void demo_basic_sum(void) {
    printf("\n=== 演示 1: 区间求和 ===\n");

    int arr[] = {1, 3, 5, 7, 9, 11};
    size_t n = sizeof(arr) / sizeof(arr[0]);

    printf("原始数组: ");
    for (size_t i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    segment_tree_t st;
    memset(&st, 0, sizeof(st));
    segment_tree_init(&st, arr, n);

    int sum = segment_tree_query(&st, 0, n - 1);
    printf("区间 [0, %zu] 求和: %d\n", n - 1, sum);

    sum = segment_tree_query(&st, 1, 3);
    printf("区间 [1, 3] 求和: %d\n", sum);

    segment_tree_free(&st);
}

static void demo_update(void) {
    printf("\n=== 演示 2: 单点更新 ===\n");

    int arr[] = {1, 2, 3, 4, 5};
    size_t n = sizeof(arr) / sizeof(arr[0]);

    printf("原始数组: ");
    for (size_t i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    segment_tree_t st;
    memset(&st, 0, sizeof(st));
    segment_tree_init(&st, arr, n);

    int sum = segment_tree_query(&st, 0, n - 1);
    printf("更新前区间 [0, 4] 求和: %d\n", sum);

    printf("\n更新 arr[2] = 3 -> 10\n");
    segment_tree_update(&st, 2, 10);

    sum = segment_tree_query(&st, 0, n - 1);
    printf("更新后区间 [0, 4] 求和: %d\n", sum);

    segment_tree_free(&st);
}

static void demo_state(void) {
    printf("\n=== 演示 3: 线段树状态 ===\n");

    int arr[] = {1, 2, 3, 4, 5, 6, 7, 8};
    size_t n = sizeof(arr) / sizeof(arr[0]);

    segment_tree_t st;
    memset(&st, 0, sizeof(st));
    segment_tree_init(&st, arr, n);

    segment_tree_state_t state;
    segment_tree_error_t error;
    if (segment_tree_get_state(&st, &state, &error)) {
        printf("线段树状态:\n");
        printf("  原始数组大小: %zu\n", state.size);
        printf("  线段树大小: %zu\n", state.tree_size);
        printf("  已初始化: %s\n", state.initialized ? "是" : "否");
    }

    segment_tree_free(&st);
}

static void demo_error_handling(void) {
    printf("\n=== 演示 4: 错误处理 ===\n");

    segment_tree_t st;
    segment_tree_error_t error;

    printf("测试空指针初始化:\n");
    if (!segment_tree_init_ex(NULL, NULL, 0, NULL, &error)) {
        printf("  正确捕获错误: %d\n", error);
    }

    int arr[] = {1, 2, 3};
    memset(&st, 0, sizeof(st));
    segment_tree_init(&st, arr, 3);

    printf("\n测试越界更新:\n");
    if (!segment_tree_update_ex(&st, 100, 5, &error)) {
        printf("  正确捕获越界错误: %d\n", error);
    }

    segment_tree_free(&st);
}

static void demo_reset(void) {
    printf("\n=== 演示 5: 重置线段树 ===\n");

    int arr1[] = {1, 2, 3, 4, 5};
    int arr2[] = {10, 20, 30};

    segment_tree_t st;
    memset(&st, 0, sizeof(st));
    segment_tree_init(&st, arr1, 5);

    int sum = segment_tree_query(&st, 0, 4);
    printf("原数组求和: %d\n", sum);

    printf("\n重置为新数组...\n");
    segment_tree_error_t error;
    segment_tree_reset(&st, arr2, 3, &error);

    sum = segment_tree_query(&st, 0, 2);
    printf("新数组求和: %d\n", sum);

    segment_tree_free(&st);
}

int main(void) {
    printf("========================================\n");
    printf("    线段树演示\n");
    printf("========================================\n");

    demo_basic_sum();
    demo_update();
    demo_state();
    demo_error_handling();
    demo_reset();

    printf("\n========================================\n");
    printf("演示完成!\n");
    printf("========================================\n");

    return 0;
}
