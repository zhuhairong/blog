#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/retry.h"

void test_retry_default_config() {
    TEST(Retry_DefaultConfig);
    retry_config_t config = retry_default_config();
    EXPECT_TRUE(config.max_attempts > 0);
    EXPECT_TRUE(config.base_delay_ms > 0);
}

void test_retry_init() {
    TEST(Retry_Init);
    retry_policy_t policy;
    memset(&policy, 0, sizeof(policy));
    
    retry_init(&policy, 3, 100);
    EXPECT_EQ(policy.config.max_attempts, (size_t)3);
}

void test_retry_types() {
    TEST(Retry_Types);
    retry_config_t config;
    memset(&config, 0, sizeof(config));
    /* retry_config_t type compiles and is instantiable */

    retry_state_t state;
    memset(&state, 0, sizeof(state));
    /* retry_state_t type compiles and is instantiable */
}

void test_retry_error_values() {
    TEST(Retry_ErrorValues);
    EXPECT_TRUE(RETRY_OK == 0);
    EXPECT_TRUE(RETRY_ERROR_NULL_PTR != 0);
    EXPECT_TRUE(RETRY_ERROR_INVALID_ARGS != 0);
}

void test_retry_delay_values() {
    TEST(Retry_DelayValues);
    EXPECT_TRUE(RETRY_DELAY_CONSTANT == 0);
    EXPECT_TRUE(RETRY_DELAY_LINEAR == 1);
    EXPECT_TRUE(RETRY_DELAY_EXPONENTIAL == 2);
}

int main() {
    UTEST_BEGIN();
    test_retry_default_config();
    test_retry_init();
    test_retry_types();
    test_retry_error_values();
    test_retry_delay_values();

    UTEST_END();
}
