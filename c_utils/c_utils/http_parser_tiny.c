#include "http_parser_tiny.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

static const char *VALID_METHODS[] = {
    "GET", "POST", "PUT", "DELETE", "HEAD", "OPTIONS", "PATCH", "TRACE", "CONNECT", NULL
};

static const char *VALID_VERSIONS[] = {
    "HTTP/1.0", "HTTP/1.1", "HTTP/2", "HTTP/3", NULL
};

http_parser_tiny_error_t http_parse_request_line(const char *buf, size_t buf_len, http_req_t *req) {
    if (!buf || !req || buf_len == 0) {
        req->has_error = true;
        req->error = HTTP_PARSER_TINY_INVALID_INPUT;
        snprintf(req->error_msg, sizeof(req->error_msg), "Invalid input parameters");
        return HTTP_PARSER_TINY_INVALID_INPUT;
    }

    http_req_reset(req);

    int ret = sscanf(buf, "%15s %255s %15s", req->method, req->path, req->version);
    if (ret != 3) {
        req->has_error = true;
        req->error = HTTP_PARSER_TINY_PARSE_ERROR;
        snprintf(req->error_msg, sizeof(req->error_msg), "Failed to parse request line");
        return HTTP_PARSER_TINY_PARSE_ERROR;
    }

    if (!http_method_is_valid(req->method)) {
        req->has_error = true;
        req->error = HTTP_PARSER_TINY_INVALID_METHOD;
        snprintf(req->error_msg, sizeof(req->error_msg), "Invalid HTTP method: %s", req->method);
        return HTTP_PARSER_TINY_INVALID_METHOD;
    }

    if (!req->path[0] || req->path[0] != '/') {
        req->has_error = true;
        req->error = HTTP_PARSER_TINY_INVALID_PATH;
        snprintf(req->error_msg, sizeof(req->error_msg), "Invalid HTTP path: %s", req->path);
        return HTTP_PARSER_TINY_INVALID_PATH;
    }

    if (!http_version_is_valid(req->version)) {
        req->has_error = true;
        req->error = HTTP_PARSER_TINY_INVALID_VERSION;
        snprintf(req->error_msg, sizeof(req->error_msg), "Invalid HTTP version: %s", req->version);
        return HTTP_PARSER_TINY_INVALID_VERSION;
    }

    return HTTP_PARSER_TINY_OK;
}

void http_req_reset(http_req_t *req) {
    if (req) {
        memset(req->method, 0, sizeof(req->method));
        memset(req->path, 0, sizeof(req->path));
        memset(req->version, 0, sizeof(req->version));
        req->has_error = false;
        req->error = HTTP_PARSER_TINY_OK;
        memset(req->error_msg, 0, sizeof(req->error_msg));
    }
}

bool http_method_is_valid(const char *method) {
    if (!method) {
        return false;
    }

    for (int i = 0; VALID_METHODS[i]; i++) {
        if (strcmp(method, VALID_METHODS[i]) == 0) {
            return true;
        }
    }

    return false;
}

bool http_version_is_valid(const char *version) {
    if (!version) {
        return false;
    }

    for (int i = 0; VALID_VERSIONS[i]; i++) {
        if (strcmp(version, VALID_VERSIONS[i]) == 0) {
            return true;
        }
    }

    return false;
}
