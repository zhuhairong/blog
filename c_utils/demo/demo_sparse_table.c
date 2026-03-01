#include <stdio.h>
#include "../c_utils/sparse_table.h"

int main() {
    printf("=== Sparse Table Demo ===\n\n");
    
    // 测试数据
    int arr[] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19};
    size_t n = sizeof(arr) / sizeof(arr[0]);
    
    // 测试基本稀疏表
    printf("1. 基本稀疏表初始化和查询:\n");
    printf("   原始数组: ");
    for (size_t i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
    
    sparse_table_t st;
    sparse_table_init(&st, arr, n);
    
    // 测试查询
    printf("   查询区间 [0, 4]: %d\n", sparse_table_query(&st, 0, 4));
    printf("   查询区间 [2, 7]: %d\n", sparse_table_query(&st, 2, 7));
    printf("   查询区间 [5, 9]: %d\n", sparse_table_query(&st, 5, 9));
    printf("   查询区间 [0, 9]: %d\n", sparse_table_query(&st, 0, 9));
    printf("   查询区间 [3, 3]: %d\n", sparse_table_query(&st, 3, 3));
    
    // 测试不同的数组
    printf("\n2. 测试不同的数组:\n");
    int arr2[] = {10, 8, 6, 4, 2, 0, -2, -4, -6, -8};
    size_t n2 = sizeof(arr2) / sizeof(arr2[0]);
    
    printf("   原始数组: ");
    for (size_t i = 0; i < n2; i++) {
        printf("%d ", arr2[i]);
    }
    printf("\n");
    
    sparse_table_t st2;
    sparse_table_init(&st2, arr2, n2);
    
    printf("   查询区间 [0, 4]: %d\n", sparse_table_query(&st2, 0, 4));
    printf("   查询区间 [2, 7]: %d\n", sparse_table_query(&st2, 2, 7));
    printf("   查询区间 [5, 9]: %d\n", sparse_table_query(&st2, 5, 9));
    
    // 释放资源
    sparse_table_free(&st);
    sparse_table_free(&st2);
    
    printf("\n=== Sparse Table Demo 完成 ===\n");
    return 0;
}