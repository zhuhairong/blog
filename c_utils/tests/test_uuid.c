#include "../c_utils/utest.h"
#include "../c_utils/uuid.h"
#include <string.h>

void test_uuid_create() {
    TEST(UUID_Create);
    uuid_ctx_t* ctx = NULL;
    uuid_config_t config = {
        .use_hyphens = true,
        .uppercase = false,
        .strict_validation = true,
        .max_string_length = 64
    };
    uuid_error_t err = uuid_create(&ctx, &config);
    EXPECT_EQ(err, UUID_OK);
    EXPECT_TRUE(ctx != NULL);
    uuid_destroy(ctx);
}

void test_uuid_generate_v4() {
    TEST(UUID_GenerateV4);
    uuid_ctx_t* ctx = NULL;
    uuid_create(&ctx, NULL);
    
    uuid_t uuid;
    uuid_error_t err = uuid_generate_v4(ctx, &uuid);
    EXPECT_EQ(err, UUID_OK);
    
    uuid_destroy(ctx);
}

void test_uuid_version() {
    TEST(UUID_Version);
    uuid_ctx_t* ctx = NULL;
    uuid_create(&ctx, NULL);
    
    uuid_t uuid;
    uuid_generate_v4(ctx, &uuid);
    
    int ver = uuid_get_version(&uuid);
    EXPECT_EQ(ver, 4);
    
    uuid_destroy(ctx);
}

void test_uuid_to_string() {
    TEST(UUID_ToString);
    uuid_ctx_t* ctx = NULL;
    uuid_create(&ctx, NULL);
    
    uuid_t uuid;
    uuid_generate_v4(ctx, &uuid);
    
    char uuid_str[UUID_STR_LEN];
    uuid_error_t err = uuid_to_string(ctx, &uuid, uuid_str);
    EXPECT_EQ(err, UUID_OK);
    EXPECT_EQ(strlen(uuid_str), 36);
    
    uuid_destroy(ctx);
}

void test_uuid_parse() {
    TEST(UUID_Parse);
    uuid_ctx_t* ctx = NULL;
    uuid_create(&ctx, NULL);
    
    uuid_t uuid;
    uuid_generate_v4(ctx, &uuid);
    
    char uuid_str[UUID_STR_LEN];
    uuid_to_string(ctx, &uuid, uuid_str);
    
    uuid_t parsed;
    uuid_error_t err = uuid_parse(ctx, uuid_str, &parsed);
    EXPECT_EQ(err, UUID_OK);
    
    EXPECT_EQ(uuid_compare(&uuid, &parsed), 0);
    
    uuid_destroy(ctx);
}

void test_uuid_compare() {
    TEST(UUID_Compare);
    uuid_ctx_t* ctx = NULL;
    uuid_create(&ctx, NULL);
    
    uuid_t uuid1, uuid2;
    uuid_generate_v4(ctx, &uuid1);
    uuid_copy(&uuid2, &uuid1);
    
    EXPECT_EQ(uuid_compare(&uuid1, &uuid2), 0);
    
    uuid_destroy(ctx);
}

void test_uuid_copy() {
    TEST(UUID_Copy);
    uuid_ctx_t* ctx = NULL;
    uuid_create(&ctx, NULL);
    
    uuid_t src, dest;
    uuid_generate_v4(ctx, &src);
    uuid_copy(&dest, &src);
    
    EXPECT_EQ(uuid_compare(&src, &dest), 0);
    
    uuid_destroy(ctx);
}

void test_uuid_is_valid() {
    TEST(UUID_IsValid);
    EXPECT_TRUE(uuid_is_valid("550e8400-e29b-41d4-a716-446655440000"));
    EXPECT_TRUE(uuid_is_valid("550e8400e29b41d4a716446655440000"));
    EXPECT_TRUE(!uuid_is_valid("invalid-uuid"));
    EXPECT_TRUE(!uuid_is_valid("550e8400-e29b-41d4-a716"));
}

void test_uuid_is_nil() {
    TEST(UUID_IsNil);
    uuid_t nil_uuid = {{0}};
    EXPECT_TRUE(uuid_is_nil(&nil_uuid));
    
    uuid_ctx_t* ctx = NULL;
    uuid_create(&ctx, NULL);
    
    uuid_t uuid;
    uuid_generate_v4(ctx, &uuid);
    EXPECT_TRUE(!uuid_is_nil(&uuid));
    
    uuid_destroy(ctx);
}

void test_uuid_v4_legacy() {
    TEST(UUID_V4_Legacy);
    char uuid_str[37];
    uuid_v4(uuid_str);
    EXPECT_EQ(strlen(uuid_str), 36);
    EXPECT_TRUE(uuid_is_valid(uuid_str));
}

int main() {
    UTEST_BEGIN();
    test_uuid_create();
    test_uuid_generate_v4();
    test_uuid_version();
    test_uuid_to_string();
    test_uuid_parse();
    test_uuid_compare();
    test_uuid_copy();
    test_uuid_is_valid();
    test_uuid_is_nil();
    test_uuid_v4_legacy();
    UTEST_END();
}
