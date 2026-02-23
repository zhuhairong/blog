#include "../c_utils/utest.h"
#include "../c_utils/version.h"
#include <string.h>

void test_version_create() {
    TEST(Version_Create);
    version_ctx_t* ctx = NULL;
    version_config_t config = {
        .allow_pre_release = true,
        .allow_build_metadata = true,
        .strict_mode = false,
        .max_version_length = 256
    };
    version_error_t err = version_create(&ctx, &config);
    EXPECT_EQ(err, VERSION_OK);
    EXPECT_TRUE(ctx != NULL);
    version_destroy(ctx);
}

void test_version_parse() {
    TEST(Version_Parse);
    version_ctx_t* ctx = NULL;
    version_create(&ctx, NULL);
    
    version_t v;
    version_error_t err = version_parse(ctx, "1.2.3", &v);
    EXPECT_EQ(err, VERSION_OK);
    EXPECT_EQ(v.major, 1);
    EXPECT_EQ(v.minor, 2);
    EXPECT_EQ(v.patch, 3);
    
    version_destroy(ctx);
}

void test_version_parse_two_parts() {
    TEST(Version_Parse_TwoParts);
    version_ctx_t* ctx = NULL;
    version_create(&ctx, NULL);
    
    version_t v;
    version_error_t err = version_parse(ctx, "1.2", &v);
    EXPECT_EQ(err, VERSION_OK);
    EXPECT_EQ(v.major, 1);
    EXPECT_EQ(v.minor, 2);
    EXPECT_EQ(v.patch, 0);
    
    version_destroy(ctx);
}

void test_version_compare() {
    TEST(Version_Compare);
    version_t v1 = {1, 2, 3, NULL, NULL};
    version_t v2 = {1, 2, 4, NULL, NULL};
    version_t v3 = {1, 2, 3, NULL, NULL};
    
    EXPECT_EQ(version_compare(v1, v2), -1);
    EXPECT_EQ(version_compare(v2, v1), 1);
    EXPECT_EQ(version_compare(v1, v3), 0);
}

void test_version_to_string() {
    TEST(Version_ToString);
    version_ctx_t* ctx = NULL;
    version_create(&ctx, NULL);
    
    version_t v = {2, 1, 0, NULL, NULL};
    char buf[64];
    version_error_t err = version_to_string(ctx, &v, buf, sizeof(buf));
    EXPECT_EQ(err, VERSION_OK);
    EXPECT_STR_EQ(buf, "2.1.0");
    
    version_destroy(ctx);
}

void test_version_increment() {
    TEST(Version_Increment);
    version_ctx_t* ctx = NULL;
    version_create(&ctx, NULL);
    
    version_t v = {1, 2, 3, NULL, NULL};
    
    version_increment(ctx, &v, 2);
    EXPECT_EQ(v.major, 1);
    EXPECT_EQ(v.minor, 2);
    EXPECT_EQ(v.patch, 4);
    
    version_increment(ctx, &v, 1);
    EXPECT_EQ(v.major, 1);
    EXPECT_EQ(v.minor, 3);
    EXPECT_EQ(v.patch, 0);
    
    version_increment(ctx, &v, 0);
    EXPECT_EQ(v.major, 2);
    EXPECT_EQ(v.minor, 0);
    EXPECT_EQ(v.patch, 0);
    
    version_destroy(ctx);
}

void test_version_is_valid() {
    TEST(Version_IsValid);
    version_t v1 = {1, 2, 3, NULL, NULL};
    version_t v2 = {-1, 2, 3, NULL, NULL};
    version_t v3 = {1, -2, 3, NULL, NULL};
    
    EXPECT_TRUE(version_is_valid(&v1));
    EXPECT_TRUE(!version_is_valid(&v2));
    EXPECT_TRUE(!version_is_valid(&v3));
}

void test_version_copy() {
    TEST(Version_Copy);
    version_t src = {1, 2, 3, NULL, NULL};
    version_t dest;
    
    version_error_t err = version_copy(&dest, &src);
    EXPECT_EQ(err, VERSION_OK);
    EXPECT_EQ(dest.major, 1);
    EXPECT_EQ(dest.minor, 2);
    EXPECT_EQ(dest.patch, 3);
}

int main() {
    UTEST_BEGIN();
    test_version_create();
    test_version_parse();
    test_version_parse_two_parts();
    test_version_compare();
    test_version_to_string();
    test_version_increment();
    test_version_is_valid();
    test_version_copy();
    UTEST_END();
}
