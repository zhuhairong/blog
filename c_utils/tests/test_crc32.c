#include "../c_utils/utest.h"
#include "../c_utils/crc32.h"
#include <string.h>

void test_crc32_init() {
    TEST(CRC32_Init);
    crc32_context_t ctx;
    crc32_error_t err;
    EXPECT_TRUE(crc32_init(&ctx, CRC32_STANDARD, &err));
    EXPECT_EQ(err, CRC32_OK);
}

void test_crc32_empty() {
    TEST(CRC32_Empty);
    crc32_context_t ctx;
    crc32_init(&ctx, CRC32_STANDARD, NULL);
    
    uint32_t crc = crc32_final(&ctx);
    EXPECT_EQ(crc, 0x00000000);
}

void test_crc32_hello() {
    TEST(CRC32_Hello);
    crc32_context_t ctx;
    crc32_init(&ctx, CRC32_STANDARD, NULL);
    
    const char *input = "hello";
    crc32_update(&ctx, input, 5);
    
    uint32_t crc = crc32_final(&ctx);
    EXPECT_EQ(crc, 0x3610A686);
}

void test_crc32_update_chunks() {
    TEST(CRC32_UpdateChunks);
    crc32_context_t ctx;
    crc32_init(&ctx, CRC32_STANDARD, NULL);
    
    crc32_update(&ctx, "hel", 3);
    crc32_update(&ctx, "lo", 2);
    
    uint32_t crc = crc32_final(&ctx);
    EXPECT_EQ(crc, 0x3610A686);
}

void test_crc32_long() {
    TEST(CRC32_Long);
    crc32_context_t ctx;
    crc32_init(&ctx, CRC32_STANDARD, NULL);
    
    const char *input = "The quick brown fox jumps over the lazy dog";
    crc32_update(&ctx, input, strlen(input));
    
    uint32_t crc = crc32_final(&ctx);
    EXPECT_EQ(crc, 0x414FA339);
}

void test_crc32_variants() {
    TEST(CRC32_Variants);
    crc32_context_t ctx;
    crc32_error_t err;
    
    EXPECT_TRUE(crc32_init(&ctx, CRC32_STANDARD, &err));
    EXPECT_EQ(err, CRC32_OK);
    
    EXPECT_TRUE(crc32_init(&ctx, CRC32_C, &err));
    EXPECT_EQ(err, CRC32_OK);
}

int main() {
    UTEST_BEGIN();
    test_crc32_init();
    test_crc32_empty();
    test_crc32_hello();
    test_crc32_update_chunks();
    test_crc32_long();
    test_crc32_variants();
    UTEST_END();
}
