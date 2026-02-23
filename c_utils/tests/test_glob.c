#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/glob.h"

void test_glob_default_config() {
    TEST(Glob_DefaultConfig);
    glob_config_t config = glob_default_config();
    EXPECT_TRUE(config.max_matches > 0);
}

void test_glob_match_basic() {
    TEST(Glob_MatchBasic);
    glob_error_t error;
    
    bool result = glob_match("*.txt", "test.txt", GLOB_FLAG_NONE, &error);
    EXPECT_TRUE(result);
    
    result = glob_match("*.txt", "test.c", GLOB_FLAG_NONE, &error);
    EXPECT_FALSE(result);
}

void test_glob_match_question() {
    TEST(Glob_MatchQuestion);
    glob_error_t error;
    
    bool result = glob_match("test?.txt", "test1.txt", GLOB_FLAG_NONE, &error);
    EXPECT_TRUE(result);
    
    result = glob_match("test?.txt", "test12.txt", GLOB_FLAG_NONE, &error);
    EXPECT_FALSE(result);
}

void test_glob_match_bracket() {
    TEST(Glob_MatchBracket);
    glob_error_t error;
    
    bool result = glob_match("test[123].txt", "test1.txt", GLOB_FLAG_NONE, &error);
    EXPECT_TRUE(result);
    
    result = glob_match("test[123].txt", "test4.txt", GLOB_FLAG_NONE, &error);
    EXPECT_FALSE(result);
}

void test_glob_match_null() {
    TEST(Glob_MatchNull);
    glob_error_t error;
    
    bool result = glob_match(NULL, "test.txt", GLOB_FLAG_NONE, &error);
    EXPECT_FALSE(result);
    EXPECT_EQ(error, GLOB_ERROR_INVALID_PARAM);
    
    result = glob_match("*.txt", NULL, GLOB_FLAG_NONE, &error);
    EXPECT_FALSE(result);
}

int main() {
    test_glob_default_config();
    test_glob_match_basic();
    test_glob_match_question();
    test_glob_match_bracket();
    test_glob_match_null();

    return 0;
}
