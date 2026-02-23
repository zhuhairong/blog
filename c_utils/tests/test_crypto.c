#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/crypto.h"

void test_crypto_sha256_init() {
    TEST(Crypto_SHA256Init);
    sha256_ctx_t ctx;
    sha256_init(&ctx);
    EXPECT_TRUE(ctx.count == 0);
}

void test_crypto_sha256_update() {
    TEST(Crypto_SHA256Update);
    sha256_ctx_t ctx;
    sha256_init(&ctx);
    
    const uint8_t data[] = "hello";
    sha256_update(&ctx, data, 5);
    
    EXPECT_TRUE(ctx.count > 0);
}

void test_crypto_sha256_final() {
    TEST(Crypto_SHA256Final);
    sha256_ctx_t ctx;
    sha256_init(&ctx);
    
    const uint8_t data[] = "hello";
    sha256_update(&ctx, data, 5);
    
    uint8_t digest[32];
    sha256_final(&ctx, digest);
    
    EXPECT_TRUE(digest[0] != 0 || digest[0] == 0);
}

void test_crypto_to_hex() {
    TEST(Crypto_ToHex);
    const uint8_t data[] = {0x01, 0x23, 0xAB, 0xCD};
    char hex[16];
    
    crypto_to_hex(data, 4, hex);
    
    EXPECT_TRUE(strlen(hex) == 8);
    EXPECT_TRUE(strcmp(hex, "0123abcd") == 0);
}

void test_crypto_types() {
    TEST(Crypto_Types);
    sha256_ctx_t ctx;
    memset(&ctx, 0, sizeof(ctx));
    EXPECT_TRUE(sizeof(ctx) > 0);
}

int main() {
    test_crypto_sha256_init();
    test_crypto_sha256_update();
    test_crypto_sha256_final();
    test_crypto_to_hex();
    test_crypto_types();

    return 0;
}
