#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/run_length_limited.h"

void test_rll_default_config() {
    TEST(Rll_DefaultConfig);
    rll_config_t config = rll_default_config();
    EXPECT_EQ(config.min_run_length, (size_t)2);
    EXPECT_EQ(config.max_run_length, (size_t)127);
    EXPECT_FALSE(config.use_optimized);
    EXPECT_TRUE(config.check_input);
    EXPECT_TRUE(config.check_output);
}

void test_rll_1_7_default_config() {
    TEST(Rll_1_7_DefaultConfig);
    rll_config_t config = rll_1_7_default_config();
    EXPECT_EQ(config.min_run_length, (size_t)1);
    EXPECT_EQ(config.max_run_length, (size_t)7);
    EXPECT_TRUE(config.use_optimized);
}

void test_rll_encode_basic() {
    TEST(Rll_EncodeBasic);
    const unsigned char input[] = "AAAAABBBBBCCCC";
    size_t input_len = strlen((const char*)input);
    unsigned char output[256];
    size_t encoded_len = rll_encode_basic(input, input_len, output);
    EXPECT_TRUE(encoded_len > 0);
}

void test_rll_encode_basic_null() {
    TEST(Rll_EncodeBasicNull);
    unsigned char output[256];
    size_t encoded_len = rll_encode_basic(NULL, 10, output);
    EXPECT_EQ(encoded_len, (size_t)0);
}

void test_rll_encode() {
    TEST(Rll_Encode);
    const unsigned char input[] = "AAAAABBBBBCCCC";
    size_t input_len = strlen((const char*)input);
    unsigned char output[256];
    rll_result_t result;
    rll_error_t error;
    size_t encoded_len = rll_encode(input, input_len, output, 256, NULL, &result, &error);
    EXPECT_TRUE(encoded_len > 0);
    EXPECT_EQ(error, RLL_OK);
    EXPECT_EQ(result.input_size, input_len);
    EXPECT_TRUE(result.output_size > 0);
}

void test_rll_encode_null() {
    TEST(Rll_EncodeNull);
    unsigned char output[256];
    rll_error_t error;
    size_t encoded_len = rll_encode(NULL, 10, output, 256, NULL, NULL, &error);
    EXPECT_EQ(encoded_len, (size_t)0);
    EXPECT_EQ(error, RLL_ERROR_NULL_PTR);
}

void test_rll_encode_empty() {
    TEST(Rll_EncodeEmpty);
    unsigned char output[256];
    rll_error_t error;
    size_t encoded_len = rll_encode(output, 0, output, 256, NULL, NULL, &error);
    EXPECT_EQ(encoded_len, (size_t)0);
    EXPECT_EQ(error, RLL_OK);
}

void test_rll_encode_output_too_small() {
    TEST(Rll_EncodeOutputTooSmall);
    const unsigned char input[] = "AAAAABBBBBCCCC";
    size_t input_len = strlen((const char*)input);
    unsigned char output[10];
    rll_error_t error;
    size_t encoded_len = rll_encode(input, input_len, output, 10, NULL, NULL, &error);
    EXPECT_EQ(encoded_len, (size_t)0);
    EXPECT_EQ(error, RLL_ERROR_OUTPUT_TOO_SMALL);
}

void test_rll_decode() {
    TEST(Rll_Decode);
    const unsigned char input[] = "AAAAABBBBBCCCC";
    size_t input_len = strlen((const char*)input);
    unsigned char encoded[256];
    rll_result_t encode_result;
    rll_error_t encode_error;
    size_t encoded_len = rll_encode(input, input_len, encoded, 256, NULL, &encode_result, &encode_error);
    
    unsigned char decoded[256];
    rll_result_t decode_result;
    rll_error_t decode_error;
    size_t decoded_len = rll_decode(encoded, encoded_len, decoded, 256, NULL, &decode_result, &decode_error);
    
    EXPECT_EQ(decoded_len, input_len);
    EXPECT_EQ(memcmp(decoded, input, input_len), 0);
    EXPECT_EQ(decode_error, RLL_OK);
}

void test_rll_decode_null() {
    TEST(Rll_DecodeNull);
    unsigned char output[256];
    rll_error_t error;
    size_t decoded_len = rll_decode(NULL, 10, output, 256, NULL, NULL, &error);
    EXPECT_EQ(decoded_len, (size_t)0);
    EXPECT_EQ(error, RLL_ERROR_NULL_PTR);
}

