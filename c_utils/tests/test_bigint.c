#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/bigint.h"

void test_bigint_from_str() {
    TEST(Bigint_FromStr);
    bigint_t* b = bigint_from_str("12345");
    EXPECT_TRUE(b != NULL);
    
    char* str = bigint_to_str(b);
    EXPECT_TRUE(str != NULL);
    EXPECT_TRUE(strcmp(str, "12345") == 0);
    
    free(str);
    bigint_free(b);
}

void test_bigint_from_str_large() {
    TEST(Bigint_FromStrLarge);
    bigint_t* b = bigint_from_str("123456789012345678901234567890");
    EXPECT_TRUE(b != NULL);
    
    char* str = bigint_to_str(b);
    EXPECT_TRUE(str != NULL);
    EXPECT_TRUE(strcmp(str, "123456789012345678901234567890") == 0);
    
    free(str);
    bigint_free(b);
}

void test_bigint_zero() {
    TEST(Bigint_Zero);
    bigint_t* b = bigint_zero();
    EXPECT_TRUE(b != NULL);
    EXPECT_TRUE(bigint_is_zero(b));
    
    char* str = bigint_to_str(b);
    EXPECT_TRUE(strcmp(str, "0") == 0);
    
    free(str);
    bigint_free(b);
}

void test_bigint_one() {
    TEST(Bigint_One);
    bigint_t* b = bigint_one();
    EXPECT_TRUE(b != NULL);
    EXPECT_FALSE(bigint_is_zero(b));
    
    char* str = bigint_to_str(b);
    EXPECT_TRUE(strcmp(str, "1") == 0);
    
    free(str);
    bigint_free(b);
}

void test_bigint_copy() {
    TEST(Bigint_Copy);
    bigint_t* a = bigint_from_str("12345");
    bigint_t* b = bigint_copy(a);
    
    EXPECT_TRUE(b != NULL);
    EXPECT_TRUE(bigint_compare(a, b) == 0);
    
    bigint_free(a);
    bigint_free(b);
}

void test_bigint_add() {
    TEST(Bigint_Add);
    bigint_t* a = bigint_from_str("12345");
    bigint_t* b = bigint_from_str("67890");
    
    bigint_t* c = bigint_add(a, b);
    EXPECT_TRUE(c != NULL);
    
    char* str = bigint_to_str(c);
    EXPECT_TRUE(strcmp(str, "80235") == 0);
    
    free(str);
    bigint_free(a);
    bigint_free(b);
    bigint_free(c);
}

void test_bigint_add_with_carry() {
    TEST(Bigint_AddWithCarry);
    bigint_t* a = bigint_from_str("999999999999");
    bigint_t* b = bigint_from_str("1");
    
    bigint_t* c = bigint_add(a, b);
    EXPECT_TRUE(c != NULL);
    
    char* str = bigint_to_str(c);
    EXPECT_TRUE(strcmp(str, "1000000000000") == 0);
    
    free(str);
    bigint_free(a);
    bigint_free(b);
    bigint_free(c);
}

void test_bigint_sub() {
    TEST(Bigint_Sub);
    bigint_t* a = bigint_from_str("10000");
    bigint_t* b = bigint_from_str("1234");
    
    bigint_t* c = bigint_sub(a, b);
    EXPECT_TRUE(c != NULL);
    
    char* str = bigint_to_str(c);
    EXPECT_TRUE(strcmp(str, "8766") == 0);
    
    free(str);
    bigint_free(a);
    bigint_free(b);
    bigint_free(c);
}

void test_bigint_sub_with_borrow() {
    TEST(Bigint_SubWithBorrow);
    bigint_t* a = bigint_from_str("1000000000");
    bigint_t* b = bigint_from_str("1");
    
    bigint_t* c = bigint_sub(a, b);
    EXPECT_TRUE(c != NULL);
    
    char* str = bigint_to_str(c);
    EXPECT_TRUE(strcmp(str, "999999999") == 0);
    
    free(str);
    bigint_free(a);
    bigint_free(b);
    bigint_free(c);
}

void test_bigint_mul() {
    TEST(Bigint_Mul);
    bigint_t* a = bigint_from_str("12345");
    bigint_t* b = bigint_from_str("67890");
    
    bigint_t* c = bigint_mul(a, b);
    EXPECT_TRUE(c != NULL);
    
    char* str = bigint_to_str(c);
    EXPECT_TRUE(strcmp(str, "838102050") == 0);
    
    free(str);
    bigint_free(a);
    bigint_free(b);
    bigint_free(c);
}

void test_bigint_mul_large() {
    TEST(Bigint_MulLarge);
    bigint_t* a = bigint_from_str("12345678901234567890");
    bigint_t* b = bigint_from_str("98765432109876543210");
    
    bigint_t* c = bigint_mul(a, b);
    EXPECT_TRUE(c != NULL);
    
    bigint_free(a);
    bigint_free(b);
    bigint_free(c);
}

void test_bigint_compare() {
    TEST(Bigint_Compare);
    bigint_t* a = bigint_from_str("100");
    bigint_t* b = bigint_from_str("200");
    bigint_t* c = bigint_from_str("100");
    
    EXPECT_TRUE(bigint_compare(a, b) < 0);
    EXPECT_TRUE(bigint_compare(b, a) > 0);
    EXPECT_TRUE(bigint_compare(a, c) == 0);
    
    bigint_free(a);
    bigint_free(b);
    bigint_free(c);
}

void test_bigint_is_zero() {
    TEST(Bigint_IsZero);
    bigint_t* a = bigint_zero();
    bigint_t* b = bigint_from_str("123");
    
    EXPECT_TRUE(bigint_is_zero(a));
    EXPECT_FALSE(bigint_is_zero(b));
    
    bigint_free(a);
    bigint_free(b);
}

void test_bigint_free_null() {
    TEST(Bigint_FreeNull);
    bigint_free(NULL);
}

void test_bigint_stress_operations() {
    TEST(Bigint_StressOperations);
    bigint_t* a = bigint_from_str("1");
    
    for (int i = 0; i < 100; i++) {
        bigint_t* b = bigint_add(a, a);
        bigint_free(a);
        a = b;
    }
    
    EXPECT_TRUE(a != NULL);
    
    bigint_free(a);
}

int main() {
    test_bigint_from_str();
    test_bigint_from_str_large();
    test_bigint_zero();
    test_bigint_one();
    test_bigint_copy();
    test_bigint_add();
    test_bigint_add_with_carry();
    test_bigint_sub();
    test_bigint_sub_with_borrow();
    test_bigint_mul();
    test_bigint_mul_large();
    test_bigint_compare();
    test_bigint_is_zero();
    test_bigint_free_null();
    test_bigint_stress_operations();

    return 0;
}
