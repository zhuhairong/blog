#include "http.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

static char* str_dup_len(const char *s, size_t len) {
    char *res = malloc(len + 1);
    if (!res) return NULL;
    memcpy(res, s, len);
    res[len] = '\0';
    return res;
}

static char* str_dup(const char *s) {
    if (!s) return NULL;
    return strdup(s);
}

static char* str_tolower(const char *s) {
    if (!s) return NULL;
    char *res = strdup(s);
    if (!res) return NULL;
    for (char *p = res; *p; p++) {
        *p = (char)tolower((unsigned char)*p);
    }
    return res;
}

// 默认 HTTP 配置
http_config_t http_default_config(void) {
    http_config_t config = {
        .parse_query = true,
        .parse_fragment = true,
        .preserve_case = false,
        .max_headers = 100,
        .max_header_size = 8192,
        .max_body_size = 1024 * 1024 * 10  // 10MB
    };
    return config;
}

// 解析 HTTP 请求
http_req_t* http_parse_request(const char *raw, const http_config_t *config, http_error_t *error) {
    if (!raw) {
        if (error) *error = HTTP_ERROR_INVALID_PARAM;
        return NULL;
    }

    http_req_t *req = calloc(1, sizeof(http_req_t));
    if (!req) {
        if (error) *error = HTTP_ERROR_MEMORY_ALLOC;
        return NULL;
    }

    const char *p = raw;
    
    // 解析请求行: METHOD PATH VERSION
    const char *space = strchr(p, ' ');
    if (!space) goto error;
    req->method = str_dup_len(p, space - p);
    if (!req->method) goto error;
    
    p = space + 1;
    space = strchr(p, ' ');
    if (!space) goto error;
    req->path = str_dup_len(p, space - p);
    if (!req->path) goto error;
    
    p = space + 1;
    const char *line_end = strstr(p, "\r\n");
    if (!line_end) goto error;
    req->version = str_dup_len(p, line_end - p);
    if (!req->version) goto error;
    
    p = line_end + 2;

    // 解析头部
    size_t max_headers = config ? config->max_headers : 100;
    while (*p && strncmp(p, "\r\n", 2) != 0) {
        if (req->header_count >= max_headers) break;
        
        const char *colon = strchr(p, ':');
        const char *next_line = strstr(p, "\r\n");
        if (!colon || !next_line || colon > next_line) break;

        // 扩展头部数组
        char **new_headers = realloc(req->headers, sizeof(char*) * (req->header_count + 1));
        char **new_values = realloc(req->values, sizeof(char*) * (req->header_count + 1));
        if (!new_headers || !new_values) {
            free(new_headers);
            free(new_values);
            goto error;
        }
        req->headers = new_headers;
        req->values = new_values;

        // 解析头部名称
        req->headers[req->header_count] = str_dup_len(p, colon - p);
        
        // 解析头部值（跳过冒号和空格）
        const char *value_start = colon + 1;
        while (value_start < next_line && (*value_start == ' ' || *value_start == '\t')) {
            value_start++;
        }
        req->values[req->header_count] = str_dup_len(value_start, next_line - value_start);
        
        if (!req->headers[req->header_count] || !req->values[req->header_count]) {
            goto error;
        }
        
        req->header_count++;
        p = next_line + 2;
    }

    // 跳过空行
    if (strncmp(p, "\r\n", 2) == 0) {
        p += 2;
    }

    // 解析请求体
    if (*p) {
        size_t body_len = strlen(p);
        size_t max_body = config ? config->max_body_size : (1024 * 1024 * 10);
        if (body_len > max_body) body_len = max_body;
        
        req->body = malloc(body_len + 1);
        if (req->body) {
            memcpy(req->body, p, body_len);
            req->body[body_len] = '\0';
            req->body_len = body_len;
        }
    }

    if (error) *error = HTTP_OK;
    return req;

error:
    http_req_free(req);
    if (error) *error = HTTP_ERROR_PARSE_FAILED;
    return NULL;
}

