#include <stdio.h>
#include <stdlib.h>
#include "../c_utils/utest.h"
#include "../c_utils/wav_header.h"

void test_wav_hdr_create() {
    TEST(WavHdr_Create);
    wav_hdr_ctx_t* ctx;
    wav_hdr_error_t err = wav_hdr_create(&ctx, NULL);
    EXPECT_EQ(err, WAV_HDR_OK);
    EXPECT_TRUE(ctx != NULL);
    wav_hdr_destroy(ctx);
}

void test_wav_hdr_create_null() {
    TEST(WavHdr_CreateNull);
    wav_hdr_error_t err = wav_hdr_create(NULL, NULL);
    EXPECT_EQ(err, WAV_HDR_INVALID_PARAMS);
}

void test_wav_hdr_init_safe() {
    TEST(WavHdr_InitSafe);
    wav_hdr_ctx_t* ctx;
    wav_hdr_create(&ctx, NULL);
    wav_hdr_t hdr;
    wav_hdr_error_t err = wav_hdr_init_safe(ctx, &hdr, 44100, 2, 16, 44100 * 2 * 2);
    EXPECT_EQ(err, WAV_HDR_OK);
    EXPECT_EQ(hdr.sample_rate, (uint32_t)44100);
    EXPECT_EQ(hdr.num_channels, (uint16_t)2);
    EXPECT_EQ(hdr.bits_per_sample, (uint16_t)16);
    wav_hdr_destroy(ctx);
}

void test_wav_hdr_init_safe_null() {
    TEST(WavHdr_InitSafeNull);
    wav_hdr_error_t err = wav_hdr_init_safe(NULL, NULL, 44100, 2, 16, 0);
    EXPECT_EQ(err, WAV_HDR_INVALID_PARAMS);
}

void test_wav_hdr_init_safe_invalid_params() {
    TEST(WavHdr_InitSafeInvalidParams);
    wav_hdr_ctx_t* ctx;
    wav_hdr_create(&ctx, NULL);
    wav_hdr_t hdr;
    wav_hdr_error_t err = wav_hdr_init_safe(ctx, &hdr, 0, 2, 16, 0);
    EXPECT_EQ(err, WAV_HDR_INVALID_PARAMS);
    wav_hdr_destroy(ctx);
}

void test_wav_hdr_validate() {
    TEST(WavHdr_Validate);
    wav_hdr_ctx_t* ctx;
    wav_hdr_create(&ctx, NULL);
    wav_hdr_t hdr;
    wav_hdr_init(&hdr, 44100, 2, 16, 0);
    wav_hdr_error_t err = wav_hdr_validate(ctx, &hdr);
    EXPECT_EQ(err, WAV_HDR_OK);
    wav_hdr_destroy(ctx);
}

void test_wav_hdr_validate_null() {
    TEST(WavHdr_ValidateNull);
    wav_hdr_error_t err = wav_hdr_validate(NULL, NULL);
    EXPECT_EQ(err, WAV_HDR_INVALID_PARAMS);
}

void test_wav_hdr_get_data_size() {
    TEST(WavHdr_GetDataSize);
    wav_hdr_t hdr;
    wav_hdr_init(&hdr, 44100, 2, 16, 1000);
    uint32_t size = wav_hdr_get_data_size(&hdr);
    EXPECT_EQ(size, (uint32_t)1000);
}

void test_wav_hdr_get_data_size_null() {
    TEST(WavHdr_GetDataSizeNull);
    uint32_t size = wav_hdr_get_data_size(NULL);
    EXPECT_EQ(size, (uint32_t)0);
}

void test_wav_hdr_set_data_size() {
    TEST(WavHdr_SetDataSize);
    wav_hdr_ctx_t* ctx;
    wav_hdr_create(&ctx, NULL);
    wav_hdr_t hdr;
    wav_hdr_init(&hdr, 44100, 2, 16, 1000);
    wav_hdr_error_t err = wav_hdr_set_data_size(ctx, &hdr, 2000);
    EXPECT_EQ(err, WAV_HDR_OK);
    EXPECT_EQ(hdr.subchunk2_size, (uint32_t)2000);
    EXPECT_EQ(hdr.chunk_size, (uint32_t)36 + 2000);
    wav_hdr_destroy(ctx);
}

void test_wav_hdr_set_data_size_null() {
    TEST(WavHdr_SetDataSizeNull);
    wav_hdr_error_t err = wav_hdr_set_data_size(NULL, NULL, 2000);
    EXPECT_EQ(err, WAV_HDR_INVALID_PARAMS);
}

void test_wav_hdr_get_file_size() {
    TEST(WavHdr_GetFileSize);
    wav_hdr_t hdr;
    wav_hdr_init(&hdr, 44100, 2, 16, 1000);
    uint32_t size = wav_hdr_get_file_size(&hdr);
    EXPECT_EQ(size, (uint32_t)36 + 1000 + 8);
}

