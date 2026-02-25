/**
 * 双向链表演示程序
 *
 * 功能：
 * - 基本链表操作
 * - 遍历和查找
 * - 排序和反转
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/list.h"

// 演示 1: 基本操作
static void demo_basic(void) {
    printf("\n=== 演示 1: 基本操作 ===\n");

    printf("创建链表...\n");
    list_t *list = list_create();

    printf("\n添加元素 (1, 2, 3):\n");
    int *v1 = malloc(sizeof(int)); *v1 = 1;
    int *v2 = malloc(sizeof(int)); *v2 = 2;
    int *v3 = malloc(sizeof(int)); *v3 = 3;

    list_push_back(list, v1);
    list_push_back(list, v2);
    list_push_back(list, v3);

    printf("  链表大小: %zu\n", list_size(list));

    printf("\n访问元素:\n");
    printf("  头部: %d\n", *(int*)list_front(list));
    printf("  尾部: %d\n", *(int*)list_back(list));
    printf("  索引 1: %d\n", *(int*)list_at(list, 1));

    printf("\n弹出头部:\n");
    int *front = (int*)list_pop_front(list);
    printf("  弹出值: %d\n", *front);
    printf("  剩余大小: %zu\n", list_size(list));
    free(front);

    printf("\n弹出尾部:\n");
    int *back = (int*)list_pop_back(list);
    printf("  弹出值: %d\n", *back);
    printf("  剩余大小: %zu\n", list_size(list));
    free(back);

    // 清理
    while (!list_is_empty(list)) {
        free(list_pop_front(list));
    }
    list_free(list);
}

// 演示 2: 遍历操作
static void demo_iteration(void) {
    printf("\n=== 演示 2: 遍历操作 ===\n");

    list_t *list = list_create();

    // 添加元素
    for (int i = 1; i <= 5; i++) {
        int *v = malloc(sizeof(int));
        *v = i * 10;
        list_push_back(list, v);
    }

    printf("正向遍历:\n  ");
    list_foreach(list, node) {
        printf("%d ", *(int*)node->data);
    }
    printf("\n");

    printf("反向遍历:\n  ");
    list_foreach_reverse(list, node) {
        printf("%d ", *(int*)node->data);
    }
    printf("\n");

    // 清理
    list_clear_with_data(list, free);
    list_free(list);
}

// 演示 3: 插入和删除
static void demo_insert_remove(void) {
    printf("\n=== 演示 3: 插入和删除 ===\n");

    list_t *list = list_create();

    // 初始: 1, 3, 5
    int *v1 = malloc(sizeof(int)); *v1 = 1;
    int *v3 = malloc(sizeof(int)); *v3 = 3;
    int *v5 = malloc(sizeof(int)); *v5 = 5;
    list_push_back(list, v1);
    list_push_back(list, v3);
    list_push_back(list, v5);

    printf("初始链表: 1, 3, 5\n");

    // 在索引 1 处插入 2
    int *v2 = malloc(sizeof(int)); *v2 = 2;
    list_insert(list, 1, v2);
    printf("插入 2 到位置 1: ");
    list_foreach(list, node) {
        printf("%d ", *(int*)node->data);
    }
    printf("\n");

    // 删除索引 2 (值为 3)
    list_remove(list, 2);
    printf("删除位置 2: ");
    list_foreach(list, node) {
        printf("%d ", *(int*)node->data);
    }
    printf("\n");

    // 清理
    list_clear_with_data(list, free);
    list_free(list);
}

// 整数比较函数
static int int_compare(const void *a, const void *b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return ia - ib;
}

// 演示 4: 查找和排序
static void demo_find_sort(void) {
    printf("\n=== 演示 4: 查找和排序 ===\n");

    list_t *list = list_create();

    // 添加无序元素
    int values[] = {30, 10, 50, 20, 40};
    for (int i = 0; i < 5; i++) {
        int *v = malloc(sizeof(int));
        *v = values[i];
        list_push_back(list, v);
    }

    printf("初始链表: ");
    list_foreach(list, node) {
        printf("%d ", *(int*)node->data);
    }
    printf("\n");

    // 查找
    int target = 30;
    list_node_t *found = list_find(list, &target, int_compare);
    if (found) {
        printf("找到 %d: %s\n", target, *(int*)found->data == target ? "是" : "否");
    }

    // 排序
    list_sort(list, int_compare);
    printf("排序后: ");
    list_foreach(list, node) {
        printf("%d ", *(int*)node->data);
    }
    printf("\n");

    // 反转
    list_reverse(list);
    printf("反转后: ");
    list_foreach(list, node) {
        printf("%d ", *(int*)node->data);
    }
    printf("\n");

    // 清理
    list_clear_with_data(list, free);
    list_free(list);
}

// 演示 5: 链表 vs 数组
static void demo_comparison(void) {
    printf("\n=== 演示 5: 链表 vs 数组 ===\n");

    printf("链表优势:\n");
    printf("  - 动态大小\n");
    printf("  - O(1) 插入/删除 (已知位置)\n");
    printf("  - 无需预分配空间\n");
    printf("  - 内存利用率高\n\n");

    printf("链表劣势:\n");
    printf("  - O(n) 随机访问\n");
    printf("  - 额外内存开销 (指针)\n");
    printf("  - 缓存不友好\n\n");

    printf("数组优势:\n");
    printf("  - O(1) 随机访问\n");
    printf("  - 缓存友好\n");
    printf("  - 内存连续\n\n");

    printf("数组劣势:\n");
    printf("  - 固定大小或重新分配\n");
    printf("  - O(n) 插入/删除\n");
}

// 演示 6: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 6: 应用场景 ===\n");

    printf("1. 实现其他数据结构\n");
    printf("   - 栈 (Stack)\n");
    printf("   - 队列 (Queue)\n");
    printf("   - 哈希表 (链地址法)\n");
    printf("\n");

    printf("2. 内存管理\n");
    printf("   - 内存池\n");
    printf("   - 空闲块管理\n");
    printf("\n");

    printf("3. 操作系统\n");
    printf("   - 进程调度\n");
    printf("   - 文件系统\n");
    printf("\n");

    printf("4. 图算法\n");
    printf("   - 邻接表\n");
    printf("   - 路径记录\n");
}

int main(void) {
    printf("========================================\n");
    printf("    双向链表演示\n");
    printf("========================================\n");

    demo_basic();
    demo_iteration();
    demo_insert_remove();
    demo_find_sort();
    demo_comparison();
    demo_applications();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
