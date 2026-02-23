#include "url.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// 创建 URL 上下文
url_error_t url_create(url_ctx_t** ctx, const url_config_t* config) {
    if (!ctx) return URL_INVALID_PARAMS;
    
    *ctx = malloc(sizeof(url_ctx_t));
    if (!*ctx) return URL_MEMORY_ERROR;
    
    if (config) {
        (*ctx)->config = *config;
    } else {
        (*ctx)->config.strict_mode = false;
        (*ctx)->config.max_url_length = 2048;
        (*ctx)->config.max_query_params = 100;
        (*ctx)->config.allow_fragment = true;
    }
    (*ctx)->last_error = URL_OK;
    
    return URL_OK;
}

// 销毁 URL 上下文
void url_destroy(url_ctx_t* ctx) {
    free(ctx);
}

static inline int is_rfc3986_unreserved(char c) {
    return isalnum((unsigned char)c) || c == '-' || c == '_' || c == '.' || c == '~';
}

static int hex_to_int(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return -1;
}

// URL 编码
url_error_t url_encode_ex(url_ctx_t* ctx, const char *in, char *out, size_t *out_size) {
    (void)ctx;
    if (!in || !out_size) return URL_INVALID_PARAMS;
    
    size_t len = 0;
    const char *p = in;
    
    while (*p) {
        if (is_rfc3986_unreserved(*p)) {
            if (out && len < *out_size - 1) {
                out[len] = *p;
            }
            len++;
        } else {
            if (out && len + 3 < *out_size) {
                sprintf(out + len, "%%%02X", (unsigned char)*p);
            }
            len += 3;
        }
        p++;
    }
    
    if (out) {
        if (len < *out_size) {
            out[len] = '\0';
        } else {
            return URL_BUFFER_TOO_SMALL;
        }
    }
    
    *out_size = len + 1;
    return URL_OK;
}

// URL 解码
url_error_t url_decode_ex(url_ctx_t* ctx, const char *in, char *out, size_t *out_size) {
    (void)ctx;
    if (!in || !out_size) return URL_INVALID_PARAMS;
    
    size_t len = 0;
    const char *p = in;
    
    while (*p) {
        if (*p == '%' && isxdigit((unsigned char)p[1]) && isxdigit((unsigned char)p[2])) {
            if (out && len < *out_size - 1) {
                out[len] = (char)((hex_to_int(p[1]) << 4) | hex_to_int(p[2]));
            }
            p += 3;
        } else {
            if (out && len < *out_size - 1) {
                out[len] = (*p == '+' ? ' ' : *p);
            }
            p++;
        }
        len++;
    }
    
    if (out) {
        if (len < *out_size) {
            out[len] = '\0';
        } else {
            return URL_BUFFER_TOO_SMALL;
        }
    }
    
    *out_size = len + 1;
    return URL_OK;
}

