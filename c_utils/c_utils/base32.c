#include "base32.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

static const char b32_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

// 返回编码后所需的缓冲区大小
size_t base32_encode_size(size_t input_len) {
    return ((input_len + 4) / 5 * 8) + 1;
}

// 编码 (RFC 4648)
void base32_encode(const unsigned char *in, size_t in_len, char *out) {
    if (!in || !out) return;
    
    uint32_t buffer = 0;
    int bits = 0;
    size_t j = 0;

    for (size_t i = 0; i < in_len; i++) {
        buffer = (buffer << 8) | in[i];
        bits += 8;
        while (bits >= 5) {
            out[j++] = b32_table[(buffer >> (bits - 5)) & 0x1F];
            bits -= 5;
        }
    }

    if (bits > 0) {
        out[j++] = b32_table[(buffer << (5 - bits)) & 0x1F];
    }

    while (j % 8 != 0) out[j++] = '=';
    out[j] = '\0';
}

// 编码并分配内存
char* base32_encode_alloc(const unsigned char *in, size_t in_len) {
    if (!in) return NULL;
    
    size_t out_size = base32_encode_size(in_len);
    char *out = malloc(out_size);
    if (!out) return NULL;
    
    base32_encode(in, in_len, out);
    return out;
}

static int b32_val(char c) {
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= '2' && c <= '7') return c - '2' + 26;
    if (c >= 'a' && c <= 'z') return c - 'a'; // 大小写不敏感
    return -1;
}

// 解码
size_t base32_decode(const char *in, size_t in_len, unsigned char *out) {
    if (!in || !out) return 0;
    
    uint32_t buffer = 0;
    int bits = 0;
    size_t j = 0;

    for (size_t i = 0; i < in_len; i++) {
        if (in[i] == '=') break;
        int v = b32_val(in[i]);
        if (v < 0) return 0;

        buffer = (buffer << 5) | v;
        bits += 5;
        if (bits >= 8) {
            out[j++] = (buffer >> (bits - 8)) & 0xFF;
            bits -= 8;
        }
    }
    return j;
}

// 解码并分配内存
unsigned char* base32_decode_alloc(const char *in, size_t in_len, size_t *out_len) {
    if (!in) {
        if (out_len) *out_len = 0;
        return NULL;
    }
    
    size_t max_out_len = (in_len * 5 + 7) / 8;
    unsigned char *out = malloc(max_out_len);
    if (!out) {
        if (out_len) *out_len = 0;
        return NULL;
    }
    
    size_t decoded = base32_decode(in, in_len, out);
    if (decoded == 0) {
        free(out);
        if (out_len) *out_len = 0;
        return NULL;
    }
    
    // 可能需要调整大小
    unsigned char *result = realloc(out, decoded);
    if (!result) {
        free(out);
        if (out_len) *out_len = 0;
        return NULL;
    }
    
    if (out_len) *out_len = decoded;
    return result;
}

// 验证输入是否是有效的 Base32 编码
bool base32_is_valid(const char *in, size_t in_len) {
    if (!in) return false;
    
    size_t pad_count = 0;
    for (size_t i = 0; i < in_len; i++) {
        if (in[i] == '=') {
            pad_count++;
            if (pad_count > 6) return false;
        } else {
            if (pad_count > 0) return false; // 填充字符必须在末尾
            if (b32_val(in[i]) < 0) return false;
        }
    }
    
    if (in_len % 8 != 0 && pad_count == 0) return false;
    return true;
}
