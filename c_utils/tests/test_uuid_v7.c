#include "../c_utils/utest.h"
#include "../c_utils/uuid_v7.h"
#include <string.h>

void test_uuid_v7_create() {
    TEST(UUID_V7_Create);
    uuid_v7_ctx_t* ctx = NULL;
    uuid_v7_config_t config = {
        .use_hyphens = true,
        .uppercase = false,
        .use_monotonic = true,
        .use_entropy = false,
        .entropy_size = 0
    };
    uuid_v7_error_t err = uuid_v7_create(&ctx, &config);
    EXPECT_EQ(err, UUID_V7_OK);
    EXPECT_TRUE(ctx != NULL);
    uuid_v7_destroy(ctx);
}

void test_uuid_v7_generate() {
    TEST(UUID_V7_Generate);
    uuid_v7_ctx_t* ctx = NULL;
    uuid_v7_create(&ctx, NULL);
    
    uint8_t uuid[16];
    uuid_v7_error_t err = uuid_v7_generate(ctx, uuid);
    EXPECT_EQ(err, UUID_V7_OK);
    
    uuid_v7_destroy(ctx);
}

void test_uuid_v7_version() {
    TEST(UUID_V7_Version);
    uuid_v7_ctx_t* ctx = NULL;
    uuid_v7_create(&ctx, NULL);
    
    uint8_t uuid[16];
    uuid_v7_generate(ctx, uuid);
    
    EXPECT_EQ((uuid[6] >> 4) & 0x0F, 7);
    EXPECT_EQ((uuid[8] >> 6) & 0x03, 2);
    
    uuid_v7_destroy(ctx);
}

void test_uuid_v7_string() {
    TEST(UUID_V7_String);
    uuid_v7_ctx_t* ctx = NULL;
    uuid_v7_create(&ctx, NULL);
    
    char uuid_str[37];
    uuid_v7_error_t err = uuid_v7_generate_string(ctx, uuid_str);
    EXPECT_EQ(err, UUID_V7_OK);
    EXPECT_EQ(strlen(uuid_str), 36);
    
    uuid_v7_destroy(ctx);
}

void test_uuid_v7_timestamp() {
    TEST(UUID_V7_Timestamp);
    uuid_v7_ctx_t* ctx = NULL;
    uuid_v7_create(&ctx, NULL);
    
    uint8_t uuid[16];
    uuid_v7_generate(ctx, uuid);
    
    uint64_t timestamp;
    uuid_v7_error_t err = uuid_v7_extract_timestamp(uuid, &timestamp);
    EXPECT_EQ(err, UUID_V7_OK);
    EXPECT_TRUE(timestamp > 0);
    
    uuid_v7_destroy(ctx);
}

void test_uuid_v7_with_timestamp() {
    TEST(UUID_V7_WithTimestamp);
    uuid_v7_ctx_t* ctx = NULL;
    uuid_v7_create(&ctx, NULL);
    
    uint8_t uuid[16];
    uint64_t ts = 1700000000000ULL;
    uuid_v7_error_t err = uuid_v7_generate_with_timestamp(ctx, ts, uuid);
    EXPECT_EQ(err, UUID_V7_OK);
    
    uint64_t extracted;
    uuid_v7_extract_timestamp(uuid, &extracted);
    EXPECT_EQ(extracted, ts);
    
    uuid_v7_destroy(ctx);
}

void test_uuid_v7_batch() {
    TEST(UUID_V7_Batch);
    uuid_v7_ctx_t* ctx = NULL;
    uuid_v7_create(&ctx, NULL);
    
    uint8_t uuids[10][16];
    uuid_v7_error_t err = uuid_v7_generate_batch(ctx, (uint8_t*)uuids, 10);
    EXPECT_EQ(err, UUID_V7_OK);
    
    uuid_v7_destroy(ctx);
}

void test_uuid_v7_monotonic() {
    TEST(UUID_V7_Monotonic);
    uuid_v7_ctx_t* ctx = NULL;
    uuid_v7_config_t config = {
        .use_hyphens = true,
        .uppercase = false,
        .use_monotonic = true,
        .use_entropy = false,
        .entropy_size = 0
    };
    uuid_v7_create(&ctx, &config);
    
    uint8_t uuid1[16], uuid2[16];
    uuid_v7_generate(ctx, uuid1);
    uuid_v7_generate(ctx, uuid2);
    
    uint64_t ts1, ts2;
    uuid_v7_extract_timestamp(uuid1, &ts1);
    uuid_v7_extract_timestamp(uuid2, &ts2);
    
    EXPECT_TRUE(ts2 >= ts1);
    
    uuid_v7_destroy(ctx);
}

void test_uuid_v7_strerror() {
    TEST(UUID_V7_Strerror);
    EXPECT_STR_EQ(uuid_v7_strerror(UUID_V7_OK), "Success");
    EXPECT_STR_EQ(uuid_v7_strerror(UUID_V7_INVALID_PARAMS), "Invalid parameters");
    EXPECT_STR_EQ(uuid_v7_strerror(UUID_V7_MEMORY_ERROR), "Memory error");
}

int main() {
    UTEST_BEGIN();
    test_uuid_v7_create();
    test_uuid_v7_generate();
    test_uuid_v7_version();
    test_uuid_v7_string();
    test_uuid_v7_timestamp();
    test_uuid_v7_with_timestamp();
    test_uuid_v7_batch();
    test_uuid_v7_monotonic();
    test_uuid_v7_strerror();
    UTEST_END();
}
