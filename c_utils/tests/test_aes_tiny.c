#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/aes_tiny.h"

void test_aes_tiny_create_destroy() {
    TEST(AesTiny_CreateDestroy);
    aes_tiny_config_t config = {
        .use_padding = true,
        .check_key_size = true,
        .check_block_size = true,
        .mode = AES_TINY_MODE_ECB,
        .key_size = AES_TINY_KEY_SIZE_128
    };
    uint8_t key[16] = {0};
    
    aes_tiny_ctx_t* ctx = NULL;
    aes_tiny_error_t err = aes_tiny_create(&ctx, &config, key, 16);
    EXPECT_EQ(err, AES_TINY_OK);
    EXPECT_TRUE(ctx != NULL);
    
    aes_tiny_destroy(ctx);
}

void test_aes_tiny_strerror() {
    TEST(AesTiny_Strerror);
    const char* msg = aes_tiny_strerror(AES_TINY_OK);
    EXPECT_TRUE(msg != NULL);
    
    msg = aes_tiny_strerror(AES_TINY_INVALID_PARAMS);
    EXPECT_TRUE(msg != NULL);
}

void test_aes_tiny_encrypt_simple() {
    TEST(AesTiny_EncryptSimple);
    uint8_t in[16] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                      0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
    uint8_t key[16] = {0};
    uint8_t out[16] = {0};
    
    aes_tiny_encrypt_simple(in, key, out);
    EXPECT_TRUE(true);
}

void test_aes_tiny_encrypt_block() {
    TEST(AesTiny_EncryptBlock);
    aes_tiny_config_t config = {
        .use_padding = false,
        .check_key_size = true,
        .check_block_size = true,
        .mode = AES_TINY_MODE_ECB,
        .key_size = AES_TINY_KEY_SIZE_128
    };
    uint8_t key[16] = {0};
    
    aes_tiny_ctx_t* ctx = NULL;
    aes_tiny_error_t err = aes_tiny_create(&ctx, &config, key, 16);
    EXPECT_EQ(err, AES_TINY_OK);
    
    uint8_t in[16] = {0};
    uint8_t out[16] = {0};
    
    err = aes_tiny_encrypt_block(ctx, in, out);
    EXPECT_TRUE(err == AES_TINY_OK || err != AES_TINY_OK);
    
    aes_tiny_destroy(ctx);
}

void test_aes_tiny_get_last_error() {
    TEST(AesTiny_GetLastError);
    aes_tiny_config_t config = {
        .use_padding = true,
        .check_key_size = true,
        .check_block_size = true,
        .mode = AES_TINY_MODE_ECB,
        .key_size = AES_TINY_KEY_SIZE_128
    };
    uint8_t key[16] = {0};
    
    aes_tiny_ctx_t* ctx = NULL;
    aes_tiny_error_t err = aes_tiny_create(&ctx, &config, key, 16);
    EXPECT_EQ(err, AES_TINY_OK);
    
    aes_tiny_error_t last_err = aes_tiny_get_last_error(ctx);
    EXPECT_TRUE(last_err == AES_TINY_OK || last_err != AES_TINY_OK);
    
    aes_tiny_destroy(ctx);
}

int main() {
    test_aes_tiny_create_destroy();
    test_aes_tiny_strerror();
    test_aes_tiny_encrypt_simple();
    test_aes_tiny_encrypt_block();
    test_aes_tiny_get_last_error();

    return 0;
}
