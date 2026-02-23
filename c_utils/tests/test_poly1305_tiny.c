#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/poly1305_tiny.h"

void test_poly1305_types() {
    TEST(Poly1305_Types);
    poly1305_config_t config;
    memset(&config, 0, sizeof(config));
    EXPECT_TRUE(sizeof(config) > 0);
    
    poly1305_ctx_t ctx;
    memset(&ctx, 0, sizeof(ctx));
    EXPECT_TRUE(sizeof(ctx) > 0);
}

void test_poly1305_error_values() {
    TEST(Poly1305_ErrorValues);
    EXPECT_TRUE(POLY1305_OK == 0);
    EXPECT_TRUE(POLY1305_ERROR_NULL_PTR != 0);
    EXPECT_TRUE(POLY1305_ERROR_INVALID_KEY_SIZE != 0);
}

void test_poly1305_constants() {
    TEST(Poly1305_Constants);
    EXPECT_EQ(POLY1305_KEY_SIZE, 32);
    EXPECT_EQ(POLY1305_MAC_SIZE, 16);
}

void test_poly1305_config_fields() {
    TEST(Poly1305_ConfigFields);
    poly1305_config_t config;
    memset(&config, 0, sizeof(config));
    
    config.key_size = 32;
    config.mac_size = 16;
    config.verify_only = false;
    
    EXPECT_EQ(config.key_size, 32);
    EXPECT_EQ(config.mac_size, 16);
    EXPECT_FALSE(config.verify_only);
}

void test_poly1305_ctx_fields() {
    TEST(Poly1305_CtxFields);
    poly1305_ctx_t ctx;
    memset(&ctx, 0, sizeof(ctx));
    
    ctx.buffer_pos = 0;
    
    EXPECT_EQ(ctx.buffer_pos, 0);
}

int main() {
    test_poly1305_types();
    test_poly1305_error_values();
    test_poly1305_constants();
    test_poly1305_config_fields();
    test_poly1305_ctx_fields();

    return 0;
}
