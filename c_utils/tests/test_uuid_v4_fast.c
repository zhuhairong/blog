#include "../c_utils/utest.h"
#include "../c_utils/uuid_v4_fast.h"
#include <string.h>

void test_uuid_v4_fast_create() {
    TEST(UUID_V4_Fast_Create);
    uuid_v4_fast_ctx_t* ctx = NULL;
    uuid_v4_fast_config_t config = {
        .use_hyphens = true,
        .uppercase = false,
        .use_entropy_pool = false,
        .prng_seed_size = 16
    };
    uuid_v4_fast_error_t err = uuid_v4_fast_create(&ctx, &config);
    EXPECT_EQ(err, UUID_V4_FAST_OK);
    EXPECT_TRUE(ctx != NULL);
    uuid_v4_fast_destroy(ctx);
}

void test_uuid_v4_fast_generate() {
    TEST(UUID_V4_Fast_Generate);
    uuid_v4_fast_ctx_t* ctx = NULL;
    uuid_v4_fast_create(&ctx, NULL);
    
    uint8_t uuid[16];
    uuid_v4_fast_error_t err = uuid_v4_fast_generate(ctx, uuid);
    EXPECT_EQ(err, UUID_V4_FAST_OK);
    
    uuid_v4_fast_destroy(ctx);
}

void test_uuid_v4_fast_version() {
    TEST(UUID_V4_Fast_Version);
    uuid_v4_fast_ctx_t* ctx = NULL;
    uuid_v4_fast_create(&ctx, NULL);
    
    uint8_t uuid[16];
    uuid_v4_fast_generate(ctx, uuid);
    
    EXPECT_EQ((uuid[6] >> 4) & 0x0F, 4);
    EXPECT_EQ((uuid[8] >> 6) & 0x03, 2);
    
    uuid_v4_fast_destroy(ctx);
}

void test_uuid_v4_fast_string() {
    TEST(UUID_V4_Fast_String);
    uuid_v4_fast_ctx_t* ctx = NULL;
    uuid_v4_fast_create(&ctx, NULL);
    
    char uuid_str[37];
    uuid_v4_fast_error_t err = uuid_v4_fast_generate_string(ctx, uuid_str);
    EXPECT_EQ(err, UUID_V4_FAST_OK);
    EXPECT_EQ(strlen(uuid_str), 36);
    
    uuid_v4_fast_destroy(ctx);
}

void test_uuid_v4_fast_batch() {
    TEST(UUID_V4_Fast_Batch);
    uuid_v4_fast_ctx_t* ctx = NULL;
    uuid_v4_fast_create(&ctx, NULL);
    
    uint8_t uuids[10][16];
    uuid_v4_fast_error_t err = uuid_v4_fast_generate_batch(ctx, (uint8_t*)uuids, 10);
    EXPECT_EQ(err, UUID_V4_FAST_OK);
    
    uuid_v4_fast_destroy(ctx);
}

void test_uuid_v4_fast_reset_seed() {
    TEST(UUID_V4_Fast_ResetSeed);
    uuid_v4_fast_ctx_t* ctx = NULL;
    uuid_v4_fast_create(&ctx, NULL);
    
    uint8_t seed[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    uuid_v4_fast_error_t err = uuid_v4_fast_reset_seed(ctx, seed, 16);
    EXPECT_EQ(err, UUID_V4_FAST_OK);
    
    uuid_v4_fast_destroy(ctx);
}

void test_uuid_v4_fast_generate_count() {
    TEST(UUID_V4_Fast_GenerateCount);
    uuid_v4_fast_ctx_t* ctx = NULL;
    uuid_v4_fast_create(&ctx, NULL);
    
    EXPECT_EQ(uuid_v4_fast_get_generate_count(ctx), 0);
    
    uint8_t uuid[16];
    uuid_v4_fast_generate(ctx, uuid);
    EXPECT_EQ(uuid_v4_fast_get_generate_count(ctx), 1);
    
    uuid_v4_fast_generate(ctx, uuid);
    EXPECT_EQ(uuid_v4_fast_get_generate_count(ctx), 2);
    
    uuid_v4_fast_destroy(ctx);
}

void test_uuid_v4_fast_strerror() {
    TEST(UUID_V4_Fast_Strerror);
    EXPECT_STR_EQ(uuid_v4_fast_strerror(UUID_V4_FAST_OK), "Success");
    EXPECT_STR_EQ(uuid_v4_fast_strerror(UUID_V4_FAST_INVALID_PARAMS), "Invalid parameters");
    EXPECT_STR_EQ(uuid_v4_fast_strerror(UUID_V4_FAST_MEMORY_ERROR), "Memory error");
}

int main() {
    UTEST_BEGIN();
    test_uuid_v4_fast_create();
    test_uuid_v4_fast_generate();
    test_uuid_v4_fast_version();
    test_uuid_v4_fast_string();
    test_uuid_v4_fast_batch();
    test_uuid_v4_fast_reset_seed();
    test_uuid_v4_fast_generate_count();
    test_uuid_v4_fast_strerror();
    UTEST_END();
}
