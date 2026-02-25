/**
 * @file demo_binary_search.c
 * @brief 二分查找算法演示
 *
 * 本演示展示了二分查找的各种用法，包括：
 * - 基本查找
 * - 上下界查找
 * - 自定义比较函数
 * - 数组排序检查
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/binary_search.h"

// 整数比较函数
static int compare_int(const void* a, const void* b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return (ia > ib) - (ia < ib);
}

// 字符串比较函数
static int compare_str(const void* a, const void* b) {
    return strcmp(*(const char**)a, *(const char**)b);
}

// 打印整数数组
static void print_int_array(const int* arr, size_t n, const char* label) {
    printf("%s: [", label);
    for (size_t i = 0; i < n; i++) {
        printf("%d", arr[i]);
        if (i < n - 1) printf(", ");
    }
    printf("]\n");
}

// 演示 1: 基本查找
static void demo_basic_search(void) {
    printf("\n=== 演示 1: 基本二分查找 ===\n");

    int arr[] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19};
    size_t n = sizeof(arr) / sizeof(arr[0]);

    print_int_array(arr, n, "数组");

    int targets[] = {7, 1, 19, 10, 20};
    int num_targets = sizeof(targets) / sizeof(targets[0]);

    printf("\n查找结果:\n");
    for (int i = 0; i < num_targets; i++) {
        int key = targets[i];
        int* result = (int*)binary_search(&key, arr, n, sizeof(int), compare_int);

        if (result) {
            printf("  %d: 找到，索引 %zu\n", key, (size_t)(result - arr));
        } else {
            printf("  %d: 未找到\n", key);
        }
    }
}

// 演示 2: 查找索引
static void demo_search_index(void) {
    printf("\n=== 演示 2: 查找索引 ===\n");

    int arr[] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    size_t n = sizeof(arr) / sizeof(arr[0]);

    print_int_array(arr, n, "数组");

    int targets[] = {30, 10, 100, 25, 110};
    int num_targets = sizeof(targets) / sizeof(targets[0]);

    printf("\n查找索引:\n");
    for (int i = 0; i < num_targets; i++) {
        int key = targets[i];
        size_t index = binary_search_index(&key, arr, n, sizeof(int), compare_int);

        if (index != (size_t)-1) {
            printf("  %d: 索引 %zu\n", key, index);
        } else {
            printf("  %d: 未找到\n", key);
        }
    }
}

// 演示 3: 下界查找
static void demo_lower_bound(void) {
    printf("\n=== 演示 3: 下界查找 ===\n");

    int arr[] = {1, 2, 4, 4, 4, 7, 9};
    size_t n = sizeof(arr) / sizeof(arr[0]);

    print_int_array(arr, n, "数组");
    printf("说明: 下界返回第一个不小于 key 的元素\n");

    int targets[] = {0, 3, 4, 6, 10};
    int num_targets = sizeof(targets) / sizeof(targets[0]);

    printf("\n下界查找:\n");
    for (int i = 0; i < num_targets; i++) {
        int key = targets[i];
        int* result = (int*)binary_search_lower_bound(&key, arr, n, sizeof(int), compare_int);

        if (result) {
            size_t index = (size_t)(result - arr);
            printf("  %d: 下界在索引 %zu (值 %d)\n", key, index, *result);
        } else {
            printf("  %d: 无下界 (所有元素都小于 key)\n", key);
        }
    }
}

// 演示 4: 上界查找
static void demo_upper_bound(void) {
    printf("\n=== 演示 4: 上界查找 ===\n");

    int arr[] = {1, 2, 4, 4, 4, 7, 9};
    size_t n = sizeof(arr) / sizeof(arr[0]);

    print_int_array(arr, n, "数组");
    printf("说明: 上界返回第一个大于 key 的元素\n");

    int targets[] = {0, 3, 4, 6, 10};
    int num_targets = sizeof(targets) / sizeof(targets[0]);

    printf("\n上界查找:\n");
    for (int i = 0; i < num_targets; i++) {
        int key = targets[i];
        int* result = (int*)binary_search_upper_bound(&key, arr, n, sizeof(int), compare_int);

        if (result) {
            size_t index = (size_t)(result - arr);
            printf("  %d: 上界在索引 %zu (值 %d)\n", key, index, *result);
        } else {
            printf("  %d: 无上界 (所有元素都不大于 key)\n", key);
        }
    }
}

// 演示 5: 上下界索引查找
static void demo_bound_indices(void) {
    printf("\n=== 演示 5: 上下界索引查找 ===\n");

    int arr[] = {1, 2, 4, 4, 4, 4, 7, 9};
    size_t n = sizeof(arr) / sizeof(arr[0]);

    print_int_array(arr, n, "数组");

    int key = 4;
    size_t lower = binary_search_lower_bound_index(&key, arr, n, sizeof(int), compare_int);
    size_t upper = binary_search_upper_bound_index(&key, arr, n, sizeof(int), compare_int);

    printf("\n查找 %d:\n", key);
    printf("  下界索引: %zu\n", lower);
    printf("  上界索引: %zu\n", upper);
    printf("  元素 %d 出现次数: %zu\n", key, upper - lower);
    printf("  范围: [%zu, %zu)\n", lower, upper);
}

// 演示 6: 字符串数组查找
static void demo_string_search(void) {
    printf("\n=== 演示 6: 字符串数组查找 ===\n");

    const char* fruits[] = {
        "apple", "banana", "cherry", "grape", "orange",
        "peach", "pear", "strawberry", "watermelon"
    };
    size_t n = sizeof(fruits) / sizeof(fruits[0]);

    printf("水果数组:\n  ");
    for (size_t i = 0; i < n; i++) {
        printf("\"%s\"", fruits[i]);
        if (i < n - 1) printf(", ");
    }
    printf("\n");

    const char* targets[] = {"cherry", "apple", "watermelon", "kiwi", "melon"};
    int num_targets = sizeof(targets) / sizeof(targets[0]);

    printf("\n查找结果:\n");
    for (int i = 0; i < num_targets; i++) {
        const char* key = targets[i];
        const char** result = (const char**)binary_search(
            &key, fruits, n, sizeof(char*), compare_str);

        if (result) {
            printf("  \"%s\": 找到，索引 %zu\n", key, (size_t)(result - fruits));
        } else {
            printf("  \"%s\": 未找到\n", key);
        }
    }
}

// 演示 7: 数组排序检查
static void demo_sorted_check(void) {
    printf("\n=== 演示 7: 数组排序检查 ===\n");

    int sorted_arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int unsorted_arr[] = {1, 3, 2, 4, 5, 7, 6, 8, 9, 10};

    size_t n1 = sizeof(sorted_arr) / sizeof(sorted_arr[0]);
    size_t n2 = sizeof(unsorted_arr) / sizeof(unsorted_arr[0]);

    print_int_array(sorted_arr, n1, "有序数组");
    bool is_sorted1 = binary_search_is_sorted(sorted_arr, n1, sizeof(int), compare_int);
    printf("  是否已排序: %s\n", is_sorted1 ? "是" : "否");

    print_int_array(unsorted_arr, n2, "无序数组");
    bool is_sorted2 = binary_search_is_sorted(unsorted_arr, n2, sizeof(int), compare_int);
    printf("  是否已排序: %s\n", is_sorted2 ? "是" : "否");
}

// 演示 8: 范围查询
static void demo_range_query(void) {
    printf("\n=== 演示 8: 范围查询 ===\n");

    int arr[] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25};
    size_t n = sizeof(arr) / sizeof(arr[0]);

    print_int_array(arr, n, "数组");

    int low = 8, high = 18;
    printf("\n查询范围 [%d, %d]:\n", low, high);

    // 查找下界
    size_t lower = binary_search_lower_bound_index(&low, arr, n, sizeof(int), compare_int);
    // 查找上界
    size_t upper = binary_search_upper_bound_index(&high, arr, n, sizeof(int), compare_int);

    printf("  下界索引: %zu (值 %d)\n", lower, arr[lower]);
    printf("  上界索引: %zu (值 %d)\n", upper - 1, arr[upper - 1]);
    printf("  范围内的元素:");

    for (size_t i = lower; i < upper; i++) {
        printf(" %d", arr[i]);
    }
    printf("\n");
}

// 演示 9: 查找插入位置
static void demo_insert_position(void) {
    printf("\n=== 演示 9: 查找插入位置 ===\n");

    int arr[] = {10, 20, 30, 40, 50, 60, 70, 80};
    size_t n = sizeof(arr) / sizeof(arr[0]);

    print_int_array(arr, n, "数组");

    int values[] = {5, 25, 45, 85};
    int num_values = sizeof(values) / sizeof(values[0]);

    printf("\n查找插入位置:\n");
    for (int i = 0; i < num_values; i++) {
        int key = values[i];
        size_t pos = binary_search_lower_bound_index(&key, arr, n, sizeof(int), compare_int);
        printf("  %d 应插入索引 %zu\n", key, pos);
    }
}

// 演示 10: 性能对比
static void demo_performance(void) {
    printf("\n=== 演示 10: 二分查找 vs 线性查找 ===\n");

    const int sizes[] = {100, 1000, 10000};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    printf("数组大小 | 二分查找步数 | 线性查找平均步数\n");
    printf("---------|--------------|-----------------\n");

    for (int i = 0; i < num_sizes; i++) {
        int n = sizes[i];
        int binary_steps = 0;
        int linear_avg = n / 2;

        // 计算二分查找最大步数
        int temp = n;
        while (temp > 0) {
            temp /= 2;
            binary_steps++;
        }

        printf("  %6d |      %6d |          %6d\n", n, binary_steps, linear_avg);
    }

    printf("\n说明: 二分查找时间复杂度 O(log n)，线性查找 O(n)\n");
}

int main(void) {
    printf("========================================\n");
    printf("    二分查找算法演示程序\n");
    printf("========================================\n");

    demo_basic_search();
    demo_search_index();
    demo_lower_bound();
    demo_upper_bound();
    demo_bound_indices();
    demo_string_search();
    demo_sorted_check();
    demo_range_query();
    demo_insert_position();
    demo_performance();

    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("========================================\n");

    return 0;
}
