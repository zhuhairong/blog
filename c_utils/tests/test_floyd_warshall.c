#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/floyd_warshall.h"

void test_floyd_warshall_types() {
    TEST(FloydWarshall_Types);
    floyd_result_t result;
    memset(&result, 0, sizeof(result));
    EXPECT_TRUE(sizeof(result) > 0);
}

void test_floyd_warshall_error_values() {
    TEST(FloydWarshall_ErrorValues);
    EXPECT_TRUE(FLOYD_OK == 0);
    EXPECT_TRUE(FLOYD_ERROR_INVALID_PARAM != 0);
    EXPECT_TRUE(FLOYD_ERROR_MEMORY_ALLOC != 0);
}

void test_floyd_inf_value() {
    TEST(FloydWarshall_InfValue);
    EXPECT_TRUE(FLOYD_INF == 1000000);
}

void test_floyd_warshall_result_fields() {
    TEST(FloydWarshall_ResultFields);
    floyd_result_t result;
    memset(&result, 0, sizeof(result));
    
    result.nodes = 5;
    result.has_negative_cycle = false;
    result.has_error = false;
    
    EXPECT_EQ(result.nodes, 5);
    EXPECT_FALSE(result.has_negative_cycle);
    EXPECT_FALSE(result.has_error);
}

void test_floyd_warshall_free_null() {
    TEST(FloydWarshall_FreeNull);
    floyd_warshall_free(NULL);
    EXPECT_TRUE(true);
}

int main() {
    test_floyd_warshall_types();
    test_floyd_warshall_error_values();
    test_floyd_inf_value();
    test_floyd_warshall_result_fields();
    test_floyd_warshall_free_null();

    return 0;
}
