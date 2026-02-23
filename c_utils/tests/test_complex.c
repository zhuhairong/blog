#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../c_utils/utest.h"
#include "../c_utils/complex.h"

#define EPSILON 1e-10

void test_complex_add() {
    TEST(Complex_Add);
    complex_t a = {3.0, 2.0};
    complex_t b = {1.0, 4.0};
    complex_t c = complex_add(a, b);
    
    EXPECT_TRUE(fabs(c.real - 4.0) < EPSILON);
    EXPECT_TRUE(fabs(c.imag - 6.0) < EPSILON);
}

void test_complex_sub() {
    TEST(Complex_Sub);
    complex_t a = {5.0, 7.0};
    complex_t b = {2.0, 3.0};
    complex_t c = complex_sub(a, b);
    
    EXPECT_TRUE(fabs(c.real - 3.0) < EPSILON);
    EXPECT_TRUE(fabs(c.imag - 4.0) < EPSILON);
}

void test_complex_mul() {
    TEST(Complex_Mul);
    complex_t a = {2.0, 3.0};
    complex_t b = {1.0, 2.0};
    complex_t c = complex_mul(a, b);
    
    EXPECT_TRUE(fabs(c.real - (-4.0)) < EPSILON);
    EXPECT_TRUE(fabs(c.imag - 7.0) < EPSILON);
}

void test_complex_div() {
    TEST(Complex_Div);
    complex_t a = {1.0, 1.0};
    complex_t b = {1.0, -1.0};
    complex_error_t error;
    complex_t c = complex_div(a, b, &error);
    
    EXPECT_EQ(error, COMPLEX_OK);
    EXPECT_TRUE(fabs(c.real - 0.0) < EPSILON);
    EXPECT_TRUE(fabs(c.imag - 1.0) < EPSILON);
}

void test_complex_div_by_zero() {
    TEST(Complex_DivByZero);
    complex_t a = {1.0, 1.0};
    complex_t b = {0.0, 0.0};
    complex_error_t error;
    complex_div(a, b, &error);
    
    EXPECT_EQ(error, COMPLEX_ERROR_DIVISION_BY_ZERO);
}

void test_complex_abs() {
    TEST(Complex_Abs);
    complex_t a = {3.0, 4.0};
    double abs_val = complex_abs(a);
    
    EXPECT_TRUE(fabs(abs_val - 5.0) < EPSILON);
}

void test_complex_conj() {
    TEST(Complex_Conj);
    complex_t a = {3.0, 4.0};
    complex_t c = complex_conj(a);
    
    EXPECT_TRUE(fabs(c.real - 3.0) < EPSILON);
    EXPECT_TRUE(fabs(c.imag - (-4.0)) < EPSILON);
}

void test_complex_arg() {
    TEST(Complex_Arg);
    complex_t a = {1.0, 1.0};
    double arg = complex_arg(a);
    
    EXPECT_TRUE(fabs(arg - M_PI_4) < EPSILON);
}

void test_complex_exp() {
    TEST(Complex_Exp);
    complex_t a = {0.0, M_PI};
    complex_t c = complex_exp(a);
    
    EXPECT_TRUE(fabs(c.real - (-1.0)) < EPSILON);
    EXPECT_TRUE(fabs(c.imag - 0.0) < EPSILON);
}

void test_complex_log() {
    TEST(Complex_Log);
    complex_t a = {1.0, 0.0};
    complex_error_t error;
    complex_t c = complex_log(a, &error);
    
    EXPECT_EQ(error, COMPLEX_OK);
    EXPECT_TRUE(fabs(c.real - 0.0) < EPSILON);
    EXPECT_TRUE(fabs(c.imag - 0.0) < EPSILON);
}

void test_complex_sqrt() {
    TEST(Complex_Sqrt);
    complex_t a = {4.0, 0.0};
    complex_t c = complex_sqrt(a);
    
    EXPECT_TRUE(fabs(c.real - 2.0) < EPSILON);
    EXPECT_TRUE(fabs(c.imag - 0.0) < EPSILON);
}

void test_complex_sqrt_negative() {
    TEST(Complex_SqrtNegative);
    complex_t a = {-4.0, 0.0};
    complex_t c = complex_sqrt(a);
    
    EXPECT_TRUE(fabs(c.real - 0.0) < EPSILON);
    EXPECT_TRUE(fabs(c.imag - 2.0) < EPSILON);
}

void test_complex_sin() {
    TEST(Complex_Sin);
    complex_t a = {0.0, 0.0};
    complex_t c = complex_sin(a);
    
    EXPECT_TRUE(fabs(c.real - 0.0) < EPSILON);
    EXPECT_TRUE(fabs(c.imag - 0.0) < EPSILON);
}

void test_complex_cos() {
    TEST(Complex_Cos);
    complex_t a = {0.0, 0.0};
    complex_t c = complex_cos(a);
    
    EXPECT_TRUE(fabs(c.real - 1.0) < EPSILON);
    EXPECT_TRUE(fabs(c.imag - 0.0) < EPSILON);
}

void test_complex_polar() {
    TEST(Complex_Polar);
    complex_t c = complex_polar(1.0, M_PI_2);
    
    EXPECT_TRUE(fabs(c.real - 0.0) < EPSILON);
    EXPECT_TRUE(fabs(c.imag - 1.0) < EPSILON);
}

void test_complex_is_real() {
    TEST(Complex_IsReal);
    complex_t a = {5.0, 0.0};
    complex_t b = {5.0, 3.0};
    
    EXPECT_TRUE(complex_is_real(a));
    EXPECT_FALSE(complex_is_real(b));
}

void test_complex_is_imag() {
    TEST(Complex_IsImag);
    complex_t a = {0.0, 5.0};
    complex_t b = {3.0, 5.0};
    
    EXPECT_TRUE(complex_is_imag(a));
    EXPECT_FALSE(complex_is_imag(b));
}

void test_complex_equal() {
    TEST(Complex_Equal);
    complex_t a = {1.0, 2.0};
    complex_t b = {1.0, 2.0};
    complex_t c = {1.0, 3.0};
    
    EXPECT_TRUE(complex_equal(a, b, EPSILON));
    EXPECT_FALSE(complex_equal(a, c, EPSILON));
}

void test_complex_strerror() {
    TEST(Complex_Strerror);
    const char* msg = complex_strerror(COMPLEX_OK);
    EXPECT_TRUE(msg != NULL);
    
    msg = complex_strerror(COMPLEX_ERROR_DIVISION_BY_ZERO);
    EXPECT_TRUE(msg != NULL);
}

void test_complex_constants() {
    TEST(Complex_Constants);
    EXPECT_TRUE(complex_is_real(COMPLEX_ONE));
    EXPECT_TRUE(complex_is_imag(COMPLEX_I));
    EXPECT_TRUE(complex_is_real(COMPLEX_ZERO));
}

int main() {
    test_complex_add();
    test_complex_sub();
    test_complex_mul();
    test_complex_div();
    test_complex_div_by_zero();
    test_complex_abs();
    test_complex_conj();
    test_complex_arg();
    test_complex_exp();
    test_complex_log();
    test_complex_sqrt();
    test_complex_sqrt_negative();
    test_complex_sin();
    test_complex_cos();
    test_complex_polar();
    test_complex_is_real();
    test_complex_is_imag();
    test_complex_equal();
    test_complex_strerror();
    test_complex_constants();

    return 0;
}
