/**
 * @file demo_arena.c
 * @brief 内存池 (Arena) 分配器演示
 *
 * 本演示展示了 Arena 内存分配器的各种用法，包括：
 * - 基本内存分配
 * - 零初始化分配
 * - 内存重新分配
 * - 内存池重置
 * - 多 chunk 管理
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../c_utils/arena.h"

// 演示 1: 基本内存分配
static void demo_basic_allocation(void) {
    printf("\n=== 演示 1: 基本内存分配 ===\n");

    arena_t* arena = arena_create_default();
    if (!arena) {
        fprintf(stderr, "创建 arena 失败\n");
        return;
    }

    printf("Arena 创建成功\n");
    printf("初始 chunk 数: %zu\n", arena_chunk_count(arena));
    printf("总分配大小: %zu 字节\n", arena_total_allocated(arena));

    // 分配一些内存
    char* str1 = arena_alloc(arena, 64);
    strcpy(str1, "Hello, Arena!");
    printf("\n分配 64 字节: \"%s\"\n", str1);
    printf("总分配大小: %zu 字节\n", arena_total_allocated(arena));

    int* numbers = arena_alloc(arena, 10 * sizeof(int));
    for (int i = 0; i < 10; i++) {
        numbers[i] = i * i;
    }
    printf("\n分配 10 个整数:\n");
    for (int i = 0; i < 10; i++) {
        printf("  numbers[%d] = %d\n", i, numbers[i]);
    }
    printf("总分配大小: %zu 字节\n", arena_total_allocated(arena));

    arena_destroy(arena);
    printf("\nArena 已销毁\n");
}

// 演示 2: 零初始化分配
static void demo_zeroed_allocation(void) {
    printf("\n=== 演示 2: 零初始化分配 ===\n");

    arena_t* arena = arena_create(1024);
    if (!arena) {
        fprintf(stderr, "创建 arena 失败\n");
        return;
    }

    // 普通分配（未初始化）
    int* raw_data = arena_alloc(arena, 10 * sizeof(int));
    printf("普通分配（前 5 个值，未初始化）:\n");
    for (int i = 0; i < 5; i++) {
        printf("  raw_data[%d] = %d\n", i, raw_data[i]);
    }

    // 零初始化分配
    int* zeroed_data = arena_alloc_zeroed(arena, 10 * sizeof(int));
    printf("\n零初始化分配（前 5 个值）:\n");
    for (int i = 0; i < 5; i++) {
        printf("  zeroed_data[%d] = %d\n", i, zeroed_data[i]);
    }

    arena_destroy(arena);
}

// 演示 3: 内存重新分配
static void demo_reallocation(void) {
    printf("\n=== 演示 3: 内存重新分配 ===\n");

    arena_t* arena = arena_create_default();
    if (!arena) {
        fprintf(stderr, "创建 arena 失败\n");
        return;
    }

    // 初始分配
    size_t initial_size = 16;
    char* buffer = arena_alloc(arena, initial_size);
    strcpy(buffer, "Hello");
    printf("初始分配 %zu 字节: \"%s\"\n", initial_size, buffer);

    // 重新分配（扩大）
    size_t new_size = 64;
    buffer = arena_realloc(arena, buffer, initial_size, new_size);
    strcat(buffer, ", World! This is a longer string.");
    printf("重新分配到 %zu 字节: \"%s\"\n", new_size, buffer);

    printf("总分配大小: %zu 字节\n", arena_total_allocated(arena));

    arena_destroy(arena);
}

// 演示 4: Arena 重置
static void demo_reset(void) {
    printf("\n=== 演示 4: Arena 重置 ===\n");

    arena_t* arena = arena_create(256);
    if (!arena) {
        fprintf(stderr, "创建 arena 失败\n");
        return;
    }

    printf("初始 chunk 数: %zu\n", arena_chunk_count(arena));

    // 分配一些内存
    for (int i = 0; i < 5; i++) {
        char* ptr = arena_alloc(arena, 128);
        sprintf(ptr, "Block %d", i);
        printf("分配块 %d: \"%s\"\n", i, ptr);
    }

    printf("重置前 chunk 数: %zu\n", arena_chunk_count(arena));
    printf("重置前总分配: %zu 字节\n", arena_total_allocated(arena));

    // 重置 arena
    arena_reset(arena);
    printf("\nArena 已重置\n");
    printf("重置后 chunk 数: %zu\n", arena_chunk_count(arena));
    printf("重置后总分配: %zu 字节\n", arena_total_allocated(arena));

    // 重置后可以重新使用
    char* new_ptr = arena_alloc(arena, 64);
    strcpy(new_ptr, "Reused after reset!");
    printf("\n重置后分配: \"%s\"\n", new_ptr);

    arena_destroy(arena);
}

// 演示 5: 保留第一个 chunk 的重置
static void demo_reset_keep_first(void) {
    printf("\n=== 演示 5: 保留第一个 chunk 的重置 ===\n");

    arena_t* arena = arena_create(256);
    if (!arena) {
        fprintf(stderr, "创建 arena 失败\n");
        return;
    }

    // 分配一些内存以创建多个 chunk
    for (int i = 0; i < 10; i++) {
        arena_alloc(arena, 128);
    }

    printf("重置前 chunk 数: %zu\n", arena_chunk_count(arena));

    // 保留第一个 chunk 重置
    arena_reset_keep_first(arena);
    printf("保留第一个 chunk 重置后 chunk 数: %zu\n", arena_chunk_count(arena));

    arena_destroy(arena);
}

// 演示 6: 内存包含检查
static void demo_contains(void) {
    printf("\n=== 演示 6: 内存包含检查 ===\n");

    arena_t* arena = arena_create_default();
    if (!arena) {
        fprintf(stderr, "创建 arena 失败\n");
        return;
    }

    char* ptr1 = arena_alloc(arena, 64);
    char* ptr2 = arena_alloc(arena, 64);
    int external_var = 42;

    printf("ptr1 (%p) 在 arena 中: %s\n",
           (void*)ptr1, arena_contains(arena, ptr1) ? "是" : "否");
    printf("ptr2 (%p) 在 arena 中: %s\n",
           (void*)ptr2, arena_contains(arena, ptr2) ? "是" : "否");
    printf("external_var (%p) 在 arena 中: %s\n",
           (void*)&external_var, arena_contains(arena, &external_var) ? "是" : "否");

    arena_destroy(arena);
}

// 演示 7: 复杂数据结构分配
static void demo_complex_structures(void) {
    printf("\n=== 演示 7: 复杂数据结构分配 ===\n");

    typedef struct {
        int id;
        char name[32];
        double value;
    } item_t;

    typedef struct {
        item_t* items;
        int count;
        double total;
    } container_t;

    arena_t* arena = arena_create_default();
    if (!arena) {
        fprintf(stderr, "创建 arena 失败\n");
        return;
    }

    // 分配容器
    container_t* container = arena_alloc(arena, sizeof(container_t));
    container->count = 5;
    container->total = 0.0;

    // 分配 items 数组
    container->items = arena_alloc(arena, container->count * sizeof(item_t));

    // 初始化数据
    for (int i = 0; i < container->count; i++) {
        container->items[i].id = i + 1;
        sprintf(container->items[i].name, "Item_%d", i + 1);
        container->items[i].value = (i + 1) * 10.5;
        container->total += container->items[i].value;
    }

    // 打印数据
    printf("容器数据:\n");
    printf("  项目数: %d\n", container->count);
    printf("  总值: %.2f\n", container->total);
    printf("  项目列表:\n");
    for (int i = 0; i < container->count; i++) {
        printf("    [%d] ID=%d, Name=%s, Value=%.2f\n",
               i, container->items[i].id,
               container->items[i].name,
               container->items[i].value);
    }

    printf("\n总分配大小: %zu 字节\n", arena_total_allocated(arena));

    arena_destroy(arena);
}

// 演示 8: 多 Arena 使用场景
static void demo_multiple_arenas(void) {
    printf("\n=== 演示 8: 多 Arena 使用场景 ===\n");

    // 创建不同用途的 arena
    arena_t* temp_arena = arena_create(1024);      // 临时数据
    arena_t* persistent_arena = arena_create(4096); // 持久数据

    if (!temp_arena || !persistent_arena) {
        fprintf(stderr, "创建 arena 失败\n");
        if (temp_arena) arena_destroy(temp_arena);
        if (persistent_arena) arena_destroy(persistent_arena);
        return;
    }

    // 在临时 arena 中分配
    char* temp_buffer = arena_alloc(temp_arena, 256);
    sprintf(temp_buffer, "This is temporary data");
    printf("临时 arena: \"%s\"\n", temp_buffer);

    // 在持久 arena 中分配
    char* persistent_buffer = arena_alloc(persistent_arena, 256);
    sprintf(persistent_buffer, "This is persistent data");
    printf("持久 arena: \"%s\"\n", persistent_buffer);

    // 处理完成后重置临时 arena
    printf("\n重置临时 arena...\n");
    arena_reset(temp_arena);

    // 持久数据仍然有效
    printf("持久数据仍然有效: \"%s\"\n", persistent_buffer);

    arena_destroy(temp_arena);
    arena_destroy(persistent_arena);
}

int main(void) {
    printf("========================================\n");
    printf("    Arena 内存分配器演示程序\n");
    printf("========================================\n");

    demo_basic_allocation();
    demo_zeroed_allocation();
    demo_reallocation();
    demo_reset();
    demo_reset_keep_first();
    demo_contains();
    demo_complex_structures();
    demo_multiple_arenas();

    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("========================================\n");

    return 0;
}
