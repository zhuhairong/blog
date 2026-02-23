#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/glob_match.h"

void test_glob_match_simple() {
    TEST(Glob_MatchSimple);
    glob_match_error_t err;
    
    bool result = glob_match_simple("*.txt", "file.txt", GLOB_MATCH_FLAG_NONE, &err);
    EXPECT_TRUE(result);
    
    result = glob_match_simple("*.txt", "file.c", GLOB_MATCH_FLAG_NONE, &err);
    EXPECT_FALSE(result);
}

void test_glob_match_question() {
    TEST(Glob_MatchQuestion);
    glob_match_error_t err;
    
    bool result = glob_match_simple("file?.txt", "file1.txt", GLOB_MATCH_FLAG_NONE, &err);
    EXPECT_TRUE(result);
    
    result = glob_match_simple("file?.txt", "file12.txt", GLOB_MATCH_FLAG_NONE, &err);
    EXPECT_FALSE(result);
}

void test_glob_match_exact() {
    TEST(Glob_MatchExact);
    glob_match_error_t err;
    
    bool result = glob_match_simple("hello", "hello", GLOB_MATCH_FLAG_NONE, &err);
    EXPECT_TRUE(result);
    
    result = glob_match_simple("hello", "world", GLOB_MATCH_FLAG_NONE, &err);
    EXPECT_FALSE(result);
}

void test_glob_match_default_config() {
    TEST(Glob_MatchDefaultConfig);
    glob_match_config_t config = glob_match_default_config();
    
    EXPECT_EQ(config.wildcard_single, '?');
    EXPECT_EQ(config.wildcard_any, '*');
}

void test_glob_match_strerror() {
    TEST(Glob_MatchStrerror);
    const char* msg = glob_match_strerror(GLOB_MATCH_OK);
    EXPECT_TRUE(msg != NULL);
    
    msg = glob_match_strerror(GLOB_MATCH_ERROR_INVALID_PARAM);
    EXPECT_TRUE(msg != NULL);
}

int main() {
    test_glob_match_simple();
    test_glob_match_question();
    test_glob_match_exact();
    test_glob_match_default_config();
    test_glob_match_strerror();

    return 0;
}
