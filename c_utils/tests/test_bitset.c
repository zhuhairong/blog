#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/bitset.h"

void test_bitset_create() {
    TEST(Bitset_Create);
    bitset_t* bs = bitset_create(64);
    EXPECT_TRUE(bs != NULL);
    EXPECT_EQ(bitset_size(bs), (size_t)64);
    bitset_free(bs);
}

void test_bitset_create_zero() {
    TEST(Bitset_CreateZero);
    bitset_t* bs = bitset_create(0);
    EXPECT_TRUE(bs != NULL);
    bitset_free(bs);
}

void test_bitset_create_large() {
    TEST(Bitset_CreateLarge);
    bitset_t* bs = bitset_create(10000);
    EXPECT_TRUE(bs != NULL);
    EXPECT_EQ(bitset_size(bs), (size_t)10000);
    bitset_free(bs);
}

void test_bitset_set_and_test() {
    TEST(Bitset_SetAndTest);
    bitset_t* bs = bitset_create(64);
    
    bitset_set(bs, 0);
    bitset_set(bs, 31);
    bitset_set(bs, 63);
    
    EXPECT_TRUE(bitset_test(bs, 0));
    EXPECT_TRUE(bitset_test(bs, 31));
    EXPECT_TRUE(bitset_test(bs, 63));
    EXPECT_FALSE(bitset_test(bs, 1));
    EXPECT_FALSE(bitset_test(bs, 62));
    
    bitset_free(bs);
}

void test_bitset_clear() {
    TEST(Bitset_Clear);
    bitset_t* bs = bitset_create(64);
    
    bitset_set(bs, 10);
    EXPECT_TRUE(bitset_test(bs, 10));
    
    bitset_clear(bs, 10);
    EXPECT_FALSE(bitset_test(bs, 10));
    
    bitset_free(bs);
}

void test_bitset_flip() {
    TEST(Bitset_Flip);
    bitset_t* bs = bitset_create(64);
    
    EXPECT_FALSE(bitset_test(bs, 5));
    bitset_flip(bs, 5);
    EXPECT_TRUE(bitset_test(bs, 5));
    bitset_flip(bs, 5);
    EXPECT_FALSE(bitset_test(bs, 5));
    
    bitset_free(bs);
}

void test_bitset_set_all() {
    TEST(Bitset_SetAll);
    bitset_t* bs = bitset_create(64);
    
    bitset_set_all(bs);
    
    for (size_t i = 0; i < 64; i++) {
        EXPECT_TRUE(bitset_test(bs, i));
    }
    
    bitset_free(bs);
}

void test_bitset_clear_all() {
    TEST(Bitset_ClearAll);
    bitset_t* bs = bitset_create(64);
    
    bitset_set_all(bs);
    bitset_clear_all(bs);
    
    for (size_t i = 0; i < 64; i++) {
        EXPECT_FALSE(bitset_test(bs, i));
    }
    
    bitset_free(bs);
}

void test_bitset_flip_all() {
    TEST(Bitset_FlipAll);
    bitset_t* bs = bitset_create(64);
    
    bitset_set(bs, 0);
    bitset_set(bs, 63);
    
    bitset_flip_all(bs);
    
    EXPECT_FALSE(bitset_test(bs, 0));
    EXPECT_TRUE(bitset_test(bs, 1));
    EXPECT_FALSE(bitset_test(bs, 63));
    
    bitset_free(bs);
}

void test_bitset_set_range() {
    TEST(Bitset_SetRange);
    bitset_t* bs = bitset_create(100);
    
    bitset_set_range(bs, 10, 19);
    
    for (size_t i = 0; i < 10; i++) {
        EXPECT_FALSE(bitset_test(bs, i));
    }
    for (size_t i = 10; i < 20; i++) {
        EXPECT_TRUE(bitset_test(bs, i));
    }
    for (size_t i = 20; i < 100; i++) {
        EXPECT_FALSE(bitset_test(bs, i));
    }
    
    bitset_free(bs);
}

