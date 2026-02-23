#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/fenwick_tree.h"

void test_fenwick_create() {
    TEST(Fenwick_Create);
    fenwick_error_t error;
    fenwick_tree_t* ft = fenwick_tree_create(10, FENWICK_TYPE_INT, &error);
    EXPECT_TRUE(ft != NULL);
    EXPECT_EQ(error, FENWICK_OK);
    fenwick_tree_free(ft);
}

void test_fenwick_create_zero() {
    TEST(Fenwick_CreateZero);
    fenwick_error_t error;
    fenwick_tree_t* ft = fenwick_tree_create(0, FENWICK_TYPE_INT, &error);
    fenwick_tree_free(ft);
}

void test_fenwick_update_query() {
    TEST(Fenwick_UpdateQuery);
    fenwick_error_t error;
    fenwick_tree_t* ft = fenwick_tree_create(5, FENWICK_TYPE_INT, &error);
    
    fenwick_tree_update(ft, 1, 10.0, &error);
    fenwick_tree_update(ft, 2, 20.0, &error);
    fenwick_tree_update(ft, 3, 30.0, &error);
    
    double result = 0;
    fenwick_tree_query(ft, 3, &result, &error);
    EXPECT_EQ(error, FENWICK_OK);
    EXPECT_EQ(result, 60.0);
    
    fenwick_tree_free(ft);
}

void test_fenwick_range_query() {
    TEST(Fenwick_RangeQuery);
    fenwick_error_t error;
    fenwick_tree_t* ft = fenwick_tree_create(5, FENWICK_TYPE_INT, &error);
    
    fenwick_tree_update(ft, 1, 10.0, &error);
    fenwick_tree_update(ft, 2, 20.0, &error);
    fenwick_tree_update(ft, 3, 30.0, &error);
    fenwick_tree_update(ft, 4, 40.0, &error);
    
    double result = 0;
    fenwick_tree_range_query(ft, 2, 4, &result, &error);
    EXPECT_EQ(error, FENWICK_OK);
    EXPECT_EQ(result, 90.0);
    
    fenwick_tree_free(ft);
}

void test_fenwick_size() {
    TEST(Fenwick_Size);
    fenwick_error_t error;
    fenwick_tree_t* ft = fenwick_tree_create(10, FENWICK_TYPE_INT, &error);
    
    size_t size = fenwick_tree_size(ft);
    EXPECT_EQ(size, (size_t)10);
    
    fenwick_tree_free(ft);
}

void test_fenwick_type_float() {
    TEST(Fenwick_TypeFloat);
    fenwick_error_t error;
    fenwick_tree_t* ft = fenwick_tree_create(5, FENWICK_TYPE_FLOAT, &error);
    EXPECT_TRUE(ft != NULL);
    
    fenwick_tree_update(ft, 1, 1.5, &error);
    fenwick_tree_update(ft, 2, 2.5, &error);
    
    double result = 0;
    fenwick_tree_query(ft, 2, &result, &error);
    EXPECT_EQ(error, FENWICK_OK);
    
    fenwick_tree_free(ft);
}

void test_fenwick_type_double() {
    TEST(Fenwick_TypeDouble);
    fenwick_error_t error;
    fenwick_tree_t* ft = fenwick_tree_create(5, FENWICK_TYPE_DOUBLE, &error);
    EXPECT_TRUE(ft != NULL);
    
    fenwick_tree_update(ft, 1, 1.5, &error);
    fenwick_tree_update(ft, 2, 2.5, &error);
    
    double result = 0;
    fenwick_tree_query(ft, 2, &result, &error);
    EXPECT_EQ(error, FENWICK_OK);
    
    fenwick_tree_free(ft);
}

void test_fenwick_strerror() {
    TEST(Fenwick_Strerror);
    const char* msg = fenwick_strerror(FENWICK_OK);
    EXPECT_TRUE(msg != NULL);
    
    msg = fenwick_strerror(FENWICK_ERROR_INVALID_PARAM);
    EXPECT_TRUE(msg != NULL);
}

void test_fenwick_free_null() {
    TEST(Fenwick_FreeNull);
    fenwick_tree_free(NULL);
}

void test_fenwick_multiple_updates() {
    TEST(Fenwick_MultipleUpdates);
    fenwick_error_t error;
    fenwick_tree_t* ft = fenwick_tree_create(100, FENWICK_TYPE_INT, &error);
    
    for (size_t i = 1; i <= 100; i++) {
        fenwick_tree_update(ft, i, (double)i, &error);
    }
    
    double result = 0;
    fenwick_tree_query(ft, 100, &result, &error);
    EXPECT_EQ(error, FENWICK_OK);
    EXPECT_EQ(result, 5050.0);
    
    fenwick_tree_free(ft);
}

int main() {
    test_fenwick_create();
    test_fenwick_create_zero();
    test_fenwick_update_query();
    test_fenwick_range_query();
    test_fenwick_size();
    test_fenwick_type_float();
    test_fenwick_type_double();
    test_fenwick_strerror();
    test_fenwick_free_null();
    test_fenwick_multiple_updates();

    return 0;
}
