#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/kv_store.h"

void test_kv_types() {
    TEST(KV_Types);
    kv_config_t config;
    memset(&config, 0, sizeof(config));
    EXPECT_TRUE(sizeof(config) > 0);
    
    kv_entry_t entry;
    memset(&entry, 0, sizeof(entry));
    EXPECT_TRUE(sizeof(entry) > 0);
}

void test_kv_error_values() {
    TEST(KV_ErrorValues);
    EXPECT_TRUE(KV_OK == 0);
    EXPECT_TRUE(KV_INVALID_INPUT != 0);
    EXPECT_TRUE(KV_KEY_NOT_FOUND != 0);
}

void test_kv_config_fields() {
    TEST(KV_ConfigFields);
    kv_config_t config;
    memset(&config, 0, sizeof(config));
    
    config.enable_compression = true;
    config.max_key_length = 256;
    config.max_value_length = 1024;
    
    EXPECT_TRUE(config.enable_compression);
    EXPECT_TRUE(config.max_key_length == 256);
    EXPECT_TRUE(config.max_value_length == 1024);
}

void test_kv_default_config() {
    TEST(KV_DefaultConfig);
    kv_config_t config;
    kv_get_default_config(&config);
    EXPECT_TRUE(config.max_key_length > 0);
    EXPECT_TRUE(config.max_value_length > 0);
}

void test_kv_exists() {
    TEST(KV_Exists);
    bool exists = kv_exists("/nonexistent/file.kv", "test_key");
    EXPECT_FALSE(exists);
}

int main() {
    test_kv_types();
    test_kv_error_values();
    test_kv_config_fields();
    test_kv_default_config();
    test_kv_exists();

    return 0;
}
