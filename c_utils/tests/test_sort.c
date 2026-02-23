#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/sort.h"

static int int_compare(const void *a, const void *b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return (ia > ib) - (ia < ib);
}

void test_sort_quicksort() {
    TEST(Sort_Quicksort);
    int arr[] = {5, 2, 8, 1, 9, 3, 7, 4, 6};
    
    sort_quicksort(arr, 9, sizeof(int), int_compare);
    
    for (int i = 0; i < 8; i++) {
        EXPECT_TRUE(arr[i] <= arr[i+1]);
    }
}

void test_sort_quicksort_sorted() {
    TEST(Sort_QuicksortSorted);
    int arr[] = {1, 2, 3, 4, 5};
    
    sort_quicksort(arr, 5, sizeof(int), int_compare);
    
    for (int i = 0; i < 4; i++) {
        EXPECT_TRUE(arr[i] <= arr[i+1]);
    }
}

void test_sort_quicksort_reverse() {
    TEST(Sort_QuicksortReverse);
    int arr[] = {5, 4, 3, 2, 1};
    
    sort_quicksort(arr, 5, sizeof(int), int_compare);
    
    for (int i = 0; i < 4; i++) {
        EXPECT_TRUE(arr[i] <= arr[i+1]);
    }
}

void test_sort_quicksort_duplicates() {
    TEST(Sort_QuicksortDuplicates);
    int arr[] = {3, 1, 3, 2, 3, 1, 2};
    
    sort_quicksort(arr, 7, sizeof(int), int_compare);
    
    for (int i = 0; i < 6; i++) {
        EXPECT_TRUE(arr[i] <= arr[i+1]);
    }
}

void test_sort_quicksort_single() {
    TEST(Sort_QuicksortSingle);
    int arr[] = {42};
    
    sort_quicksort(arr, 1, sizeof(int), int_compare);
    
    EXPECT_EQ(arr[0], 42);
}

void test_sort_quicksort_two() {
    TEST(Sort_QuicksortTwo);
    int arr[] = {2, 1};
    
    sort_quicksort(arr, 2, sizeof(int), int_compare);
    
    EXPECT_TRUE(arr[0] <= arr[1]);
}

void test_sort_quicksort_empty() {
    TEST(Sort_QuicksortEmpty);
    int arr[] = {1};
    
    sort_quicksort(arr, 0, sizeof(int), int_compare);
    
    EXPECT_EQ(arr[0], 1);
}

void test_sort_quicksort_negative() {
    TEST(Sort_QuicksortNegative);
    int arr[] = {-3, -1, -2, -5, -4};
    
    sort_quicksort(arr, 5, sizeof(int), int_compare);
    
    for (int i = 0; i < 4; i++) {
        EXPECT_TRUE(arr[i] <= arr[i+1]);
    }
}

int main() {
    test_sort_quicksort();
    test_sort_quicksort_sorted();
    test_sort_quicksort_reverse();
    test_sort_quicksort_duplicates();
    test_sort_quicksort_single();
    test_sort_quicksort_two();
    test_sort_quicksort_empty();
    test_sort_quicksort_negative();

    return 0;
}
