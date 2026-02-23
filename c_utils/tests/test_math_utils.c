#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../c_utils/utest.h"
#include "../c_utils/math_utils.h"

void test_math_clamp() {
    TEST(Math_Clamp);
    EXPECT_EQ(math_clamp(5, 0, 10), 5);
    EXPECT_EQ(math_clamp(-5, 0, 10), 0);
    EXPECT_EQ(math_clamp(15, 0, 10), 10);
}

void test_math_lerp() {
    TEST(Math_Lerp);
    double result = math_lerp(0, 10, 0.5);
    EXPECT_TRUE(fabs(result - 5.0) < 0.0001);
    
    result = math_lerp(0, 10, 0);
    EXPECT_TRUE(fabs(result - 0.0) < 0.0001);
    
    result = math_lerp(0, 10, 1);
    EXPECT_TRUE(fabs(result - 10.0) < 0.0001);
}

void test_math_rand_int() {
    TEST(Math_RandInt);
    for (int i = 0; i < 100; i++) {
        int val = math_rand_int(0, 10);
        EXPECT_TRUE(val >= 0 && val <= 10);
    }
}

void test_math_rand_double() {
    TEST(Math_RandDouble);
    for (int i = 0; i < 100; i++) {
        double val = math_rand_double();
        EXPECT_TRUE(val >= 0.0 && val <= 1.0);
    }
}

void test_math_is_pow2() {
    TEST(Math_IsPow2);
    EXPECT_TRUE(math_is_pow2(1));
    EXPECT_TRUE(math_is_pow2(2));
    EXPECT_TRUE(math_is_pow2(4));
    EXPECT_TRUE(math_is_pow2(8));
    EXPECT_TRUE(math_is_pow2(1024));
    EXPECT_FALSE(math_is_pow2(0));
    EXPECT_FALSE(math_is_pow2(3));
    EXPECT_FALSE(math_is_pow2(5));
    EXPECT_FALSE(math_is_pow2(6));
}

void test_math_utils_create_destroy() {
    TEST(Math_Utils_CreateDestroy);
    math_utils_ctx_t* ctx = NULL;
    math_utils_error_t error = math_utils_create(&ctx, NULL);
    
    EXPECT_EQ(error, MATH_UTILS_OK);
    EXPECT_TRUE(ctx != NULL);
    
    math_utils_destroy(ctx);
}

void test_math_utils_clamp_safe() {
    TEST(Math_Utils_ClampSafe);
    math_utils_ctx_t* ctx = NULL;
    math_utils_create(&ctx, NULL);
    
    double out;
    math_utils_error_t error = math_utils_clamp_safe(ctx, 5, 0, 10, &out);
    EXPECT_EQ(error, MATH_UTILS_OK);
    EXPECT_TRUE(fabs(out - 5.0) < 0.0001);
    
    math_utils_destroy(ctx);
}

void test_math_utils_lerp_safe() {
    TEST(Math_Utils_LerpSafe);
    math_utils_ctx_t* ctx = NULL;
    math_utils_create(&ctx, NULL);
    
    double out;
    math_utils_error_t error = math_utils_lerp_safe(ctx, 0, 10, 0.5, &out);
    EXPECT_EQ(error, MATH_UTILS_OK);
    EXPECT_TRUE(fabs(out - 5.0) < 0.0001);
    
    math_utils_destroy(ctx);
}

void test_math_utils_next_pow2() {
    TEST(Math_Utils_NextPow2);
    EXPECT_EQ(math_utils_next_pow2(0), (size_t)1);
    EXPECT_EQ(math_utils_next_pow2(1), (size_t)1);
    EXPECT_EQ(math_utils_next_pow2(2), (size_t)2);
    EXPECT_EQ(math_utils_next_pow2(3), (size_t)4);
    EXPECT_EQ(math_utils_next_pow2(5), (size_t)8);
    EXPECT_EQ(math_utils_next_pow2(1023), (size_t)1024);
}

void test_math_utils_prev_pow2() {
    TEST(Math_Utils_PrevPow2);
    EXPECT_EQ(math_utils_prev_pow2(1), (size_t)1);
    EXPECT_EQ(math_utils_prev_pow2(2), (size_t)2);
    EXPECT_EQ(math_utils_prev_pow2(3), (size_t)2);
    EXPECT_EQ(math_utils_prev_pow2(5), (size_t)4);
    EXPECT_EQ(math_utils_prev_pow2(1025), (size_t)1024);
}

void test_math_utils_deg_rad() {
    TEST(Math_Utils_DegRad);
    double rad = math_utils_deg_to_rad(180);
    EXPECT_TRUE(fabs(rad - 3.14159265) < 0.0001);
    
    double deg = math_utils_rad_to_deg(3.14159265);
    EXPECT_TRUE(fabs(deg - 180.0) < 0.0001);
}

void test_math_utils_normalize_angle() {
    TEST(Math_Utils_NormalizeAngle);
    EXPECT_TRUE(fabs(math_utils_normalize_angle(0) - 0) < 0.0001);
    EXPECT_TRUE(fabs(math_utils_normalize_angle(360) - 0) < 0.0001);
    EXPECT_TRUE(fabs(math_utils_normalize_angle(720) - 0) < 0.0001);
    EXPECT_TRUE(fabs(math_utils_normalize_angle(-90) - 270) < 0.0001);
}

void test_math_utils_strerror() {
    TEST(Math_Utils_Strerror);
    const char* msg = math_utils_strerror(MATH_UTILS_OK);
    EXPECT_TRUE(msg != NULL);
    
    msg = math_utils_strerror(MATH_UTILS_DIVISION_BY_ZERO);
    EXPECT_TRUE(msg != NULL);
}

int main() {
    test_math_clamp();
    test_math_lerp();
    test_math_rand_int();
    test_math_rand_double();
    test_math_is_pow2();
    test_math_utils_create_destroy();
    test_math_utils_clamp_safe();
    test_math_utils_lerp_safe();
    test_math_utils_next_pow2();
    test_math_utils_prev_pow2();
    test_math_utils_deg_rad();
    test_math_utils_normalize_angle();
    test_math_utils_strerror();

    return 0;
}
