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
    
    EXPECT_TRUE(sa[0] >= 0 && sa[0] < 6);
}

void test_suffix_array_build_sorted() {
    TEST(SuffixArray_BuildSorted);
    const char* s = "abc";
    int sa[10] = {0};
    
    suffix_array_build(s, sa);
    
    EXPECT_TRUE(sa[0] >= 0 && sa[0] < 3);
    EXPECT_TRUE(sa[1] >= 0 && sa[1] < 3);
    EXPECT_TRUE(sa[2] >= 0 && sa[2] < 3);
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
    
    EXPECT_TRUE(sa[0] >= 0 && sa[0] < 4);
}

int main() {
    test_suffix_array_build();
    test_suffix_array_build_sorted();
    test_suffix_array_single_char();
    test_suffix_array_empty();
    test_suffix_array_repeated();

    return 0;
}
