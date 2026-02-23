#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/dstr.h"

void test_dstr_new() {
    TEST(Dstr_New);
    dstr s = dstr_new("hello");
    EXPECT_TRUE(s != NULL);
    EXPECT_EQ(dstr_len(s), (size_t)5);
    EXPECT_TRUE(strcmp(s, "hello") == 0);
    dstr_free(s);
}

void test_dstr_new_len() {
    TEST(Dstr_NewLen);
    dstr s = dstr_new_len("hello world", 5);
    EXPECT_TRUE(s != NULL);
    EXPECT_EQ(dstr_len(s), (size_t)5);
    EXPECT_TRUE(strcmp(s, "hello") == 0);
    dstr_free(s);
}

void test_dstr_new_empty() {
    TEST(Dstr_NewEmpty);
    dstr s = dstr_new_empty();
    EXPECT_TRUE(s != NULL);
    EXPECT_EQ(dstr_len(s), (size_t)0);
    EXPECT_TRUE(strcmp(s, "") == 0);
    dstr_free(s);
}

void test_dstr_free_null() {
    TEST(Dstr_FreeNull);
    dstr_free(NULL);
}

void test_dstr_clear() {
    TEST(Dstr_Clear);
    dstr s = dstr_new("hello");
    dstr_clear(s);
    EXPECT_EQ(dstr_len(s), (size_t)0);
    EXPECT_TRUE(strcmp(s, "") == 0);
    dstr_free(s);
}

void test_dstr_append() {
    TEST(Dstr_Append);
    dstr s = dstr_new("hello");
    s = dstr_append(s, " world");
    EXPECT_TRUE(s != NULL);
    EXPECT_EQ(dstr_len(s), (size_t)11);
    EXPECT_TRUE(strcmp(s, "hello world") == 0);
    dstr_free(s);
}

void test_dstr_append_len() {
    TEST(Dstr_AppendLen);
    dstr s = dstr_new("hello");
    s = dstr_append_len(s, " world!", 6);
    EXPECT_TRUE(s != NULL);
    EXPECT_EQ(dstr_len(s), (size_t)11);
    EXPECT_TRUE(strcmp(s, "hello world") == 0);
    dstr_free(s);
}

void test_dstr_append_char() {
    TEST(Dstr_AppendChar);
    dstr s = dstr_new("hello");
    s = dstr_append_char(s, '!');
    EXPECT_TRUE(s != NULL);
    EXPECT_EQ(dstr_len(s), (size_t)6);
    EXPECT_TRUE(strcmp(s, "hello!") == 0);
    dstr_free(s);
}

void test_dstr_prepend() {
    TEST(Dstr_Prepend);
    dstr s = dstr_new("world");
    s = dstr_prepend(s, "hello ");
    EXPECT_TRUE(s != NULL);
    EXPECT_EQ(dstr_len(s), (size_t)11);
    EXPECT_TRUE(strcmp(s, "hello world") == 0);
    dstr_free(s);
}

void test_dstr_prepend_len() {
    TEST(Dstr_PrependLen);
    dstr s = dstr_new("world");
    s = dstr_prepend_len(s, "hello ", 6);
    EXPECT_TRUE(s != NULL);
    EXPECT_EQ(dstr_len(s), (size_t)11);
    EXPECT_TRUE(strcmp(s, "hello world") == 0);
    dstr_free(s);
}

void test_dstr_substr() {
    TEST(Dstr_Substr);
    dstr s = dstr_new("hello world");
    dstr sub = dstr_substr(s, 0, 5);
    EXPECT_TRUE(sub != NULL);
    EXPECT_TRUE(strcmp(sub, "hello") == 0);
    dstr_free(sub);
    dstr_free(s);
}

void test_dstr_dup() {
    TEST(Dstr_Dup);
    dstr s = dstr_new("hello");
    dstr dup = dstr_dup(s);
    EXPECT_TRUE(dup != NULL);
    EXPECT_TRUE(strcmp(dup, "hello") == 0);
    EXPECT_TRUE(dup != s);
    dstr_free(dup);
    dstr_free(s);
}

void test_dstr_cmp() {
    TEST(Dstr_Cmp);
    dstr s1 = dstr_new("hello");
    dstr s2 = dstr_new("hello");
    dstr s3 = dstr_new("world");
    
    EXPECT_EQ(dstr_cmp(s1, s2), 0);
    EXPECT_TRUE(dstr_cmp(s1, s3) < 0);
    
    dstr_free(s1);
    dstr_free(s2);
    dstr_free(s3);
}

void test_dstr_casecmp() {
    TEST(Dstr_Casecmp);
    dstr s1 = dstr_new("Hello");
    dstr s2 = dstr_new("HELLO");
    
    EXPECT_EQ(dstr_casecmp(s1, s2), 0);
    
    dstr_free(s1);
    dstr_free(s2);
}

void test_dstr_starts_with() {
    TEST(Dstr_StartsWith);
    dstr s = dstr_new("hello world");
    
    EXPECT_TRUE(dstr_starts_with(s, "hello"));
    EXPECT_FALSE(dstr_starts_with(s, "world"));
    
    dstr_free(s);
}

