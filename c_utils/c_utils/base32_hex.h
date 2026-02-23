#ifndef C_UTILS_BASE32_HEX_H
#define C_UTILS_BASE32_HEX_H

#include <stddef.h>
#include <stdbool.h>

// Base32 Hex 变体 (RFC 4648)

// 返回编码后所需的缓冲区大小 (包括 \0)
size_t base32_hex_encode_size(size_t input_len);

// 编码
void base32_hex_encode(const unsigned char *in, size_t len, char *out);

// 编码并分配内存 (返回的指针需要 free)
char* base32_hex_encode_alloc(const unsigned char *in, size_t len);

// 解码 (返回实际解码后的字节数)
size_t base32_hex_decode(const char *in, size_t len, unsigned char *out);

// 解码并分配内存 (返回的指针需要 free，实际长度存入 out_len)
unsigned char* base32_hex_decode_alloc(const char *in, size_t len, size_t *out_len);

// 验证输入是否是有效的 Base32 Hex 编码
bool base32_hex_is_valid(const char *in, size_t len);

#endif // C_UTILS_BASE32_HEX_H
