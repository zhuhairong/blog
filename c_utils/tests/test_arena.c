#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/arena.h"

void test_arena_create() {
    TEST(Arena_Create);
    arena_t* arena = arena_create(1024);
    EXPECT_TRUE(arena != NULL);
    arena_destroy(arena);
}

void test_arena_create_default() {
    TEST(Arena_CreateDefault);
    arena_t* arena = arena_create_default();
    EXPECT_TRUE(arena != NULL);
    arena_destroy(arena);
}

void test_arena_alloc() {
    TEST(Arena_Alloc);
    arena_t* arena = arena_create(1024);
    
    void* ptr = arena_alloc(arena, 100);
    EXPECT_TRUE(ptr != NULL);
    
    memset(ptr, 0xAB, 100);
    
    arena_destroy(arena);
}

void test_arena_alloc_zeroed() {
    TEST(Arena_AllocZeroed);
    arena_t* arena = arena_create(1024);
    
    void* ptr = arena_alloc_zeroed(arena, 100);
    EXPECT_TRUE(ptr != NULL);
    
    unsigned char* bytes = (unsigned char*)ptr;
    for (int i = 0; i < 100; i++) {
        EXPECT_EQ(bytes[i], 0);
    }
    
    arena_destroy(arena);
}

void test_arena_alloc_multiple() {
    TEST(Arena_AllocMultiple);
    arena_t* arena = arena_create(1024);
    
    void* ptr1 = arena_alloc(arena, 100);
    void* ptr2 = arena_alloc(arena, 200);
    void* ptr3 = arena_alloc(arena, 300);
    
    EXPECT_TRUE(ptr1 != NULL);
    EXPECT_TRUE(ptr2 != NULL);
    EXPECT_TRUE(ptr3 != NULL);
    EXPECT_TRUE(ptr1 != ptr2);
    EXPECT_TRUE(ptr2 != ptr3);
    
    arena_destroy(arena);
}

void test_arena_realloc() {
    TEST(Arena_Realloc);
    arena_t* arena = arena_create(1024);
    
    void* ptr = arena_alloc(arena, 100);
    EXPECT_TRUE(ptr != NULL);
    
    memset(ptr, 0xAB, 100);
    
    void* new_ptr = arena_realloc(arena, ptr, 100, 200);
    EXPECT_TRUE(new_ptr != NULL);
    
    arena_destroy(arena);
}

void test_arena_reset() {
    TEST(Arena_Reset);
    arena_t* arena = arena_create(1024);
    
    arena_alloc(arena, 100);
    arena_alloc(arena, 200);
    
    size_t before = arena_total_allocated(arena);
    EXPECT_TRUE(before > 0);
    
    arena_reset(arena);
    
    void* ptr = arena_alloc(arena, 50);
    EXPECT_TRUE(ptr != NULL);
    
    arena_destroy(arena);
}

void test_arena_reset_keep_first() {
    TEST(Arena_ResetKeepFirst);
    arena_t* arena = arena_create(1024);
    
    void* ptr1 = arena_alloc(arena, 100);
    memset(ptr1, 0xAB, 100);
    
    arena_alloc(arena, 200);
    
    arena_reset_keep_first(arena);
    
    void* ptr2 = arena_alloc(arena, 50);
    EXPECT_TRUE(ptr2 != NULL);
    
    arena_destroy(arena);
}

void test_arena_contains() {
    TEST(Arena_Contains);
    arena_t* arena = arena_create(1024);
    
    void* ptr = arena_alloc(arena, 100);
    
    EXPECT_TRUE(arena_contains(arena, ptr));
    EXPECT_FALSE(arena_contains(arena, (void*)0xDEADBEEF));
    
    arena_destroy(arena);
}

void test_arena_total_allocated() {
    TEST(Arena_TotalAllocated);
    arena_t* arena = arena_create(1024);
    
    size_t initial = arena_total_allocated(arena);
    
    arena_alloc(arena, 100);
    arena_alloc(arena, 200);
    
    size_t after = arena_total_allocated(arena);
    EXPECT_TRUE(after >= initial + 300);
    
    arena_destroy(arena);
}

void test_arena_chunk_count() {
    TEST(Arena_ChunkCount);
    arena_t* arena = arena_create(100);
    
    size_t initial = arena_chunk_count(arena);
    
    for (int i = 0; i < 20; i++) {
        arena_alloc(arena, 50);
    }
    
    size_t after = arena_chunk_count(arena);
    EXPECT_TRUE(after >= initial);
    
    arena_destroy(arena);
}

void test_arena_large_allocation() {
    TEST(Arena_LargeAllocation);
    arena_t* arena = arena_create(100);
    
    void* ptr = arena_alloc(arena, 10000);
    EXPECT_TRUE(ptr != NULL);
    
    arena_destroy(arena);
}

void test_arena_stress_many_allocations() {
    TEST(Arena_StressManyAllocations);
    arena_t* arena = arena_create(4096);
    
    for (int i = 0; i < 1000; i++) {
        void* ptr = arena_alloc(arena, 64);
        EXPECT_TRUE(ptr != NULL);
        memset(ptr, i, 64);
    }
    
    arena_destroy(arena);
}

void test_arena_destroy_null() {
    TEST(Arena_DestroyNull);
    arena_destroy(NULL);
}

void test_arena_edge_case_zero_size() {
    TEST(Arena_EdgeCaseZeroSize);
    arena_t* arena = arena_create(1024);
    
    void* ptr = arena_alloc(arena, 0);
    
    arena_destroy(arena);
}

int main() {
    test_arena_create();
    test_arena_create_default();
    test_arena_alloc();
    test_arena_alloc_zeroed();
    test_arena_alloc_multiple();
    test_arena_realloc();
    test_arena_reset();
    test_arena_reset_keep_first();
    test_arena_contains();
    test_arena_total_allocated();
    test_arena_chunk_count();
    test_arena_large_allocation();
    test_arena_stress_many_allocations();
    test_arena_destroy_null();
    test_arena_edge_case_zero_size();

    return 0;
}
