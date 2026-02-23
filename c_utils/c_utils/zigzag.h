#ifndef C_UTILS_ZIGZAG_H
#define C_UTILS_ZIGZAG_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// ZigZag 错误码
typedef enum {
    ZIGZAG_OK = 0,
    ZIGZAG_INVALID_PARAMS = -1,
    ZIGZAG_MEMORY_ERROR = -2,
    ZIGZAG_BUFFER_TOO_SMALL = -3,
    ZIGZAG_OVERFLOW = -4,
    ZIGZAG_UNDERFLOW = -5
} zigzag_error_t;

// ZigZag 配置选项
typedef struct {
    bool check_overflow;        // 是否检查溢出
    bool use_signed;            // 是否使用有符号整数
    size_t max_batch_size;      // 最大批量大小
} zigzag_config_t;

// ZigZag 上下文
typedef struct {
    zigzag_config_t config;
    zigzag_error_t last_error;
    size_t encode_count;
    size_t decode_count;
} zigzag_ctx_t;

// 创建 ZigZag 上下文
zigzag_error_t zigzag_create(zigzag_ctx_t** ctx, const zigzag_config_t* config);

// 销毁 ZigZag 上下文
void zigzag_destroy(zigzag_ctx_t* ctx);

// 编码 int64_t 为 zigzag
// 返回 ZIGZAG_OK 表示成功，其他值表示错误
zigzag_error_t zigzag_encode_int64(zigzag_ctx_t* ctx, int64_t n, uint64_t* out);

// 解码 zigzag 为 int64_t
// 返回 ZIGZAG_OK 表示成功，其他值表示错误
zigzag_error_t zigzag_decode_int64(zigzag_ctx_t* ctx, uint64_t n, int64_t* out);

// 编码 int32_t 为 zigzag
// 返回 ZIGZAG_OK 表示成功，其他值表示错误
zigzag_error_t zigzag_encode_int32(zigzag_ctx_t* ctx, int32_t n, uint32_t* out);

// 解码 zigzag 为 int32_t
// 返回 ZIGZAG_OK 表示成功，其他值表示错误
zigzag_error_t zigzag_decode_int32(zigzag_ctx_t* ctx, uint32_t n, int32_t* out);

// 批量编码 int64_t 数组
// 返回 ZIGZAG_OK 表示成功，其他值表示错误
zigzag_error_t zigzag_encode_batch_int64(zigzag_ctx_t* ctx, const int64_t* in, uint64_t* out, size_t count);

// 批量解码为 int64_t 数组
// 返回 ZIGZAG_OK 表示成功，其他值表示错误
zigzag_error_t zigzag_decode_batch_int64(zigzag_ctx_t* ctx, const uint64_t* in, int64_t* out, size_t count);

// 批量编码 int32_t 数组
// 返回 ZIGZAG_OK 表示成功，其他值表示错误
zigzag_error_t zigzag_encode_batch_int32(zigzag_ctx_t* ctx, const int32_t* in, uint32_t* out, size_t count);

// 批量解码为 int32_t 数组
// 返回 ZIGZAG_OK 表示成功，其他值表示错误
zigzag_error_t zigzag_decode_batch_int32(zigzag_ctx_t* ctx, const uint32_t* in, int32_t* out, size_t count);

// 传统编码函数（向后兼容）
uint64_t zigzag_encode(int64_t n);

// 传统解码函数（向后兼容）
int64_t zigzag_decode(uint64_t n);

// 获取最后一次错误
zigzag_error_t zigzag_get_last_error(zigzag_ctx_t* ctx);

// 获取错误信息
const char* zigzag_strerror(zigzag_error_t error);

#endif // C_UTILS_ZIGZAG_H