// URL 解析
url_error_t url_parse(url_ctx_t* ctx, const char *url_str, url_parse_result_t** result) {
    if (!ctx || !url_str || !result) return URL_INVALID_PARAMS;
    
    *result = calloc(1, sizeof(url_parse_result_t));
    if (!*result) return URL_MEMORY_ERROR;
    
    // 简单解析实现
    const char *p = url_str;
    
    // 解析 scheme
    const char *scheme_end = strstr(p, "://");
    if (scheme_end) {
        size_t len = scheme_end - p;
        (*result)->url.scheme = malloc(len + 1);
        if (!(*result)->url.scheme) {
            free(*result);
            return URL_MEMORY_ERROR;
        }
        strncpy((*result)->url.scheme, p, len);
        (*result)->url.scheme[len] = '\0';
        p = scheme_end + 3;
    }
    
    // 解析 host 和 port
    const char *path_start = strchr(p, '/');
    const char *query_start = strchr(p, '?');
    const char *fragment_start = strchr(p, '#');
    
    const char *host_end = path_start;
    if (!host_end || (query_start && query_start < host_end)) {
        host_end = query_start;
    }
    if (!host_end || (fragment_start && fragment_start < host_end)) {
        host_end = fragment_start;
    }
    if (!host_end) {
        host_end = p + strlen(p);
    }
    
    // 检查端口
    const char *port_start = memchr(p, ':', host_end - p);
    if (port_start) {
        size_t host_len = port_start - p;
        (*result)->url.host = malloc(host_len + 1);
        if (!(*result)->url.host) {
            url_parse_result_destroy(*result);
            return URL_MEMORY_ERROR;
        }
        strncpy((*result)->url.host, p, host_len);
        (*result)->url.host[host_len] = '\0';
        
        (*result)->url.port = atoi(port_start + 1);
    } else {
        size_t host_len = host_end - p;
        (*result)->url.host = malloc(host_len + 1);
        if (!(*result)->url.host) {
            url_parse_result_destroy(*result);
            return URL_MEMORY_ERROR;
        }
        strncpy((*result)->url.host, p, host_len);
        (*result)->url.host[host_len] = '\0';
        
        // 默认端口
        if ((*result)->url.scheme) {
            if (strcmp((*result)->url.scheme, "http") == 0) {
                (*result)->url.port = 80;
            } else if (strcmp((*result)->url.scheme, "https") == 0) {
                (*result)->url.port = 443;
            }
        }
    }
    
    p = host_end;
    
    // 解析 path
    if (*p == '/') {
        const char *path_end = query_start;
        if (!path_end || (fragment_start && fragment_start < path_end)) {
            path_end = fragment_start;
        }
        if (!path_end) {
            path_end = p + strlen(p);
        }
        
        size_t path_len = path_end - p;
        (*result)->url.path = malloc(path_len + 1);
        if (!(*result)->url.path) {
            url_parse_result_destroy(*result);
            return URL_MEMORY_ERROR;
        }
        strncpy((*result)->url.path, p, path_len);
        (*result)->url.path[path_len] = '\0';
        
        p = path_end;
    } else {
        (*result)->url.path = strdup("/");
    }
    
    // 解析 query
    if (*p == '?') {
        p++;
        const char *query_end = fragment_start;
        if (!query_end) {
            query_end = p + strlen(p);
        }
        
        size_t query_len = query_end - p;
        (*result)->url.query = malloc(query_len + 1);
        if (!(*result)->url.query) {
            url_parse_result_destroy(*result);
            return URL_MEMORY_ERROR;
        }
        strncpy((*result)->url.query, p, query_len);
        (*result)->url.query[query_len] = '\0';
        
        p = query_end;
    }
    
    // 解析 fragment
    if (*p == '#') {
        p++;
        (*result)->url.fragment = strdup(p);
    }
    
    (*result)->last_error = URL_OK;
    return URL_OK;
}

// 销毁 URL 解析结果
void url_parse_result_destroy(url_parse_result_t* result) {
    if (!result) return;
    
    free(result->url.scheme);
    free(result->url.host);
    free(result->url.path);
    free(result->url.query);
    free(result->url.fragment);
    
    for (size_t i = 0; i < result->param_count; i++) {
        free(result->params[i].key);
        free(result->params[i].value);
    }
    free(result->params);
    
    free(result);
}

// 构建 URL
url_error_t url_build(url_ctx_t* ctx, const url_t* url, char *out, size_t *out_size) {
    (void)ctx;
    if (!url || !out_size) return URL_INVALID_PARAMS;
    
    size_t len = 0;
    
    if (url->scheme) {
        len += strlen(url->scheme) + 3; // "://"
    }
    if (url->host) {
        len += strlen(url->host);
    }
    if (url->port > 0 && url->port != 80 && url->port != 443) {
        len += 6; // ":port"
    }
    if (url->path) {
        len += strlen(url->path);
    }
    if (url->query) {
        len += 1 + strlen(url->query); // "?query"
    }
    if (url->fragment) {
        len += 1 + strlen(url->fragment); // "#fragment"
    }
    
    if (!out) {
        *out_size = len + 1;
        return URL_OK;
    }
    
    if (*out_size < len + 1) {
        return URL_BUFFER_TOO_SMALL;
    }
    
    out[0] = '\0';
    
    if (url->scheme) {
        strcat(out, url->scheme);
        strcat(out, "://");
    }
    if (url->host) {
        strcat(out, url->host);
    }
    if (url->port > 0 && url->port != 80 && url->port != 443) {
        char port_str[8];
        sprintf(port_str, ":%d", url->port);
        strcat(out, port_str);
    }
    if (url->path) {
        strcat(out, url->path);
    }
    if (url->query) {
        strcat(out, "?");
        strcat(out, url->query);
    }
    if (url->fragment) {
        strcat(out, "#");
        strcat(out, url->fragment);
    }
    
    return URL_OK;
}

