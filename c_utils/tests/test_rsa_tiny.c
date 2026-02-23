#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/rsa_tiny.h"

void test_rsa_modpow_basic() {
    TEST(RSA_Modpow_Basic);
    uint64_t result = rsa_tiny_modpow(2, 10, 1000);
    EXPECT_EQ(result, 24);
}

void test_rsa_modpow_zero_exp() {
    TEST(RSA_Modpow_ZeroExp);
    uint64_t result = rsa_tiny_modpow(5, 0, 100);
    EXPECT_EQ(result, 1);
}

void test_rsa_modpow_zero_base() {
    TEST(RSA_Modpow_ZeroBase);
    uint64_t result = rsa_tiny_modpow(0, 5, 100);
    EXPECT_EQ(result, 0);
}

void test_rsa_types() {
    TEST(RSA_Types);
    rsa_config_t config;
    memset(&config, 0, sizeof(config));
    EXPECT_TRUE(sizeof(config) > 0);
    
    rsa_key_t key;
    memset(&key, 0, sizeof(key));
    EXPECT_TRUE(sizeof(key) > 0);
}

void test_rsa_error_values() {
    TEST(RSA_ErrorValues);
    EXPECT_TRUE(RSA_OK == 0);
    EXPECT_TRUE(RSA_ERROR_NULL_PTR != 0);
    EXPECT_TRUE(RSA_ERROR_INVALID_ARGS != 0);
}

int main() {
    test_rsa_modpow_basic();
    test_rsa_modpow_zero_exp();
    test_rsa_modpow_zero_base();
    test_rsa_types();
    test_rsa_error_values();

    return 0;
}
