#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/sort_merge.h"

void test_sort_merge_types() {
    TEST(SortMerge_Types);
    merge_config_t config;
    memset(&config, 0, sizeof(config));
    EXPECT_TRUE(sizeof(config) > 0);
    
    merge_state_t state;
    memset(&state, 0, sizeof(state));
    EXPECT_TRUE(sizeof(state) > 0);
}

void test_sort_merge_error_values() {
    TEST(SortMerge_ErrorValues);
    EXPECT_TRUE(MERGE_OK == 0);
    EXPECT_TRUE(MERGE_ERROR_INVALID_PARAMS != 0);
    EXPECT_TRUE(MERGE_ERROR_MEMORY != 0);
}

void test_sort_merge_config_fields() {
    TEST(SortMerge_ConfigFields);
    merge_config_t config;
    memset(&config, 0, sizeof(config));
    
    config.stable = true;
    config.in_place = false;
    config.parallel = false;
    
    EXPECT_TRUE(config.stable);
    EXPECT_FALSE(config.in_place);
    EXPECT_FALSE(config.parallel);
}

void test_sort_merge_state_fields() {
    TEST(SortMerge_StateFields);
    merge_state_t state;
    memset(&state, 0, sizeof(state));
    
    state.comparisons = 100;
    state.is_sorted = true;
    state.elements_sorted = 50;
    
    EXPECT_EQ(state.comparisons, 100);
    EXPECT_TRUE(state.is_sorted);
    EXPECT_EQ(state.elements_sorted, 50);
}

void test_sort_merge_basic() {
    TEST(SortMerge_Basic);
    int arr[] = {5, 2, 8, 1, 9, 3};
    size_t n = sizeof(arr) / sizeof(arr[0]);
    
    sort_merge(arr, n);
    
    EXPECT_TRUE(arr[0] <= arr[1]);
    EXPECT_TRUE(arr[1] <= arr[2]);
}

int main() {
    test_sort_merge_types();
    test_sort_merge_error_values();
    test_sort_merge_config_fields();
    test_sort_merge_state_fields();
    test_sort_merge_basic();

    return 0;
}
