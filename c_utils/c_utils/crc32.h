#ifndef C_UTILS_CRC32_H
#define C_UTILS_CRC32_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// CRC32 变体
typedef enum {
    CRC32_STANDARD,  // 标准 CRC32 (IEEE 802.3)
    CRC32_C,         // CRC32C (Castagnoli)
    CRC32_K,         // CRC32K (Koopman)
    CRC32_Q          // CRC32Q
} crc32_variant_t;

// CRC32 错误码
typedef enum {
    CRC32_OK = 0,
    CRC32_ERROR_INVALID_PARAM,
    CRC32_ERROR_FILE_OPEN,
    CRC32_ERROR_FILE_READ,
    CRC32_ERROR_UNSUPPORTED_VARIANT
} crc32_error_t;

// CRC32 上下文
typedef struct {
    uint32_t crc;
    crc32_variant_t variant;
    const uint32_t *table;
} crc32_context_t;

// 初始化 CRC32 上下文
// ctx: 上下文指针
// variant: CRC32 变体
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool crc32_init(crc32_context_t *ctx, crc32_variant_t variant, crc32_error_t *error);

// 更新 CRC32 计算
// ctx: 上下文指针
// data: 数据指针
// len: 数据长度
// 返回: 成功返回 true，失败返回 false
bool crc32_update(crc32_context_t *ctx, const void *data, size_t len);

// 获取最终 CRC32 值
// ctx: 上下文指针
// 返回: CRC32 值
uint32_t crc32_final(crc32_context_t *ctx);

// 计算数据的 CRC32 值
// data: 数据指针
// len: 数据长度
// variant: CRC32 变体
// error: 错误码（输出参数，可为 NULL）
// 返回: CRC32 值，失败返回 0
uint32_t crc32_compute(const void *data, size_t len, crc32_variant_t variant, crc32_error_t *error);

// 计算文件的 CRC32 值
// filename: 文件路径
// variant: CRC32 变体
// error: 错误码（输出参数，可为 NULL）
// 返回: CRC32 值，失败返回 0
uint32_t crc32_compute_file(const char *filename, crc32_variant_t variant, crc32_error_t *error);

// 获取 CRC32 表
// variant: CRC32 变体
// 返回: CRC32 表指针，不支持的变体返回 NULL
const uint32_t* crc32_get_table(crc32_variant_t variant);

// 获取错误信息
// error: 错误码
// 返回: 错误信息字符串
const char* crc32_strerror(crc32_error_t error);

#endif // C_UTILS_CRC32_H
