#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/sort_utils.h"

static int compare_int(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

void test_sort_utils_types() {
    TEST(SortUtils_Types);
    sort_utils_config_t config;
    memset(&config, 0, sizeof(config));
    EXPECT_TRUE(sizeof(config) > 0);
    
    sort_utils_state_t state;
    memset(&state, 0, sizeof(state));
    EXPECT_TRUE(sizeof(state) > 0);
}

void test_sort_utils_error_values() {
    TEST(SortUtils_ErrorValues);
    EXPECT_TRUE(SORT_UTILS_OK == 0);
    EXPECT_TRUE(SORT_UTILS_ERROR_INVALID_PARAMS != 0);
    EXPECT_TRUE(SORT_UTILS_ERROR_MEMORY != 0);
}

void test_sort_utils_config_fields() {
    TEST(SortUtils_ConfigFields);
    sort_utils_config_t config;
    memset(&config, 0, sizeof(config));
    
    config.enable_statistics = true;
    config.enable_optimizations = true;
    config.max_memory = 1024 * 1024;
    
    EXPECT_TRUE(config.enable_statistics);
    EXPECT_TRUE(config.enable_optimizations);
    EXPECT_TRUE(config.max_memory == 1024 * 1024);
}

void test_sort_utils_state_fields() {
    TEST(SortUtils_StateFields);
    sort_utils_state_t state;
    memset(&state, 0, sizeof(state));
    
    state.comparisons = 100;
    state.swaps = 50;
    state.is_initialized = true;
    
    EXPECT_EQ(state.comparisons, 100);
    EXPECT_EQ(state.swaps, 50);
    EXPECT_TRUE(state.is_initialized);
}

void test_sort_utils_quicksort() {
    TEST(SortUtils_Quicksort);
    int arr[] = {5, 2, 8, 1, 9, 3};
    size_t n = sizeof(arr) / sizeof(arr[0]);
    
    sort_utils_quicksort(arr, n, sizeof(int), compare_int);
    
    EXPECT_TRUE(arr[0] <= arr[1]);
    EXPECT_TRUE(arr[1] <= arr[2]);
}

int main() {
    test_sort_utils_types();
    test_sort_utils_error_values();
    test_sort_utils_config_fields();
    test_sort_utils_state_fields();
    test_sort_utils_quicksort();

    return 0;
}
