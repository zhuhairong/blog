#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/hmac.h"

void test_hmac_default_config() {
    TEST(HMAC_DefaultConfig);
    hmac_config_t config = hmac_default_config(HMAC_ALGO_SHA256);
    EXPECT_EQ(config.algo, HMAC_ALGO_SHA256);
    EXPECT_TRUE(config.digest_size > 0);
}

void test_hmac_init_null() {
    TEST(HMAC_InitNull);
    hmac_error_t error;
    uint8_t key[] = "test_key";
    
    bool result = hmac_init(NULL, key, sizeof(key) - 1, NULL, &error);
    EXPECT_FALSE(result);
    EXPECT_EQ(error, HMAC_ERROR_INVALID_PARAM);
}

void test_hmac_init_empty_key() {
    TEST(HMAC_InitEmptyKey);
    hmac_error_t error;
    hmac_ctx_t ctx;
    memset(&ctx, 0, sizeof(ctx));
    
    bool result = hmac_init(&ctx, NULL, 0, NULL, &error);
    EXPECT_FALSE(result);
}

void test_hmac_update_null() {
    TEST(HMAC_UpdateNull);
    hmac_error_t error;
    uint8_t data[] = "test_data";
    
    bool result = hmac_update(NULL, data, sizeof(data) - 1, &error);
    EXPECT_FALSE(result);
    EXPECT_EQ(error, HMAC_ERROR_INVALID_PARAM);
}

void test_hmac_final_null() {
    TEST(HMAC_FinalNull);
    hmac_error_t error;
    uint8_t digest[32];
    
    bool result = hmac_final(NULL, digest, sizeof(digest), &error);
    EXPECT_FALSE(result);
    EXPECT_EQ(error, HMAC_ERROR_INVALID_PARAM);
}

int main() {
    test_hmac_default_config();
    test_hmac_init_null();
    test_hmac_init_empty_key();
    test_hmac_update_null();
    test_hmac_final_null();

    return 0;
}