// 解析 HTTP 响应
http_res_t* http_parse_response(const char *raw, const http_config_t *config, http_error_t *error) {
    if (!raw) {
        if (error) *error = HTTP_ERROR_INVALID_PARAM;
        return NULL;
    }

    http_res_t *res = calloc(1, sizeof(http_res_t));
    if (!res) {
        if (error) *error = HTTP_ERROR_MEMORY_ALLOC;
        return NULL;
    }

    const char *p = raw;
    
    // 解析状态行: VERSION STATUS_CODE STATUS_MESSAGE
    const char *space = strchr(p, ' ');
    if (!space) goto error;
    res->version = str_dup_len(p, space - p);
    if (!res->version) goto error;
    
    p = space + 1;
    space = strchr(p, ' ');
    if (!space) goto error;
    char *status_str = str_dup_len(p, space - p);
    if (!status_str) goto error;
    res->status_code = atoi(status_str);
    free(status_str);
    
    p = space + 1;
    const char *line_end = strstr(p, "\r\n");
    if (!line_end) goto error;
    res->status_message = str_dup_len(p, line_end - p);
    if (!res->status_message) goto error;
    
    p = line_end + 2;

    // 解析头部
    size_t max_headers = config ? config->max_headers : 100;
    while (*p && strncmp(p, "\r\n", 2) != 0) {
        if (res->header_count >= max_headers) break;
        
        const char *colon = strchr(p, ':');
        const char *next_line = strstr(p, "\r\n");
        if (!colon || !next_line || colon > next_line) break;

        char **new_headers = realloc(res->headers, sizeof(char*) * (res->header_count + 1));
        char **new_values = realloc(res->values, sizeof(char*) * (res->header_count + 1));
        if (!new_headers || !new_values) {
            free(new_headers);
            free(new_values);
            goto error;
        }
        res->headers = new_headers;
        res->values = new_values;

        res->headers[res->header_count] = str_dup_len(p, colon - p);
        
        const char *value_start = colon + 1;
        while (value_start < next_line && (*value_start == ' ' || *value_start == '\t')) {
            value_start++;
        }
        res->values[res->header_count] = str_dup_len(value_start, next_line - value_start);
        
        if (!res->headers[res->header_count] || !res->values[res->header_count]) {
            goto error;
        }
        
        res->header_count++;
        p = next_line + 2;
    }

    // 跳过空行
    if (strncmp(p, "\r\n", 2) == 0) {
        p += 2;
    }

    // 解析响应体
    if (*p) {
        size_t body_len = strlen(p);
        size_t max_body = config ? config->max_body_size : (1024 * 1024 * 10);
        if (body_len > max_body) body_len = max_body;
        
        res->body = malloc(body_len + 1);
        if (res->body) {
            memcpy(res->body, p, body_len);
            res->body[body_len] = '\0';
            res->body_len = body_len;
        }
    }

    if (error) *error = HTTP_OK;
    return res;

error:
    http_res_free(res);
    if (error) *error = HTTP_ERROR_PARSE_FAILED;
    return NULL;
}

// 释放 HTTP 请求
void http_req_free(http_req_t *req) {
    if (!req) return;
    
    free(req->method);
    free(req->path);
    free(req->query);
    free(req->fragment);
    free(req->version);
    free(req->body);
    
    for (size_t i = 0; i < req->header_count; i++) {
        free(req->headers[i]);
        free(req->values[i]);
    }
    free(req->headers);
    free(req->values);
    
    free(req);
}

// 释放 HTTP 响应
void http_res_free(http_res_t *res) {
    if (!res) return;
    
    free(res->version);
    free(res->status_message);
    free(res->body);
    
    for (size_t i = 0; i < res->header_count; i++) {
        free(res->headers[i]);
        free(res->values[i]);
    }
    free(res->headers);
    free(res->values);
    
    free(res);
}

// 获取 HTTP 请求头部
const char* http_req_get_header(const http_req_t *req, const char *name, http_error_t *error) {
    if (!req || !name) {
        if (error) *error = HTTP_ERROR_INVALID_PARAM;
        return NULL;
    }
    
    for (size_t i = 0; i < req->header_count; i++) {
        if (strcasecmp(req->headers[i], name) == 0) {
            if (error) *error = HTTP_OK;
            return req->values[i];
        }
    }
    
    if (error) *error = HTTP_OK;
    return NULL;
}

