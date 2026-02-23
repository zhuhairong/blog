#include "../c_utils/utest.h"
#include "../c_utils/url_codec.h"
#include <string.h>
#include <stdlib.h>

void test_url_codec_create() {
    TEST(URL_Codec_Create);
    url_codec_ctx_t* ctx = NULL;
    url_codec_error_t err = url_codec_create(&ctx, NULL);
    EXPECT_EQ(err, URL_CODEC_OK);
    EXPECT_TRUE(ctx != NULL);
    url_codec_destroy(ctx);
}

void test_url_codec_encode() {
    TEST(URL_Codec_Encode);
    url_codec_ctx_t* ctx = NULL;
    url_codec_create(&ctx, NULL);
    
    const char* input = "hello world";
    size_t out_size = 0;
    
    url_codec_error_t err = url_codec_encode(ctx, input, strlen(input), NULL, &out_size);
    EXPECT_EQ(err, URL_CODEC_OK);
    EXPECT_TRUE(out_size > 0);
    
    char* output = malloc(out_size);
    err = url_codec_encode(ctx, input, strlen(input), output, &out_size);
    EXPECT_EQ(err, URL_CODEC_OK);
    EXPECT_STR_EQ(output, "hello+world");
    
    free(output);
    url_codec_destroy(ctx);
}

void test_url_codec_decode() {
    TEST(URL_Codec_Decode);
    url_codec_ctx_t* ctx = NULL;
    url_codec_create(&ctx, NULL);
    
    const char* input = "hello%20world";
    size_t out_size = 0;
    
    url_codec_error_t err = url_codec_decode(ctx, input, strlen(input), NULL, &out_size);
    EXPECT_EQ(err, URL_CODEC_OK);
    
    char* output = malloc(out_size + 1);
    err = url_codec_decode(ctx, input, strlen(input), output, &out_size);
    EXPECT_EQ(err, URL_CODEC_OK);
    output[out_size] = '\0';
    EXPECT_STR_EQ(output, "hello world");
    
    free(output);
    url_codec_destroy(ctx);
}

void test_url_codec_roundtrip() {
    TEST(URL_Codec_Roundtrip);
    url_codec_ctx_t* ctx = NULL;
    url_codec_create(&ctx, NULL);
    
    const char* input = "test=value&name=张三";
    size_t enc_size = 0;
    
    url_codec_encode(ctx, input, strlen(input), NULL, &enc_size);
    char* encoded = malloc(enc_size);
    url_codec_encode(ctx, input, strlen(input), encoded, &enc_size);
    
    size_t dec_size = 0;
    url_codec_decode(ctx, encoded, strlen(encoded), NULL, &dec_size);
    char* decoded = malloc(dec_size + 1);
    url_codec_decode(ctx, encoded, strlen(encoded), decoded, &dec_size);
    decoded[dec_size] = '\0';
    
    EXPECT_STR_EQ(decoded, input);
    
    free(encoded);
    free(decoded);
    url_codec_destroy(ctx);
}

void test_url_codec_special_chars() {
    TEST(URL_Codec_SpecialChars);
    url_codec_ctx_t* ctx = NULL;
    url_codec_create(&ctx, NULL);
    
    const char* input = "!@#$%^&*()";
    size_t out_size = 0;
    
    url_codec_encode(ctx, input, strlen(input), NULL, &out_size);
    char* output = malloc(out_size);
    url_codec_error_t err = url_codec_encode(ctx, input, strlen(input), output, &out_size);
    EXPECT_EQ(err, URL_CODEC_OK);
    EXPECT_TRUE(strlen(output) > strlen(input));
    
    free(output);
    url_codec_destroy(ctx);
}

int main() {
    UTEST_BEGIN();
    test_url_codec_create();
    test_url_codec_encode();
    test_url_codec_decode();
    test_url_codec_roundtrip();
    test_url_codec_special_chars();
    UTEST_END();
}
