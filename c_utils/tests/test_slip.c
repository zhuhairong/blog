#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/slip.h"

void test_slip_config_init() {
    TEST(Slip_ConfigInit);
    slip_config_t config;
    slip_config_init(&config);
    EXPECT_TRUE(config.add_end_delimiter);
    EXPECT_FALSE(config.add_start_delimiter);
    EXPECT_FALSE(config.enable_crc);
    EXPECT_TRUE(config.enable_escape);
    EXPECT_EQ(config.max_packet_size, (size_t)4096);
    EXPECT_EQ(config.buffer_size, (size_t)8192);
}

void test_slip_state_init() {
    TEST(Slip_StateInit);
    slip_state_t state;
    slip_state_init(&state);
    EXPECT_EQ(state.last_error, SLIP_OK);
    EXPECT_EQ(state.total_encoded, (size_t)0);
    EXPECT_EQ(state.total_decoded, (size_t)0);
    EXPECT_TRUE(state.is_initialized);
}

void test_slip_reset_state() {
    TEST(Slip_ResetState);
    slip_state_t state;
    state.total_encoded = 100;
    state.total_decoded = 200;
    slip_reset_state(&state);
    EXPECT_EQ(state.total_encoded, (size_t)0);
    EXPECT_EQ(state.total_decoded, (size_t)0);
}

void test_slip_encode() {
    TEST(Slip_Encode);
    const unsigned char input[] = "Hello World";
    size_t input_len = strlen((const char*)input);
    unsigned char output[256];
    size_t encoded_len = slip_encode(input, input_len, output);
    EXPECT_TRUE(encoded_len > 0);
    EXPECT_EQ(output[0], (unsigned char)SLIP_END);
    EXPECT_EQ(output[encoded_len - 1], (unsigned char)SLIP_END);
}

void test_slip_encode_null() {
    TEST(Slip_EncodeNull);
    unsigned char output[256];
    size_t encoded_len = slip_encode(NULL, 10, output);
    EXPECT_EQ(encoded_len, (size_t)0);
}

void test_slip_encode_ex() {
    TEST(Slip_EncodeEx);
    const unsigned char input[] = "Hello World";
    size_t input_len = strlen((const char*)input);
    unsigned char output[256];
    slip_config_t config;
    slip_config_init(&config);
    slip_state_t state;
    slip_state_init(&state);
    size_t encoded_len = slip_encode_ex(input, input_len, output, 256, &config, &state);
    EXPECT_TRUE(encoded_len > 0);
    EXPECT_EQ(state.last_error, SLIP_OK);
}

void test_slip_encode_ex_null() {
    TEST(Slip_EncodeExNull);
    unsigned char output[256];
    slip_state_t state;
    size_t encoded_len = slip_encode_ex(NULL, 10, output, 256, NULL, &state);
    EXPECT_EQ(encoded_len, (size_t)0);
    EXPECT_EQ(state.last_error, SLIP_ERROR_INVALID_PARAMS);
}

void test_slip_encode_ex_buffer_too_small() {
    TEST(Slip_EncodeExBufferTooSmall);
    const unsigned char input[] = "Hello World";
    size_t input_len = strlen((const char*)input);
    unsigned char output[10];
    slip_state_t state;
    size_t encoded_len = slip_encode_ex(input, input_len, output, 10, NULL, &state);
    EXPECT_EQ(encoded_len, (size_t)0);
    EXPECT_EQ(state.last_error, SLIP_ERROR_BUFFER_TOO_SMALL);
}

void test_slip_encode_with_special_chars() {
    TEST(Slip_EncodeWithSpecialChars);
    const unsigned char input[] = { SLIP_END, SLIP_ESC, 0x41, 0x42 };
    size_t input_len = 4;
    unsigned char output[256];
    size_t encoded_len = slip_encode(input, input_len, output);
    EXPECT_TRUE(encoded_len > 0);
}

void test_slip_decode() {
    TEST(Slip_Decode);
    const unsigned char input[] = "Hello World";
    size_t input_len = strlen((const char*)input);
    unsigned char encoded[256];
    size_t encoded_len = slip_encode(input, input_len, encoded);
    unsigned char decoded[256];
    slip_state_t state;
    slip_state_init(&state);
    size_t decoded_len = slip_decode(encoded, encoded_len, decoded, 256, &state);
    EXPECT_EQ(decoded_len, input_len);
    EXPECT_EQ(memcmp(decoded, input, input_len), 0);
}

