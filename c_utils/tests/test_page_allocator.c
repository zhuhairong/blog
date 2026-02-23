#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/page_allocator.h"

void test_page_alloc_default_config() {
    TEST(PageAlloc_DefaultConfig);
    page_alloc_config_t config = page_alloc_default_config();
    EXPECT_EQ(config.alignment, (size_t)0);
    EXPECT_TRUE(config.zero_initialize);
    EXPECT_EQ(config.min_size, (size_t)1);
    EXPECT_EQ(config.max_size, (size_t)0);
}

void test_page_alloc_get_page_size() {
    TEST(PageAlloc_GetPageSize);
    size_t page_size = page_alloc_get_page_size();
    EXPECT_TRUE(page_size > 0);
    EXPECT_TRUE((page_size & (page_size - 1)) == 0);
}

void test_page_alloc() {
    TEST(PageAlloc);
    void* ptr = page_alloc(1024);
    EXPECT_TRUE(ptr != NULL);
    EXPECT_TRUE(page_is_aligned(ptr, 0));
    page_free(ptr);
}

void test_page_alloc_zero_size() {
    TEST(PageAlloc_ZeroSize);
    void* ptr = page_alloc(0);
    EXPECT_TRUE(ptr == NULL);
}

void test_page_alloc_ex() {
    TEST(PageAlloc_Ex);
    page_alloc_config_t config = page_alloc_default_config();
    page_alloc_error_t error;
    void* ptr = page_alloc_ex(1024, &config, &error);
    EXPECT_TRUE(ptr != NULL);
    EXPECT_EQ(error, PAGE_ALLOC_OK);
    page_free(ptr);
}

void test_page_alloc_ex_zero_initialize() {
    TEST(PageAlloc_ExZeroInitialize);
    page_alloc_config_t config = page_alloc_default_config();
    config.zero_initialize = true;
    page_alloc_error_t error;
    char* ptr = (char*)page_alloc_ex(1024, &config, &error);
    EXPECT_TRUE(ptr != NULL);
    for (int i = 0; i < 1024; i++) {
        EXPECT_EQ(ptr[i], (char)0);
    }
    page_free(ptr);
}

void test_page_free_ex() {
    TEST(PageAlloc_FreeEx);
    void* ptr = page_alloc(1024);
    page_alloc_error_t error;
    bool success = page_free_ex(ptr, &error);
    EXPECT_TRUE(success);
    EXPECT_EQ(error, PAGE_ALLOC_OK);
}

void test_page_free_ex_null() {
    TEST(PageAlloc_FreeExNull);
    page_alloc_error_t error;
    bool success = page_free_ex(NULL, &error);
    EXPECT_FALSE(success);
    EXPECT_EQ(error, PAGE_ALLOC_ERROR_NULL_PTR);
}

void test_page_alloc_array() {
    TEST(PageAlloc_Array);
    void* ptrs[5];
    page_alloc_error_t error;
    bool success = page_alloc_array(5, 1024, ptrs, &error);
    EXPECT_TRUE(success);
    EXPECT_EQ(error, PAGE_ALLOC_OK);
    for (int i = 0; i < 5; i++) {
        EXPECT_TRUE(ptrs[i] != NULL);
        EXPECT_TRUE(page_is_aligned(ptrs[i], 0));
    }
    page_free_array(5, ptrs, NULL);
}

void test_page_alloc_array_null() {
    TEST(PageAlloc_ArrayNull);
    page_alloc_error_t error;
    bool success = page_alloc_array(5, 1024, NULL, &error);
    EXPECT_FALSE(success);
    EXPECT_EQ(error, PAGE_ALLOC_ERROR_NULL_PTR);
}

void test_page_free_array() {
    TEST(PageAlloc_FreeArray);
    void* ptrs[5];
    page_alloc_array(5, 1024, ptrs, NULL);
    page_alloc_error_t error;
    bool success = page_free_array(5, ptrs, &error);
    EXPECT_TRUE(success);
    EXPECT_EQ(error, PAGE_ALLOC_OK);
}

