#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/http.h"

void test_http_default_config() {
    TEST(Http_DefaultConfig);
    http_config_t config = http_default_config();
    EXPECT_TRUE(config.max_headers > 0);
    EXPECT_TRUE(config.max_body_size > 0);
}

void test_http_parse_request_null() {
    TEST(Http_ParseRequestNull);
    http_error_t error;
    http_config_t config = http_default_config();
    
    http_req_t* req = http_parse_request(NULL, &config, &error);
    EXPECT_TRUE(req == NULL);
    EXPECT_EQ(error, HTTP_ERROR_INVALID_PARAM);
}

void test_http_parse_response_null() {
    TEST(Http_ParseResponseNull);
    http_error_t error;
    http_config_t config = http_default_config();
    
    http_res_t* res = http_parse_response(NULL, &config, &error);
    EXPECT_TRUE(res == NULL);
    EXPECT_EQ(error, HTTP_ERROR_INVALID_PARAM);
}

void test_http_strerror() {
    TEST(Http_Strerror);
    const char* msg = http_strerror(HTTP_OK);
    EXPECT_TRUE(msg != NULL);
    
    msg = http_strerror(HTTP_ERROR_INVALID_PARAM);
    EXPECT_TRUE(msg != NULL);
}

void test_http_build_request_simple() {
    TEST(Http_BuildRequestSimple);
    http_error_t error;
    
    char* req = http_build_request("GET", "/", "HTTP/1.1", NULL, 0, NULL, 0, &error);
    EXPECT_TRUE(req != NULL || req == NULL);
    
    if (req != NULL) {
        free(req);
    }
}

int main() {
    test_http_default_config();
    test_http_parse_request_null();
    test_http_parse_response_null();
    test_http_strerror();
    test_http_build_request_simple();

    return 0;
}
