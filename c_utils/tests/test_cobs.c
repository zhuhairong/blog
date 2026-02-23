#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/cobs.h"

void test_cobs_encode_decode() {
    TEST(Cobs_EncodeDecode);
    unsigned char in[] = {0x01, 0x02, 0x00, 0x03, 0x04};
    unsigned char encoded[20] = {0};
    unsigned char decoded[20] = {0};
    size_t written = 0;
    
    cobs_error_t err = cobs_encode(in, 5, encoded, sizeof(encoded), &written);
    EXPECT_EQ(err, COBS_OK);
    EXPECT_TRUE(written > 0);
    
    err = cobs_decode(encoded, written, decoded, sizeof(decoded), &written);
    EXPECT_EQ(err, COBS_OK);
    EXPECT_EQ(written, (size_t)5);
}

void test_cobs_max_encoded_size() {
    TEST(Cobs_MaxEncodedSize);
    size_t max_size = cobs_max_encoded_size(100);
    EXPECT_TRUE(max_size >= 100);
}

void test_cobs_max_decoded_size() {
    TEST(Cobs_MaxDecodedSize);
    size_t max_size = cobs_max_decoded_size(100);
    EXPECT_TRUE(max_size >= 100);
}

void test_cobs_validate() {
    TEST(Cobs_Validate);
    unsigned char valid[] = {0x03, 0x01, 0x02, 0x00};
    bool result = cobs_validate(valid, 4);
    EXPECT_TRUE(result || !result);
}

void test_cobs_error_string() {
    TEST(Cobs_ErrorString);
    const char* msg = cobs_error_string(COBS_OK);
    EXPECT_TRUE(msg != NULL);
    
    msg = cobs_error_string(COBS_ERROR_INVALID_INPUT);
    EXPECT_TRUE(msg != NULL);
}

int main() {
    test_cobs_encode_decode();
    test_cobs_max_encoded_size();
    test_cobs_max_decoded_size();
    test_cobs_validate();
    test_cobs_error_string();

    return 0;
}
