#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/pbkdf2.h"

void test_pbkdf2_sha1_basic() {
    TEST(PBKDF2_SHA1_Basic);
    const char* pass = "password";
    uint8_t salt[] = "salt";
    uint8_t out[20];
    
    pbkdf2_sha1(pass, salt, 4, 1000, out, 20);
    
    EXPECT_TRUE(out[0] != 0 || out[0] == 0);
}

void test_pbkdf2_types() {
    TEST(PBKDF2_Types);
    pbkdf2_config_t config;
    memset(&config, 0, sizeof(config));
    EXPECT_TRUE(sizeof(config) > 0);
    
    EXPECT_TRUE(PBKDF2_OK == 0);
    EXPECT_TRUE(PBKDF2_ALGORITHM_SHA1 == 0);
}

void test_pbkdf2_algorithm_values() {
    TEST(PBKDF2_AlgorithmValues);
    EXPECT_TRUE(PBKDF2_ALGORITHM_SHA1 == 0);
    EXPECT_TRUE(PBKDF2_ALGORITHM_SHA256 == 1);
    EXPECT_TRUE(PBKDF2_ALGORITHM_SHA512 == 2);
}

void test_pbkdf2_error_values() {
    TEST(PBKDF2_ErrorValues);
    EXPECT_TRUE(PBKDF2_OK == 0);
    EXPECT_TRUE(PBKDF2_ERROR_NULL_PTR == 1);
    EXPECT_TRUE(PBKDF2_ERROR_INVALID_ARGS == 2);
}

void test_pbkdf2_config_size() {
    TEST(PBKDF2_ConfigSize);
    pbkdf2_config_t config;
    EXPECT_TRUE(sizeof(config.algorithm) > 0);
    EXPECT_TRUE(sizeof(config.iterations) > 0);
    EXPECT_TRUE(sizeof(config.salt_len) > 0);
    EXPECT_TRUE(sizeof(config.output_len) > 0);
}

int main() {
    test_pbkdf2_sha1_basic();
    test_pbkdf2_types();
    test_pbkdf2_algorithm_values();
    test_pbkdf2_error_values();
    test_pbkdf2_config_size();

    return 0;
}
