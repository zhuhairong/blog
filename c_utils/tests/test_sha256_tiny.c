#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/sha256_tiny.h"

void test_sha256_tiny_types() {
    TEST(SHA256_Tiny_Types);
    sha256_tiny_config_t config;
    memset(&config, 0, sizeof(config));
    EXPECT_TRUE(sizeof(config) > 0);
    
    sha256_tiny_state_t state;
    memset(&state, 0, sizeof(state));
    EXPECT_TRUE(sizeof(state) > 0);
}

void test_sha256_tiny_error_values() {
    TEST(SHA256_Tiny_ErrorValues);
    EXPECT_TRUE(SHA256_TINY_OK == 0);
    EXPECT_TRUE(SHA256_TINY_ERROR_INVALID_PARAMS != 0);
}

void test_sha256_tiny_digest_size() {
    TEST(SHA256_Tiny_DigestSize);
    EXPECT_TRUE(sizeof(uint8_t) == 1);
}

void test_sha256_tiny_config_fields() {
    TEST(SHA256_Tiny_ConfigFields);
    sha256_tiny_config_t config;
    memset(&config, 0, sizeof(config));
    
    config.enable_file_operations = true;
    config.enable_hex_output = true;
    config.max_input_size = 1024;
    
    EXPECT_TRUE(config.enable_file_operations);
    EXPECT_TRUE(config.enable_hex_output);
    EXPECT_TRUE(config.max_input_size == 1024);
}

void test_sha256_tiny_state_fields() {
    TEST(SHA256_Tiny_StateFields);
    sha256_tiny_state_t state;
    memset(&state, 0, sizeof(state));
    
    state.total_processed = 100;
    state.is_initialized = true;
    
    EXPECT_TRUE(state.total_processed == 100);
    EXPECT_TRUE(state.is_initialized);
}

int main() {
    test_sha256_tiny_types();
    test_sha256_tiny_error_values();
    test_sha256_tiny_digest_size();
    test_sha256_tiny_config_fields();
    test_sha256_tiny_state_fields();

    return 0;
}