void test_rll_1_7_encode_decode() {
    TEST(Rll_1_7_EncodeDecode);
    const unsigned char input[] = "AAABBBCCCDDDEEE";
    size_t input_len = strlen((const char*)input);
    unsigned char encoded[256];
    rll_result_t encode_result;
    rll_error_t encode_error;
    size_t encoded_len = rll_1_7_encode(input, input_len, encoded, 256, &encode_result, &encode_error);
    
    unsigned char decoded[256];
    rll_result_t decode_result;
    rll_error_t decode_error;
    size_t decoded_len = rll_1_7_decode(encoded, encoded_len, decoded, 256, &decode_result, &decode_error);
    
    EXPECT_EQ(decoded_len, input_len);
    EXPECT_EQ(memcmp(decoded, input, input_len), 0);
}

void test_rll_calculate_max_output_size() {
    TEST(Rll_CalculateMaxOutputSize);
    rll_config_t config = rll_default_config();
    size_t max_size = rll_calculate_max_output_size(100, &config);
    EXPECT_TRUE(max_size >= 100 * 2);
}

void test_rll_validate_encoded_data() {
    TEST(Rll_ValidateEncodedData);
    const unsigned char input[] = "AAAAABBBBBCCCC";
    size_t input_len = strlen((const char*)input);
    unsigned char encoded[256];
    rll_error_t encode_error;
    size_t encoded_len = rll_encode(input, input_len, encoded, 256, NULL, NULL, &encode_error);
    
    rll_error_t validate_error;
    bool valid = rll_validate_encoded_data(encoded, encoded_len, NULL, &validate_error);
    EXPECT_TRUE(valid);
    EXPECT_EQ(validate_error, RLL_OK);
}

void test_rll_validate_encoded_data_null() {
    TEST(Rll_ValidateEncodedDataNull);
    rll_error_t error;
    bool valid = rll_validate_encoded_data(NULL, 10, NULL, &error);
    EXPECT_FALSE(valid);
    EXPECT_EQ(error, RLL_ERROR_NULL_PTR);
}

void test_rll_compress_decompress_file() {
    TEST(Rll_CompressDecompressFile);
    const char* original_file = "test_rll_original.txt";
    const char* compressed_file = "test_rll_compressed.bin";
    const char* decompressed_file = "test_rll_decompressed.txt";
    
    FILE* f = fopen(original_file, "wb");
    const unsigned char data[] = "AAAAABBBBBCCCCDDDDD";
    fwrite(data, 1, sizeof(data) - 1, f);
    fclose(f);
    
    rll_config_t config = rll_default_config();
    rll_result_t result;
    rll_error_t error;
    bool success = rll_compress_file(original_file, compressed_file, &config, &result, &error);
    EXPECT_TRUE(success);
    EXPECT_EQ(error, RLL_OK);
    
    success = rll_decompress_file(compressed_file, decompressed_file, &config, &result, &error);
    EXPECT_TRUE(success);
    EXPECT_EQ(error, RLL_OK);
    
    f = fopen(original_file, "rb");
    fseek(f, 0, SEEK_END);
    long size1 = ftell(f);
    fseek(f, 0, SEEK_SET);
    unsigned char* buf1 = (unsigned char*)malloc(size1);
    fread(buf1, 1, size1, f);
    fclose(f);
    
    f = fopen(decompressed_file, "rb");
    fseek(f, 0, SEEK_END);
    long size2 = ftell(f);
    fseek(f, 0, SEEK_SET);
    unsigned char* buf2 = (unsigned char*)malloc(size2);
    fread(buf2, 1, size2, f);
    fclose(f);
    
    EXPECT_EQ(size1, size2);
    EXPECT_EQ(memcmp(buf1, buf2, size1), 0);
    
    free(buf1);
    free(buf2);
    remove(original_file);
    remove(compressed_file);
    remove(decompressed_file);
}

void test_rll_error_string() {
    TEST(Rll_ErrorString);
    const char* msg = rll_error_string(RLL_OK);
    EXPECT_STREQ(msg, "Success");
    msg = rll_error_string(RLL_ERROR_NULL_PTR);
    EXPECT_STREQ(msg, "Null pointer error");
    msg = rll_error_string(9999);
    EXPECT_STREQ(msg, "Unknown error");
}

int main() {
    test_rll_default_config();
    test_rll_1_7_default_config();
    test_rll_encode_basic();
    test_rll_encode_basic_null();
    test_rll_encode();
    test_rll_encode_null();
    test_rll_encode_empty();
    test_rll_encode_output_too_small();
    test_rll_decode();
    test_rll_decode_null();
    test_rll_1_7_encode_decode();
    test_rll_calculate_max_output_size();
    test_rll_validate_encoded_data();
    test_rll_validate_encoded_data_null();
    test_rll_compress_decompress_file();
    test_rll_error_string();

    return 0;
}
