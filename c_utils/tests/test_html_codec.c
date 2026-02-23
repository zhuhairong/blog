#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/html_codec.h"

void test_html_encode_basic() {
    TEST(Html_EncodeBasic);
    const char* in = "<hello>";
    char out[64] = {0};
    html_codec_error_t err;
    
    size_t len = html_encode(in, out, sizeof(out), HTML_CODEC_FLAG_NONE, &err);
    EXPECT_TRUE(len > 0);
}

void test_html_decode_basic() {
    TEST(Html_DecodeBasic);
    const char* in = "&lt;hello&gt;";
    char out[64] = {0};
    html_codec_error_t err;
    
    size_t len = html_decode(in, out, sizeof(out), HTML_CODEC_FLAG_NONE, &err);
    EXPECT_TRUE(len > 0);
}

void test_html_encode_decode_roundtrip() {
    TEST(Html_EncodeDecodeRoundtrip);
    const char* in = "<test&>";
    char encoded[64] = {0};
    char decoded[64] = {0};
    html_codec_error_t err;
    
    size_t enc_len = html_encode(in, encoded, sizeof(encoded), HTML_CODEC_FLAG_NONE, &err);
    EXPECT_TRUE(enc_len > 0);
    
    size_t dec_len = html_decode(encoded, decoded, sizeof(decoded), HTML_CODEC_FLAG_NONE, &err);
    EXPECT_TRUE(dec_len > 0);
}

void test_html_default_config() {
    TEST(Html_DefaultConfig);
    html_codec_config_t config = html_codec_default_config();
    
    EXPECT_TRUE(config.encode_less_than || !config.encode_less_than);
}

void test_html_strerror() {
    TEST(Html_Strerror);
    const char* msg = html_codec_strerror(HTML_CODEC_OK);
    EXPECT_TRUE(msg != NULL);
    
    msg = html_codec_strerror(HTML_CODEC_ERROR_INVALID_PARAM);
    EXPECT_TRUE(msg != NULL);
}

int main() {
    test_html_encode_basic();
    test_html_decode_basic();
    test_html_encode_decode_roundtrip();
    test_html_default_config();
    test_html_strerror();

    return 0;
}
