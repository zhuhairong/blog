#include "base58.h"
#include <string.h>
#include <stdlib.h>

static const char b58_digits[] = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

// 返回编码后所需的缓冲区大小
size_t base58_encode_size(size_t input_len) {
    return input_len * 138 / 100 + 1; // 1.38x 是最大扩展比例
}

// 编码
bool base58_encode(const unsigned char *in, size_t in_len, char *out, size_t *out_len) {
    if (!in || !out || !out_len) return false;
    
    size_t i, j, zcount = 0;
    while (zcount < in_len && in[zcount] == 0) zcount++;
    
    size_t size = (in_len - zcount) * 138 / 100 + 1;
    unsigned char buf[size];
    memset(buf, 0, size);
    
    for (i = zcount; i < in_len; i++) {
        int carry = in[i];
        for (j = size; j--; ) {
            carry += 256 * buf[j];
            buf[j] = carry % 58;
            carry /= 58;
        }
    }
    
    i = 0;
    while (i < size && buf[i] == 0) i++;
    
    if (*out_len < zcount + size - i + 1) return false;
    
    memset(out, '1', zcount);
    for (j = zcount; i < size; i++, j++) out[j] = b58_digits[buf[i]];
    out[j] = '\0';
    *out_len = j;
    return true;
}

// 编码并分配内存
char* base58_encode_alloc(const unsigned char *in, size_t in_len, size_t *out_len) {
    if (!in) {
        if (out_len) *out_len = 0;
        return NULL;
    }
    
    size_t max_out_len = base58_encode_size(in_len);
    char *out = malloc(max_out_len);
    if (!out) {
        if (out_len) *out_len = 0;
        return NULL;
    }
    
    size_t actual_len = max_out_len;
    if (!base58_encode(in, in_len, out, &actual_len)) {
        free(out);
        if (out_len) *out_len = 0;
        return NULL;
    }
    
    if (out_len) *out_len = actual_len;
    return out;
}

// 解码
bool base58_decode(const char *in, unsigned char *out, size_t *out_len) {
    if (!in || !out || !out_len) return false;
    
    size_t i, j, zcount = 0;
    while (in[zcount] == '1') zcount++;
    
    size_t in_len = strlen(in);
    size_t size = (in_len - zcount) * 733 / 1000 + 1;
    unsigned char buf[size];
    memset(buf, 0, size);
    
    for (i = zcount; i < in_len; i++) {
        const char *p = strchr(b58_digits, in[i]);
        if (!p) return false;
        int carry = (int)(p - b58_digits);
        for (j = size; j--; ) {
            carry += 58 * buf[j];
            buf[j] = carry % 256;
            carry /= 256;
        }
    }
    
    i = 0;
    while (i < size && buf[i] == 0) i++;
    
    if (*out_len < zcount + size - i) return false;
    
    memset(out, 0, zcount);
    memcpy(out + zcount, buf + i, size - i);
    *out_len = zcount + size - i;
    return true;
}

// 解码并分配内存
unsigned char* base58_decode_alloc(const char *in, size_t *out_len) {
    if (!in) {
        if (out_len) *out_len = 0;
        return NULL;
    }
    
    size_t in_len = strlen(in);
    size_t max_out_len = in_len * 733 / 1000 + 1;
    unsigned char *out = malloc(max_out_len);
    if (!out) {
        if (out_len) *out_len = 0;
        return NULL;
    }
    
    size_t actual_len = max_out_len;
    if (!base58_decode(in, out, &actual_len)) {
        free(out);
        if (out_len) *out_len = 0;
        return NULL;
    }
    
    // 调整大小
    unsigned char *result = realloc(out, actual_len);
    if (!result) {
        free(out);
        if (out_len) *out_len = 0;
        return NULL;
    }
    
    if (out_len) *out_len = actual_len;
    return result;
}

// 验证输入是否是有效的 Base58 编码
bool base58_is_valid(const char *in) {
    if (!in) return false;
    
    for (size_t i = 0; in[i]; i++) {
        if (!strchr(b58_digits, in[i])) return false;
    }
    return true;
}
