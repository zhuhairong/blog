#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/consistent_hash.h"

void test_ch_init_free() {
    TEST(Ch_InitFree);
    consistent_hash_t ch;
    ch_error_t err = ch_init(&ch, NULL);
    
    EXPECT_EQ(err, CH_OK);
    
    ch_free(&ch);
}

void test_ch_add_remove_node() {
    TEST(Ch_AddRemoveNode);
    consistent_hash_t ch;
    ch_init(&ch, NULL);
    
    ch_error_t err = ch_add_node(&ch, 1, 100);
    EXPECT_EQ(err, CH_OK);
    
    EXPECT_TRUE(ch_node_exists(&ch, 1));
    
    err = ch_remove_node(&ch, 1);
    EXPECT_EQ(err, CH_OK);
    
    EXPECT_FALSE(ch_node_exists(&ch, 1));
    
    ch_free(&ch);
}

void test_ch_get_node() {
    TEST(Ch_GetNode);
    consistent_hash_t ch;
    ch_init(&ch, NULL);
    
    ch_add_node(&ch, 1, 100);
    ch_add_node(&ch, 2, 100);
    
    int node = ch_get_node_str(&ch, "test_key");
    EXPECT_TRUE(node == 1 || node == 2);
    
    ch_free(&ch);
}

void test_ch_node_count() {
    TEST(Ch_NodeCount);
    consistent_hash_t ch;
    ch_init(&ch, NULL);
    
    EXPECT_EQ(ch_get_node_count(&ch), (size_t)0);
    
    ch_add_node(&ch, 1, 100);
    EXPECT_EQ(ch_get_node_count(&ch), (size_t)1);
    
    ch_add_node(&ch, 2, 100);
    EXPECT_EQ(ch_get_node_count(&ch), (size_t)2);
    
    ch_free(&ch);
}

void test_ch_error_string() {
    TEST(Ch_ErrorString);
    const char* msg = ch_error_string(CH_OK);
    EXPECT_TRUE(msg != NULL);
    
    msg = ch_error_string(CH_ERROR_INVALID_NODE);
    EXPECT_TRUE(msg != NULL);
}

int main() {
    test_ch_init_free();
    test_ch_add_remove_node();
    test_ch_get_node();
    test_ch_node_count();
    test_ch_error_string();

    return 0;
}
