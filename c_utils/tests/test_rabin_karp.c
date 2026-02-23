#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/rabin_karp.h"

void test_rabin_karp_search_found() {
    TEST(RabinKarp_SearchFound);
    const char* text = "hello world";
    const char* pattern = "world";
    
    int pos = rabin_karp_search(text, pattern);
    EXPECT_EQ(pos, 6);
}

void test_rabin_karp_search_not_found() {
    TEST(RabinKarp_SearchNotFound);
    const char* text = "hello world";
    const char* pattern = "xyz";
    
    int pos = rabin_karp_search(text, pattern);
    EXPECT_EQ(pos, -1);
}

void test_rabin_karp_search_beginning() {
    TEST(RabinKarp_SearchBeginning);
    const char* text = "hello world";
    const char* pattern = "hello";
    
    int pos = rabin_karp_search(text, pattern);
    EXPECT_EQ(pos, 0);
}

void test_rabin_karp_search_single_char() {
    TEST(RabinKarp_SearchSingleChar);
    const char* text = "abcdef";
    const char* pattern = "d";
    
    int pos = rabin_karp_search(text, pattern);
    EXPECT_EQ(pos, 3);
}

void test_rabin_karp_search_middle() {
    TEST(RabinKarp_SearchMiddle);
    const char* text = "abcdefgh";
    const char* pattern = "cde";
    
    int pos = rabin_karp_search(text, pattern);
    EXPECT_EQ(pos, 2);
}

int main() {
    test_rabin_karp_search_found();
    test_rabin_karp_search_not_found();
    test_rabin_karp_search_beginning();
    test_rabin_karp_search_single_char();
    test_rabin_karp_search_middle();

    return 0;
}
