#include "../c_utils/utest.h"
#include "../c_utils/sha1.h"
#include <string.h>

void test_sha1_init() {
    TEST(SHA1_Init);
    sha1_ctx_t ctx;
    sha1_init(&ctx);
}

void test_sha1_empty() {
    TEST(SHA1_Empty);
    sha1_ctx_t ctx;
    sha1_init(&ctx);
    
    uint8_t digest[20];
    sha1_final(&ctx, digest);
    
    const uint8_t expected[20] = {
        0xda, 0x39, 0xa3, 0xee, 0x5e, 0x6b, 0x4b, 0x0d, 0x32, 0x55,
        0xbf, 0xef, 0x95, 0x60, 0x18, 0x90, 0xaf, 0xd8, 0x07, 0x09
    };
    EXPECT_TRUE(memcmp(digest, expected, 20) == 0);
}

void test_sha1_hello() {
    TEST(SHA1_Hello);
    sha1_ctx_t ctx;
    sha1_init(&ctx);
    
    const char *input = "hello";
    sha1_update(&ctx, (const uint8_t*)input, 5);
    
    uint8_t digest[20];
    sha1_final(&ctx, digest);
    
    const uint8_t expected[20] = {
        0xaa, 0xf4, 0xc6, 0x1d, 0xdc, 0xc5, 0xe8, 0xa2, 0xda, 0xbe,
        0xde, 0x0f, 0x3b, 0x48, 0x2c, 0xd9, 0xae, 0xa9, 0x43, 0x4d
    };
    EXPECT_TRUE(memcmp(digest, expected, 20) == 0);
}

void test_sha1_update_chunks() {
    TEST(SHA1_UpdateChunks);
    sha1_ctx_t ctx;
    sha1_init(&ctx);
    
    const char *part1 = "hel";
    const char *part2 = "lo";
    sha1_update(&ctx, (const uint8_t*)part1, 3);
    sha1_update(&ctx, (const uint8_t*)part2, 2);
    
    uint8_t digest[20];
    sha1_final(&ctx, digest);
    
    const uint8_t expected[20] = {
        0xaa, 0xf4, 0xc6, 0x1d, 0xdc, 0xc5, 0xe8, 0xa2, 0xda, 0xbe,
        0xde, 0x0f, 0x3b, 0x48, 0x2c, 0xd9, 0xae, 0xa9, 0x43, 0x4d
    };
    EXPECT_TRUE(memcmp(digest, expected, 20) == 0);
}

void test_sha1_long() {
    TEST(SHA1_Long);
    sha1_ctx_t ctx;
    sha1_init(&ctx);
    
    const char *input = "The quick brown fox jumps over the lazy dog";
    sha1_update(&ctx, (const uint8_t*)input, strlen(input));
    
    uint8_t digest[20];
    sha1_final(&ctx, digest);
    
    const uint8_t expected[20] = {
        0x2f, 0xd4, 0xe1, 0xc6, 0x7a, 0x2d, 0x28, 0xfc, 0xed, 0x84,
        0x9e, 0xe1, 0xbb, 0x76, 0xe7, 0x39, 0x1b, 0x93, 0xeb, 0x12
    };
    EXPECT_TRUE(memcmp(digest, expected, 20) == 0);
}

int main() {
    UTEST_BEGIN();
    test_sha1_init();
    test_sha1_empty();
    test_sha1_hello();
    test_sha1_update_chunks();
    test_sha1_long();
    UTEST_END();
}
