#ifndef C_UTILS_COBS_H
#define C_UTILS_COBS_H

#include <stddef.h>
#include <stdbool.h>

// COBS 错误码
typedef enum {
    COBS_OK = 0,
    COBS_ERROR_INVALID_INPUT,
    COBS_ERROR_INVALID_ENCODED_DATA,
    COBS_ERROR_BUFFER_TOO_SMALL,
    COBS_ERROR_UNTERMINATED_PACKET
} cobs_error_t;

// COBS 编码
// in: 输入数据
// len: 输入数据长度
// out: 输出缓冲区
// out_buf_size: 输出缓冲区大小
// written: 实际写入字节数（输出参数）
// 返回: 成功返回 COBS_OK，失败返回错误码
cobs_error_t cobs_encode(const unsigned char *in, size_t len, unsigned char *out, size_t out_buf_size, size_t *written);

// COBS 解码
// in: 输入数据（COBS 编码后的数据）
// len: 输入数据长度
// out: 输出缓冲区
// out_buf_size: 输出缓冲区大小
// written: 实际写入字节数（输出参数）
// 返回: 成功返回 COBS_OK，失败返回错误码
cobs_error_t cobs_decode(const unsigned char *in, size_t len, unsigned char *out, size_t out_buf_size, size_t *written);

// 计算 COBS 编码所需的最大缓冲区大小
// len: 原始数据长度
// 返回: 最大编码长度
size_t cobs_max_encoded_size(size_t len);

// 计算 COBS 解码所需的最大缓冲区大小
// len: 编码数据长度
// 返回: 最大解码长度
size_t cobs_max_decoded_size(size_t len);

// 检查 COBS 编码数据的有效性
// in: 编码数据
// len: 编码数据长度
// 返回: 有效返回 true，无效返回 false
bool cobs_validate(const unsigned char *in, size_t len);

// 获取错误信息
// error: 错误码
// 返回: 错误信息字符串
const char* cobs_strerror(cobs_error_t error);

#endif // C_UTILS_COBS_H