void test_slip_decode_null() {
    TEST(Slip_DecodeNull);
    unsigned char output[256];
    slip_state_t state;
    size_t decoded_len = slip_decode(NULL, 10, output, 256, &state);
    EXPECT_EQ(decoded_len, (size_t)0);
}

void test_slip_decode_ex() {
    TEST(Slip_DecodeEx);
    const unsigned char input[] = "Hello World";
    size_t input_len = strlen((const char*)input);
    unsigned char encoded[256];
    slip_config_t config;
    slip_config_init(&config);
    slip_state_t state_encode;
    slip_state_init(&state_encode);
    size_t encoded_len = slip_encode_ex(input, input_len, encoded, 256, &config, &state_encode);
    unsigned char decoded[256];
    slip_state_t state_decode;
    slip_state_init(&state_decode);
    size_t decoded_len = slip_decode_ex(encoded, encoded_len, decoded, 256, &config, &state_decode);
    EXPECT_EQ(decoded_len, input_len);
    EXPECT_EQ(state_decode.last_error, SLIP_OK);
}

void test_slip_calculate_max_encoded_size() {
    TEST(Slip_CalculateMaxEncodedSize);
    slip_config_t config;
    slip_config_init(&config);
    size_t max_size = slip_calculate_max_encoded_size(10, &config);
    EXPECT_TRUE(max_size >= 10 * 2);
}

void test_slip_encode_decode_with_special_chars() {
    TEST(Slip_EncodeDecodeWithSpecialChars);
    const unsigned char input[] = { SLIP_END, SLIP_ESC, 0x41, 0x42 };
    size_t input_len = 4;
    unsigned char encoded[256];
    size_t encoded_len = slip_encode(input, input_len, encoded);
    unsigned char decoded[256];
    slip_state_t state;
    slip_state_init(&state);
    size_t decoded_len = slip_decode(encoded, encoded_len, decoded, 256, &state);
    EXPECT_EQ(decoded_len, input_len);
    EXPECT_EQ(memcmp(decoded, input, input_len), 0);
}

void test_slip_encode_decode_file() {
    TEST(Slip_EncodeDecodeFile);
    const char* original_file = "test_original.txt";
    const char* encoded_file = "test_encoded.slip";
    const char* decoded_file = "test_decoded.txt";
    
    FILE* f = fopen(original_file, "wb");
    const unsigned char data[] = "Hello World! This is a test file with SLIP special chars: " "\xC0" "\xDB";
    fwrite(data, 1, sizeof(data) - 1, f);
    fclose(f);
    
    slip_config_t config;
    slip_config_init(&config);
    slip_state_t state;
    slip_state_init(&state);
    slip_error_t err = slip_encode_file(original_file, encoded_file, &config, &state);
    EXPECT_EQ(err, SLIP_OK);
    
    slip_reset_state(&state);
    err = slip_decode_file(encoded_file, decoded_file, &config, &state);
    EXPECT_EQ(err, SLIP_OK);
    
    f = fopen(original_file, "rb");
    fseek(f, 0, SEEK_END);
    long size1 = ftell(f);
    fseek(f, 0, SEEK_SET);
    unsigned char* buf1 = (unsigned char*)malloc(size1);
    fread(buf1, 1, size1, f);
    fclose(f);
    
    f = fopen(decoded_file, "rb");
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
    remove(encoded_file);
    remove(decoded_file);
}

void test_slip_strerror() {
    TEST(Slip_Strerror);
    slip_state_t state;
    slip_state_init(&state);
    const char* msg = slip_strerror(&state);
    EXPECT_STREQ(msg, "Success");
    msg = slip_strerror(NULL);
    EXPECT_STREQ(msg, "Invalid state");
}

int main() {
    test_slip_config_init();
    test_slip_state_init();
    test_slip_reset_state();
    test_slip_encode();
    test_slip_encode_null();
    test_slip_encode_ex();
    test_slip_encode_ex_null();
    test_slip_encode_ex_buffer_too_small();
    test_slip_encode_with_special_chars();
    test_slip_decode();
    test_slip_decode_null();
    test_slip_decode_ex();
    test_slip_calculate_max_encoded_size();
    test_slip_encode_decode_with_special_chars();
    test_slip_encode_decode_file();
    test_slip_strerror();

    return 0;
}