// 验证 URL
url_error_t url_validate(url_ctx_t* ctx, const char *url_str) {
    (void)ctx;
    if (!url_str) return URL_INVALID_PARAMS;
    
    // 简单验证
    if (strlen(url_str) == 0) {
        return URL_INVALID_URL;
    }
    
    // 检查是否有 scheme
    if (!strstr(url_str, "://")) {
        return URL_INVALID_URL;
    }
    
    return URL_OK;
}

// 获取查询参数
url_error_t url_get_query_param(const url_parse_result_t* result, const char *key, char **value) {
    if (!result || !key || !value) return URL_INVALID_PARAMS;
    
    for (size_t i = 0; i < result->param_count; i++) {
        if (strcmp(result->params[i].key, key) == 0) {
            *value = strdup(result->params[i].value);
            return URL_OK;
        }
    }
    
    return URL_PARSE_ERROR;
}

// 添加查询参数
url_error_t url_add_query_param(url_parse_result_t* result, const char *key, const char *value) {
    if (!result || !key || !value) return URL_INVALID_PARAMS;
    
    url_query_param_t *new_params = realloc(result->params, 
        sizeof(url_query_param_t) * (result->param_count + 1));
    if (!new_params) return URL_MEMORY_ERROR;
    
    result->params = new_params;
    result->params[result->param_count].key = strdup(key);
    result->params[result->param_count].value = strdup(value);
    result->param_count++;
    
    return URL_OK;
}

// 移除查询参数
url_error_t url_remove_query_param(url_parse_result_t* result, const char *key) {
    if (!result || !key) return URL_INVALID_PARAMS;
    
    for (size_t i = 0; i < result->param_count; i++) {
        if (strcmp(result->params[i].key, key) == 0) {
            free(result->params[i].key);
            free(result->params[i].value);
            
            // 移动后面的参数
            for (size_t j = i; j < result->param_count - 1; j++) {
                result->params[j] = result->params[j + 1];
            }
            result->param_count--;
            
            return URL_OK;
        }
    }
    
    return URL_PARSE_ERROR;
}

// 传统 URL 编码函数（向后兼容）
size_t url_encode_legacy(const char *in, char *out) {
    size_t len = 0;
    const char *p = in;
    
    while (*p) {
        if (is_rfc3986_unreserved(*p)) {
            if (out) out[len] = *p;
            len++;
        } else {
            if (out) sprintf(out + len, "%%%02X", (unsigned char)*p);
            len += 3;
        }
        p++;
    }
    
    if (out) out[len] = '\0';
    return len;
}

// 传统 URL 解码函数（向后兼容）
size_t url_decode_legacy(const char *in, char *out) {
    size_t len = 0;
    const char *p = in;
    
    while (*p) {
        if (*p == '%' && isxdigit((unsigned char)p[1]) && isxdigit((unsigned char)p[2])) {
            if (out) out[len] = (char)((hex_to_int(p[1]) << 4) | hex_to_int(p[2]));
            p += 3;
        } else {
            if (out) out[len] = (*p == '+' ? ' ' : *p);
            p++;
        }
        len++;
    }
    
    if (out) out[len] = '\0';
    return len;
}

// 获取最后一次错误
url_error_t url_get_last_error(url_ctx_t* ctx) {
    if (!ctx) return URL_INVALID_PARAMS;
    return ctx->last_error;
}

// 获取错误信息
const char* url_strerror(url_error_t error) {
    switch (error) {
        case URL_OK: return "Success";
        case URL_INVALID_PARAMS: return "Invalid parameters";
        case URL_MEMORY_ERROR: return "Memory allocation failed";
        case URL_INVALID_URL: return "Invalid URL";
        case URL_ENCODING_ERROR: return "Encoding error";
        case URL_DECODING_ERROR: return "Decoding error";
        case URL_PARSE_ERROR: return "Parse error";
        case URL_BUFFER_TOO_SMALL: return "Buffer too small";
        default: return "Unknown error";
    }
}
