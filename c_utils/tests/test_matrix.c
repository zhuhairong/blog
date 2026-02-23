#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../c_utils/utest.h"
#include "../c_utils/matrix.h"

void test_matrix_create() {
    TEST(Matrix_Create);
    matrix_t* m = matrix_create(3, 4);
    EXPECT_TRUE(m != NULL);
    EXPECT_EQ(m->rows, (size_t)3);
    EXPECT_EQ(m->cols, (size_t)4);
    EXPECT_TRUE(m->data != NULL);
    matrix_free(m);
}

void test_matrix_create_zero_size() {
    TEST(Matrix_CreateZeroSize);
    matrix_t* m = matrix_create(0, 0);
    matrix_free(m);
}

void test_matrix_set_get() {
    TEST(Matrix_SetGet);
    matrix_t* m = matrix_create(3, 3);
    
    matrix_set(m, 0, 0, 1.0);
    matrix_set(m, 1, 1, 2.0);
    matrix_set(m, 2, 2, 3.0);
    
    EXPECT_EQ(matrix_get(m, 0, 0), 1.0);
    EXPECT_EQ(matrix_get(m, 1, 1), 2.0);
    EXPECT_EQ(matrix_get(m, 2, 2), 3.0);
    
    matrix_free(m);
}

void test_matrix_fill() {
    TEST(Matrix_Fill);
    matrix_t* m = matrix_create(2, 2);
    
    matrix_fill(m, 5.0);
    
    for (size_t i = 0; i < m->rows; i++) {
        for (size_t j = 0; j < m->cols; j++) {
            EXPECT_EQ(matrix_get(m, i, j), 5.0);
        }
    }
    
    matrix_free(m);
}

void test_matrix_zero() {
    TEST(Matrix_Zero);
    matrix_t* m = matrix_create(2, 2);
    
    matrix_fill(m, 5.0);
    matrix_zero(m);
    
    for (size_t i = 0; i < m->rows; i++) {
        for (size_t j = 0; j < m->cols; j++) {
            EXPECT_EQ(matrix_get(m, i, j), 0.0);
        }
    }
    
    matrix_free(m);
}

void test_matrix_copy() {
    TEST(Matrix_Copy);
    matrix_t* m1 = matrix_create(2, 2);
    matrix_set(m1, 0, 0, 1.0);
    matrix_set(m1, 0, 1, 2.0);
    matrix_set(m1, 1, 0, 3.0);
    matrix_set(m1, 1, 1, 4.0);
    
    matrix_t* m2 = matrix_copy(m1);
    EXPECT_TRUE(m2 != NULL);
    EXPECT_TRUE(matrix_equal(m1, m2, 0.0001));
    
    matrix_free(m1);
    matrix_free(m2);
}

void test_matrix_add() {
    TEST(Matrix_Add);
    matrix_t* a = matrix_create(2, 2);
    matrix_t* b = matrix_create(2, 2);
    
    matrix_set(a, 0, 0, 1.0);
    matrix_set(a, 0, 1, 2.0);
    matrix_set(a, 1, 0, 3.0);
    matrix_set(a, 1, 1, 4.0);
    
    matrix_set(b, 0, 0, 5.0);
    matrix_set(b, 0, 1, 6.0);
    matrix_set(b, 1, 0, 7.0);
    matrix_set(b, 1, 1, 8.0);
    
    matrix_t* c = matrix_add(a, b);
    EXPECT_TRUE(c != NULL);
    EXPECT_EQ(matrix_get(c, 0, 0), 6.0);
    EXPECT_EQ(matrix_get(c, 0, 1), 8.0);
    EXPECT_EQ(matrix_get(c, 1, 0), 10.0);
    EXPECT_EQ(matrix_get(c, 1, 1), 12.0);
    
    matrix_free(a);
    matrix_free(b);
    matrix_free(c);
}

void test_matrix_sub() {
    TEST(Matrix_Sub);
    matrix_t* a = matrix_create(2, 2);
    matrix_t* b = matrix_create(2, 2);
    
    matrix_set(a, 0, 0, 5.0);
    matrix_set(a, 0, 1, 6.0);
    matrix_set(a, 1, 0, 7.0);
    matrix_set(a, 1, 1, 8.0);
    
    matrix_set(b, 0, 0, 1.0);
    matrix_set(b, 0, 1, 2.0);
    matrix_set(b, 1, 0, 3.0);
    matrix_set(b, 1, 1, 4.0);
    
    matrix_t* c = matrix_sub(a, b);
    EXPECT_TRUE(c != NULL);
    EXPECT_EQ(matrix_get(c, 0, 0), 4.0);
    EXPECT_EQ(matrix_get(c, 0, 1), 4.0);
    EXPECT_EQ(matrix_get(c, 1, 0), 4.0);
    EXPECT_EQ(matrix_get(c, 1, 1), 4.0);
    
    matrix_free(a);
    matrix_free(b);
    matrix_free(c);
}

