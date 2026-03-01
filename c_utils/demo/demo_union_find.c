#include <stdio.h>
#include "../c_utils/union_find.h"

int main() {
    printf("=== Union Find Demo ===\n\n");
    
    // 创建并查集
    printf("1. 创建并查集:\n");
    size_t n = 10;
    union_find_t *uf = uf_create(n);
    if (uf) {
        printf("   并查集创建成功，大小: %zu\n", n);
    } else {
        printf("   并查集创建失败\n");
        return 1;
    }
    
    // 测试初始状态
    printf("\n2. 初始状态测试:\n");
    for (int i = 0; i < n; i++) {
        printf("   元素 %d 的根: %d\n", i, uf_find(uf, i));
    }
    
    // 测试合并操作
    printf("\n3. 合并操作测试:\n");
    uf_union(uf, 0, 1);
    printf("   合并 0 和 1\n");
    
    uf_union(uf, 2, 3);
    printf("   合并 2 和 3\n");
    
    uf_union(uf, 4, 5);
    printf("   合并 4 和 5\n");
    
    uf_union(uf, 6, 7);
    printf("   合并 6 和 7\n");
    
    uf_union(uf, 8, 9);
    printf("   合并 8 和 9\n");
    
    // 测试合并后的状态
    printf("\n4. 合并后状态测试:\n");
    for (int i = 0; i < n; i++) {
        printf("   元素 %d 的根: %d\n", i, uf_find(uf, i));
    }
    
    // 测试连接性
    printf("\n5. 连接性测试:\n");
    printf("   0 和 1 是否连接: %s\n", uf_connected(uf, 0, 1) ? "是" : "否");
    printf("   2 和 3 是否连接: %s\n", uf_connected(uf, 2, 3) ? "是" : "否");
    printf("   0 和 2 是否连接: %s\n", uf_connected(uf, 0, 2) ? "是" : "否");
    
    // 测试更多合并
    printf("\n6. 更多合并操作:\n");
    uf_union(uf, 0, 2);
    printf("   合并 0 和 2 (合并集合 {0,1} 和 {2,3})\n");
    
    uf_union(uf, 4, 6);
    printf("   合并 4 和 6 (合并集合 {4,5} 和 {6,7})\n");
    
    // 测试合并后的连接性
    printf("\n7. 合并后连接性测试:\n");
    printf("   0 和 2 是否连接: %s\n", uf_connected(uf, 0, 2) ? "是" : "否");
    printf("   1 和 3 是否连接: %s\n", uf_connected(uf, 1, 3) ? "是" : "否");
    printf("   4 和 6 是否连接: %s\n", uf_connected(uf, 4, 6) ? "是" : "否");
    printf("   5 和 7 是否连接: %s\n", uf_connected(uf, 5, 7) ? "是" : "否");
    
    // 测试最终状态
    printf("\n8. 最终状态测试:\n");
    for (int i = 0; i < n; i++) {
        printf("   元素 %d 的根: %d\n", i, uf_find(uf, i));
    }
    
    // 释放并查集
    uf_free(uf);
    printf("\n9. 并查集已释放\n");
    
    // 测试边界情况
    printf("\n10. 边界情况测试:\n");
    union_find_t *uf_small = uf_create(1);
    if (uf_small) {
        printf("   大小为 1 的并查集创建成功\n");
        printf("   元素 0 的根: %d\n", uf_find(uf_small, 0));
        uf_free(uf_small);
        printf("   大小为 1 的并查集已释放\n");
    }
    
    printf("\n=== Union Find Demo 完成 ===\n");
    return 0;
}