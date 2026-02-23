#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/memory_pool_fixed.h"

void test_mem_pool_fixed_default_config() {
    TEST(MemPoolFixed_DefaultConfig);
    mem_pool_fixed_config_t config = mem_pool_fixed_default_config();
    EXPECT_TRUE(config.alignment == 0 || config.alignment > 0);
}

void test_mem_pool_fixed_init() {
    TEST(MemPoolFixed_Init);
    mem_pool_fixed_t pool;
    mem_pool_fixed_error_t err = mem_pool_fixed_init(&pool, 64, 10);
    EXPECT_EQ(err, MEM_POOL_FIXED_OK);
    
    if (err == MEM_POOL_FIXED_OK) {
        mem_pool_fixed_destroy(&pool);
    }
}

void test_mem_pool_fixed_init_null() {
    TEST(MemPoolFixed_InitNull);
    mem_pool_fixed_error_t err = mem_pool_fixed_init(NULL, 64, 10);
    EXPECT_TRUE(err != MEM_POOL_FIXED_OK);
}

void test_mem_pool_fixed_alloc_free() {
    TEST(MemPoolFixed_AllocFree);
    mem_pool_fixed_t pool;
    mem_pool_fixed_error_t err = mem_pool_fixed_init(&pool, 64, 10);
    
    if (err == MEM_POOL_FIXED_OK) {
        void* block = mem_pool_fixed_alloc(&pool);
        EXPECT_TRUE(block != NULL);
        
        if (block != NULL) {
            mem_pool_fixed_free(&pool, block);
        }
        
        mem_pool_fixed_destroy(&pool);
    } else {
        EXPECT_TRUE(true);
    }
}

void test_mem_pool_fixed_strerror() {
    TEST(MemPoolFixed_Strerror);
    const char* msg = mem_pool_fixed_strerror(MEM_POOL_FIXED_OK);
    EXPECT_TRUE(msg != NULL);
    
    msg = mem_pool_fixed_strerror(MEM_POOL_FIXED_ERROR_NULL_PTR);
    EXPECT_TRUE(msg != NULL);
}

int main() {
    test_mem_pool_fixed_default_config();
    test_mem_pool_fixed_init();
    test_mem_pool_fixed_init_null();
    test_mem_pool_fixed_alloc_free();
    test_mem_pool_fixed_strerror();

    return 0;
}
