#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/lru_cache.h"

void test_lru_create() {
    TEST(LruCache_Create);
    lru_cache_t* cache = lru_cache_create(10);
    EXPECT_TRUE(cache != NULL);
    lru_cache_free(cache);
}

void test_lru_put_get() {
    TEST(LruCache_PutGet);
    lru_cache_t* cache = lru_cache_create(10);
    
    int value = 42;
    lru_cache_put(cache, "key1", &value);
    
    int* result = (int*)lru_cache_get(cache, "key1");
    EXPECT_TRUE(result != NULL);
    EXPECT_EQ(*result, 42);
    
    lru_cache_free(cache);
}

void test_lru_get_not_found() {
    TEST(LruCache_GetNotFound);
    lru_cache_t* cache = lru_cache_create(10);
    
    void* result = lru_cache_get(cache, "nonexistent");
    EXPECT_TRUE(result == NULL);
    
    lru_cache_free(cache);
}

void test_lru_contains() {
    TEST(LruCache_Contains);
    lru_cache_t* cache = lru_cache_create(10);
    
    int value = 42;
    lru_cache_put(cache, "key1", &value);
    
    EXPECT_TRUE(lru_cache_contains(cache, "key1"));
    EXPECT_FALSE(lru_cache_contains(cache, "key2"));
    
    lru_cache_free(cache);
}

void test_lru_eviction() {
    TEST(LruCache_Eviction);
    lru_cache_t* cache = lru_cache_create(3);
    
    int v1 = 1, v2 = 2, v3 = 3, v4 = 4;
    lru_cache_put(cache, "key1", &v1);
    lru_cache_put(cache, "key2", &v2);
    lru_cache_put(cache, "key3", &v3);
    lru_cache_put(cache, "key4", &v4);
    
    EXPECT_EQ(lru_cache_size(cache), (size_t)3);
    EXPECT_FALSE(lru_cache_contains(cache, "key1"));
    EXPECT_TRUE(lru_cache_contains(cache, "key4"));
    
    lru_cache_free(cache);
}

void test_lru_size() {
    TEST(LruCache_Size);
    lru_cache_t* cache = lru_cache_create(10);
    
    EXPECT_EQ(lru_cache_size(cache), (size_t)0);
    
    int value = 42;
    lru_cache_put(cache, "key1", &value);
    EXPECT_EQ(lru_cache_size(cache), (size_t)1);
    
    lru_cache_put(cache, "key2", &value);
    EXPECT_EQ(lru_cache_size(cache), (size_t)2);
    
    lru_cache_free(cache);
}

void test_lru_capacity() {
    TEST(LruCache_Capacity);
    lru_cache_t* cache = lru_cache_create(10);
    
    EXPECT_EQ(lru_cache_capacity(cache), (size_t)10);
    
    lru_cache_free(cache);
}

void test_lru_clear() {
    TEST(LruCache_Clear);
    lru_cache_t* cache = lru_cache_create(10);
    
    int value = 42;
    lru_cache_put(cache, "key1", &value);
    lru_cache_put(cache, "key2", &value);
    
    lru_cache_error_t err = lru_cache_clear(cache);
    EXPECT_EQ(err, LRU_CACHE_OK);
    EXPECT_EQ(lru_cache_size(cache), (size_t)0);
    
    lru_cache_free(cache);
}

void test_lru_remove() {
    TEST(LruCache_Remove);
    lru_cache_t* cache = lru_cache_create(10);
    
    int value = 42;
    lru_cache_put(cache, "key1", &value);
    
    lru_cache_error_t err = lru_cache_remove(cache, "key1");
    EXPECT_EQ(err, LRU_CACHE_OK);
    EXPECT_FALSE(lru_cache_contains(cache, "key1"));
    
    lru_cache_free(cache);
}

void test_lru_create_ex() {
    TEST(LruCache_CreateEx);
    lru_cache_config_t config;
    lru_cache_get_default_config(&config);
    config.capacity = 20;
    
    lru_cache_error_t error;
    lru_cache_t* cache = lru_cache_create_ex(&config, &error);
    EXPECT_TRUE(cache != NULL);
    EXPECT_EQ(error, LRU_CACHE_OK);
    
    lru_cache_free(cache);
}

void test_lru_get_stats() {
    TEST(LruCache_GetStats);
    lru_cache_t* cache = lru_cache_create(10);
    
    int value = 42;
    lru_cache_put(cache, "key1", &value);
    lru_cache_get(cache, "key1");
    lru_cache_get(cache, "nonexistent");
    
    lru_cache_stats_t stats;
    lru_cache_error_t err = lru_cache_get_stats(cache, &stats);
    EXPECT_EQ(err, LRU_CACHE_OK);
    EXPECT_EQ(stats.puts, (size_t)1);
    EXPECT_EQ(stats.hits, (size_t)1);
    EXPECT_EQ(stats.misses, (size_t)1);
    
    lru_cache_free(cache);
}

void test_lru_free_null() {
    TEST(LruCache_FreeNull);
    lru_cache_free(NULL);
}

int main() {
    test_lru_create();
    test_lru_put_get();
    test_lru_get_not_found();
    test_lru_contains();
    test_lru_eviction();
    test_lru_size();
    test_lru_capacity();
    test_lru_clear();
    test_lru_remove();
    test_lru_create_ex();
    test_lru_get_stats();
    test_lru_free_null();

    return 0;
}
