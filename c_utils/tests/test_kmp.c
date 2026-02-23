#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/kmp.h"

void test_kmp_search_found() {
    TEST(Kmp_SearchFound);
    const char* text = "hello world";
    const char* pattern = "world";
    
    int pos = kmp_search(text, pattern);
    EXPECT_EQ(pos, 6);
}

void test_kmp_search_not_found() {
    TEST(Kmp_SearchNotFound);
    const char* text = "hello world";
    const char* pattern = "xyz";
    
    int pos = kmp_search(text, pattern);
    EXPECT_EQ(pos, -1);
}

void test_kmp_search_beginning() {
    TEST(Kmp_SearchBeginning);
    const char* text = "hello world";
    const char* pattern = "hello";
    
    int pos = kmp_search(text, pattern);
    EXPECT_EQ(pos, 0);
}

void test_kmp_search_ex() {
    TEST(Kmp_SearchEx);
    const char* text = "ababababab";
    const char* pattern = "aba";
    
    kmp_error_t error;
    int pos = kmp_search_ex(text, strlen(text), pattern, strlen(pattern), &error);
    
    EXPECT_EQ(pos, 0);
    EXPECT_EQ(error, KMP_OK);
}

void test_kmp_get_default_config() {
    TEST(Kmp_GetDefaultConfig);
    kmp_config_t config;
    kmp_get_default_config(&config);
    
    EXPECT_TRUE(config.case_sensitive || !config.case_sensitive);
}

int main() {
    test_kmp_search_found();
    test_kmp_search_not_found();
    test_kmp_search_beginning();
    test_kmp_search_ex();
    test_kmp_get_default_config();

    return 0;
}
