#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/binary_search.h"

static int int_compare(const void *a, const void *b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return (ia > ib) - (ia < ib);
}

void test_binary_search_found() {
    TEST(BinarySearch_Found);
    int arr[] = {1, 3, 5, 7, 9, 11, 13};
    int key = 7;
    
    void* result = binary_search(&key, arr, 7, sizeof(int), int_compare);
    
    EXPECT_TRUE(result != NULL);
    EXPECT_EQ(*(int*)result, 7);
}

void test_binary_search_not_found() {
    TEST(BinarySearch_NotFound);
    int arr[] = {1, 3, 5, 7, 9, 11, 13};
    int key = 6;
    
    void* result = binary_search(&key, arr, 7, sizeof(int), int_compare);
    
    EXPECT_TRUE(result == NULL);
}

void test_binary_search_index() {
    TEST(BinarySearch_Index);
    int arr[] = {1, 3, 5, 7, 9, 11, 13};
    int key = 5;
    
    size_t index = binary_search_index(&key, arr, 7, sizeof(int), int_compare);
    
    EXPECT_EQ(index, (size_t)2);
}

void test_binary_search_first() {
    TEST(BinarySearch_First);
    int arr[] = {1, 3, 5, 7, 9, 11, 13};
    int key = 1;
    
    void* result = binary_search(&key, arr, 7, sizeof(int), int_compare);
    
    EXPECT_TRUE(result != NULL);
    EXPECT_EQ(*(int*)result, 1);
}

void test_binary_search_last() {
    TEST(BinarySearch_Last);
    int arr[] = {1, 3, 5, 7, 9, 11, 13};
    int key = 13;
    
    void* result = binary_search(&key, arr, 7, sizeof(int), int_compare);
    
    EXPECT_TRUE(result != NULL);
    EXPECT_EQ(*(int*)result, 13);
}

void test_binary_search_lower_bound() {
    TEST(BinarySearch_LowerBound);
    int arr[] = {1, 3, 5, 7, 9, 11, 13};
    int key = 6;
    
    void* result = binary_search_lower_bound(&key, arr, 7, sizeof(int), int_compare);
    
    EXPECT_TRUE(result != NULL);
    EXPECT_EQ(*(int*)result, 7);
}

void test_binary_search_upper_bound() {
    TEST(BinarySearch_UpperBound);
    int arr[] = {1, 3, 5, 7, 9, 11, 13};
    int key = 7;
    
    void* result = binary_search_upper_bound(&key, arr, 7, sizeof(int), int_compare);
    
    EXPECT_TRUE(result != NULL);
    EXPECT_EQ(*(int*)result, 9);
}

void test_binary_search_is_sorted() {
    TEST(BinarySearch_IsSorted);
    int sorted[] = {1, 2, 3, 4, 5};
    int unsorted[] = {5, 3, 1, 4, 2};
    
    EXPECT_TRUE(binary_search_is_sorted(sorted, 5, sizeof(int), int_compare));
    EXPECT_FALSE(binary_search_is_sorted(unsorted, 5, sizeof(int), int_compare));
}

int main() {
    test_binary_search_found();
    test_binary_search_not_found();
    test_binary_search_index();
    test_binary_search_first();
    test_binary_search_last();
    test_binary_search_lower_bound();
    test_binary_search_upper_bound();
    test_binary_search_is_sorted();

    return 0;
}
