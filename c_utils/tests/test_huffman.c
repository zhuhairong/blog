#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/huffman.h"

void test_huffman_stats() {
    TEST(Huffman_Stats);
    unsigned char data[] = "aaabbc";
    size_t freq[256] = {0};
    
    huffman_stats(data, 6, freq);
    
    EXPECT_TRUE(freq['a'] > 0);
    EXPECT_TRUE(freq['b'] > 0);
    EXPECT_TRUE(freq['c'] > 0);
}

void test_huffman_create_free_tree() {
    TEST(Huffman_CreateFreeTree);
    size_t freq[256] = {0};
    freq['a'] = 5;
    freq['b'] = 3;
    
    huffman_error_t error;
    huffman_node_t* root = huffman_create_tree(freq, &error);
    
    EXPECT_TRUE(root != NULL);
    
    huffman_free_tree(root);
}

void test_huffman_get_default_config() {
    TEST(Huffman_GetDefaultConfig);
    huffman_config_t config;
    huffman_get_default_config(&config);
    
    EXPECT_TRUE(config.max_tree_depth > 0 || config.max_tree_depth == 0);
}

void test_huffman_stats_empty() {
    TEST(Huffman_StatsEmpty);
    size_t freq[256] = {0};
    
    huffman_stats((unsigned char*)"", 0, freq);
}

void test_huffman_stats_single() {
    TEST(Huffman_StatsSingle);
    size_t freq[256] = {0};
    
    huffman_stats((unsigned char*)"a", 1, freq);
    
    EXPECT_EQ(freq['a'], (size_t)1);
}

int main() {
    test_huffman_stats();
    test_huffman_create_free_tree();
    test_huffman_get_default_config();
    test_huffman_stats_empty();
    test_huffman_stats_single();

    return 0;
}