void test_dstr_ends_with() {
    TEST(Dstr_EndsWith);
    dstr s = dstr_new("hello world");
    
    EXPECT_TRUE(dstr_ends_with(s, "world"));
    EXPECT_FALSE(dstr_ends_with(s, "hello"));
    
    dstr_free(s);
}

void test_dstr_contains() {
    TEST(Dstr_Contains);
    dstr s = dstr_new("hello world");
    
    EXPECT_TRUE(dstr_contains(s, "lo wo"));
    EXPECT_FALSE(dstr_contains(s, "xyz"));
    
    dstr_free(s);
}

void test_dstr_find() {
    TEST(Dstr_Find);
    dstr s = dstr_new("hello world");
    
    EXPECT_EQ(dstr_find(s, "world"), (ssize_t)6);
    EXPECT_EQ(dstr_find(s, "xyz"), (ssize_t)-1);
    
    dstr_free(s);
}

void test_dstr_rfind() {
    TEST(Dstr_Rfind);
    dstr s = dstr_new("hello hello");
    
    EXPECT_EQ(dstr_rfind(s, "hello"), (ssize_t)6);
    
    dstr_free(s);
}

void test_dstr_replace() {
    TEST(Dstr_Replace);
    dstr s = dstr_new("hello world");
    s = dstr_replace(s, "world", "there");
    EXPECT_TRUE(s != NULL);
    EXPECT_TRUE(strcmp(s, "hello there") == 0);
    dstr_free(s);
}

void test_dstr_trim() {
    TEST(Dstr_Trim);
    dstr s = dstr_new("  hello  ");
    dstr_trim(s, " ");
    EXPECT_EQ(dstr_len(s), (size_t)5);
    EXPECT_TRUE(strcmp(s, "hello") == 0);
    dstr_free(s);
}

void test_dstr_trim_left() {
    TEST(Dstr_TrimLeft);
    dstr s = dstr_new("  hello  ");
    dstr_trim_left(s, " ");
    EXPECT_EQ(dstr_len(s), (size_t)7);
    EXPECT_TRUE(strcmp(s, "hello  ") == 0);
    dstr_free(s);
}

void test_dstr_trim_right() {
    TEST(Dstr_TrimRight);
    dstr s = dstr_new("  hello  ");
    dstr_trim_right(s, " ");
    EXPECT_EQ(dstr_len(s), (size_t)7);
    EXPECT_TRUE(strcmp(s, "  hello") == 0);
    dstr_free(s);
}

void test_dstr_tolower() {
    TEST(Dstr_Tolower);
    dstr s = dstr_new("HELLO");
    dstr_tolower(s);
    EXPECT_TRUE(strcmp(s, "hello") == 0);
    dstr_free(s);
}

void test_dstr_toupper() {
    TEST(Dstr_Toupper);
    dstr s = dstr_new("hello");
    dstr_toupper(s);
    EXPECT_TRUE(strcmp(s, "HELLO") == 0);
    dstr_free(s);
}

void test_dstr_reverse() {
    TEST(Dstr_Reverse);
    dstr s = dstr_new("hello");
    dstr_reverse(s);
    EXPECT_TRUE(strcmp(s, "olleh") == 0);
    dstr_free(s);
}

void test_dstr_is_empty() {
    TEST(Dstr_IsEmpty);
    dstr s1 = dstr_new_empty();
    dstr s2 = dstr_new("hello");
    
    EXPECT_TRUE(dstr_is_empty(s1));
    EXPECT_FALSE(dstr_is_empty(s2));
    
    dstr_free(s1);
    dstr_free(s2);
}

void test_dstr_capacity() {
    TEST(Dstr_Capacity);
    dstr s = dstr_new("hello");
    EXPECT_TRUE(dstr_capacity(s) >= dstr_len(s));
    dstr_free(s);
}

void test_dstr_stress_append() {
    TEST(Dstr_StressAppend);
    dstr s = dstr_new_empty();
    
    for (int i = 0; i < 100; i++) {
        s = dstr_append_char(s, 'a');
        EXPECT_TRUE(s != NULL);
    }
    
    EXPECT_EQ(dstr_len(s), (size_t)100);
    dstr_free(s);
}

int main() {
    test_dstr_new();
    test_dstr_new_len();
    test_dstr_new_empty();
    test_dstr_free_null();
    test_dstr_clear();
    test_dstr_append();
    test_dstr_append_len();
    test_dstr_append_char();
    test_dstr_prepend();
    test_dstr_prepend_len();
    test_dstr_substr();
    test_dstr_dup();
    test_dstr_cmp();
    test_dstr_casecmp();
    test_dstr_starts_with();
    test_dstr_ends_with();
    test_dstr_contains();
    test_dstr_find();
    test_dstr_rfind();
    test_dstr_replace();
    test_dstr_trim();
    test_dstr_trim_left();
    test_dstr_trim_right();
    test_dstr_tolower();
    test_dstr_toupper();
    test_dstr_reverse();
    test_dstr_is_empty();
    test_dstr_capacity();
    test_dstr_stress_append();

    return 0;
}
