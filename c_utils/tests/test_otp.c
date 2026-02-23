#include "../c_utils/utest.h"
#include "../c_utils/otp.h"
#include <string.h>
#include <time.h>

static uint8_t test_secret[20] = {
    0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30,
    0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30
};

void test_otp_default_totp_config() {
    TEST(OTP_DefaultTotpConfig);
    otp_totp_config_t config = otp_default_totp_config();
    EXPECT_EQ(config.algorithm, OTP_ALGORITHM_SHA1);
    EXPECT_EQ(config.digits, 6);
    EXPECT_EQ(config.time_step, 30);
    EXPECT_EQ(config.epoch, 0);
}

void test_otp_default_hotp_config() {
    TEST(OTP_DefaultHotpConfig);
    otp_hotp_config_t config = otp_default_hotp_config();
    EXPECT_EQ(config.algorithm, OTP_ALGORITHM_SHA1);
    EXPECT_EQ(config.digits, 6);
}

void test_otp_generate_secret() {
    TEST(OTP_GenerateSecret);
    uint8_t secret[20];
    otp_error_t err;
    size_t len = otp_generate_secret(secret, sizeof(secret), &err);
    EXPECT_EQ(err, OTP_OK);
    EXPECT_EQ(len, 20);
}

void test_otp_secret_to_base32() {
    TEST(OTP_SecretToBase32);
    char base32[64];
    otp_error_t err;
    size_t len = otp_secret_to_base32(test_secret, sizeof(test_secret), 
                                        base32, sizeof(base32), &err);
    EXPECT_EQ(err, OTP_OK);
    EXPECT_TRUE(len > 0);
}

void test_otp_base32_to_secret() {
    TEST(OTP_Base32ToSecret);
    char base32[64];
    otp_secret_to_base32(test_secret, sizeof(test_secret), base32, sizeof(base32), NULL);
    
    uint8_t secret[20];
    otp_error_t err;
    size_t len = otp_base32_to_secret(base32, secret, sizeof(secret), &err);
    EXPECT_EQ(err, OTP_OK);
    EXPECT_EQ(len, sizeof(test_secret));
    EXPECT_TRUE(memcmp(secret, test_secret, len) == 0);
}

void test_otp_generate_totp() {
    TEST(OTP_GenerateTotp);
    uint64_t time_sec = 59;
    uint32_t code = otp_generate_totp(test_secret, sizeof(test_secret), time_sec);
    EXPECT_TRUE(code >= 100000 && code <= 999999);
}

void test_otp_generate_totp_ex() {
    TEST(OTP_GenerateTotpEx);
    uint64_t time_sec = 59;
    otp_totp_config_t config = otp_default_totp_config();
    otp_error_t err;
    uint32_t code = otp_generate_totp_ex(test_secret, sizeof(test_secret), time_sec, 
                                         &config, &err);
    EXPECT_EQ(err, OTP_OK);
    EXPECT_TRUE(code >= 100000 && code <= 999999);
}

void test_otp_generate_hotp() {
    TEST(OTP_GenerateHotp);
    uint64_t counter = 1;
    otp_error_t err;
    uint32_t code = otp_generate_hotp(test_secret, sizeof(test_secret), counter, 6, &err);
    EXPECT_EQ(err, OTP_OK);
    EXPECT_TRUE(code >= 100000 && code <= 999999);
}

void test_otp_generate_hotp_ex() {
    TEST(OTP_GenerateHotpEx);
    uint64_t counter = 1;
    otp_hotp_config_t config = otp_default_hotp_config();
    otp_error_t err;
    uint32_t code = otp_generate_hotp_ex(test_secret, sizeof(test_secret), counter, 
                                         &config, &err);
    EXPECT_EQ(err, OTP_OK);
    EXPECT_TRUE(code >= 100000 && code <= 999999);
}

void test_otp_verify_totp() {
    TEST(OTP_VerifyTotp);
    uint64_t time_sec = 59;
    uint32_t code = otp_generate_totp(test_secret, sizeof(test_secret), time_sec);
    otp_error_t err;
    bool verified = otp_verify_totp(test_secret, sizeof(test_secret), time_sec, 
                                     code, 0, &err);
    EXPECT_EQ(err, OTP_OK);
    EXPECT_TRUE(verified);
}

