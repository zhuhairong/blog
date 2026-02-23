#ifndef C_UTILS_ENDIAN_UTILS_H
#define C_UTILS_ENDIAN_UTILS_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// 字节序工具错误码
typedef enum {
    ENDIAN_UTILS_OK = 0,
    ENDIAN_UTILS_INVALID_PARAMS = -1,
    ENDIAN_UTILS_MEMORY_ERROR = -2,
    ENDIAN_UTILS_BUFFER_TOO_SMALL = -3,
    ENDIAN_UTILS_UNSUPPORTED_SIZE = -4
} endian_utils_error_t;

// 字节序工具配置选项
typedef struct {
    bool check_bounds;          // 是否检查边界
    bool use_optimized;         // 是否使用优化版本
    bool allow_unaligned;       // 是否允许非对齐访问
    size_t max_batch_size;      // 最大批量大小
} endian_utils_config_t;

// 字节序工具上下文
typedef struct {
    endian_utils_config_t config;
    endian_utils_error_t last_error;
    size_t swap_count;
    size_t convert_count;
    size_t batch_count;
} endian_utils_ctx_t;

// 创建字节序工具上下文
endian_utils_error_t endian_utils_create(endian_utils_ctx_t** ctx, const endian_utils_config_t* config);

// 销毁字节序工具上下文
void endian_utils_destroy(endian_utils_ctx_t* ctx);

// 检查当前系统是否为大端
bool endian_is_big(void);

// 16 位转换
uint16_t endian_swap16(uint16_t v);

// 32 位转换
uint32_t endian_swap32(uint32_t v);

// 64 位转换
uint64_t endian_swap64(uint64_t v);

// 批量转换 16 位值
// 返回 ENDIAN_UTILS_OK 表示成功，其他值表示错误
endian_utils_error_t endian_swap16_batch(endian_utils_ctx_t* ctx, const uint16_t* in, uint16_t* out, size_t count);

// 批量转换 32 位值
// 返回 ENDIAN_UTILS_OK 表示成功，其他值表示错误
endian_utils_error_t endian_swap32_batch(endian_utils_ctx_t* ctx, const uint32_t* in, uint32_t* out, size_t count);

// 批量转换 64 位值
// 返回 ENDIAN_UTILS_OK 表示成功，其他值表示错误
endian_utils_error_t endian_swap64_batch(endian_utils_ctx_t* ctx, const uint64_t* in, uint64_t* out, size_t count);

// 主机序转网络序 (大端)
uint16_t host_to_net16(uint16_t host);
uint32_t host_to_net32(uint32_t host);
uint64_t host_to_net64(uint64_t host);

// 网络序转主机序
uint16_t net_to_host16(uint16_t net);
uint32_t net_to_host32(uint32_t net);
uint64_t net_to_host64(uint64_t net);

// 从内存读取指定字节序的值
// 返回 ENDIAN_UTILS_OK 表示成功，其他值表示错误
endian_utils_error_t endian_read16(endian_utils_ctx_t* ctx, const uint8_t* buf, bool big_endian, uint16_t* out);
endian_utils_error_t endian_read32(endian_utils_ctx_t* ctx, const uint8_t* buf, bool big_endian, uint32_t* out);
endian_utils_error_t endian_read64(endian_utils_ctx_t* ctx, const uint8_t* buf, bool big_endian, uint64_t* out);

// 写入指定字节序的值到内存
// 返回 ENDIAN_UTILS_OK 表示成功，其他值表示错误
endian_utils_error_t endian_write16(endian_utils_ctx_t* ctx, uint8_t* buf, bool big_endian, uint16_t value);
endian_utils_error_t endian_write32(endian_utils_ctx_t* ctx, uint8_t* buf, bool big_endian, uint32_t value);
endian_utils_error_t endian_write64(endian_utils_ctx_t* ctx, uint8_t* buf, bool big_endian, uint64_t value);

// 获取最后一次错误
endian_utils_error_t endian_utils_get_last_error(endian_utils_ctx_t* ctx);

// 获取错误信息
const char* endian_utils_strerror(endian_utils_error_t error);

#endif // C_UTILS_ENDIAN_UTILS_H
