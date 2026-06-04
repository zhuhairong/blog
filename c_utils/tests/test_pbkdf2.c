#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/pbkdf2.h"

void test_pbkdf2_sha1_basic() {
    TEST(PBKDF2_SHA1_Basic);
    const char* pass = "password";
    uint8_t salt[] = "salt";
    uint8_t out[20];
    
    /* RFC 6070 test vector: password="password", salt="salt", c=1, dkLen=20 */
    uint8_t expected[20] = {
        0x0c, 0x60, 0xc8, 0x0f, 0x96, 0x1f, 0x0e, 0x71,
        0xf3, 0xa9, 0xb5, 0x24, 0xaf, 0x60, 0x12, 0x06,
        0x2f, 0xe0, 0x37, 0xa6
    };
    pbkdf2_sha1(pass, salt, 4, 1, out, 20);

    EXPECT_EQ(memcmp(out, expected, 20), 0);
}

void test_pbkdf2_types() {
    TEST(PBKDF2_Types);
    pbkdf2_config_t config;
    memset(&config, 0, sizeof(config));
    /* pbkdf2_config_t type compiles and is instantiable */
    
    EXPECT_TRUE(PBKDF2_OK == 0);
    EXPECT_TRUE(PBKDF2_ALGORITHM_SHA1 == 0);
}

void test_pbkdf2_algorithm_values() {
    TEST(PBKDF2_AlgorithmValues);
    EXPECT_TRUE(PBKDF2_ALGORITHM_SHA1 == 0);
    EXPECT_TRUE(PBKDF2_ALGORITHM_SHA256 == 1);
    EXPECT_TRUE(PBKDF2_ALGORITHM_SHA512 == 2);
}

void test_pbkdf2_error_values() {
    TEST(PBKDF2_ErrorValues);
    EXPECT_TRUE(PBKDF2_OK == 0);
    EXPECT_TRUE(PBKDF2_ERROR_NULL_PTR == 1);
    EXPECT_TRUE(PBKDF2_ERROR_INVALID_ARGS == 2);
}

void test_pbkdf2_config_size() {
    TEST(PBKDF2_ConfigSize);
    pbkdf2_config_t config;
    /* Verify pbkdf2_config_t struct fields are accessible at compile time */
    (void)sizeof(config.algorithm);
    (void)sizeof(config.iterations);
    (void)sizeof(config.salt_len);
    (void)sizeof(config.output_len);
}

int main() {
    UTEST_BEGIN();
    test_pbkdf2_sha1_basic();
    test_pbkdf2_types();
    test_pbkdf2_algorithm_values();
    test_pbkdf2_error_values();
    test_pbkdf2_config_size();

    UTEST_END();
}
