#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/bloom_filter_counting.h"

void test_bloom_counting_create_free() {
    TEST(BloomCounting_CreateFree);
    bloom_counting_t* bf = bloom_counting_create(1024, 3, 15);
    EXPECT_TRUE(bf != NULL);
    
    bloom_counting_free(bf);
}

void test_bloom_counting_add_check() {
    TEST(BloomCounting_AddCheck);
    bloom_counting_t* bf = bloom_counting_create(1024, 3, 15);
    EXPECT_TRUE(bf != NULL);
    
    const char* key = "test_key";
    bool result = bloom_counting_add(bf, key, strlen(key));
    EXPECT_TRUE(result);
    
    bool exists = bloom_counting_check(bf, key, strlen(key));
    EXPECT_TRUE(exists);
    
    bloom_counting_free(bf);
}

void test_bloom_counting_remove() {
    TEST(BloomCounting_Remove);
    bloom_counting_t* bf = bloom_counting_create(1024, 3, 15);
    EXPECT_TRUE(bf != NULL);
    
    const char* key = "test_key";
    bloom_counting_add(bf, key, strlen(key));
    
    bool result = bloom_counting_remove(bf, key, strlen(key));
    EXPECT_TRUE(result);
    
    bloom_counting_free(bf);
}

void test_bloom_counting_estimate() {
    TEST(BloomCounting_Estimate);
    bloom_counting_t* bf = bloom_counting_create(1024, 3, 15);
    EXPECT_TRUE(bf != NULL);
    
    const char* key = "test_key";
    bloom_counting_add(bf, key, strlen(key));
    bloom_counting_add(bf, key, strlen(key));
    
    uint8_t count = bloom_counting_estimate(bf, key, strlen(key));
    EXPECT_TRUE(count >= 2);
    
    bloom_counting_free(bf);
}

void test_bloom_counting_reset() {
    TEST(BloomCounting_Reset);
    bloom_counting_t* bf = bloom_counting_create(1024, 3, 15);
    EXPECT_TRUE(bf != NULL);
    
    const char* key = "test_key";
    bloom_counting_add(bf, key, strlen(key));
    
    bool result = bloom_counting_reset(bf);
    EXPECT_TRUE(result);
    
    bloom_counting_free(bf);
}

int main() {
    test_bloom_counting_create_free();
    test_bloom_counting_add_check();
    test_bloom_counting_remove();
    test_bloom_counting_estimate();
    test_bloom_counting_reset();

    return 0;
}
