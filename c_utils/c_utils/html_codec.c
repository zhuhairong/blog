#include "html_codec.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// 默认 HTML 编解码配置
html_codec_config_t html_codec_default_config(void) {
    html_codec_config_t config = {
        .flags = HTML_CODEC_FLAG_NONE,
        .encode_quotations = true,
        .encode_apostrophes = true,
        .encode_less_than = true,
        .encode_greater_than = true,
        .encode_ampersand = true,
        .max_entity_length = 32
    };
    return config;
}

// HTML 编码
size_t html_encode(const char *in, char *out, size_t out_size, html_codec_flags_t flags, html_codec_error_t *error) {
    (void)flags;
    if (!in) {
        if (error) *error = HTML_CODEC_ERROR_INVALID_PARAM;
        return 0;
    }
    
    size_t len = 0;
    const char *p = in;
    
    while (*p) {
        const char *entity = NULL;
        size_t entity_len = 0;
        
        switch (*p) {
            case '<':
                entity = "&lt;";
                entity_len = 4;
                break;
            case '>':
                entity = "&gt;";
                entity_len = 4;
                break;
            case '&':
                entity = "&amp;";
                entity_len = 5;
                break;
            case '"':
                entity = "&quot;";
                entity_len = 6;
                break;
            case '\'':
                entity = "&#39;";
                entity_len = 5;
                break;
            default:
                if (out && len < out_size) {
                    out[len] = *p;
                }
                len++;
                break;
        }
        
        if (entity) {
            if (out) {
                if (len + entity_len > out_size) {
                    if (error) *error = HTML_CODEC_ERROR_BUFFER_TOO_SMALL;
                    return 0;
                }
                memcpy(out + len, entity, entity_len);
            }
            len += entity_len;
        }
        
        p++;
    }
    
    if (out) {
        if (len >= out_size) {
            if (error) *error = HTML_CODEC_ERROR_BUFFER_TOO_SMALL;
            return 0;
        }
        out[len] = '\0';
    }
    
    if (error) *error = HTML_CODEC_OK;
    return len;
}

// HTML 解码
size_t html_decode(const char *in, char *out, size_t out_size, html_codec_flags_t flags, html_codec_error_t *error) {
    (void)flags;
    if (!in) {
        if (error) *error = HTML_CODEC_ERROR_INVALID_PARAM;
        return 0;
    }
    
    size_t len = 0;
    const char *p = in;
    
    while (*p) {
        if (*p == '&') {
            const char *entity_end = strchr(p, ';');
            if (entity_end) {
                size_t entity_len = entity_end - p - 1;
                char decoded = '\0';
                
                if (entity_len == 2 && strncmp(p + 1, "lt", 2) == 0) {
                    decoded = '<';
                } else if (entity_len == 2 && strncmp(p + 1, "gt", 2) == 0) {
                    decoded = '>';
                } else if (entity_len == 3 && strncmp(p + 1, "amp", 3) == 0) {
                    decoded = '&';
                } else if (entity_len == 4 && strncmp(p + 1, "quot", 4) == 0) {
                    decoded = '"';
                } else if (entity_len == 3 && strncmp(p + 1, "#39", 3) == 0) {
                    decoded = '\'';
                } else if (entity_len > 1 && p[1] == '#') {
                    // 数字实体
                    if (p[2] == 'x' || p[2] == 'X') {
                        // 十六进制
                        decoded = (char)strtol(p + 3, NULL, 16);
                    } else {
                        // 十进制
                        decoded = (char)strtol(p + 2, NULL, 10);
                    }
                }
                
                if (decoded) {
                    if (out) {
                        if (len >= out_size) {
                            if (error) *error = HTML_CODEC_ERROR_BUFFER_TOO_SMALL;
                            return 0;
                        }
                        out[len] = decoded;
                    }
                    len++;
                    p = entity_end + 1;
                    continue;
                }
            }
        }
        
        if (out) {
            if (len >= out_size) {
                if (error) *error = HTML_CODEC_ERROR_BUFFER_TOO_SMALL;
                return 0;
            }
            out[len] = *p;
        }
        len++;
        p++;
    }
    
    if (out) {
        if (len >= out_size) {
            if (error) *error = HTML_CODEC_ERROR_BUFFER_TOO_SMALL;
            return 0;
        }
        out[len] = '\0';
    }
    
    if (error) *error = HTML_CODEC_OK;
    return len;
}

// 获取错误信息
const char* html_codec_strerror(html_codec_error_t error) {
    switch (error) {
        case HTML_CODEC_OK: return "Success";
        case HTML_CODEC_ERROR_INVALID_PARAM: return "Invalid parameter";
        case HTML_CODEC_ERROR_INVALID_ENTITY: return "Invalid HTML entity";
        case HTML_CODEC_ERROR_INVALID_CHARACTER: return "Invalid character";
        case HTML_CODEC_ERROR_MEMORY_ALLOC: return "Memory allocation failed";
        case HTML_CODEC_ERROR_BUFFER_TOO_SMALL: return "Buffer too small";
        default: return "Unknown error";
    }
}
