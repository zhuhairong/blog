#include "base16.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 返回编码后所需的缓冲区大小
size_t base16_encode_size(size_t input_len) {
    return input_len * 2 + 1;
}

static const char HEX_DIGITS_UPPER[] = "0123456789ABCDEF";
static const char HEX_DIGITS_LOWER[] = "0123456789abcdef";

// 优化的编码实现
void base16_encode(const unsigned char *in, size_t in_len, char *out, bool uppercase) {
    if (!in || !out) return;
    
    const char *digits = uppercase ? HEX_DIGITS_UPPER : HEX_DIGITS_LOWER;
    
    for (size_t i = 0; i < in_len; i++) {
        unsigned char c = in[i];
        out[i * 2] = digits[(c >> 4) & 0x0F];
        out[i * 2 + 1] = digits[c & 0x0F];
    }
    out[in_len * 2] = '\0';
}

// 编码并分配内存
char* base16_encode_alloc(const unsigned char *in, size_t in_len, bool uppercase) {
    if (!in) return NULL;
    
    size_t out_size = base16_encode_size(in_len);
    char *out = malloc(out_size);
    if (!out) return NULL;
    
    base16_encode(in, in_len, out, uppercase);
    return out;
}

static int hex_val(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

// 解码
size_t base16_decode(const char *in, size_t in_len, unsigned char *out) {
    if (!in || !out) return 0;
    if (in_len % 2 != 0) return 0;
    
    for (size_t i = 0; i < in_len / 2; i++) {
        int high = hex_val(in[i * 2]);
        int low = hex_val(in[i * 2 + 1]);
        if (high < 0 || low < 0) return 0;
        out[i] = (unsigned char)((high << 4) | low);
    }
    return in_len / 2;
}

// 解码并分配内存
unsigned char* base16_decode_alloc(const char *in, size_t in_len, size_t *out_len) {
    if (!in) {
        if (out_len) *out_len = 0;
        return NULL;
    }
    
    if (in_len % 2 != 0) {
        if (out_len) *out_len = 0;
        return NULL;
    }
    
    size_t len = in_len / 2;
    unsigned char *out = malloc(len);
    if (!out) {
        if (out_len) *out_len = 0;
        return NULL;
    }
    
    size_t decoded = base16_decode(in, in_len, out);
    if (decoded != len) {
        free(out);
        if (out_len) *out_len = 0;
        return NULL;
    }
    
    if (out_len) *out_len = len;
    return out;
}

// 验证输入是否是有效的 Base16 编码
bool base16_is_valid(const char *in, size_t in_len) {
    if (!in) return false;
    if (in_len % 2 != 0) return false;
    
    for (size_t i = 0; i < in_len; i++) {
        if (hex_val(in[i]) < 0) return false;
    }
    return true;
}
