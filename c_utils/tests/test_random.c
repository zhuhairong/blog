#include "../c_utils/utest.h"
#include "../c_utils/random.h"
#include <string.h>

void test_random_seed() {
    TEST(Random_Seed);
    random_seed(12345);
    uint64_t v1 = random_u64();
    random_seed(12345);
    uint64_t v2 = random_u64();
    EXPECT_EQ(v1, v2);
}

void test_random_u64() {
    TEST(Random_U64);
    random_seed(0);
    uint64_t v = random_u64();
    EXPECT_TRUE(v != 0 || random_u64() != 0);
}

void test_random_u32() {
    TEST(Random_U32);
    uint32_t v = random_u32();
    EXPECT_TRUE(v != 0 || random_u32() != 0);
}

void test_random_u16() {
    TEST(Random_U16);
    uint16_t v = random_u16();
    (void)v;
}

void test_random_u8() {
    TEST(Random_U8);
    uint8_t v = random_u8();
    (void)v;
}

void test_random_i64() {
    TEST(Random_I64);
    int64_t v = random_i64();
    (void)v;
}

void test_random_i32() {
    TEST(Random_I32);
    int32_t v = random_i32();
    (void)v;
}

void test_random_range() {
    TEST(Random_Range);
    for (int i = 0; i < 100; i++) {
        int64_t v = random_range(10, 20);
        EXPECT_TRUE(v >= 10 && v <= 20);
    }
}

void test_random_range_ex() {
    TEST(Random_RangeEx);
    random_error_t err;
    int64_t v = random_range_ex(10, 20, &err);
    EXPECT_EQ(err, RANDOM_OK);
    EXPECT_TRUE(v >= 10 && v <= 20);
}

void test_random_double() {
    TEST(Random_Double);
    for (int i = 0; i < 100; i++) {
        double v = random_double();
        EXPECT_TRUE(v >= 0.0 && v <= 1.0);
    }
}

void test_random_double_range() {
    TEST(Random_DoubleRange);
    for (int i = 0; i < 100; i++) {
        double v = random_double_range(5.0, 10.0);
        EXPECT_TRUE(v >= 5.0 && v <= 10.0);
    }
}

void test_random_bool() {
    TEST(Random_Bool);
    int true_count = 0;
    for (int i = 0; i < 1000; i++) {
        if (random_bool(0.5)) true_count++;
    }
    EXPECT_TRUE(true_count > 300 && true_count < 700);
}

void test_random_bytes() {
    TEST(Random_Bytes);
    uint8_t buffer[32];
    EXPECT_TRUE(random_bytes(buffer, sizeof(buffer)));
    
    int non_zero = 0;
    for (int i = 0; i < 32; i++) {
        if (buffer[i] != 0) non_zero++;
    }
    EXPECT_TRUE(non_zero > 0);
}

void test_random_bytes_ex() {
    TEST(Random_BytesEx);
    uint8_t buffer[16];
    random_error_t err;
    EXPECT_TRUE(random_bytes_ex(buffer, sizeof(buffer), &err));
    EXPECT_EQ(err, RANDOM_OK);
}

void test_random_init() {
    TEST(Random_Init);
    random_ctx_t ctx;
    random_error_t err;
    EXPECT_TRUE(random_init(&ctx, NULL, &err));
    EXPECT_EQ(err, RANDOM_OK);
    EXPECT_TRUE(ctx.initialized);
}

void test_random_u64_ctx() {
    TEST(Random_U64_Ctx);
    random_ctx_t ctx;
    random_init(&ctx, NULL, NULL);
    
    uint64_t v = random_u64_ctx(&ctx);
    EXPECT_TRUE(v != 0 || random_u64_ctx(&ctx) != 0);
}

void test_random_range_ctx() {
    TEST(Random_Range_Ctx);
    random_ctx_t ctx;
    random_error_t err;
    random_init(&ctx, NULL, NULL);
    
    for (int i = 0; i < 100; i++) {
        int64_t v = random_range_ctx(&ctx, 10, 20, &err);
        EXPECT_EQ(err, RANDOM_OK);
        EXPECT_TRUE(v >= 10 && v <= 20);
    }
}

void test_random_double_ctx() {
    TEST(Random_Double_Ctx);
    random_ctx_t ctx;
    random_init(&ctx, NULL, NULL);
    
    for (int i = 0; i < 100; i++) {
        double v = random_double_ctx(&ctx);
        EXPECT_TRUE(v >= 0.0 && v <= 1.0);
    }
}

void test_random_bytes_ctx() {
    TEST(Random_Bytes_Ctx);
    random_ctx_t ctx;
    random_error_t err;
    random_init(&ctx, NULL, NULL);
    
    uint8_t buffer[16];
    EXPECT_TRUE(random_bytes_ctx(&ctx, buffer, sizeof(buffer), &err));
    EXPECT_EQ(err, RANDOM_OK);
}

void test_random_reset() {
    TEST(Random_Reset);
    random_ctx_t ctx;
    random_error_t err;
    random_init(&ctx, NULL, NULL);
    
    random_reset(&ctx, 12345, &err);
    EXPECT_EQ(err, RANDOM_OK);
    
    uint64_t v1 = random_u64_ctx(&ctx);
    random_reset(&ctx, 12345, &err);
    uint64_t v2 = random_u64_ctx(&ctx);
    EXPECT_EQ(v1, v2);
}

void test_random_default_config() {
    TEST(Random_DefaultConfig);
    random_config_t config = random_default_config();
    EXPECT_EQ(config.type, RANDOM_TYPE_XORSHIFT);
    EXPECT_TRUE(config.use_crypto_seed);
}

int main() {
    UTEST_BEGIN();
    test_random_seed();
    test_random_u64();
    test_random_u32();
    test_random_u16();
    test_random_u8();
    test_random_i64();
    test_random_i32();
    test_random_range();
    test_random_range_ex();
    test_random_double();
    test_random_double_range();
    test_random_bool();
    test_random_bytes();
    test_random_bytes_ex();
    test_random_init();
    test_random_u64_ctx();
    test_random_range_ctx();
    test_random_double_ctx();
    test_random_bytes_ctx();
    test_random_reset();
    test_random_default_config();
    UTEST_END();
}
