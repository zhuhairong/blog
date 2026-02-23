#include "base32_hex.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

static const char b32hex_table[] = "0123456789ABCDEFGHIJKLMNOPQRSTUV";

// 返回编码后所需的缓冲区大小
size_t base32_hex_encode_size(size_t input_len) {
    return ((input_len + 4) / 5 * 8) + 1;
}

// 编码
void base32_hex_encode(const unsigned char *in, size_t len, char *out) {
    if (!in || !out) return;
    
    size_t i, j;
    for (i = 0, j = 0; i < len; i += 5) {
        unsigned int b1 = in[i], b2 = (i+1 < len) ? in[i+1] : 0, b3 = (i+2 < len) ? in[i+2] : 0;
        unsigned int b4 = (i+3 < len) ? in[i+3] : 0, b5 = (i+4 < len) ? in[i+4] : 0;
        out[j++] = b32hex_table[b1 >> 3];
        out[j++] = b32hex_table[((b1 & 0x07) << 2) | (b2 >> 6)];
        if (i+1 < len) out[j++] = b32hex_table[(b2 >> 1) & 0x1F];
        if (i+1 < len) out[j++] = b32hex_table[((b2 & 0x01) << 4) | (b3 >> 4)];
        if (i+2 < len) out[j++] = b32hex_table[((b3 & 0x0F) << 1) | (b4 >> 7)];
        if (i+3 < len) out[j++] = b32hex_table[(b4 >> 2) & 0x1F];
        if (i+3 < len) out[j++] = b32hex_table[((b4 & 0x03) << 3) | (b5 >> 5)];
        if (i+4 < len) out[j++] = b32hex_table[b5 & 0x1F];
    }
    out[j] = '\0';
}

// 编码并分配内存
char* base32_hex_encode_alloc(const unsigned char *in, size_t len) {
    if (!in) return NULL;
    
    size_t out_size = base32_hex_encode_size(len);
    char *out = malloc(out_size);
    if (!out) return NULL;
    
    base32_hex_encode(in, len, out);
    return out;
}

static int b32hex_val(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'V') return c - 'A' + 10;
    if (c >= 'a' && c <= 'v') return c - 'a' + 10; // 大小写不敏感
    return -1;
}

// 解码
size_t base32_hex_decode(const char *in, size_t len, unsigned char *out) {
    if (!in || !out) return 0;
    
    uint32_t buffer = 0;
    int bits = 0;
    size_t j = 0;

    for (size_t i = 0; i < len; i++) {
        if (in[i] == '=') break;
        int v = b32hex_val(in[i]);
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
unsigned char* base32_hex_decode_alloc(const char *in, size_t len, size_t *out_len) {
    if (!in) {
        if (out_len) *out_len = 0;
        return NULL;
    }
    
    size_t max_out_len = (len * 5 + 7) / 8;
    unsigned char *out = malloc(max_out_len);
    if (!out) {
        if (out_len) *out_len = 0;
        return NULL;
    }
    
    size_t decoded = base32_hex_decode(in, len, out);
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

// 验证输入是否是有效的 Base32 Hex 编码
bool base32_hex_is_valid(const char *in, size_t len) {
    if (!in) return false;
    
    size_t pad_count = 0;
    for (size_t i = 0; i < len; i++) {
        if (in[i] == '=') {
            pad_count++;
            if (pad_count > 6) return false;
        } else {
            if (pad_count > 0) return false; // 填充字符必须在末尾
            if (b32hex_val(in[i]) < 0) return false;
        }
    }
    
    if (len % 8 != 0 && pad_count == 0) return false;
    return true;
}