void test_otp_verify_totp_ex() {
    TEST(OTP_VerifyTotpEx);
    uint64_t time_sec = 59;
    otp_totp_config_t config = otp_default_totp_config();
    uint32_t code = otp_generate_totp_ex(test_secret, sizeof(test_secret), time_sec, 
                                         &config, NULL);
    otp_error_t err;
    bool verified = otp_verify_totp_ex(test_secret, sizeof(test_secret), time_sec, 
                                        code, 0, &config, &err);
    EXPECT_EQ(err, OTP_OK);
    EXPECT_TRUE(verified);
}

void test_otp_verify_hotp() {
    TEST(OTP_VerifyHotp);
    uint64_t counter = 1;
    otp_error_t err;
    uint32_t code = otp_generate_hotp(test_secret, sizeof(test_secret), counter, 6, NULL);
    bool verified = otp_verify_hotp(test_secret, sizeof(test_secret), counter, 
                                     code, 6, &err);
    EXPECT_EQ(err, OTP_OK);
    EXPECT_TRUE(verified);
}

void test_otp_verify_hotp_ex() {
    TEST(OTP_VerifyHotpEx);
    uint64_t counter = 1;
    otp_hotp_config_t config = otp_default_hotp_config();
    uint32_t code = otp_generate_hotp_ex(test_secret, sizeof(test_secret), counter, 
                                         &config, NULL);
    otp_error_t err;
    bool verified = otp_verify_hotp_ex(test_secret, sizeof(test_secret), counter, 
                                        code, &config, &err);
    EXPECT_EQ(err, OTP_OK);
    EXPECT_TRUE(verified);
}

void test_otp_generate_uri() {
    TEST(OTP_GenerateUri);
    char uri[512];
    otp_uri_config_t uri_config = {
        .type = "totp",
        .label = "user@example.com",
        .issuer = "TestIssuer",
        .secret = test_secret,
        .secret_len = sizeof(test_secret),
        .counter = 0,
        .totp_config = otp_default_totp_config()
    };
    otp_error_t err;
    size_t len = otp_generate_uri(&uri_config, uri, sizeof(uri), &err);
    EXPECT_EQ(err, OTP_OK);
    EXPECT_TRUE(len > 0);
    EXPECT_TRUE(strstr(uri, "otpauth://") != NULL);
}

void test_otp_error_string() {
    TEST(OTP_ErrorString);
    EXPECT_STR_EQ(otp_error_string(OTP_OK), "Success");
    EXPECT_STR_EQ(otp_error_string(OTP_ERROR_NULL_PTR), "Null pointer error");
    EXPECT_STR_EQ(otp_error_string(OTP_ERROR_INVALID_ARGS), "Invalid arguments");
}

void test_otp_null_ptr() {
    TEST(OTP_NullPtr);
    otp_error_t err;
    EXPECT_EQ(otp_generate_secret(NULL, 20, &err), 0);
    EXPECT_EQ(err, OTP_ERROR_NULL_PTR);
}

void test_otp_invalid_digits() {
    TEST(OTP_InvalidDigits);
    otp_totp_config_t config = otp_default_totp_config();
    config.digits = 5;
    otp_error_t err;
    otp_generate_totp_ex(test_secret, sizeof(test_secret), 59, &config, &err);
    EXPECT_EQ(err, OTP_ERROR_INVALID_ARGS);
}

int main() {
    UTEST_BEGIN();
    test_otp_default_totp_config();
    test_otp_default_hotp_config();
    test_otp_generate_secret();
    test_otp_secret_to_base32();
    test_otp_base32_to_secret();
    test_otp_generate_totp();
    test_otp_generate_totp_ex();
    test_otp_generate_hotp();
    test_otp_generate_hotp_ex();
    test_otp_verify_totp();
    test_otp_verify_totp_ex();
    test_otp_verify_hotp();
    test_otp_verify_hotp_ex();
    test_otp_generate_uri();
    test_otp_error_string();
    test_otp_null_ptr();
    test_otp_invalid_digits();
    UTEST_END();
}
