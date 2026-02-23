#include "url_codec.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char hex[] = "0123456789ABCDEF";

url_codec_error_t url_codec_create(url_codec_ctx_t** ctx, const url_codec_config_t* config) {
    if (!ctx) return URL_CODEC_INVALID_PARAMS;
    
    *ctx = malloc(sizeof(url_codec_ctx_t));
    if (!*ctx) return URL_CODEC_MEMORY_ERROR;
    
    if (config) {
        (*ctx)->config = *config;
    } else {
        (*ctx)->config.use_plus_for_space = true;
        (*ctx)->config.encode_reserved_chars = false;
        (*ctx)->config.strict_mode = false;
        (*ctx)->config.max_input_length = 0;
    }
    (*ctx)->last_error = URL_CODEC_OK;
    
    return URL_CODEC_OK;
}

void url_codec_destroy(url_codec_ctx_t* ctx) {
    if (ctx) free(ctx);
}

static int is_unreserved(char c) {
    return isalnum((unsigned char)c) || c == '-' || c == '_' || c == '.' || c == '~';
}

url_codec_error_t url_codec_encode(url_codec_ctx_t* ctx, const char *in, size_t in_len, char *out, size_t *out_size) {
    if (!in || !out_size) return URL_CODEC_INVALID_PARAMS;
    
    size_t needed = 0;
    for (size_t i = 0; i < in_len; i++) {
        if (is_unreserved(in[i])) {
            needed++;
        } else if (ctx && ctx->config.use_plus_for_space && in[i] == ' ') {
            needed++;
        } else {
            needed += 3;
        }
    }
    needed++;
    
    if (!out) {
        *out_size = needed;
        return URL_CODEC_OK;
    }
    
    if (*out_size < needed) {
        *out_size = needed;
        return URL_CODEC_BUFFER_TOO_SMALL;
    }
    
    size_t j = 0;
    for (size_t i = 0; i < in_len; i++) {
        if (is_unreserved(in[i])) {
            out[j++] = in[i];
        } else if (ctx && ctx->config.use_plus_for_space && in[i] == ' ') {
            out[j++] = '+';
        } else {
            out[j++] = '%';
            out[j++] = hex[((unsigned char)in[i]) >> 4];
            out[j++] = hex[((unsigned char)in[i]) & 15];
        }
    }
    out[j] = '\0';
    *out_size = j + 1;
    
    return URL_CODEC_OK;
}

static int hex_val(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return -1;
}

url_codec_error_t url_codec_decode(url_codec_ctx_t* ctx, const char *in, size_t in_len, char *out, size_t *out_size) {
    if (!in || !out_size) return URL_CODEC_INVALID_PARAMS;
    
    size_t needed = 0;
    for (size_t i = 0; i < in_len; ) {
        if (in[i] == '%' && i + 2 < in_len && 
            isxdigit((unsigned char)in[i+1]) && isxdigit((unsigned char)in[i+2])) {
            needed++;
            i += 3;
        } else if (in[i] == '+') {
            needed++;
            i++;
        } else {
            needed++;
            i++;
        }
    }
    needed++;
    
    if (!out) {
        *out_size = needed;
        return URL_CODEC_OK;
    }
    
    if (*out_size < needed) {
        *out_size = needed;
        return URL_CODEC_BUFFER_TOO_SMALL;
    }
    
    size_t j = 0;
    for (size_t i = 0; i < in_len; ) {
        if (in[i] == '%' && i + 2 < in_len && 
            isxdigit((unsigned char)in[i+1]) && isxdigit((unsigned char)in[i+2])) {
            int h = hex_val(in[i+1]);
            int l = hex_val(in[i+2]);
            out[j++] = (char)((h << 4) | l);
            i += 3;
        } else if (in[i] == '+') {
            out[j++] = ' ';
            i++;
        } else {
            out[j++] = in[i++];
        }
    }
    out[j] = '\0';
    *out_size = j;
    
    return URL_CODEC_OK;
}

url_codec_error_t url_codec_encode_path(url_codec_ctx_t* ctx, const char *path, char *out, size_t *out_size) {
    if (!path || !out_size) return URL_CODEC_INVALID_PARAMS;
    return url_codec_encode(ctx, path, strlen(path), out, out_size);
}

void url_encode(const char *in, char *out) {
    if (!in || !out) return;
    size_t out_size = 0;
    url_codec_encode(NULL, in, strlen(in), NULL, &out_size);
    url_codec_encode(NULL, in, strlen(in), out, &out_size);
}

void url_decode(const char *in, char *out) {
    if (!in || !out) return;
    size_t out_size = 0;
    url_codec_decode(NULL, in, strlen(in), NULL, &out_size);
    url_codec_decode(NULL, in, strlen(in), out, &out_size);
}
