#include "../c_utils/utest.h"
#include "../c_utils/md5.h"
#include <string.h>

void test_md5_init() {
    TEST(MD5_Init);
    md5_ctx_t ctx;
    md5_init(&ctx);
    EXPECT_TRUE(ctx.initialized);
}

void test_md5_string() {
    TEST(MD5_String);
    md5_ctx_t ctx;
    md5_init(&ctx);
    
    const char *input = "";
    md5_update(&ctx, (const uint8_t*)input, 0);
    
    uint8_t digest[16];
    md5_final(&ctx, digest);
    
    const uint8_t expected[16] = {
        0xd4, 0x1d, 0x8c, 0xd9, 0x8f, 0x00, 0xb2, 0x04,
        0xe9, 0x80, 0x09, 0x98, 0xec, 0xf8, 0x42, 0x7e
    };
    EXPECT_TRUE(memcmp(digest, expected, 16) == 0);
}

void test_md5_hello() {
    TEST(MD5_Hello);
    md5_ctx_t ctx;
    md5_init(&ctx);
    
    const char *input = "hello";
    md5_update(&ctx, (const uint8_t*)input, 5);
    
    uint8_t digest[16];
    md5_final(&ctx, digest);
    
    const uint8_t expected[16] = {
        0x5d, 0x41, 0x40, 0x2a, 0xbc, 0x4b, 0x2a, 0x76,
        0xb9, 0x71, 0x9d, 0x91, 0x10, 0x17, 0xc5, 0x92
    };
    EXPECT_TRUE(memcmp(digest, expected, 16) == 0);
}

void test_md5_update_chunks() {
    TEST(MD5_UpdateChunks);
    md5_ctx_t ctx;
    md5_init(&ctx);
    
    const char *part1 = "hel";
    const char *part2 = "lo";
    md5_update(&ctx, (const uint8_t*)part1, 3);
    md5_update(&ctx, (const uint8_t*)part2, 2);
    
    uint8_t digest[16];
    md5_final(&ctx, digest);
    
    const uint8_t expected[16] = {
        0x5d, 0x41, 0x40, 0x2a, 0xbc, 0x4b, 0x2a, 0x76,
        0xb9, 0x71, 0x9d, 0x91, 0x10, 0x17, 0xc5, 0x92
    };
    EXPECT_TRUE(memcmp(digest, expected, 16) == 0);
}

void test_md5_long() {
    TEST(MD5_Long);
    md5_ctx_t ctx;
    md5_init(&ctx);
    
    const char *input = "The quick brown fox jumps over the lazy dog";
    md5_update(&ctx, (const uint8_t*)input, strlen(input));
    
    uint8_t digest[16];
    md5_final(&ctx, digest);
    
    const uint8_t expected[16] = {
        0x9e, 0x10, 0x7d, 0x9d, 0x37, 0x2b, 0xb6, 0x82,
        0x6b, 0xd8, 0x1d, 0x35, 0x42, 0xa4, 0x19, 0xd6
    };
    EXPECT_TRUE(memcmp(digest, expected, 16) == 0);
}

int main() {
    UTEST_BEGIN();
    test_md5_init();
    test_md5_string();
    test_md5_hello();
    test_md5_update_chunks();
    test_md5_long();
    UTEST_END();
}
