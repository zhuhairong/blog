#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sort_heap.h"

void demo_basic_heap_sort(void) {
    printf("\n=== 基本堆排序演示 ===\n");
    
    int arr[] = {12, 11, 13, 5, 6, 7};
    size_t n = sizeof(arr) / sizeof(arr[0]);
    
    printf("排序前: ");
    for (size_t i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
    
    sort_heap(arr, n);
    
    printf("排序后: ");
    for (size_t i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void demo_heap_sort_descending(void) {
    printf("\n=== 堆排序（降序）演示 ===\n");
    
    int arr[] = {4, 10, 3, 5, 1};
    size_t n = sizeof(arr) / sizeof(arr[0]);
    
    printf("排序前: ");
    for (size_t i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
    
    sort_heap(arr, n);
    
    printf("升序排序后: ");
    for (size_t i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
    
    printf("降序输出: ");
    for (int i = (int)n - 1; i >= 0; i--) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void demo_heap_sort_large_array(void) {
    printf("\n=== 堆排序（大数组）演示 ===\n");
    
    int arr[] = {64, 34, 25, 12, 22, 11, 90, 1, 5, 33, 77, 45, 88, 23, 56};
    size_t n = sizeof(arr) / sizeof(arr[0]);
    
    printf("排序前: ");
    for (size_t i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
    
    sort_heap(arr, n);
    
    printf("排序后: ");
    for (size_t i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void demo_heap_sort_with_duplicates(void) {
    printf("\n=== 堆排序（包含重复元素）演示 ===\n");
    
    int arr[] = {5, 2, 8, 2, 9, 1, 5, 3};
    size_t n = sizeof(arr) / sizeof(arr[0]);
    
    printf("排序前: ");
    for (size_t i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
    
    sort_heap(arr, n);
    
    printf("排序后: ");
    for (size_t i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void demo_heap_sort_edge_cases(void) {
    printf("\n=== 堆排序边界情况演示 ===\n");
    
    int empty_arr[] = {};
    printf("空数组: ");
    sort_heap(empty_arr, 0);
    printf("(无输出)\n");
    
    int single_arr[] = {42};
    printf("单元素数组: ");
    sort_heap(single_arr, 1);
    printf("%d\n", single_arr[0]);
    
    int two_arr[] = {2, 1};
    printf("两元素数组排序前: %d %d\n", two_arr[0], two_arr[1]);
    sort_heap(two_arr, 2);
    printf("两元素数组排序后: %d %d\n", two_arr[0], two_arr[1]);
}

int main(void) {
    printf("========================================\n");
    printf("    堆排序演示程序\n");
    printf("========================================\n");
    
    demo_basic_heap_sort();
    demo_heap_sort_descending();
    demo_heap_sort_large_array();
    demo_heap_sort_with_duplicates();
    demo_heap_sort_edge_cases();
    
    printf("\n=== 堆排序演示完成 ===\n");
    return 0;
}
