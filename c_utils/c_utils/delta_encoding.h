#ifndef C_UTILS_DELTA_ENCODING_H
#define C_UTILS_DELTA_ENCODING_H

#include <stddef.h>
#include <stdbool.h>

// 增量编码错误码
typedef enum {
    DELTA_ENCODING_OK = 0,
    DELTA_ENCODING_ERROR_INVALID_PARAM,
    DELTA_ENCODING_ERROR_INSUFFICIENT_DATA,
    DELTA_ENCODING_ERROR_BUFFER_TOO_SMALL,
    DELTA_ENCODING_ERROR_MEMORY_ALLOC,
    DELTA_ENCODING_ERROR_UNSUPPORTED_TYPE
} delta_encoding_error_t;

// 数据类型
typedef enum {
    DELTA_TYPE_INT8,
    DELTA_TYPE_INT16,
    DELTA_TYPE_INT32,
    DELTA_TYPE_INT64,
    DELTA_TYPE_UINT8,
    DELTA_TYPE_UINT16,
    DELTA_TYPE_UINT32,
    DELTA_TYPE_UINT64,
    DELTA_TYPE_FLOAT,
    DELTA_TYPE_DOUBLE
} delta_type_t;

// 增量编码配置
typedef struct {
    delta_type_t type;
    bool use_variable_length;
    bool allow_negative_deltas;
    size_t block_size;
} delta_encoding_config_t;

// 默认增量编码配置
// type: 数据类型
// 返回: 默认配置
delta_encoding_config_t delta_encoding_default_config(delta_type_t type);

// 增量编码（整数类型）
// in: 输入数据
// out: 输出数据
// n: 数据长度
// type: 数据类型
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool delta_encode(const void *in, void *out, size_t n, delta_type_t type, delta_encoding_error_t *error);

// 增量解码（整数类型）
// in: 输入数据
// out: 输出数据
// n: 数据长度
// type: 数据类型
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool delta_decode(const void *in, void *out, size_t n, delta_type_t type, delta_encoding_error_t *error);

// 增量编码（带配置）
// in: 输入数据
// out: 输出数据
// n: 数据长度
// config: 编码配置
// encoded_size: 编码后大小（输出参数，可为 NULL）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool delta_encode_ex(const void *in, void *out, size_t n, const delta_encoding_config_t *config, size_t *encoded_size, delta_encoding_error_t *error);

// 增量解码（带配置）
// in: 输入数据
// out: 输出数据
// n: 数据长度
// config: 解码配置
// decoded_size: 解码后大小（输出参数，可为 NULL）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool delta_decode_ex(const void *in, void *out, size_t n, const delta_encoding_config_t *config, size_t *decoded_size, delta_encoding_error_t *error);

// 计算编码所需的最大缓冲区大小
// n: 数据长度
// type: 数据类型
// use_variable_length: 是否使用可变长度编码
// 返回: 最大缓冲区大小
size_t delta_encoding_max_size(size_t n, delta_type_t type, bool use_variable_length);

// 获取数据类型大小
// type: 数据类型
// 返回: 数据类型大小（字节）
size_t delta_type_size(delta_type_t type);

// 获取错误信息
// error: 错误码
// 返回: 错误信息字符串
const char* delta_encoding_strerror(delta_encoding_error_t error);

#endif // C_UTILS_DELTA_ENCODING_H
