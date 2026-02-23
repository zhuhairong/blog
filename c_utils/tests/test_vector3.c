#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../c_utils/utest.h"
#include "../c_utils/vector3.h"

void test_vec3_add() {
    TEST(Vec3_Add);
    vec3_t a = {1.0, 2.0, 3.0};
    vec3_t b = {4.0, 5.0, 6.0};
    
    vec3_t result = vec3_add(a, b);
    
    EXPECT_TRUE(fabs(result.x - 5.0) < 0.001);
    EXPECT_TRUE(fabs(result.y - 7.0) < 0.001);
    EXPECT_TRUE(fabs(result.z - 9.0) < 0.001);
}

void test_vec3_sub() {
    TEST(Vec3_Sub);
    vec3_t a = {4.0, 5.0, 6.0};
    vec3_t b = {1.0, 2.0, 3.0};
    
    vec3_t result = vec3_sub(a, b);
    
    EXPECT_TRUE(fabs(result.x - 3.0) < 0.001);
    EXPECT_TRUE(fabs(result.y - 3.0) < 0.001);
    EXPECT_TRUE(fabs(result.z - 3.0) < 0.001);
}

void test_vec3_dot() {
    TEST(Vec3_Dot);
    vec3_t a = {1.0, 2.0, 3.0};
    vec3_t b = {4.0, 5.0, 6.0};
    
    double result = vec3_dot(a, b);
    
    EXPECT_TRUE(fabs(result - 32.0) < 0.001);
}

void test_vec3_cross() {
    TEST(Vec3_Cross);
    vec3_t a = {1.0, 0.0, 0.0};
    vec3_t b = {0.0, 1.0, 0.0};
    
    vec3_t result = vec3_cross(a, b);
    
    EXPECT_TRUE(fabs(result.x - 0.0) < 0.001);
    EXPECT_TRUE(fabs(result.y - 0.0) < 0.001);
    EXPECT_TRUE(fabs(result.z - 1.0) < 0.001);
}

void test_vec3_length() {
    TEST(Vec3_Length);
    vec3_t v = {3.0, 4.0, 0.0};
    
    double result = vec3_length(v);
    
    EXPECT_TRUE(fabs(result - 5.0) < 0.001);
}

void test_vec3_normalize() {
    TEST(Vec3_Normalize);
    vec3_t v = {3.0, 4.0, 0.0};
    
    vec3_t result = vec3_normalize(v);
    
    EXPECT_TRUE(fabs(result.x - 0.6) < 0.001);
    EXPECT_TRUE(fabs(result.y - 0.8) < 0.001);
    EXPECT_TRUE(fabs(result.z - 0.0) < 0.001);
}

void test_vec3_scale() {
    TEST(Vec3_Scale);
    vec3_t v = {1.0, 2.0, 3.0};
    
    vec3_t result = vec3_scale(v, 2.0);
    
    EXPECT_TRUE(fabs(result.x - 2.0) < 0.001);
    EXPECT_TRUE(fabs(result.y - 4.0) < 0.001);
    EXPECT_TRUE(fabs(result.z - 6.0) < 0.001);
}

void test_vec3_distance() {
    TEST(Vec3_Distance);
    vec3_t a = {0.0, 0.0, 0.0};
    vec3_t b = {3.0, 4.0, 0.0};
    
    double result = vec3_distance(a, b);
    
    EXPECT_TRUE(fabs(result - 5.0) < 0.001);
}

void test_vec3_create_destroy() {
    TEST(Vec3_CreateDestroy);
    vec3_ctx_t* ctx = NULL;
    vec3_error_t error = vec3_create(&ctx, NULL);
    
    EXPECT_EQ(error, VEC3_OK);
    EXPECT_TRUE(ctx != NULL);
    
    vec3_destroy(ctx);
}

void test_vec3_divide() {
    TEST(Vec3_Divide);
    vec3_ctx_t* ctx = NULL;
    vec3_create(&ctx, NULL);
    
    vec3_t v = {10.0, 20.0, 30.0};
    vec3_t result;
    
    vec3_error_t error = vec3_divide(ctx, v, 2.0, &result);
    EXPECT_EQ(error, VEC3_OK);
    EXPECT_TRUE(fabs(result.x - 5.0) < 0.001);
    EXPECT_TRUE(fabs(result.y - 10.0) < 0.001);
    EXPECT_TRUE(fabs(result.z - 15.0) < 0.001);
    
    vec3_destroy(ctx);
}

void test_vec3_zero() {
    TEST(Vec3_Zero);
    vec3_t result = vec3_zero();
    
    EXPECT_TRUE(fabs(result.x - 0.0) < 0.001);
    EXPECT_TRUE(fabs(result.y - 0.0) < 0.001);
    EXPECT_TRUE(fabs(result.z - 0.0) < 0.001);
}

void test_vec3_unit() {
    TEST(Vec3_Unit);
    vec3_t x = vec3_unit_x();
    vec3_t y = vec3_unit_y();
    vec3_t z = vec3_unit_z();
    
    EXPECT_TRUE(fabs(x.x - 1.0) < 0.001);
    EXPECT_TRUE(fabs(y.y - 1.0) < 0.001);
    EXPECT_TRUE(fabs(z.z - 1.0) < 0.001);
}

void test_vec3_strerror() {
    TEST(Vec3_Strerror);
    const char* msg = vec3_strerror(VEC3_OK);
    EXPECT_TRUE(msg != NULL);
    
    msg = vec3_strerror(VEC3_DIVISION_BY_ZERO);
    EXPECT_TRUE(msg != NULL);
}

int main() {
    test_vec3_add();
    test_vec3_sub();
    test_vec3_dot();
    test_vec3_cross();
    test_vec3_length();
    test_vec3_normalize();
    test_vec3_scale();
    test_vec3_distance();
    test_vec3_create_destroy();
    test_vec3_divide();
    test_vec3_zero();
    test_vec3_unit();
    test_vec3_strerror();

    return 0;
}
