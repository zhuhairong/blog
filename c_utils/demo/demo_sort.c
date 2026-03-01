#include <stdio.h>
#include "../c_utils/sort.h"
#include "../c_utils/sort_heap.h"
#include "../c_utils/sort_merge.h"

// 比较函数
int compare_int(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

void print_array(int arr[], size_t n) {
    for (size_t i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int main() {
    printf("=== Sort Demo ===\n\n");
    
    // 测试数据
    int arr[] = {5, 2, 9, 1, 5, 6, 3, 8, 4, 7};
    size_t n = sizeof(arr) / sizeof(arr[0]);
    
    printf("原始数组: ");
    print_array(arr, n);
    
    // 测试快速排序
    printf("\n1. 快速排序:\n");
    int arr_quick[10] = {5, 2, 9, 1, 5, 6, 3, 8, 4, 7};
    sort_quicksort(arr_quick, n, sizeof(int), compare_int);
    printf("   排序后: ");
    print_array(arr_quick, n);
    
    // 测试堆排序
    printf("\n2. 堆排序:\n");
    int arr_heap[10] = {5, 2, 9, 1, 5, 6, 3, 8, 4, 7};
    sort_heap(arr_heap, n);
    printf("   排序后: ");
    print_array(arr_heap, n);
    
    // 测试归并排序
    printf("\n3. 归并排序:\n");
    int arr_merge[10] = {5, 2, 9, 1, 5, 6, 3, 8, 4, 7};
    sort_merge(arr_merge, n);
    printf("   排序后: ");
    print_array(arr_merge, n);
    
    printf("\n=== Sort Demo 完成 ===\n");
    return 0;
}