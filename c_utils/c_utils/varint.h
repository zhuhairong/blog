#ifndef C_UTILS_VARINT_H
#define C_UTILS_VARINT_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// Varint 错误码
typedef enum {
    VARINT_OK = 0,
    VARINT_INVALID_PARAMS = -1,
    VARINT_BUFFER_TOO_SMALL = -2,
    VARINT_INVALID_DATA = -3,
    VARINT_OVERFLOW = -4,
    VARINT_UNDERFLOW = -5
} varint_error_t;

// Varint 配置选项
typedef struct {
    bool use_zigzag;           // 是否使用 ZigZag 编码
    bool allow_overflow;        // 是否允许溢出
    size_t max_encode_size;     // 最大编码大小
    size_t max_decode_size;     // 最大解码大小
} varint_config_t;

// Varint 上下文
typedef struct {
    varint_config_t config;
    varint_error_t last_error;
    size_t encode_count;
    size_t decode_count;
} varint_ctx_t;

// 创建 Varint 上下文
varint_error_t varint_create(varint_ctx_t** ctx, const varint_config_t* config);

// 销毁 Varint 上下文
void varint_destroy(varint_ctx_t* ctx);

// 编码 uint64_t 为 varint
// 返回 VARINT_OK 表示成功，其他值表示错误
// 如果 buf 为 NULL，则返回所需的缓冲区大小
varint_error_t varint_encode_uint64(varint_ctx_t* ctx, uint64_t val, uint8_t *buf, size_t *out_size);

// 解码 varint 为 uint64_t
// 返回 VARINT_OK 表示成功，其他值表示错误
varint_error_t varint_decode_uint64(varint_ctx_t* ctx, const uint8_t *buf, size_t buf_size, uint64_t *val, size_t *out_size);

// 编码 int64_t 为 varint (使用 ZigZag 编码)
// 返回 VARINT_OK 表示成功，其他值表示错误
varint_error_t varint_encode_int64(varint_ctx_t* ctx, int64_t val, uint8_t *buf, size_t *out_size);

// 解码 varint 为 int64_t (使用 ZigZag 编码)
// 返回 VARINT_OK 表示成功，其他值表示错误
varint_error_t varint_decode_int64(varint_ctx_t* ctx, const uint8_t *buf, size_t buf_size, int64_t *val, size_t *out_size);

// 批量编码 uint64_t 数组
// 返回 VARINT_OK 表示成功，其他值表示错误
varint_error_t varint_encode_batch_uint64(varint_ctx_t* ctx, const uint64_t* vals, size_t count, uint8_t *buf, size_t *out_size);

// 批量解码为 uint64_t 数组
// 返回 VARINT_OK 表示成功，其他值表示错误
varint_error_t varint_decode_batch_uint64(varint_ctx_t* ctx, const uint8_t *buf, size_t buf_size, uint64_t* vals, size_t count, size_t *out_size);

// 计算 varint 编码大小
// 返回编码所需的字节数
size_t varint_calc_size(uint64_t val);

// 传统编码函数（向后兼容）
size_t varint_encode(uint64_t val, uint8_t *buf);

// 传统解码函数（向后兼容）
size_t varint_decode(const uint8_t *buf, uint64_t *val);

// 获取最后一次错误
varint_error_t varint_get_last_error(varint_ctx_t* ctx);

// 获取错误信息
const char* varint_strerror(varint_error_t error);

#endif // C_UTILS_VARINT_H
