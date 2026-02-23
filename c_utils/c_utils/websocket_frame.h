#ifndef C_UTILS_WEBSOCKET_FRAME_H
#define C_UTILS_WEBSOCKET_FRAME_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// WebSocket 帧错误码
typedef enum {
    WS_FRAME_OK = 0,
    WS_FRAME_INVALID_PARAMS = -1,
    WS_FRAME_INVALID_FORMAT = -2,
    WS_FRAME_MEMORY_ERROR = -3,
    WS_FRAME_BUFFER_TOO_SMALL = -4,
    WS_FRAME_UNSUPPORTED_OPCODE = -5,
    WS_FRAME_PAYLOAD_TOO_LARGE = -6
} ws_frame_error_t;

// WebSocket 帧配置选项
typedef struct {
    bool strict_validation;     // 是否使用严格验证
    bool allow_fragmented;      // 是否允许分片帧
    bool check_payload_size;    // 是否检查 payload 大小
    size_t max_payload_size;    // 最大 payload 大小
    size_t max_frame_size;      // 最大帧大小
} ws_frame_config_t;

// WebSocket 帧头结构体
typedef struct {
    uint8_t fin;
    uint8_t opcode;
    uint8_t masked;
    uint64_t payload_len;
    uint8_t mask[4];
} ws_frame_hdr_t;

// WebSocket 帧结构体
typedef struct {
    ws_frame_hdr_t hdr;
    uint8_t *payload;
    size_t payload_size;
} ws_frame_t;

// WebSocket 帧上下文
typedef struct {
    ws_frame_config_t config;
    ws_frame_error_t last_error;
    size_t encode_count;
    size_t decode_count;
    size_t validate_count;
} ws_frame_ctx_t;

// 创建 WebSocket 帧上下文
ws_frame_error_t ws_frame_create(ws_frame_ctx_t** ctx, const ws_frame_config_t* config);

// 销毁 WebSocket 帧上下文
void ws_frame_destroy(ws_frame_ctx_t* ctx);

// 编码 WebSocket 帧头
// 返回 WS_FRAME_OK 表示成功，其他值表示错误
// 如果 buf 为 NULL，则返回所需的缓冲区大小
ws_frame_error_t ws_frame_encode_hdr_safe(ws_frame_ctx_t* ctx, ws_frame_hdr_t *hdr, uint8_t *buf, size_t *out_size);

// 解码 WebSocket 帧头
// 返回 WS_FRAME_OK 表示成功，其他值表示错误
ws_frame_error_t ws_frame_decode_hdr(ws_frame_ctx_t* ctx, const uint8_t *buf, size_t buf_size, ws_frame_hdr_t *hdr, size_t *out_size);

// 编码完整 WebSocket 帧
// 返回 WS_FRAME_OK 表示成功，其他值表示错误
ws_frame_error_t ws_frame_encode(ws_frame_ctx_t* ctx, const ws_frame_t* frame, uint8_t *buf, size_t *out_size);

// 解码完整 WebSocket 帧
// 返回 WS_FRAME_OK 表示成功，其他值表示错误
ws_frame_error_t ws_frame_decode(ws_frame_ctx_t* ctx, const uint8_t *buf, size_t buf_size, ws_frame_t* frame, size_t *out_size);

// 验证 WebSocket 帧头
// 返回 WS_FRAME_OK 表示成功，其他值表示错误
ws_frame_error_t ws_frame_validate_hdr(ws_frame_ctx_t* ctx, const ws_frame_hdr_t* hdr);

// 应用掩码
void ws_frame_apply_mask(uint8_t *data, size_t len, const uint8_t mask[4]);

// 生成随机掩码
void ws_frame_generate_mask(uint8_t mask[4]);

// 计算帧大小
// 返回完整帧的大小（包括头和 payload）
size_t ws_frame_calc_size(const ws_frame_hdr_t* hdr);

// 销毁 WebSocket 帧
void ws_frame_free(ws_frame_t* frame);

// 传统编码函数（向后兼容）
size_t ws_frame_encode_hdr(ws_frame_hdr_t *hdr, uint8_t *buf);

// 获取最后一次错误
ws_frame_error_t ws_frame_get_last_error(ws_frame_ctx_t* ctx);

// 获取错误信息
const char* ws_frame_strerror(ws_frame_error_t error);

#endif // C_UTILS_WEBSOCKET_FRAME_H
