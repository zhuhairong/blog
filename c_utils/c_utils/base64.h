#ifndef C_UTILS_BASE64_H
#define C_UTILS_BASE64_H

#include <stddef.h>
#include <stdbool.h>

// Base64 编码 (RFC 4648)

// 返回编码后所需的缓冲区大小 (包括 \0)
size_t base64_encode_size(size_t input_len);

// 返回解码后所需的缓冲区大小
size_t base64_decode_size(size_t input_len);

// 编码
void base64_encode(const unsigned char *in, size_t in_len, char *out);

// 编码并分配内存 (返回的指针需要 free)
char* base64_encode_alloc(const unsigned char *in, size_t in_len);

// URL 安全编码
void base64_url_encode(const unsigned char *in, size_t in_len, char *out);

// URL 安全编码并分配内存 (返回的指针需要 free)
char* base64_url_encode_alloc(const unsigned char *in, size_t in_len);

// 解码 (返回实际解码后的字节数，失败返回 0)
size_t base64_decode(const char *in, size_t in_len, unsigned char *out);

// 解码并分配内存 (返回的指针需要 free，实际长度存入 out_len)
unsigned char* base64_decode_alloc(const char *in, size_t in_len, size_t *out_len);

// URL 安全解码
size_t base64_url_decode(const char *in, size_t in_len, unsigned char *out);

// URL 安全解码并分配内存 (返回的指针需要 free，实际长度存入 out_len)
unsigned char* base64_url_decode_alloc(const char *in, size_t in_len, size_t *out_len);

// 验证输入是否是有效的 Base64 编码
bool base64_is_valid(const char *in, size_t in_len);

// 验证输入是否是有效的 Base64 URL 编码
bool base64_url_is_valid(const char *in, size_t in_len);

#endif // C_UTILS_BASE64_H