// 获取 HTTP 响应头部
const char* http_res_get_header(const http_res_t *res, const char *name, http_error_t *error) {
    if (!res || !name) {
        if (error) *error = HTTP_ERROR_INVALID_PARAM;
        return NULL;
    }
    
    for (size_t i = 0; i < res->header_count; i++) {
        if (strcasecmp(res->headers[i], name) == 0) {
            if (error) *error = HTTP_OK;
            return res->values[i];
        }
    }
    
    if (error) *error = HTTP_OK;
    return NULL;
}

// 添加 HTTP 请求头部
bool http_req_add_header(http_req_t *req, const char *name, const char *value, http_error_t *error) {
    if (!req || !name || !value) {
        if (error) *error = HTTP_ERROR_INVALID_PARAM;
        return false;
    }
    
    char **new_headers = realloc(req->headers, sizeof(char*) * (req->header_count + 1));
    char **new_values = realloc(req->values, sizeof(char*) * (req->header_count + 1));
    if (!new_headers || !new_values) {
        free(new_headers);
        free(new_values);
        if (error) *error = HTTP_ERROR_MEMORY_ALLOC;
        return false;
    }
    
    req->headers = new_headers;
    req->values = new_values;
    
    req->headers[req->header_count] = strdup(name);
    req->values[req->header_count] = strdup(value);
    
    if (!req->headers[req->header_count] || !req->values[req->header_count]) {
        free(req->headers[req->header_count]);
        free(req->values[req->header_count]);
        if (error) *error = HTTP_ERROR_MEMORY_ALLOC;
        return false;
    }
    
    req->header_count++;
    
    if (error) *error = HTTP_OK;
    return true;
}

// 添加 HTTP 响应头部
bool http_res_add_header(http_res_t *res, const char *name, const char *value, http_error_t *error) {
    if (!res || !name || !value) {
        if (error) *error = HTTP_ERROR_INVALID_PARAM;
        return false;
    }
    
    char **new_headers = realloc(res->headers, sizeof(char*) * (res->header_count + 1));
    char **new_values = realloc(res->values, sizeof(char*) * (res->header_count + 1));
    if (!new_headers || !new_values) {
        free(new_headers);
        free(new_values);
        if (error) *error = HTTP_ERROR_MEMORY_ALLOC;
        return false;
    }
    
    res->headers = new_headers;
    res->values = new_values;
    
    res->headers[res->header_count] = strdup(name);
    res->values[res->header_count] = strdup(value);
    
    if (!res->headers[res->header_count] || !res->values[res->header_count]) {
        free(res->headers[res->header_count]);
        free(res->values[res->header_count]);
        if (error) *error = HTTP_ERROR_MEMORY_ALLOC;
        return false;
    }
    
    res->header_count++;
    
    if (error) *error = HTTP_OK;
    return true;
}

// 删除 HTTP 请求头部
bool http_req_delete_header(http_req_t *req, const char *name, http_error_t *error) {
    if (!req || !name) {
        if (error) *error = HTTP_ERROR_INVALID_PARAM;
        return false;
    }
    
    for (size_t i = 0; i < req->header_count; i++) {
        if (strcasecmp(req->headers[i], name) == 0) {
            free(req->headers[i]);
            free(req->values[i]);
            
            // 移动后面的元素
            for (size_t j = i; j < req->header_count - 1; j++) {
                req->headers[j] = req->headers[j + 1];
                req->values[j] = req->values[j + 1];
            }
            
            req->header_count--;
            if (error) *error = HTTP_OK;
            return true;
        }
    }
    
    if (error) *error = HTTP_OK;
    return false;
}

// 删除 HTTP 响应头部
bool http_res_delete_header(http_res_t *res, const char *name, http_error_t *error) {
    if (!res || !name) {
        if (error) *error = HTTP_ERROR_INVALID_PARAM;
        return false;
    }
    
    for (size_t i = 0; i < res->header_count; i++) {
        if (strcasecmp(res->headers[i], name) == 0) {
            free(res->headers[i]);
            free(res->values[i]);
            
            for (size_t j = i; j < res->header_count - 1; j++) {
                res->headers[j] = res->headers[j + 1];
                res->values[j] = res->values[j + 1];
            }
            
            res->header_count--;
            if (error) *error = HTTP_OK;
            return true;
        }
    }
    
    if (error) *error = HTTP_OK;
    return false;
}