void test_matrix_mul() {
    TEST(Matrix_Mul);
    matrix_t* a = matrix_create(2, 3);
    matrix_t* b = matrix_create(3, 2);
    
    matrix_set(a, 0, 0, 1.0);
    matrix_set(a, 0, 1, 2.0);
    matrix_set(a, 0, 2, 3.0);
    matrix_set(a, 1, 0, 4.0);
    matrix_set(a, 1, 1, 5.0);
    matrix_set(a, 1, 2, 6.0);
    
    matrix_set(b, 0, 0, 7.0);
    matrix_set(b, 0, 1, 8.0);
    matrix_set(b, 1, 0, 9.0);
    matrix_set(b, 1, 1, 10.0);
    matrix_set(b, 2, 0, 11.0);
    matrix_set(b, 2, 1, 12.0);
    
    matrix_t* c = matrix_mul(a, b);
    EXPECT_TRUE(c != NULL);
    EXPECT_EQ(c->rows, (size_t)2);
    EXPECT_EQ(c->cols, (size_t)2);
    
    matrix_free(a);
    matrix_free(b);
    matrix_free(c);
}

void test_matrix_transpose() {
    TEST(Matrix_Transpose);
    matrix_t* m = matrix_create(2, 3);
    
    matrix_set(m, 0, 0, 1.0);
    matrix_set(m, 0, 1, 2.0);
    matrix_set(m, 0, 2, 3.0);
    matrix_set(m, 1, 0, 4.0);
    matrix_set(m, 1, 1, 5.0);
    matrix_set(m, 1, 2, 6.0);
    
    matrix_t* t = matrix_transpose(m);
    EXPECT_TRUE(t != NULL);
    EXPECT_EQ(t->rows, (size_t)3);
    EXPECT_EQ(t->cols, (size_t)2);
    EXPECT_EQ(matrix_get(t, 0, 0), 1.0);
    EXPECT_EQ(matrix_get(t, 0, 1), 4.0);
    EXPECT_EQ(matrix_get(t, 1, 0), 2.0);
    EXPECT_EQ(matrix_get(t, 1, 1), 5.0);
    
    matrix_free(m);
    matrix_free(t);
}

void test_matrix_scalar_mul() {
    TEST(Matrix_ScalarMul);
    matrix_t* m = matrix_create(2, 2);
    
    matrix_set(m, 0, 0, 1.0);
    matrix_set(m, 0, 1, 2.0);
    matrix_set(m, 1, 0, 3.0);
    matrix_set(m, 1, 1, 4.0);
    
    matrix_t* s = matrix_scalar_mul(m, 2.0);
    EXPECT_TRUE(s != NULL);
    EXPECT_EQ(matrix_get(s, 0, 0), 2.0);
    EXPECT_EQ(matrix_get(s, 0, 1), 4.0);
    EXPECT_EQ(matrix_get(s, 1, 0), 6.0);
    EXPECT_EQ(matrix_get(s, 1, 1), 8.0);
    
    matrix_free(m);
    matrix_free(s);
}

void test_matrix_identity() {
    TEST(Matrix_Identity);
    matrix_t* m = matrix_identity(3);
    EXPECT_TRUE(m != NULL);
    
    for (size_t i = 0; i < 3; i++) {
        for (size_t j = 0; j < 3; j++) {
            if (i == j) {
                EXPECT_EQ(matrix_get(m, i, j), 1.0);
            } else {
                EXPECT_EQ(matrix_get(m, i, j), 0.0);
            }
        }
    }
    
    matrix_free(m);
}

void test_matrix_equal() {
    TEST(Matrix_Equal);
    matrix_t* a = matrix_create(2, 2);
    matrix_t* b = matrix_create(2, 2);
    matrix_t* c = matrix_create(2, 2);
    
    matrix_set(a, 0, 0, 1.0);
    matrix_set(a, 0, 1, 2.0);
    matrix_set(a, 1, 0, 3.0);
    matrix_set(a, 1, 1, 4.0);
    
    matrix_set(b, 0, 0, 1.0);
    matrix_set(b, 0, 1, 2.0);
    matrix_set(b, 1, 0, 3.0);
    matrix_set(b, 1, 1, 4.0);
    
    matrix_set(c, 0, 0, 1.0);
    matrix_set(c, 0, 1, 2.0);
    matrix_set(c, 1, 0, 3.0);
    matrix_set(c, 1, 1, 5.0);
    
    EXPECT_TRUE(matrix_equal(a, b, 0.0001));
    EXPECT_FALSE(matrix_equal(a, c, 0.0001));
    
    matrix_free(a);
    matrix_free(b);
    matrix_free(c);
}

void test_matrix_is_square() {
    TEST(Matrix_IsSquare);
    matrix_t* a = matrix_create(3, 3);
    matrix_t* b = matrix_create(2, 3);
    
    EXPECT_TRUE(matrix_is_square(a));
    EXPECT_FALSE(matrix_is_square(b));
    
    matrix_free(a);
    matrix_free(b);
}

void test_matrix_trace() {
    TEST(Matrix_Trace);
    matrix_t* m = matrix_create(3, 3);
    
    matrix_set(m, 0, 0, 1.0);
    matrix_set(m, 1, 1, 2.0);
    matrix_set(m, 2, 2, 3.0);
    
    EXPECT_EQ(matrix_trace(m), 6.0);
    
    matrix_free(m);
}

void test_matrix_free_null() {
    TEST(Matrix_FreeNull);
    matrix_free(NULL);
}

int main() {
    test_matrix_create();
    test_matrix_create_zero_size();
    test_matrix_set_get();
    test_matrix_fill();
    test_matrix_zero();
    test_matrix_copy();
    test_matrix_add();
    test_matrix_sub();
    test_matrix_mul();
    test_matrix_transpose();
    test_matrix_scalar_mul();
    test_matrix_identity();
    test_matrix_equal();
    test_matrix_is_square();
    test_matrix_trace();
    test_matrix_free_null();

    return 0;
}
