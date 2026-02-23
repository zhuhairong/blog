#ifndef C_UTILS_BASE16_H
#define C_UTILS_BASE16_H

#include <stddef.h>
#include <stdbool.h>

// 返回编码后所需的缓冲区大小 (包括 \0)
size_t base16_encode_size(size_t input_len);

// 编码
void base16_encode(const unsigned char *in, size_t in_len, char *out, bool uppercase);

// 编码并分配内存 (返回的指针需要 free)
char* base16_encode_alloc(const unsigned char *in, size_t in_len, bool uppercase);

// 解码 (返回实际解码后的字节数)
size_t base16_decode(const char *in, size_t in_len, unsigned char *out);

// 解码并分配内存 (返回的指针需要 free，实际长度存入 out_len)
unsigned char* base16_decode_alloc(const char *in, size_t in_len, size_t *out_len);

// 验证输入是否是有效的 Base16 编码
bool base16_is_valid(const char *in, size_t in_len);

#endif // C_UTILS_BASE16_H
