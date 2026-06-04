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
    
    /* SHA-256("hello") known-answer test vector (RFC 6234) */
    char hex[65];
    crypto_to_hex(digest, 32, hex);
    EXPECT_STR_EQ(hex, "2cf24dba5fb0a30e26e83b2ac5b9e29e1b161e5c1fa7425e73043362938b9824");
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
    /* sha256_ctx_t type compiles and is instantiable */
}

int main() {
    UTEST_BEGIN();
    test_crypto_sha256_init();
    test_crypto_sha256_update();
    test_crypto_sha256_final();
    test_crypto_to_hex();
    test_crypto_types();

    UTEST_END();
}
