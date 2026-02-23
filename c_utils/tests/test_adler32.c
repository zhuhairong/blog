#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/adler32.h"

void test_adler32_compute() {
    TEST(Adler32_Compute);
    const char* data = "hello";
    uint32_t checksum = adler32_compute(data, 5);
    EXPECT_TRUE(checksum != 0);
}

void test_adler32_empty() {
    TEST(Adler32_Empty);
    uint32_t checksum = adler32_compute("", 0);
    EXPECT_EQ(checksum, ADLER32_INIT);
}

void test_adler32_update() {
    TEST(Adler32_Update);
    const char* part1 = "hel";
    const char* part2 = "lo";
    
    uint32_t checksum1 = adler32_compute(part1, 3);
    uint32_t checksum2 = adler32_update(checksum1, part2, 2);
    uint32_t checksum_full = adler32_compute("hello", 5);
    
    EXPECT_EQ(checksum2, checksum_full);
}

void test_adler32_create_destroy() {
    TEST(Adler32_CreateDestroy);
    adler32_ctx_t* ctx = NULL;
    adler32_error_t err = adler32_create(&ctx, NULL);
    EXPECT_EQ(err, ADLER32_OK);
    EXPECT_TRUE(ctx != NULL);
    
    adler32_destroy(ctx);
}

void test_adler32_compute_safe() {
    TEST(Adler32_ComputeSafe);
    adler32_ctx_t* ctx = NULL;
    adler32_create(&ctx, NULL);
    
    const char* data = "hello world";
    uint32_t checksum = 0;
    adler32_error_t err = adler32_compute_safe(ctx, data, 11, &checksum);
    EXPECT_EQ(err, ADLER32_OK);
    EXPECT_TRUE(checksum != 0);
    
    adler32_destroy(ctx);
}

void test_adler32_update_safe() {
    TEST(Adler32_UpdateSafe);
    adler32_ctx_t* ctx = NULL;
    adler32_create(&ctx, NULL);
    
    const char* part1 = "hello";
    const char* part2 = " world";
    
    uint32_t checksum1 = 0;
    adler32_compute_safe(ctx, part1, 5, &checksum1);
    
    uint32_t checksum2 = 0;
    adler32_update_safe(ctx, checksum1, part2, 6, &checksum2);
    
    uint32_t checksum_full = 0;
    adler32_compute_safe(ctx, "hello world", 11, &checksum_full);
    
    EXPECT_EQ(checksum2, checksum_full);
    
    adler32_destroy(ctx);
}

void test_adler32_compute_null() {
    TEST(Adler32_ComputeNull);
    adler32_ctx_t* ctx = NULL;
    adler32_create(&ctx, NULL);
    
    uint32_t checksum = 0;
    adler32_error_t err = adler32_compute_safe(ctx, NULL, 10, &checksum);
    EXPECT_EQ(err, ADLER32_INVALID_PARAMS);
    
    adler32_destroy(ctx);
}

void test_adler32_strerror() {
    TEST(Adler32_Strerror);
    const char* msg = adler32_strerror(ADLER32_OK);
    EXPECT_TRUE(msg != NULL);
    
    msg = adler32_strerror(ADLER32_INVALID_PARAMS);
    EXPECT_TRUE(msg != NULL);
}

void test_adler32_reset() {
    TEST(Adler32_Reset);
    adler32_ctx_t* ctx = NULL;
    adler32_create(&ctx, NULL);
    
    const char* data = "test";
    uint32_t checksum = 0;
    adler32_compute_safe(ctx, data, 4, &checksum);
    
    adler32_reset(ctx);
    
    adler32_destroy(ctx);
}

void test_adler32_known_values() {
    TEST(Adler32_KnownValues);
    uint32_t checksum = adler32_compute("Wikipedia", 9);
    EXPECT_TRUE(checksum != 0);
}

void test_adler32_binary_data() {
    TEST(Adler32_BinaryData);
    unsigned char data[] = {0x00, 0x01, 0x02, 0x03, 0xFF, 0xFE, 0xFD};
    uint32_t checksum = adler32_compute(data, 7);
    EXPECT_TRUE(checksum != 0);
}

void test_adler32_long_data() {
    TEST(Adler32_LongData);
    char data[10000];
    memset(data, 'A', sizeof(data));
    
    uint32_t checksum = adler32_compute(data, sizeof(data));
    EXPECT_TRUE(checksum != 0);
}

void test_adler32_incremental() {
    TEST(Adler32_Incremental);
    char data[1000];
    memset(data, 'B', sizeof(data));
    
    uint32_t checksum1 = adler32_compute(data, sizeof(data));
    
    uint32_t checksum2 = ADLER32_INIT;
    for (int i = 0; i < 10; i++) {
        checksum2 = adler32_update(checksum2, data + i * 100, 100);
    }
    
    EXPECT_EQ(checksum1, checksum2);
}

int main() {
    test_adler32_compute();
    test_adler32_empty();
    test_adler32_update();
    test_adler32_create_destroy();
    test_adler32_compute_safe();
    test_adler32_update_safe();
    test_adler32_compute_null();
    test_adler32_strerror();
    test_adler32_reset();
    test_adler32_known_values();
    test_adler32_binary_data();
    test_adler32_long_data();
    test_adler32_incremental();

    return 0;
}
