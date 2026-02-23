#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/version_compare.h"

void test_version_compare_str() {
    TEST(VersionCompare_Str);
    EXPECT_EQ(version_compare_str("1.0.0", "1.0.0"), 0);
    EXPECT_EQ(version_compare_str("1.0.0", "1.0.1"), -1);
    EXPECT_EQ(version_compare_str("1.0.1", "1.0.0"), 1);
    EXPECT_EQ(version_compare_str("2.0.0", "1.9.9"), 1);
    EXPECT_EQ(version_compare_str("1.9.9", "2.0.0"), -1);
}

void test_version_compare_str_major() {
    TEST(VersionCompare_StrMajor);
    EXPECT_EQ(version_compare_str("2.0.0", "1.0.0"), 1);
    EXPECT_EQ(version_compare_str("1.0.0", "2.0.0"), -1);
}

void test_version_compare_str_minor() {
    TEST(VersionCompare_StrMinor);
    EXPECT_EQ(version_compare_str("1.2.0", "1.1.0"), 1);
    EXPECT_EQ(version_compare_str("1.1.0", "1.2.0"), -1);
}

void test_version_compare_str_patch() {
    TEST(VersionCompare_StrPatch);
    EXPECT_EQ(version_compare_str("1.0.2", "1.0.1"), 1);
    EXPECT_EQ(version_compare_str("1.0.1", "1.0.2"), -1);
}

void test_version_compare_create_destroy() {
    TEST(VersionCompare_CreateDestroy);
    version_compare_ctx_t* ctx = NULL;
    version_compare_error_t error = version_compare_create(&ctx, NULL);
    
    EXPECT_EQ(error, VERSION_COMPARE_OK);
    EXPECT_TRUE(ctx != NULL);
    
    version_compare_destroy(ctx);
}

void test_version_compare_str_safe() {
    TEST(VersionCompare_StrSafe);
    version_compare_ctx_t* ctx = NULL;
    version_compare_create(&ctx, NULL);
    
    int result;
    version_compare_error_t error = version_compare_str_safe(ctx, "1.0.0", "2.0.0", &result);
    EXPECT_EQ(error, VERSION_COMPARE_OK);
    EXPECT_EQ(result, -1);
    
    version_compare_destroy(ctx);
}

void test_version_compare_validate() {
    TEST(VersionCompare_Validate);
    version_compare_ctx_t* ctx = NULL;
    version_compare_create(&ctx, NULL);
    
    bool valid = version_compare_validate(ctx, "1.0.0");
    EXPECT_TRUE(valid);
    
    valid = version_compare_validate(ctx, "1.2.3.4.5");
    EXPECT_TRUE(valid);
    
    version_compare_destroy(ctx);
}

void test_version_compare_strerror() {
    TEST(VersionCompare_Strerror);
    const char* msg = version_compare_strerror(VERSION_COMPARE_OK);
    EXPECT_TRUE(msg != NULL);
    
    msg = version_compare_strerror(VERSION_COMPARE_INVALID_PARAMS);
    EXPECT_TRUE(msg != NULL);
}

int main() {
    test_version_compare_str();
    test_version_compare_str_major();
    test_version_compare_str_minor();
    test_version_compare_str_patch();
    test_version_compare_create_destroy();
    test_version_compare_str_safe();
    test_version_compare_validate();
    test_version_compare_strerror();

    return 0;
}
