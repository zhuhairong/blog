#ifndef C_UTILS_MD5_H
#define C_UTILS_MD5_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// MD5 错误码
typedef enum {
    MD5_OK = 0,
    MD5_INVALID_INPUT = -1,
    MD5_FILE_ERROR = -2,
    MD5_MEMORY_ERROR = -3,
    MD5_BUFFER_TOO_SMALL = -4,
    MD5_UPDATE_ERROR = -5,
    MD5_FINAL_ERROR = -6,
    MD5_INIT_ERROR = -7
} md5_error_t;

// MD5 配置
typedef struct {
    bool enable_optimization;
    bool enable_stats;
    bool use_secure_memory;
    size_t chunk_size;
} md5_config_t;

// MD5 统计信息
typedef struct {
    size_t total_bytes;
    size_t chunks_processed;
    double processing_time;
} md5_stats_t;

typedef struct {
    uint32_t state[4];
    uint32_t count[2];
    uint8_t  buffer[64];
    bool initialized;
    md5_stats_t stats;
} md5_ctx_t;

void md5_init(md5_ctx_t *ctx);
void md5_update(md5_ctx_t *ctx, const uint8_t *data, size_t len);
void md5_final(md5_ctx_t *ctx, uint8_t digest[16]);

// 带错误处理的初始化
md5_error_t md5_init_ex(md5_ctx_t *ctx, const md5_config_t *config);

// 带错误处理的更新
md5_error_t md5_update_ex(md5_ctx_t *ctx, const uint8_t *data, size_t len);

// 带错误处理的最终计算
md5_error_t md5_final_ex(md5_ctx_t *ctx, uint8_t digest[16]);

// 计算字符串的 MD5
md5_error_t md5_string(const char *str, uint8_t digest[16]);

// 计算文件的 MD5
md5_error_t md5_file(const char *filename, uint8_t digest[16], md5_error_t *error);

// 获取 MD5 十六进制字符串
md5_error_t md5_to_hex(const uint8_t digest[16], char *hex, size_t hex_size);

// 计算字符串的 MD5 并返回十六进制字符串
md5_error_t md5_string_hex(const char *str, char *hex, size_t hex_size);

// 计算文件的 MD5 并返回十六进制字符串
md5_error_t md5_file_hex(const char *filename, char *hex, size_t hex_size, md5_error_t *error);

// 获取统计信息
md5_error_t md5_get_stats(const md5_ctx_t *ctx, md5_stats_t *stats);

// 重置 MD5 上下文
md5_error_t md5_reset(md5_ctx_t *ctx);

// 获取默认配置
void md5_get_default_config(md5_config_t *config);

#endif // C_UTILS_MD5_H
