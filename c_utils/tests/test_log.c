#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../c_utils/utest.h"
#include "../c_utils/log.h"

void test_log_level_values() {
    TEST(Log_LevelValues);
    EXPECT_EQ(LOG_LEVEL_TRACE, 0);
    EXPECT_EQ(LOG_LEVEL_DEBUG, 1);
    EXPECT_EQ(LOG_LEVEL_INFO, 2);
    EXPECT_EQ(LOG_LEVEL_WARN, 3);
    EXPECT_EQ(LOG_LEVEL_ERROR, 4);
    EXPECT_EQ(LOG_LEVEL_FATAL, 5);
}

void test_log_set_level() {
    TEST(Log_SetLevel);
    log_set_level(LOG_LEVEL_INFO);
    /* smoke test: verify log_set_level is callable without crash */
}

void test_log_set_quiet() {
    TEST(Log_SetQuiet);
    log_set_quiet(1);
    log_set_quiet(0);
    /* smoke test: verify log_set_quiet is callable without crash */
}

void test_log_write() {
    TEST(Log_Write);
    log_set_quiet(1);
    LOG_INFO("Test log message");
    log_set_quiet(0);
    /* smoke test: verify LOG_INFO macro is callable without crash */
}

void test_log_level_macros() {
    TEST(Log_LevelMacros);
    log_set_quiet(1);
    LOGT("Trace");
    LOGD("Debug");
    LOGI("Info");
    LOGW("Warn");
    LOGE("Error");
    log_set_quiet(0);
    /* smoke test: verify all log level macros are callable without crash */
}

int main() {
    UTEST_BEGIN();
    test_log_level_values();
    test_log_set_level();
    test_log_set_quiet();
    test_log_write();
    test_log_level_macros();

    UTEST_END();
}
