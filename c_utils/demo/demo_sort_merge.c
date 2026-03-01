#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sort_merge.h"

void demo_basic_merge_sort(void) {
    printf("\n=== 基本归并排序演示 ===\n");
    
    int arr[] = {12, 11, 13, 5, 6, 7};
    size_t n = sizeof(arr) / sizeof(arr[0]);
    
    printf("排序前: ");
    for (size_t i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
    
    sort_merge(arr, n);
    
    printf("排序后: ");
    for (size_t i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void demo_merge_sort_large_array(void) {
    printf("\n=== 归并排序（大数组）演示 ===\n");
    
    int arr[] = {64, 34, 25, 12, 22, 11, 90, 1, 5, 33, 77, 45, 88, 23, 56};
    size_t n = sizeof(arr) / sizeof(arr[0]);
    
    printf("排序前: ");
    for (size_t i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
    
    sort_merge(arr, n);
    
    printf("排序后: ");
    for (size_t i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void demo_merge_sort_with_duplicates(void) {
    printf("\n=== 归并排序（包含重复元素）演示 ===\n");
    
    int arr[] = {5, 2, 8, 2, 9, 1, 5, 3};
    size_t n = sizeof(arr) / sizeof(arr[0]);
    
    printf("排序前: ");
    for (size_t i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
    
    sort_merge(arr, n);
    
    printf("排序后: ");
    for (size_t i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void demo_merge_sort_edge_cases(void) {
    printf("\n=== 归并排序边界情况演示 ===\n");
    
    int empty_arr[] = {};
    printf("空数组: ");
    sort_merge(empty_arr, 0);
    printf("(无输出)\n");
    
    int single_arr[] = {42};
    printf("单元素数组: ");
    sort_merge(single_arr, 1);
    printf("%d\n", single_arr[0]);
    
    int two_arr[] = {2, 1};
    printf("两元素数组排序前: %d %d\n", two_arr[0], two_arr[1]);
    sort_merge(two_arr, 2);
    printf("两元素数组排序后: %d %d\n", two_arr[0], two_arr[1]);
}

void demo_merge_sort_stability(void) {
    printf("\n=== 归并排序稳定性演示 ===\n");
    
    int arr[] = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3};
    size_t n = sizeof(arr) / sizeof(arr[0]);
    
    printf("排序前: ");
    for (size_t i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
    
    sort_merge(arr, n);
    
    printf("排序后（稳定排序，相同元素保持原有顺序）: ");
    for (size_t i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int main(void) {
    printf("========================================\n");
    printf("    归并排序演示程序\n");
    printf("========================================\n");
    
    demo_basic_merge_sort();
    demo_merge_sort_large_array();
    demo_merge_sort_with_duplicates();
    demo_merge_sort_edge_cases();
    demo_merge_sort_stability();
    
    printf("\n=== 归并排序演示完成 ===\n");
    return 0;
}
