#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../c_utils/utest.h"
#include "../c_utils/quaternion.h"

void test_quat_add() {
    TEST(Quat_Add);
    quat_t a = {1.0, 2.0, 3.0, 4.0};
    quat_t b = {5.0, 6.0, 7.0, 8.0};
    
    quat_t result = quat_add(a, b);
    
    EXPECT_TRUE(fabs(result.w - 6.0) < 0.001);
    EXPECT_TRUE(fabs(result.x - 8.0) < 0.001);
    EXPECT_TRUE(fabs(result.y - 10.0) < 0.001);
    EXPECT_TRUE(fabs(result.z - 12.0) < 0.001);
}

void test_quat_mul() {
    TEST(Quat_Mul);
    quat_t a = {1.0, 0.0, 0.0, 0.0};
    quat_t b = {1.0, 0.0, 0.0, 0.0};
    
    quat_t result = quat_mul(a, b);
    
    EXPECT_TRUE(fabs(result.w - 1.0) < 0.001);
}

void test_quat_normalize() {
    TEST(Quat_Normalize);
    quat_t q = {0.0, 3.0, 4.0, 0.0};
    
    quat_t result = quat_normalize(q);
    
    EXPECT_TRUE(fabs(result.w - 0.0) < 0.001);
    EXPECT_TRUE(fabs(result.x - 0.6) < 0.001);
    EXPECT_TRUE(fabs(result.y - 0.8) < 0.001);
    EXPECT_TRUE(fabs(result.z - 0.0) < 0.001);
}

void test_quat_mul_identity() {
    TEST(Quat_MulIdentity);
    quat_t identity = {1.0, 0.0, 0.0, 0.0};
    quat_t q = {0.707, 0.707, 0.0, 0.0};
    
    quat_t result = quat_mul(identity, q);
    
    EXPECT_TRUE(fabs(result.w - q.w) < 0.001);
    EXPECT_TRUE(fabs(result.x - q.x) < 0.001);
}

void test_quat_add_zero() {
    TEST(Quat_AddZero);
    quat_t zero = {0.0, 0.0, 0.0, 0.0};
    quat_t q = {1.0, 2.0, 3.0, 4.0};
    
    quat_t result = quat_add(zero, q);
    
    EXPECT_TRUE(fabs(result.w - 1.0) < 0.001);
    EXPECT_TRUE(fabs(result.x - 2.0) < 0.001);
    EXPECT_TRUE(fabs(result.y - 3.0) < 0.001);
    EXPECT_TRUE(fabs(result.z - 4.0) < 0.001);
}

int main() {
    test_quat_add();
    test_quat_mul();
    test_quat_normalize();
    test_quat_mul_identity();
    test_quat_add_zero();

    return 0;
}
