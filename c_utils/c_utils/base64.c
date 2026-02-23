#include "base64.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

static const char b64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char b64_url_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

// 返回编码后所需的缓冲区大小
size_t base64_encode_size(size_t input_len) {
    return ((input_len + 2) / 3 * 4) + 1;
}

// 返回解码后所需的缓冲区大小
size_t base64_decode_size(size_t input_len) {
    return (input_len / 4 * 3);
}

// 编码
void base64_encode(const unsigned char *in, size_t in_len, char *out) {
    if (!in || !out) return;
    
    size_t i, j;
    for (i = 0, j = 0; i < in_len; i += 3, j += 4) {
        uint32_t v = in[i] << 16;
        if (i + 1 < in_len) v |= in[i + 1] << 8;
        if (i + 2 < in_len) v |= in[i + 2];

        out[j] = b64_table[(v >> 18) & 0x3f];
        out[j + 1] = b64_table[(v >> 12) & 0x3f];
        out[j + 2] = (i + 1 < in_len) ? b64_table[(v >> 6) & 0x3f] : '=';
        out[j + 3] = (i + 2 < in_len) ? b64_table[v & 0x3f] : '=';
    }
    out[j] = '\0';
}

// 编码并分配内存
char* base64_encode_alloc(const unsigned char *in, size_t in_len) {
    if (!in) return NULL;
    
    size_t out_size = base64_encode_size(in_len);
    char *out = malloc(out_size);
    if (!out) return NULL;
    
    base64_encode(in, in_len, out);
    return out;
}

// URL 安全编码
void base64_url_encode(const unsigned char *in, size_t in_len, char *out) {
    if (!in || !out) return;
    
    size_t i, j;
    for (i = 0, j = 0; i < in_len; i += 3, j += 4) {
        uint32_t v = in[i] << 16;
        if (i + 1 < in_len) v |= in[i + 1] << 8;
        if (i + 2 < in_len) v |= in[i + 2];

        out[j] = b64_url_table[(v >> 18) & 0x3f];
        out[j + 1] = b64_url_table[(v >> 12) & 0x3f];
        out[j + 2] = (i + 1 < in_len) ? b64_url_table[(v >> 6) & 0x3f] : '=';
        out[j + 3] = (i + 2 < in_len) ? b64_url_table[v & 0x3f] : '=';
    }
    out[j] = '\0';
}

// URL 安全编码并分配内存
char* base64_url_encode_alloc(const unsigned char *in, size_t in_len) {
    if (!in) return NULL;
    
    size_t out_size = base64_encode_size(in_len);
    char *out = malloc(out_size);
    if (!out) return NULL;
    
    base64_url_encode(in, in_len, out);
    return out;
}

static int b64_val(char c) {
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a' + 26;
    if (c >= '0' && c <= '9') return c - '0' + 52;
    if (c == '+') return 62;
    if (c == '/') return 63;
    if (c == '-' || c == '_') return c == '-' ? 62 : 63; // URL 安全字符
    return -1;
}

// 解码
size_t base64_decode(const char *in, size_t in_len, unsigned char *out) {
    if (!in || !out) return 0;
    if (in_len % 4 != 0) return 0;
    
    size_t out_len = 0;
    for (size_t i = 0; i < in_len; i += 4) {
        int v1 = b64_val(in[i]);
        int v2 = b64_val(in[i + 1]);
        int v3 = (in[i + 2] == '=') ? 0 : b64_val(in[i + 2]);
        int v4 = (in[i + 3] == '=') ? 0 : b64_val(in[i + 3]);

        if (v1 < 0 || v2 < 0 || v3 < 0 || v4 < 0) return 0;

        uint32_t v = (v1 << 18) | (v2 << 12) | (v3 << 6) | v4;
        out[out_len++] = (v >> 16) & 0xff;
        if (in[i + 2] != '=') out[out_len++] = (v >> 8) & 0xff;
        if (in[i + 3] != '=') out[out_len++] = v & 0xff;
    }
    return out_len;
}

// 解码并分配内存
unsigned char* base64_decode_alloc(const char *in, size_t in_len, size_t *out_len) {
    if (!in) {
        if (out_len) *out_len = 0;
        return NULL;
    }
    
    size_t max_out_len = base64_decode_size(in_len);
    unsigned char *out = malloc(max_out_len);
    if (!out) {
        if (out_len) *out_len = 0;
        return NULL;
    }
    
    size_t decoded = base64_decode(in, in_len, out);
    if (decoded == 0) {
        free(out);
        if (out_len) *out_len = 0;
        return NULL;
    }
    
    // 调整大小
    unsigned char *result = realloc(out, decoded);
    if (!result) {
        free(out);
        if (out_len) *out_len = 0;
        return NULL;
    }
    
    if (out_len) *out_len = decoded;
    return result;
}

// URL 安全解码
size_t base64_url_decode(const char *in, size_t in_len, unsigned char *out) {
    return base64_decode(in, in_len, out); // 使用相同的解码函数
}

// URL 安全解码并分配内存
unsigned char* base64_url_decode_alloc(const char *in, size_t in_len, size_t *out_len) {
    return base64_decode_alloc(in, in_len, out_len); // 使用相同的解码函数
}

// 验证输入是否是有效的 Base64 编码
bool base64_is_valid(const char *in, size_t in_len) {
    if (!in) return false;
    if (in_len % 4 != 0) return false;
    
    size_t pad_count = 0;
    for (size_t i = 0; i < in_len; i++) {
        if (in[i] == '=') {
            pad_count++;
            if (pad_count > 2) return false;
        } else {
            if (pad_count > 0) return false; // 填充字符必须在末尾
            if (b64_val(in[i]) < 0) return false;
        }
    }
    return true;
}

// 验证输入是否是有效的 Base64 URL 编码
bool base64_url_is_valid(const char *in, size_t in_len) {
    if (!in) return false;
    if (in_len % 4 != 0) return false;
    
    size_t pad_count = 0;
    for (size_t i = 0; i < in_len; i++) {
        if (in[i] == '=') {
            pad_count++;
            if (pad_count > 2) return false;
        } else {
            if (pad_count > 0) return false; // 填充字符必须在末尾
            if (b64_val(in[i]) < 0) return false;
        }
    }
    return true;
}
