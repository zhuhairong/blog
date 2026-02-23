#ifndef C_UTILS_ADLER32_H
#define C_UTILS_ADLER32_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// Adler-32 错误码
typedef enum {
    ADLER32_OK = 0,
    ADLER32_INVALID_PARAMS = -1,
    ADLER32_MEMORY_ERROR = -2,
    ADLER32_FILE_ERROR = -3,
    ADLER32_BUFFER_TOO_SMALL = -4,
    ADLER32_OVERFLOW = -5
} adler32_error_t;

// Adler-32 配置选项
typedef struct {
    bool check_overflow;        // 是否检查溢出
    bool use_large_window;      // 是否使用大窗口
    bool verify_result;         // 是否验证结果
    size_t max_batch_size;      // 最大批量大小
    size_t max_file_size;       // 最大文件大小
} adler32_config_t;

// Adler-32 上下文
typedef struct {
    adler32_config_t config;
    adler32_error_t last_error;
    size_t compute_count;
    size_t update_count;
    size_t file_count;
} adler32_ctx_t;

// 创建 Adler-32 上下文
adler32_error_t adler32_create(adler32_ctx_t** ctx, const adler32_config_t* config);

// 销毁 Adler-32 上下文
void adler32_destroy(adler32_ctx_t* ctx);

// 计算 Adler-32 校验和
// 返回 ADLER32_OK 表示成功，其他值表示错误
adler32_error_t adler32_compute_safe(adler32_ctx_t* ctx, const void *data, size_t len, uint32_t* out);

// 增量计算 Adler-32
// 返回 ADLER32_OK 表示成功，其他值表示错误
adler32_error_t adler32_update_safe(adler32_ctx_t* ctx, uint32_t adler, const void *data, size_t len, uint32_t* out);

// 批量计算 Adler-32
// 返回 ADLER32_OK 表示成功，其他值表示错误
adler32_error_t adler32_compute_batch(adler32_ctx_t* ctx, const void** data, const size_t* lengths, size_t count, uint32_t* out);

// 从文件计算 Adler-32
// 返回 ADLER32_OK 表示成功，其他值表示错误
adler32_error_t adler32_compute_file(adler32_ctx_t* ctx, const char* filename, uint32_t* out);

// 从内存块计算 Adler-32
// 返回 ADLER32_OK 表示成功，其他值表示错误
adler32_error_t adler32_compute_block(adler32_ctx_t* ctx, const void* data, size_t len, size_t block_size, uint32_t* out);

// 重置 Adler-32 上下文
void adler32_reset(adler32_ctx_t* ctx);

// 获取 Adler-32 初始值
#define ADLER32_INIT 1

// 传统计算函数（向后兼容）
uint32_t adler32_compute(const void *data, size_t len);

// 传统增量计算函数（向后兼容）
uint32_t adler32_update(uint32_t adler, const void *data, size_t len);

// 获取最后一次错误
adler32_error_t adler32_get_last_error(adler32_ctx_t* ctx);

// 获取错误信息
const char* adler32_strerror(adler32_error_t error);

#endif // C_UTILS_ADLER32_H
