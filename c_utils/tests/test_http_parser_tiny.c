#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/http_parser_tiny.h"

void test_http_parse_request_line_basic() {
    TEST(HttpParseRequestLine_Basic);
    http_req_t req;
    http_req_reset(&req);
    
    const char* request = "GET / HTTP/1.1\r\n";
    http_parser_tiny_error_t err = http_parse_request_line(request, strlen(request), &req);
    EXPECT_EQ(err, HTTP_PARSER_TINY_OK);
    EXPECT_TRUE(strcmp(req.method, "GET") == 0);
}

void test_http_parse_request_line_post() {
    TEST(HttpParseRequestLine_Post);
    http_req_t req;
    http_req_reset(&req);
    
    const char* request = "POST /api/test HTTP/1.1\r\n";
    http_parser_tiny_error_t err = http_parse_request_line(request, strlen(request), &req);
    EXPECT_EQ(err, HTTP_PARSER_TINY_OK);
    EXPECT_TRUE(strcmp(req.method, "POST") == 0);
}

void test_http_method_is_valid() {
    TEST(HttpMethod_IsValid);
    EXPECT_TRUE(http_method_is_valid("GET"));
    EXPECT_TRUE(http_method_is_valid("POST"));
    EXPECT_TRUE(http_method_is_valid("PUT"));
    EXPECT_TRUE(http_method_is_valid("DELETE"));
    EXPECT_FALSE(http_method_is_valid("INVALID"));
}

void test_http_version_is_valid() {
    TEST(HttpVersion_IsValid);
    EXPECT_TRUE(http_version_is_valid("HTTP/1.0"));
    EXPECT_TRUE(http_version_is_valid("HTTP/1.1"));
    EXPECT_TRUE(http_version_is_valid("HTTP/2.0"));
    EXPECT_FALSE(http_version_is_valid("HTTP/3.0"));
}

void test_http_req_reset() {
    TEST(HttpReq_Reset);
    http_req_t req;
    http_req_reset(&req);
    EXPECT_EQ(req.method[0], '\0');
    EXPECT_EQ(req.path[0], '\0');
    EXPECT_EQ(req.version[0], '\0');
    EXPECT_FALSE(req.has_error);
}

int main() {
    test_http_parse_request_line_basic();
    test_http_parse_request_line_post();
    test_http_method_is_valid();
    test_http_version_is_valid();
    test_http_req_reset();

    return 0;
}