void test_wav_hdr_get_file_size_null() {
    TEST(WavHdr_GetFileSizeNull);
    uint32_t size = wav_hdr_get_file_size(NULL);
    EXPECT_EQ(size, (uint32_t)0);
}

void test_wav_hdr_is_pcm() {
    TEST(WavHdr_IsPcm);
    wav_hdr_t hdr;
    wav_hdr_init(&hdr, 44100, 2, 16, 0);
    bool is_pcm = wav_hdr_is_pcm(&hdr);
    EXPECT_TRUE(is_pcm);
}

void test_wav_hdr_is_pcm_null() {
    TEST(WavHdr_IsPcmNull);
    bool is_pcm = wav_hdr_is_pcm(NULL);
    EXPECT_FALSE(is_pcm);
}

void test_wav_hdr_get_format_name() {
    TEST(WavHdr_GetFormatName);
    const char* name = wav_hdr_get_format_name(1);
    EXPECT_STREQ(name, "PCM");
    name = wav_hdr_get_format_name(2);
    EXPECT_STREQ(name, "Microsoft ADPCM");
    name = wav_hdr_get_format_name(9999);
    EXPECT_STREQ(name, "Unknown");
}

void test_wav_hdr_get_last_error() {
    TEST(WavHdr_GetLastError);
    wav_hdr_ctx_t* ctx;
    wav_hdr_create(&ctx, NULL);
    wav_hdr_error_t err = wav_hdr_get_last_error(ctx);
    EXPECT_EQ(err, WAV_HDR_OK);
    wav_hdr_destroy(ctx);
}

void test_wav_hdr_get_last_error_null() {
    TEST(WavHdr_GetLastErrorNull);
    wav_hdr_error_t err = wav_hdr_get_last_error(NULL);
    EXPECT_EQ(err, WAV_HDR_INVALID_PARAMS);
}

void test_wav_hdr_strerror() {
    TEST(WavHdr_Strerror);
    const char* msg = wav_hdr_strerror(WAV_HDR_OK);
    EXPECT_STREQ(msg, "Success");
    msg = wav_hdr_strerror(WAV_HDR_INVALID_PARAMS);
    EXPECT_STREQ(msg, "Invalid parameters");
    msg = wav_hdr_strerror(9999);
    EXPECT_STREQ(msg, "Unknown error");
}

void test_wav_hdr_init() {
    TEST(WavHdr_Init);
    wav_hdr_t hdr;
    wav_hdr_init(&hdr, 44100, 2, 16, 1000);
    EXPECT_EQ(hdr.sample_rate, (uint32_t)44100);
    EXPECT_EQ(hdr.num_channels, (uint16_t)2);
    EXPECT_EQ(hdr.bits_per_sample, (uint16_t)16);
}

void test_wav_hdr_write_and_read() {
    TEST(WavHdr_WriteAndRead);
    wav_hdr_ctx_t* ctx;
    wav_hdr_create(&ctx, NULL);
    wav_hdr_t hdr;
    wav_hdr_init(&hdr, 44100, 2, 16, 1000);
    
    const char* test_file = "test_wav_write_read.wav";
    wav_hdr_error_t err = wav_hdr_write(ctx, test_file, &hdr);
    EXPECT_EQ(err, WAV_HDR_OK);
    
    wav_hdr_t hdr_read;
    err = wav_hdr_read(ctx, test_file, &hdr_read);
    EXPECT_EQ(err, WAV_HDR_OK);
    EXPECT_EQ(hdr_read.sample_rate, hdr.sample_rate);
    EXPECT_EQ(hdr_read.num_channels, hdr.num_channels);
    EXPECT_EQ(hdr_read.bits_per_sample, hdr.bits_per_sample);
    
    remove(test_file);
    wav_hdr_destroy(ctx);
}

int main() {
    test_wav_hdr_create();
    test_wav_hdr_create_null();
    test_wav_hdr_init_safe();
    test_wav_hdr_init_safe_null();
    test_wav_hdr_init_safe_invalid_params();
    test_wav_hdr_validate();
    test_wav_hdr_validate_null();
    test_wav_hdr_get_data_size();
    test_wav_hdr_get_data_size_null();
    test_wav_hdr_set_data_size();
    test_wav_hdr_set_data_size_null();
    test_wav_hdr_get_file_size();
    test_wav_hdr_get_file_size_null();
    test_wav_hdr_is_pcm();
    test_wav_hdr_is_pcm_null();
    test_wav_hdr_get_format_name();
    test_wav_hdr_get_last_error();
    test_wav_hdr_get_last_error_null();
    test_wav_hdr_strerror();
    test_wav_hdr_init();
    test_wav_hdr_write_and_read();

    return 0;
}
