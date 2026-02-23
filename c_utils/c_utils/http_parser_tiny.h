#ifndef C_UTILS_HTTP_PARSER_TINY_H
#define C_UTILS_HTTP_PARSER_TINY_H

#include <stddef.h>
#include <stdbool.h>

// HTTP 解析错误码
typedef enum {
    HTTP_PARSER_TINY_OK = 0,
    HTTP_PARSER_TINY_INVALID_INPUT = -1,
    HTTP_PARSER_TINY_INVALID_METHOD = -2,
    HTTP_PARSER_TINY_INVALID_PATH = -3,
    HTTP_PARSER_TINY_INVALID_VERSION = -4,
    HTTP_PARSER_TINY_BUFFER_TOO_SMALL = -5,
    HTTP_PARSER_TINY_PARSE_ERROR = -6
} http_parser_tiny_error_t;

typedef struct {
    char method[16];
    char path[256];
    char version[16];
    bool has_error;
    http_parser_tiny_error_t error;
    char error_msg[256];
} http_req_t;

// 极简 HTTP 请求行解析
http_parser_tiny_error_t http_parse_request_line(const char *buf, size_t buf_len, http_req_t *req);

// 重置 HTTP 请求结构
void http_req_reset(http_req_t *req);

// 验证 HTTP 方法是否有效
bool http_method_is_valid(const char *method);

// 验证 HTTP 版本是否有效
bool http_version_is_valid(const char *version);

#endif // C_UTILS_HTTP_PARSER_TINY_H
