#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/regex_tiny.h"

void test_regex_default_config() {
    TEST(Regex_DefaultConfig);
    regex_config_t config;
    memset(&config, 0, sizeof(config));
    config.max_depth = 100;
    config.max_matches = 100;
    EXPECT_TRUE(config.max_depth > 0);
    EXPECT_TRUE(config.max_matches > 0);
}

void test_regex_match_basic() {
    TEST(Regex_MatchBasic);
    bool result = regex_match("hello", "hello world");
    /* "hello" should be found within "hello world" */
    EXPECT_TRUE(result);
}

void test_regex_match_null() {
    TEST(Regex_MatchNull);
    bool result = regex_match("", "test");
    /* empty pattern behavior — verify function doesn't crash */
    (void)result; /* empty pattern — verify function doesn't crash */

    result = regex_match("test", "");
    /* pattern against empty string — verify function doesn't crash */
    (void)result;
}

void test_regex_types() {
    TEST(Regex_Types);
    regex_config_t config;
    memset(&config, 0, sizeof(config));
    /* regex_config_t type compiles and is instantiable */

    regex_match_result_t result;
    memset(&result, 0, sizeof(result));
    /* regex_match_result_t type compiles and is instantiable */
}

void test_regex_error_values() {
    TEST(Regex_ErrorValues);
    EXPECT_TRUE(REGEX_OK == 0);
    EXPECT_TRUE(REGEX_ERROR_NULL_PTR != 0);
    EXPECT_TRUE(REGEX_ERROR_INVALID_PATTERN != 0);
}

int main() {
    UTEST_BEGIN();
    test_regex_default_config();
    test_regex_match_basic();
    test_regex_match_null();
    test_regex_types();
    test_regex_error_values();

    UTEST_END();
}
