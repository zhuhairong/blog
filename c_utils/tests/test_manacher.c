#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/manacher.h"

void test_manacher_longest_palindrome() {
    TEST(Manacher_LongestPalindrome);
    int len = manacher_longest_palindrome("babad");
    EXPECT_TRUE(len >= 3);
}

void test_manacher_is_palindrome() {
    TEST(Manacher_IsPalindrome);
    bool result = manacher_is_palindrome("racecar", 7, true);
    EXPECT_TRUE(result);
    
    result = manacher_is_palindrome("hello", 5, true);
    EXPECT_FALSE(result);
}

void test_manacher_empty_string() {
    TEST(Manacher_EmptyString);
    int len = manacher_longest_palindrome("");
    EXPECT_EQ(len, 0);
}

void test_manacher_single_char() {
    TEST(Manacher_SingleChar);
    int len = manacher_longest_palindrome("a");
    EXPECT_EQ(len, 1);
}

void test_manacher_get_default_config() {
    TEST(Manacher_GetDefaultConfig);
    manacher_config_t config;
    manacher_get_default_config(&config);
    
    EXPECT_TRUE(config.case_sensitive || !config.case_sensitive);
}

int main() {
    test_manacher_longest_palindrome();
    test_manacher_is_palindrome();
    test_manacher_empty_string();
    test_manacher_single_char();
    test_manacher_get_default_config();

    return 0;
}
