/**
 * 堆 (优先队列) 演示程序
 *
 * 功能：
 * - 最小堆和最大堆
 * - 基本堆操作
 * - 堆排序
 * - 批量操作
 */

#include <stdio.h>
#include <string.h>
#include "../c_utils/heap.h"

// 整数比较函数
static int int_compare_asc(const void *a, const void *b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return (ia > ib) - (ia < ib);
}

static int int_compare_desc(const void *a, const void *b) {
    return -int_compare_asc(a, b);
}

// 演示 1: 最小堆
static void demo_min_heap(void) {
    printf("\n=== 演示 1: 最小堆 ===\n");

    heap_error_t error;
    heap_config_t config = heap_default_config(HEAP_TYPE_MIN, sizeof(int), NULL);

    printf("创建最小堆...\n");
    heap_t *heap = heap_create(&config, &error);
    if (!heap) {
        printf("创建失败\n");
        return;
    }

    printf("\n插入元素: 50, 30, 20, 40, 10\n");
    int values[] = {50, 30, 20, 40, 10};
    for (int i = 0; i < 5; i++) {
        heap_push(heap, &values[i], &error);
        printf("  插入 %d\n", values[i]);
    }

    printf("\n堆大小: %zu\n", heap_size(heap));

    printf("\n弹出元素 (从小到大):\n");
    while (!heap_is_empty(heap)) {
        int val;
        heap_pop(heap, &val, &error);
        printf("  %d\n", val);
    }

    heap_free(heap);
}

// 演示 2: 最大堆
static void demo_max_heap(void) {
    printf("\n=== 演示 2: 最大堆 ===\n");

    heap_error_t error;
    heap_config_t config = heap_default_config(HEAP_TYPE_MAX, sizeof(int), NULL);

    printf("创建最大堆...\n");
    heap_t *heap = heap_create(&config, &error);
    if (!heap) {
        printf("创建失败\n");
        return;
    }

    printf("\n插入元素: 25, 15, 35, 5, 45\n");
    int values[] = {25, 15, 35, 5, 45};
    for (int i = 0; i < 5; i++) {
        heap_push(heap, &values[i], &error);
    }

    printf("\n弹出元素 (从大到小):\n");
    while (!heap_is_empty(heap)) {
        int val;
        heap_pop(heap, &val, &error);
        printf("  %d\n", val);
    }

    heap_free(heap);
}

// 演示 3: 堆排序
static void demo_heap_sort(void) {
    printf("\n=== 演示 3: 堆排序 ===\n");

    heap_error_t error;

    int data[] = {64, 34, 25, 12, 22, 11, 90, 5};
    int n = sizeof(data) / sizeof(data[0]);

    printf("原始数组:\n  ");
    for (int i = 0; i < n; i++) {
        printf("%d ", data[i]);
    }
    printf("\n");

    // 升序排序
    heap_config_t config = heap_default_config(HEAP_TYPE_MIN, sizeof(int), NULL);
    heap_sort(data, n, sizeof(int), &config, &error);

    printf("升序排序后:\n  ");
    for (int i = 0; i < n; i++) {
        printf("%d ", data[i]);
    }
    printf("\n");

    // 降序排序
    config = heap_default_config(HEAP_TYPE_MAX, sizeof(int), NULL);
    heap_sort(data, n, sizeof(int), &config, &error);

    printf("降序排序后:\n  ");
    for (int i = 0; i < n; i++) {
        printf("%d ", data[i]);
    }
    printf("\n");
}

// 演示 4: 批量操作
static void demo_batch(void) {
    printf("\n=== 演示 4: 批量操作 ===\n");

    heap_error_t error;
    heap_config_t config = heap_default_config(HEAP_TYPE_MIN, sizeof(int), NULL);
    config.capacity = 10;

    heap_t *heap = heap_create(&config, &error);

    printf("批量插入 10 个元素:\n");
    int values[] = {9, 7, 5, 3, 1, 2, 4, 6, 8, 0};
    heap_push_batch(heap, values, 10, &error);
    printf("  插入完成，堆大小: %zu\n", heap_size(heap));

    printf("\n批量弹出 5 个元素:\n");
    int out[5];
    size_t popped;
    heap_pop_batch(heap, out, 5, &popped, &error);
    printf("  弹出 %zu 个元素: ", popped);
    for (size_t i = 0; i < popped; i++) {
        printf("%d ", out[i]);
    }
    printf("\n");

    heap_free(heap);
}

// 演示 5: 查看顶部元素
static void demo_peek(void) {
    printf("\n=== 演示 5: 查看顶部元素 ===\n");

    heap_error_t error;
    heap_config_t config = heap_default_config(HEAP_TYPE_MIN, sizeof(int), NULL);
    heap_t *heap = heap_create(&config, &error);

    int values[] = {42, 17, 89, 5, 33};
    for (int i = 0; i < 5; i++) {
        heap_push(heap, &values[i], &error);
    }

    printf("堆中元素: 42, 17, 89, 5, 33\n");

    int *top = (int*)heap_peek(heap, &error);
    if (top) {
        printf("\n顶部元素 (最小值): %d\n", *top);
    }

    printf("\npeek 后堆大小不变: %zu\n", heap_size(heap));

    heap_free(heap);
}

// 演示 6: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 6: 应用场景 ===\n");

    printf("1. 优先队列\n");
    printf("   - 任务调度 (优先级)\n");
    printf("   - 事件处理 (时间戳)\n");
    printf("\n");

    printf("2. Top K 问题\n");
    printf("   - 最大 K 个元素\n");
    printf("   - 最小 K 个元素\n");
    printf("\n");

    printf("3. 合并有序序列\n");
    printf("   - 多路归并排序\n");
    printf("   - 合并多个日志文件\n");
    printf("\n");

    printf("4. 图算法\n");
    printf("   - Dijkstra 最短路径\n");
    printf("   - Prim 最小生成树\n");
    printf("\n");

    printf("5. 中位数查找\n");
    printf("   - 双堆法\n");
    printf("   - 动态数据流\n");
}

// 演示 7: 复杂度分析
static void demo_complexity(void) {
    printf("\n=== 演示 7: 复杂度分析 ===\n");

    printf("堆操作复杂度:\n\n");

    printf("时间复杂度:\n");
    printf("  插入 (push):     O(log n)\n");
    printf("  弹出 (pop):      O(log n)\n");
    printf("  查看 (peek):     O(1)\n");
    printf("  建堆:            O(n)\n");
    printf("  堆排序:          O(n log n)\n");
    printf("\n");

    printf("空间复杂度:\n");
    printf("  O(n) - 存储 n 个元素\n");
    printf("\n");

    printf("特点:\n");
    printf("  - 原地排序 (堆排序)\n");
    printf("  - 不稳定排序\n");
    printf("  - 适合部分有序数据\n");
}

int main(void) {
    printf("========================================\n");
    printf("    堆 (优先队列) 演示\n");
    printf("========================================\n");

    demo_min_heap();
    demo_max_heap();
    demo_heap_sort();
    demo_batch();
    demo_peek();
    demo_applications();
    demo_complexity();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
