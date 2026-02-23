#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/delta_encoding.h"

void test_delta_encode_decode_int32() {
    TEST(Delta_EncodeDecodeInt32);
    int32_t in[] = {100, 102, 105, 110, 120};
    int32_t encoded[5] = {0};
    int32_t decoded[5] = {0};
    
    delta_encoding_error_t err;
    bool result = delta_encode(in, encoded, 5, DELTA_TYPE_INT32, &err);
    EXPECT_TRUE(result);
    
    result = delta_decode(encoded, decoded, 5, DELTA_TYPE_INT32, &err);
    EXPECT_TRUE(result);
    
    for (int i = 0; i < 5; i++) {
        EXPECT_EQ(in[i], decoded[i]);
    }
}

void test_delta_default_config() {
    TEST(Delta_DefaultConfig);
    delta_encoding_config_t config = delta_encoding_default_config(DELTA_TYPE_INT32);
    
    EXPECT_EQ(config.type, DELTA_TYPE_INT32);
}

void test_delta_strerror() {
    TEST(Delta_Strerror);
    const char* msg = delta_encoding_strerror(DELTA_ENCODING_OK);
    EXPECT_TRUE(msg != NULL);
    
    msg = delta_encoding_strerror(DELTA_ENCODING_ERROR_INVALID_PARAM);
    EXPECT_TRUE(msg != NULL);
}

void test_delta_encode_single() {
    TEST(Delta_EncodeSingle);
    int32_t in[] = {42};
    int32_t encoded[1] = {0};
    int32_t decoded[1] = {0};
    
    delta_encoding_error_t err;
    bool result = delta_encode(in, encoded, 1, DELTA_TYPE_INT32, &err);
    EXPECT_TRUE(result);
    
    result = delta_decode(encoded, decoded, 1, DELTA_TYPE_INT32, &err);
    EXPECT_TRUE(result);
    EXPECT_EQ(in[0], decoded[0]);
}

void test_delta_encode_int64() {
    TEST(Delta_EncodeInt64);
    int64_t in[] = {1000, 2000, 3500, 5000};
    int64_t encoded[4] = {0};
    int64_t decoded[4] = {0};
    
    delta_encoding_error_t err;
    bool result = delta_encode(in, encoded, 4, DELTA_TYPE_INT64, &err);
    EXPECT_TRUE(result);
    
    result = delta_decode(encoded, decoded, 4, DELTA_TYPE_INT64, &err);
    EXPECT_TRUE(result);
    
    for (int i = 0; i < 4; i++) {
        EXPECT_EQ(in[i], decoded[i]);
    }
}

int main() {
    test_delta_encode_decode_int32();
    test_delta_default_config();
    test_delta_strerror();
    test_delta_encode_single();
    test_delta_encode_int64();

    return 0;
}
