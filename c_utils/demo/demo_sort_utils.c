#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sort_utils.h"

int int_compare(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

void demo_quicksort(void) {
    printf("\n=== 快速排序演示 ===\n");
    
    int arr[] = {9, 7, 5, 11, 12, 2, 14, 3, 10, 6};
    size_t n = sizeof(arr) / sizeof(arr[0]);
    
    printf("排序前: ");
    for (size_t i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
    
    sort_utils_quicksort(arr, n, sizeof(int), int_compare);
    
    printf("排序后: ");
    for (size_t i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void demo_binary_search(void) {
    printf("\n=== 二分查找演示 ===\n");
    
    int arr[] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19};
    size_t n = sizeof(arr) / sizeof(arr[0]);
    
    printf("数组: ");
    for (size_t i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
    
    int targets[] = {7, 10, 1, 20};
    for (int i = 0; i < 4; i++) {
        int index = sort_binary_search(&targets[i], arr, n, sizeof(int), int_compare);
        if (index >= 0) {
            printf("查找 %d: 找到，索引 = %d\n", targets[i], index);
        } else {
            printf("查找 %d: 未找到\n", targets[i]);
        }
    }
}

void demo_quicksort_strings(void) {
    printf("\n=== 快速排序字符串演示 ===\n");
    
    const char* arr[] = {"banana", "apple", "orange", "grape", "mango"};
    size_t n = sizeof(arr) / sizeof(arr[0]);
    
    printf("排序前: ");
    for (size_t i = 0; i < n; i++) {
        printf("%s ", arr[i]);
    }
    printf("\n");
    
    sort_utils_quicksort(arr, n, sizeof(const char*), (int (*)(const void*, const void*))strcmp);
    
    printf("排序后: ");
    for (size_t i = 0; i < n; i++) {
        printf("%s ", arr[i]);
    }
    printf("\n");
}

void demo_quicksort_with_duplicates(void) {
    printf("\n=== 快速排序（包含重复元素）演示 ===\n");
    
    int arr[] = {5, 2, 8, 2, 9, 1, 5, 3};
    size_t n = sizeof(arr) / sizeof(arr[0]);
    
    printf("排序前: ");
    for (size_t i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
    
    sort_utils_quicksort(arr, n, sizeof(int), int_compare);
    
    printf("排序后: ");
    for (size_t i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void demo_binary_search_edge_cases(void) {
    printf("\n=== 二分查找边界情况演示 ===\n");
    
    int arr[] = {1, 2, 3, 4, 5};
    size_t n = sizeof(arr) / sizeof(arr[0]);
    
    int first = 1;
    int last = 5;
    int middle = 3;
    int not_exist = 6;
    
    printf("数组: 1 2 3 4 5\n");
    
    int index = sort_binary_search(&first, arr, n, sizeof(int), int_compare);
    printf("查找第一个元素 %d: 索引 = %d\n", first, index);
    
    index = sort_binary_search(&last, arr, n, sizeof(int), int_compare);
    printf("查找最后一个元素 %d: 索引 = %d\n", last, index);
    
    index = sort_binary_search(&middle, arr, n, sizeof(int), int_compare);
    printf("查找中间元素 %d: 索引 = %d\n", middle, index);
    
    index = sort_binary_search(&not_exist, arr, n, sizeof(int), int_compare);
    printf("查找不存在的元素 %d: 索引 = %d (未找到)\n", not_exist, index);
}

int main(void) {
    printf("========================================\n");
    printf("    排序工具演示程序\n");
    printf("========================================\n");
    
    demo_quicksort();
    demo_binary_search();
    demo_quicksort_strings();
    demo_quicksort_with_duplicates();
    demo_binary_search_edge_cases();
    
    printf("\n=== 排序工具演示完成 ===\n");
    return 0;
}
