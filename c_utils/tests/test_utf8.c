#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/utf8.h"

void test_utf8_is_valid() {
    TEST(UTF8_IsValid);
    EXPECT_TRUE(utf8_is_valid("hello"));
    EXPECT_TRUE(utf8_is_valid(""));
    EXPECT_TRUE(utf8_is_valid("中文测试"));
}

void test_utf8_strlen() {
    TEST(UTF8_Strlen);
    EXPECT_EQ(utf8_strlen("hello"), (size_t)5);
    EXPECT_EQ(utf8_strlen(""), (size_t)0);
    EXPECT_EQ(utf8_strlen("中文"), (size_t)2);
}

void test_utf8_strlen_ascii() {
    TEST(UTF8_StrlenAscii);
    const char* str = "Hello, World!";
    EXPECT_EQ(utf8_strlen(str), (size_t)13);
}

void test_utf8_strlen_mixed() {
    TEST(UTF8_StrlenMixed);
    const char* str = "Hello世界";
    EXPECT_EQ(utf8_strlen(str), (size_t)7);
}

void test_utf8_is_valid_null() {
    TEST(UTF8_IsValidNull);
    EXPECT_FALSE(utf8_is_valid(NULL));
}

void test_utf8_strlen_null() {
    TEST(UTF8_StrlenNull);
    EXPECT_EQ(utf8_strlen(NULL), (size_t)0);
}

int main() {
    test_utf8_is_valid();
    test_utf8_strlen();
    test_utf8_strlen_ascii();
    test_utf8_strlen_mixed();
    test_utf8_is_valid_null();
    test_utf8_strlen_null();

    return 0;
}