void test_bitset_clear_range() {
    TEST(Bitset_ClearRange);
    bitset_t* bs = bitset_create(100);
    
    bitset_set_all(bs);
    bitset_clear_range(bs, 10, 19);
    
    for (size_t i = 0; i < 10; i++) {
        EXPECT_TRUE(bitset_test(bs, i));
    }
    for (size_t i = 10; i < 20; i++) {
        EXPECT_FALSE(bitset_test(bs, i));
    }
    for (size_t i = 20; i < 100; i++) {
        EXPECT_TRUE(bitset_test(bs, i));
    }
    
    bitset_free(bs);
}

void test_bitset_count() {
    TEST(Bitset_Count);
    bitset_t* bs = bitset_create(64);
    
    bitset_set(bs, 0);
    bitset_set(bs, 10);
    bitset_set(bs, 20);
    bitset_set(bs, 30);
    
    EXPECT_EQ(bitset_count(bs), (size_t)4);
    
    bitset_free(bs);
}

void test_bitset_is_empty() {
    TEST(Bitset_IsEmpty);
    bitset_t* bs = bitset_create(64);
    
    EXPECT_TRUE(bitset_is_empty(bs));
    
    bitset_set(bs, 5);
    EXPECT_FALSE(bitset_is_empty(bs));
    
    bitset_free(bs);
}

void test_bitset_is_all_set() {
    TEST(Bitset_IsAllSet);
    bitset_t* bs = bitset_create(64);
    
    EXPECT_FALSE(bitset_is_all_set(bs));
    
    bitset_set_all(bs);
    EXPECT_TRUE(bitset_is_all_set(bs));
    
    bitset_free(bs);
}

void test_bitset_find_first_set() {
    TEST(Bitset_FindFirstSet);
    bitset_t* bs = bitset_create(100);
    
    bitset_set(bs, 50);
    bitset_set(bs, 75);
    
    EXPECT_EQ(bitset_find_first_set(bs, 0), (size_t)50);
    EXPECT_EQ(bitset_find_first_set(bs, 51), (size_t)75);
    
    bitset_free(bs);
}

void test_bitset_find_first_clear() {
    TEST(Bitset_FindFirstClear);
    bitset_t* bs = bitset_create(100);
    
    bitset_set_range(bs, 0, 49);
    
    EXPECT_EQ(bitset_find_first_clear(bs, 0), (size_t)50);
    
    bitset_free(bs);
}

void test_bitset_and() {
    TEST(Bitset_And);
    bitset_t* a = bitset_create(64);
    bitset_t* b = bitset_create(64);
    
    bitset_set(a, 0);
    bitset_set(a, 1);
    bitset_set(b, 1);
    bitset_set(b, 2);
    
    bitset_t* result = bitset_and(a, b);
    EXPECT_TRUE(result != NULL);
    EXPECT_FALSE(bitset_test(result, 0));
    EXPECT_TRUE(bitset_test(result, 1));
    EXPECT_FALSE(bitset_test(result, 2));
    
    bitset_free(a);
    bitset_free(b);
    bitset_free(result);
}

void test_bitset_or() {
    TEST(Bitset_Or);
    bitset_t* a = bitset_create(64);
    bitset_t* b = bitset_create(64);
    
    bitset_set(a, 0);
    bitset_set(b, 1);
    
    bitset_t* result = bitset_or(a, b);
    EXPECT_TRUE(result != NULL);
    EXPECT_TRUE(bitset_test(result, 0));
    EXPECT_TRUE(bitset_test(result, 1));
    
    bitset_free(a);
    bitset_free(b);
    bitset_free(result);
}

void test_bitset_xor() {
    TEST(Bitset_Xor);
    bitset_t* a = bitset_create(64);
    bitset_t* b = bitset_create(64);
    
    bitset_set(a, 0);
    bitset_set(a, 1);
    bitset_set(b, 1);
    bitset_set(b, 2);
    
    bitset_t* result = bitset_xor(a, b);
    EXPECT_TRUE(result != NULL);
    EXPECT_TRUE(bitset_test(result, 0));
    EXPECT_FALSE(bitset_test(result, 1));
    EXPECT_TRUE(bitset_test(result, 2));
    
    bitset_free(a);
    bitset_free(b);
    bitset_free(result);
}

