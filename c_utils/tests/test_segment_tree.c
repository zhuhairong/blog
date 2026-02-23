#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/segment_tree.h"

void test_segment_tree_init() {
    TEST(SegmentTree_Init);
    int arr[] = {1, 3, 5, 7, 9, 11};
    segment_tree_t st;
    memset(&st, 0, sizeof(st));
    segment_tree_init(&st, arr, 6);
    
    EXPECT_TRUE(st.tree != NULL);
    EXPECT_EQ(st.n, (size_t)6);
    
    segment_tree_free(&st);
}

void test_segment_tree_query_sum() {
    TEST(SegmentTree_QuerySum);
    int arr[] = {1, 3, 5, 7, 9, 11};
    segment_tree_t st;
    memset(&st, 0, sizeof(st));
    segment_tree_config_t config = segment_tree_default_config();
    config.operation = SEGMENT_TREE_OP_SUM;
    
    segment_tree_error_t error;
    segment_tree_init_ex(&st, arr, 6, &config, &error);
    
    int result = segment_tree_query(&st, 0, 5);
    EXPECT_EQ(result, 36);
    
    result = segment_tree_query(&st, 1, 3);
    EXPECT_EQ(result, 15);
    
    segment_tree_free(&st);
}

void test_segment_tree_update() {
    TEST(SegmentTree_Update);
    int arr[] = {1, 3, 5, 7, 9, 11};
    segment_tree_t st;
    memset(&st, 0, sizeof(st));
    segment_tree_config_t config = segment_tree_default_config();
    config.operation = SEGMENT_TREE_OP_SUM;
    
    segment_tree_error_t error;
    segment_tree_init_ex(&st, arr, 6, &config, &error);
    
    segment_tree_update(&st, 2, 10);
    
    int result = segment_tree_query(&st, 0, 5);
    EXPECT_EQ(result, 41);
    
    segment_tree_free(&st);
}

void test_segment_tree_default_config() {
    TEST(SegmentTree_DefaultConfig);
    segment_tree_config_t config = segment_tree_default_config();
    EXPECT_TRUE(config.operation >= SEGMENT_TREE_OP_MIN);
    EXPECT_TRUE(config.operation < SEGMENT_TREE_OP_COUNT);
}

void test_segment_tree_is_initialized() {
    TEST(SegmentTree_IsInitialized);
    int arr[] = {1, 2, 3, 4, 5};
    segment_tree_t st;
    memset(&st, 0, sizeof(st));
    segment_tree_init(&st, arr, 5);
    
    EXPECT_TRUE(segment_tree_is_initialized(&st));
    
    segment_tree_free(&st);
}

void test_segment_tree_single_element() {
    TEST(SegmentTree_SingleElement);
    int arr[] = {42};
    segment_tree_t st;
    memset(&st, 0, sizeof(st));
    segment_tree_config_t config = segment_tree_default_config();
    config.operation = SEGMENT_TREE_OP_SUM;
    
    segment_tree_error_t error;
    segment_tree_init_ex(&st, arr, 1, &config, &error);
    
    int result = segment_tree_query(&st, 0, 0);
    EXPECT_EQ(result, 42);
    
    segment_tree_free(&st);
}

void test_segment_tree_free_null() {
    TEST(SegmentTree_FreeNull);
    segment_tree_free(NULL);
}

int main() {
    test_segment_tree_init();
    test_segment_tree_query_sum();
    test_segment_tree_update();
    test_segment_tree_default_config();
    test_segment_tree_is_initialized();
    test_segment_tree_single_element();
    test_segment_tree_free_null();

    return 0;
}
