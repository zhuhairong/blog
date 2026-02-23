#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/sort_heap.h"

void test_sort_heap_types() {
    TEST(SortHeap_Types);
    heap_config_t config;
    memset(&config, 0, sizeof(config));
    EXPECT_TRUE(sizeof(config) > 0);
    
    heap_state_t state;
    memset(&state, 0, sizeof(state));
    EXPECT_TRUE(sizeof(state) > 0);
}

void test_sort_heap_error_values() {
    TEST(SortHeap_ErrorValues);
    EXPECT_TRUE(HEAP_OK == 0);
    EXPECT_TRUE(HEAP_ERROR_INVALID_PARAMS != 0);
    EXPECT_TRUE(HEAP_ERROR_MEMORY != 0);
}

void test_sort_heap_type_values() {
    TEST(SortHeap_TypeValues);
    EXPECT_EQ(HEAP_TYPE_MAX, 0);
    EXPECT_EQ(HEAP_TYPE_MIN, 1);
}

void test_sort_heap_config_fields() {
    TEST(SortHeap_ConfigFields);
    heap_config_t config;
    memset(&config, 0, sizeof(config));
    
    config.type = HEAP_TYPE_MAX;
    config.use_malloc = true;
    config.initial_capacity = 100;
    
    EXPECT_EQ(config.type, HEAP_TYPE_MAX);
    EXPECT_TRUE(config.use_malloc);
    EXPECT_EQ(config.initial_capacity, 100);
}

void test_sort_heap_state_fields() {
    TEST(SortHeap_StateFields);
    heap_state_t state;
    memset(&state, 0, sizeof(state));
    
    state.size = 50;
    state.capacity = 100;
    state.is_initialized = true;
    
    EXPECT_EQ(state.size, 50);
    EXPECT_EQ(state.capacity, 100);
    EXPECT_TRUE(state.is_initialized);
}

int main() {
    test_sort_heap_types();
    test_sort_heap_error_values();
    test_sort_heap_type_values();
    test_sort_heap_config_fields();
    test_sort_heap_state_fields();

    return 0;
}