void test_bitset_not() {
    TEST(Bitset_Not);
    bitset_t* bs = bitset_create(64);
    
    bitset_set(bs, 0);
    bitset_set(bs, 63);
    
    bitset_t* result = bitset_not(bs);
    EXPECT_TRUE(result != NULL);
    EXPECT_FALSE(bitset_test(result, 0));
    EXPECT_TRUE(bitset_test(result, 1));
    EXPECT_FALSE(bitset_test(result, 63));
    
    bitset_free(bs);
    bitset_free(result);
}

void test_bitset_equals() {
    TEST(Bitset_Equals);
    bitset_t* a = bitset_create(64);
    bitset_t* b = bitset_create(64);
    
    EXPECT_TRUE(bitset_equals(a, b));
    
    bitset_set(a, 5);
    EXPECT_FALSE(bitset_equals(a, b));
    
    bitset_set(b, 5);
    EXPECT_TRUE(bitset_equals(a, b));
    
    bitset_free(a);
    bitset_free(b);
}

void test_bitset_clone() {
    TEST(Bitset_Clone);
    bitset_t* bs = bitset_create(64);
    
    bitset_set(bs, 10);
    bitset_set(bs, 20);
    bitset_set(bs, 30);
    
    bitset_t* clone = bitset_clone(bs);
    EXPECT_TRUE(clone != NULL);
    EXPECT_TRUE(bitset_equals(bs, clone));
    
    bitset_free(bs);
    bitset_free(clone);
}

void test_bitset_resize() {
    TEST(Bitset_Resize);
    bitset_t* bs = bitset_create(64);
    
    bitset_set(bs, 10);
    
    bool resized = bitset_resize(bs, 128);
    EXPECT_TRUE(resized);
    EXPECT_EQ(bitset_size(bs), (size_t)128);
    EXPECT_TRUE(bitset_test(bs, 10));
    
    bitset_free(bs);
}

void test_bitset_to_bytes() {
    TEST(Bitset_ToBytes);
    bitset_t* bs = bitset_create(16);
    
    bitset_set(bs, 0);
    bitset_set(bs, 8);
    
    uint8_t bytes[2];
    bool success = bitset_to_bytes(bs, bytes, sizeof(bytes));
    EXPECT_TRUE(success);
    
    bitset_free(bs);
}

void test_bitset_create_from_bytes() {
    TEST(Bitset_CreateFromBytes);
    uint8_t bytes[] = {0x01, 0x80};
    
    bitset_t* bs = bitset_create_from_bytes(bytes, 2, 16);
    EXPECT_TRUE(bs != NULL);
    EXPECT_TRUE(bitset_test(bs, 0));
    EXPECT_TRUE(bitset_test(bs, 15));
    
    bitset_free(bs);
}

void test_bitset_free_null() {
    TEST(Bitset_FreeNull);
    bitset_free(NULL);
}

int main() {
    test_bitset_create();
    test_bitset_create_zero();
    test_bitset_create_large();
    test_bitset_set_and_test();
    test_bitset_clear();
    test_bitset_flip();
    test_bitset_set_all();
    test_bitset_clear_all();
    test_bitset_flip_all();
    test_bitset_set_range();
    test_bitset_clear_range();
    test_bitset_count();
    test_bitset_is_empty();
    test_bitset_is_all_set();
    test_bitset_find_first_set();
    test_bitset_find_first_clear();
    test_bitset_and();
    test_bitset_or();
    test_bitset_xor();
    test_bitset_not();
    test_bitset_equals();
    test_bitset_clone();
    test_bitset_resize();
    test_bitset_to_bytes();
    test_bitset_create_from_bytes();
    test_bitset_free_null();

    return 0;
}
