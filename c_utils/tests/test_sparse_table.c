#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/sparse_table.h"

void test_sparse_table_init() {
    TEST(SparseTable_Init);
    int arr[] = {1, 3, 2, 7, 9, 11};
    sparse_table_t st;
    memset(&st, 0, sizeof(st));
    
    sparse_table_init(&st, arr, 6);
    
    EXPECT_TRUE(st.st != NULL);
    EXPECT_EQ(st.n, (size_t)6);
    
    sparse_table_free(&st);
}

void test_sparse_table_query_min() {
    TEST(SparseTable_QueryMin);
    int arr[] = {1, 3, 2, 7, 9, 11};
    sparse_table_t st;
    memset(&st, 0, sizeof(st));
    
    sparse_table_init(&st, arr, 6);
    
    int result = sparse_table_query(&st, 0, 2);
    EXPECT_EQ(result, 1);
    
    result = sparse_table_query(&st, 2, 4);
    EXPECT_EQ(result, 2);
    
    sparse_table_free(&st);
}

void test_sparse_table_query_single() {
    TEST(SparseTable_QuerySingle);
    int arr[] = {5, 3, 7, 1, 9};
    sparse_table_t st;
    memset(&st, 0, sizeof(st));
    
    sparse_table_init(&st, arr, 5);
    
    int result = sparse_table_query(&st, 2, 2);
    EXPECT_EQ(result, 7);
    
    sparse_table_free(&st);
}

void test_sparse_table_query_full() {
    TEST(SparseTable_QueryFull);
    int arr[] = {5, 3, 7, 1, 9};
    sparse_table_t st;
    memset(&st, 0, sizeof(st));
    
    sparse_table_init(&st, arr, 5);
    
    int result = sparse_table_query(&st, 0, 4);
    EXPECT_EQ(result, 1);
    
    sparse_table_free(&st);
}

void test_sparse_table_free() {
    TEST(SparseTable_Free);
    int arr[] = {1, 2, 3};
    sparse_table_t st;
    memset(&st, 0, sizeof(st));
    
    sparse_table_init(&st, arr, 3);
    sparse_table_free(&st);
    
    EXPECT_TRUE(1);
}

int main() {
    test_sparse_table_init();
    test_sparse_table_query_min();
    test_sparse_table_query_single();
    test_sparse_table_query_full();
    test_sparse_table_free();

    return 0;
}
