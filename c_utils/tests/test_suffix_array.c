#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/suffix_array.h"

void test_suffix_array_build() {
    TEST(SuffixArray_Build);
    const char* s = "banana";
    int sa[10] = {0};
    
    suffix_array_build(s, sa);
    
    /* "banana" suffixes sorted: "a"(5),"ana"(3),"anana"(1),"banana"(0),"na"(4),"nana"(2) */
    int expected[] = {5, 3, 1, 0, 4, 2};
    EXPECT_TRUE(memcmp(sa, expected, 6 * sizeof(int)) == 0);
}

void test_suffix_array_build_sorted() {
    TEST(SuffixArray_BuildSorted);
    const char* s = "abc";
    int sa[10] = {0};
    
    suffix_array_build(s, sa);
    
    /* "abc" suffixes sorted: "abc"(0),"bc"(1),"c"(2) */
    EXPECT_EQ(sa[0], 0);
    EXPECT_EQ(sa[1], 1);
    EXPECT_EQ(sa[2], 2);
}

void test_suffix_array_single_char() {
    TEST(SuffixArray_SingleChar);
    const char* s = "a";
    int sa[10] = {0};
    
    suffix_array_build(s, sa);
    
    EXPECT_EQ(sa[0], 0);
}

void test_suffix_array_empty() {
    TEST(SuffixArray_Empty);
    const char* s = "";
    int sa[10] = {0};
    
    suffix_array_build(s, sa);
}

void test_suffix_array_repeated() {
    TEST(SuffixArray_Repeated);
    const char* s = "aaaa";
    int sa[10] = {0};
    
    suffix_array_build(s, sa);
    
    /* "aaaa": all suffixes equal, indices in descending order */
    int expected[] = {3, 2, 1, 0};
    EXPECT_TRUE(memcmp(sa, expected, 4 * sizeof(int)) == 0);
}

int main() {
    UTEST_BEGIN();
    test_suffix_array_build();
    test_suffix_array_build_sorted();
    test_suffix_array_single_char();
    test_suffix_array_empty();
    test_suffix_array_repeated();

    UTEST_END();
}