void test_page_realloc() {
    TEST(PageAlloc_Realloc);
    page_alloc_error_t error;
    char* ptr = (char*)page_alloc(1024);
    strcpy(ptr, "Hello World");
    char* new_ptr = (char*)page_realloc(ptr, 1024, 2048, &error);
    EXPECT_TRUE(new_ptr != NULL);
    EXPECT_EQ(error, PAGE_ALLOC_OK);
    EXPECT_STREQ(new_ptr, "Hello World");
    page_free(new_ptr);
}

void test_page_realloc_new_size_zero() {
    TEST(PageAlloc_ReallocNewSizeZero);
    page_alloc_error_t error;
    void* ptr = page_alloc(1024);
    void* new_ptr = page_realloc(ptr, 1024, 0, &error);
    EXPECT_TRUE(new_ptr == NULL);
}

void test_page_is_aligned() {
    TEST(PageAlloc_IsAligned);
    void* ptr = page_alloc(1024);
    bool aligned = page_is_aligned(ptr, 0);
    EXPECT_TRUE(aligned);
    aligned = page_is_aligned(ptr, page_alloc_get_page_size());
    EXPECT_TRUE(aligned);
    page_free(ptr);
}

void test_page_is_aligned_null() {
    TEST(PageAlloc_IsAlignedNull);
    bool aligned = page_is_aligned(NULL, 0);
    EXPECT_FALSE(aligned);
}

void test_page_alloc_get_stats() {
    TEST(PageAlloc_GetStats);
    page_alloc_reset_stats(NULL);
    void* ptr1 = page_alloc(1024);
    void* ptr2 = page_alloc(2048);
    page_alloc_stats_t stats;
    page_alloc_error_t error;
    bool success = page_alloc_get_stats(&stats, &error);
    EXPECT_TRUE(success);
    EXPECT_EQ(error, PAGE_ALLOC_OK);
    EXPECT_TRUE(stats.total_allocated > 0);
    EXPECT_TRUE(stats.allocation_count >= 2);
    page_free(ptr1);
    page_free(ptr2);
}

void test_page_alloc_get_stats_null() {
    TEST(PageAlloc_GetStatsNull);
    page_alloc_error_t error;
    bool success = page_alloc_get_stats(NULL, &error);
    EXPECT_FALSE(success);
    EXPECT_EQ(error, PAGE_ALLOC_ERROR_NULL_PTR);
}

void test_page_alloc_reset_stats() {
    TEST(PageAlloc_ResetStats);
    void* ptr = page_alloc(1024);
    page_alloc_error_t error;
    bool success = page_alloc_reset_stats(&error);
    EXPECT_TRUE(success);
    EXPECT_EQ(error, PAGE_ALLOC_OK);
    page_alloc_stats_t stats;
    page_alloc_get_stats(&stats, NULL);
    EXPECT_EQ(stats.total_allocated, (size_t)0);
    page_free(ptr);
}

void test_page_alloc_error_string() {
    TEST(PageAlloc_ErrorString);
    const char* msg = page_alloc_error_string(PAGE_ALLOC_OK);
    EXPECT_STREQ(msg, "Success");
    msg = page_alloc_error_string(PAGE_ALLOC_ERROR_NULL_PTR);
    EXPECT_STREQ(msg, "Null pointer error");
    msg = page_alloc_error_string(9999);
    EXPECT_STREQ(msg, "Unknown error");
}

int main() {
    test_page_alloc_default_config();
    test_page_alloc_get_page_size();
    test_page_alloc();
    test_page_alloc_zero_size();
    test_page_alloc_ex();
    test_page_alloc_ex_zero_initialize();
    test_page_free_ex();
    test_page_free_ex_null();
    test_page_alloc_array();
    test_page_alloc_array_null();
    test_page_free_array();
    test_page_realloc();
    test_page_realloc_new_size_zero();
    test_page_is_aligned();
    test_page_is_aligned_null();
    test_page_alloc_get_stats();
    test_page_alloc_get_stats_null();
    test_page_alloc_reset_stats();
    test_page_alloc_error_string();

    return 0;
}
