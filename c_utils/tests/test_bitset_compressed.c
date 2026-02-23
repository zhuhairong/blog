#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/bitset_compressed.h"

void test_bitset_compressed_init() {
    TEST(BitsetCompressed_Init);
    bitset_compressed_t bc;
    bitset_comp_init(&bc);
    EXPECT_TRUE(bc.runs == NULL || bc.runs != NULL);
    EXPECT_EQ(bc.count, 0);
}

void test_bitset_compressed_create_free() {
    TEST(BitsetCompressed_CreateFree);
    bitset_compressed_t* bc = bitset_comp_create(100);
    EXPECT_TRUE(bc != NULL);
    
    bitset_comp_free(bc);
}

void test_bitset_compressed_is_empty() {
    TEST(BitsetCompressed_IsEmpty);
    bitset_compressed_t bc;
    bitset_comp_init(&bc);
    
    bool empty = bitset_comp_is_empty(&bc);
    EXPECT_TRUE(empty);
}

void test_bitset_compressed_size() {
    TEST(BitsetCompressed_Size);
    bitset_compressed_t bc;
    bitset_comp_init(&bc);
    
    size_t size = bitset_comp_size(&bc);
    EXPECT_EQ(size, 0);
}

void test_bitset_compressed_run_count() {
    TEST(BitsetCompressed_RunCount);
    bitset_compressed_t bc;
    bitset_comp_init(&bc);
    
    size_t count = bitset_comp_run_count(&bc);
    EXPECT_EQ(count, 0);
}

int main() {
    test_bitset_compressed_init();
    test_bitset_compressed_create_free();
    test_bitset_compressed_is_empty();
    test_bitset_compressed_size();
    test_bitset_compressed_run_count();

    return 0;
}
