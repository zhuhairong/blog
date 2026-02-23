#ifndef C_UTILS_BASE58_H
#define C_UTILS_BASE58_H

#include <stddef.h>
#include <stdbool.h>

// Base58 编码 (Bitcoin 风格)

// 返回编码后所需的缓冲区大小 (包括 \0)
size_t base58_encode_size(size_t input_len);

// 编码
bool base58_encode(const unsigned char *in, size_t in_len, char *out, size_t *out_len);

// 编码并分配内存 (返回的指针需要 free)
char* base58_encode_alloc(const unsigned char *in, size_t in_len, size_t *out_len);

// 解码
bool base58_decode(const char *in, unsigned char *out, size_t *out_len);

// 解码并分配内存 (返回的指针需要 free，实际长度存入 out_len)
unsigned char* base58_decode_alloc(const char *in, size_t *out_len);

// 验证输入是否是有效的 Base58 编码
bool base58_is_valid(const char *in);

#endif // C_UTILS_BASE58_H
