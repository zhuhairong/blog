#include <stdio.h>
#include "../c_utils/vec.h"

int main() {
    printf("=== Vec Demo ===\n\n");
    
    // 创建vec（存储int类型）
    printf("1. 创建vec (int类型):\n");
    vec_t *vec = vec_create(sizeof(int));
    if (vec) {
        printf("   Vec创建成功\n");
    } else {
        printf("   Vec创建失败\n");
        return 1;
    }
    
    // 测试初始状态
    printf("\n2. 初始状态测试:\n");
    printf("   大小: %zu\n", vec_size(vec));
    printf("   容量: %zu\n", vec_capacity(vec));
    printf("   是否为空: %s\n", vec_is_empty(vec) ? "是" : "否");
    
    // 测试push操作
    printf("\n3. 测试push操作:\n");
    int values[] = {10, 20, 30, 40, 50};
    size_t num_values = sizeof(values) / sizeof(values[0]);
    
    for (size_t i = 0; i < num_values; i++) {
        if (vec_push(vec, &values[i])) {
            printf("   push %d 成功\n", values[i]);
        } else {
            printf("   push %d 失败\n", values[i]);
        }
    }
    
    // 测试状态
    printf("\n4. 操作后状态测试:\n");
    printf("   大小: %zu\n", vec_size(vec));
    printf("   容量: %zu\n", vec_capacity(vec));
    printf("   是否为空: %s\n", vec_is_empty(vec) ? "是" : "否");
    
    // 测试访问元素
    printf("\n5. 测试访问元素:\n");
    for (size_t i = 0; i < vec_size(vec); i++) {
        int *value = (int *)vec_at(vec, i);
        if (value) {
            printf("   vec[%zu] = %d\n", i, *value);
        }
    }
    
    // 测试insert操作
    printf("\n6. 测试insert操作:\n");
    int insert_value = 25;
    if (vec_insert(vec, 2, &insert_value)) {
        printf("   在索引 2 插入 %d 成功\n", insert_value);
    } else {
        printf("   在索引 2 插入 %d 失败\n", insert_value);
    }
    
    // 测试插入后的状态
    printf("\n7. 插入后状态测试:\n");
    printf("   大小: %zu\n", vec_size(vec));
    for (size_t i = 0; i < vec_size(vec); i++) {
        int *value = (int *)vec_at(vec, i);
        if (value) {
            printf("   vec[%zu] = %d\n", i, *value);
        }
    }
    
    // 测试remove操作
    printf("\n8. 测试remove操作:\n");
    if (vec_remove(vec, 4)) {
        printf("   删除索引 4 的元素成功\n");
    } else {
        printf("   删除索引 4 的元素失败\n");
    }
    
    // 测试删除后的状态
    printf("\n9. 删除后状态测试:\n");
    printf("   大小: %zu\n", vec_size(vec));
    for (size_t i = 0; i < vec_size(vec); i++) {
        int *value = (int *)vec_at(vec, i);
        if (value) {
            printf("   vec[%zu] = %d\n", i, *value);
        }
    }
    
    // 测试pop操作
    printf("\n10. 测试pop操作:\n");
    int pop_value;
    if (vec_pop(vec, &pop_value)) {
        printf("   pop 成功，值: %d\n", pop_value);
    } else {
        printf("   pop 失败\n");
    }
    
    // 测试pop后的状态
    printf("\n11. pop后状态测试:\n");
    printf("   大小: %zu\n", vec_size(vec));
    for (size_t i = 0; i < vec_size(vec); i++) {
        int *value = (int *)vec_at(vec, i);
        if (value) {
            printf("   vec[%zu] = %d\n", i, *value);
        }
    }
    
    // 测试clear操作
    printf("\n12. 测试clear操作:\n");
    vec_clear(vec);
    printf("   清空后大小: %zu\n", vec_size(vec));
    printf("   清空后是否为空: %s\n", vec_is_empty(vec) ? "是" : "否");
    
    // 测试存储不同类型（存储double类型）
    printf("\n13. 测试存储double类型:\n");
    vec_t *vec_double = vec_create(sizeof(double));
    if (vec_double) {
        printf("   Double Vec创建成功\n");
        
        double double_values[] = {1.1, 2.2, 3.3, 4.4, 5.5};
        size_t num_double_values = sizeof(double_values) / sizeof(double_values[0]);
        
        for (size_t i = 0; i < num_double_values; i++) {
            if (vec_push(vec_double, &double_values[i])) {
                printf("   push %f 成功\n", double_values[i]);
            }
        }
        
        printf("   Double Vec大小: %zu\n", vec_size(vec_double));
        for (size_t i = 0; i < vec_size(vec_double); i++) {
            double *value = (double *)vec_at(vec_double, i);
            if (value) {
                printf("   vec[%zu] = %f\n", i, *value);
            }
        }
        
        vec_free(vec_double);
        printf("   Double Vec已释放\n");
    }
    
    // 释放vec
    vec_free(vec);
    printf("\n14. Vec已释放\n");
    
    printf("\n=== Vec Demo 完成 ===\n");
    return 0;
}