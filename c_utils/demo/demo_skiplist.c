#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/skiplist.h"

// 比较函数
int compare_int(const void *a, const void *b) {
    int *ia = (int *)a;
    int *ib = (int *)b;
    return (*ia) - (*ib);
}

// 简单打印跳表内容（使用基本操作）
void print_skiplist(skiplist_t *sl) {
    printf("   跳表内容: ");
    // 由于没有迭代器，我们手动打印一些已知键的元素
    int keys[] = {5, 10, 15, 20, 25, 30, 35, 40, 45, 50};
    size_t num_keys = sizeof(keys) / sizeof(keys[0]);
    bool first = true;
    
    for (size_t i = 0; i < num_keys; i++) {
        void *value = skiplist_get(sl, &keys[i]);
        if (value) {
            if (!first) {
                printf(", ");
            }
            printf("%d: %s", keys[i], (char *)value);
            first = false;
        }
    }
    printf("\n");
}

int main() {
    printf("=== Skip List Demo ===\n\n");
    
    // 创建跳表
    printf("1. 创建跳表:\n");
    skiplist_t *sl = skiplist_create(compare_int);
    if (sl) {
        printf("   跳表创建成功\n");
    } else {
        printf("   跳表创建失败\n");
        return 1;
    }
    
    // 插入元素
    printf("\n2. 插入元素:\n");
    int keys[] = {10, 20, 30, 15, 25, 5, 35};
    char *values[] = {"Value10", "Value20", "Value30", "Value15", "Value25", "Value5", "Value35"};
    size_t num_elements = sizeof(keys) / sizeof(keys[0]);
    
    for (size_t i = 0; i < num_elements; i++) {
        skiplist_insert(sl, &keys[i], values[i]);
        printf("   插入: %d -> %s\n", keys[i], values[i]);
    }
    
    // 打印跳表
    print_skiplist(sl);
    
    // 查找元素
    printf("\n3. 查找元素:\n");
    int search_keys[] = {20, 15, 40};
    size_t num_searches = sizeof(search_keys) / sizeof(search_keys[0]);
    
    for (size_t i = 0; i < num_searches; i++) {
        void *value = skiplist_get(sl, &search_keys[i]);
        if (value) {
            printf("   查找 %d: 找到 -> %s\n", search_keys[i], (char *)value);
        } else {
            printf("   查找 %d: 未找到\n", search_keys[i]);
        }
    }
    
    // 删除元素
    printf("\n4. 删除元素:\n");
    int delete_keys[] = {15, 30};
    size_t num_deletes = sizeof(delete_keys) / sizeof(delete_keys[0]);
    
    for (size_t i = 0; i < num_deletes; i++) {
        skiplist_delete(sl, &delete_keys[i]);
        printf("   删除: %d\n", delete_keys[i]);
    }
    
    // 打印跳表
    print_skiplist(sl);
    
    // 检查元素是否存在（使用查找代替）
    printf("\n5. 检查元素是否存在:\n");
    int check_keys[] = {20, 15};
    size_t num_checks = sizeof(check_keys) / sizeof(check_keys[0]);
    
    for (size_t i = 0; i < num_checks; i++) {
        void *value = skiplist_get(sl, &check_keys[i]);
        printf("   %d 是否存在: %s\n", check_keys[i], value ? "是" : "否");
    }
    
    // 批量插入（使用单个插入代替）
    printf("\n6. 批量插入:\n");
    int batch_keys[] = {40, 45, 50};
    char *batch_values[] = {"Value40", "Value45", "Value50"};
    size_t batch_count = sizeof(batch_keys) / sizeof(batch_keys[0]);
    
    for (size_t i = 0; i < batch_count; i++) {
        skiplist_insert(sl, &batch_keys[i], batch_values[i]);
        printf("   插入: %d -> %s\n", batch_keys[i], batch_values[i]);
    }
    printf("   批量插入完成\n");
    
    // 打印跳表
    print_skiplist(sl);
    
    // 释放资源
    skiplist_free(sl);
    
    printf("\n=== Skip List Demo 完成 ===\n");
    return 0;
}