// 构建 HTTP 请求
char* http_build_request(const char *method, const char *path, const char *version, 
                         const char **headers, size_t header_count, 
                         const char *body, size_t body_len, http_error_t *error) {
    if (!method || !path || !version) {
        if (error) *error = HTTP_ERROR_INVALID_PARAM;
        return NULL;
    }
    
    // 计算所需缓冲区大小
    size_t total_len = strlen(method) + 1 + strlen(path) + 1 + strlen(version) + 2; // 请求行
    
    for (size_t i = 0; i < header_count; i++) {
        if (headers[i]) {
            total_len += strlen(headers[i]) + 2; // 头部 + \r\n
        }
    }
    
    total_len += 2; // 空行 \r\n
    
    if (body && body_len > 0) {
        total_len += body_len;
    }
    
    total_len += 1; // 终止符
    
    char *request = malloc(total_len);
    if (!request) {
        if (error) *error = HTTP_ERROR_MEMORY_ALLOC;
        return NULL;
    }
    
    // 构建请求
    size_t pos = 0;
    pos += (size_t)snprintf(request + pos, total_len - pos, "%s %s %s\r\n", method, path, version);
    
    for (size_t i = 0; i < header_count; i++) {
        if (headers[i]) {
            pos += (size_t)snprintf(request + pos, total_len - pos, "%s\r\n", headers[i]);
        }
    }
    
    pos += (size_t)snprintf(request + pos, total_len - pos, "\r\n");
    
    if (body && body_len > 0) {
        memcpy(request + pos, body, body_len);
        pos += body_len;
    }
    
    request[pos] = '\0';
    
    if (error) *error = HTTP_OK;
    return request;
}

// 构建 HTTP 响应
char* http_build_response(const char *version, int status_code, const char *status_message,
                          const char **headers, size_t header_count,
                          const char *body, size_t body_len, http_error_t *error) {
    if (!version || !status_message) {
        if (error) *error = HTTP_ERROR_INVALID_PARAM;
        return NULL;
    }
    
    // 计算所需缓冲区大小
    size_t total_len = strlen(version) + 1 + 10 + 1 + strlen(status_message) + 2; // 状态行
    
    for (size_t i = 0; i < header_count; i++) {
        if (headers[i]) {
            total_len += strlen(headers[i]) + 2;
        }
    }
    
    total_len += 2; // 空行
    
    if (body && body_len > 0) {
        total_len += body_len;
    }
    
    total_len += 1;
    
    char *response = malloc(total_len);
    if (!response) {
        if (error) *error = HTTP_ERROR_MEMORY_ALLOC;
        return NULL;
    }
    
    // 构建响应
    size_t pos = 0;
    pos += (size_t)snprintf(response + pos, total_len - pos, "%s %d %s\r\n", version, status_code, status_message);
    
    for (size_t i = 0; i < header_count; i++) {
        if (headers[i]) {
            pos += (size_t)snprintf(response + pos, total_len - pos, "%s\r\n", headers[i]);
        }
    }
    
    pos += (size_t)snprintf(response + pos, total_len - pos, "\r\n");
    
    if (body && body_len > 0) {
        memcpy(response + pos, body, body_len);
        pos += body_len;
    }
    
    response[pos] = '\0';
    
    if (error) *error = HTTP_OK;
    return response;
}

// 获取错误信息
const char* http_strerror(http_error_t error) {
    switch (error) {
        case HTTP_OK: return "Success";
        case HTTP_ERROR_INVALID_PARAM: return "Invalid parameter";
        case HTTP_ERROR_INVALID_REQUEST: return "Invalid request";
        case HTTP_ERROR_INVALID_RESPONSE: return "Invalid response";
        case HTTP_ERROR_MEMORY_ALLOC: return "Memory allocation failed";
        case HTTP_ERROR_BUFFER_TOO_SMALL: return "Buffer too small";
        case HTTP_ERROR_PARSE_FAILED: return "Parse failed";
        case HTTP_ERROR_UNSUPPORTED_METHOD: return "Unsupported method";
        case HTTP_ERROR_UNSUPPORTED_VERSION: return "Unsupported version";
        default: return "Unknown error";
    }
}
