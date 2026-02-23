#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../c_utils/utest.h"
#include "../c_utils/cron.h"

void test_cron_parse_basic() {
    TEST(Cron_ParseBasic);
    cron_expr_t cron;
    memset(&cron, 0, sizeof(cron));
    
    bool result = cron_parse("* * * * *", &cron);
    EXPECT_TRUE(result);
    EXPECT_FALSE(cron.has_error);
}

void test_cron_parse_specific() {
    TEST(Cron_ParseSpecific);
    cron_expr_t cron;
    memset(&cron, 0, sizeof(cron));
    
    bool result = cron_parse("0 0 * * *", &cron);
    EXPECT_TRUE(result);
    EXPECT_FALSE(cron.has_error);
}

void test_cron_parse_invalid() {
    TEST(Cron_ParseInvalid);
    cron_expr_t cron;
    memset(&cron, 0, sizeof(cron));
    
    bool result = cron_parse("invalid", &cron);
    EXPECT_FALSE(result);
    EXPECT_TRUE(cron.has_error);
}

void test_cron_match() {
    TEST(Cron_Match);
    cron_expr_t cron;
    memset(&cron, 0, sizeof(cron));
    
    bool result = cron_parse("* * * * *", &cron);
    EXPECT_TRUE(result);
    
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    
    bool matches = cron_match(&cron, t);
    EXPECT_TRUE(matches);
}

void test_cron_match_str() {
    TEST(Cron_MatchStr);
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    cron_error_t error;
    
    bool matches = cron_match_str("* * * * *", t, &error);
    EXPECT_TRUE(matches);
}

int main() {
    test_cron_parse_basic();
    test_cron_parse_specific();
    test_cron_parse_invalid();
    test_cron_match();
    test_cron_match_str();

    return 0;
}
