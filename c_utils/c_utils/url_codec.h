#ifndef C_UTILS_URL_CODEC_H
#define C_UTILS_URL_CODEC_H

#include <stddef.h>
#include <stdbool.h>

// URL 编解码错误码
typedef enum {
    URL_CODEC_OK = 0,
    URL_CODEC_INVALID_PARAMS = -1,
    URL_CODEC_MEMORY_ERROR = -2,
    URL_CODEC_ENCODING_ERROR = -3,
    URL_CODEC_DECODING_ERROR = -4,
    URL_CODEC_BUFFER_TOO_SMALL = -5
} url_codec_error_t;

// URL 编解码配置选项
typedef struct {
    bool use_plus_for_space;     // 是否使用 '+' 表示空格
    bool encode_reserved_chars;   // 是否编码保留字符
    bool strict_mode;             // 是否使用严格模式
    size_t max_input_length;      // 最大输入长度
} url_codec_config_t;

// URL 编解码上下文
typedef struct {
    url_codec_config_t config;
    url_codec_error_t last_error;
} url_codec_ctx_t;

// 创建 URL 编解码上下文
url_codec_error_t url_codec_create(url_codec_ctx_t** ctx, const url_codec_config_t* config);

// 销毁 URL 编解码上下文
void url_codec_destroy(url_codec_ctx_t* ctx);

// URL 编码
// 返回 URL_CODEC_OK 表示成功，其他值表示错误
// 如果 out 为 NULL，则返回所需的缓冲区大小
url_codec_error_t url_codec_encode(url_codec_ctx_t* ctx, const char *in, size_t in_len, char *out, size_t *out_size);

// URL 解码
// 返回 URL_CODEC_OK 表示成功，其他值表示错误
// 如果 out 为 NULL，则返回所需的缓冲区大小
url_codec_error_t url_codec_decode(url_codec_ctx_t* ctx, const char *in, size_t in_len, char *out, size_t *out_size);

// URL 路径编码
// 返回 URL_CODEC_OK 表示成功，其他值表示错误
url_codec_error_t url_codec_encode_path(url_codec_ctx_t* ctx, const char *path, char *out, size_t *out_size);

// URL 查询参数编码
// 返回 URL_CODEC_OK 表示成功，其他值表示错误
url_codec_error_t url_codec_encode_query_param(url_codec_ctx_t* ctx, const char *key, const char *value, char *out, size_t *out_size);

// URL 片段编码
// 返回 URL_CODEC_OK 表示成功，其他值表示错误
url_codec_error_t url_codec_encode_fragment(url_codec_ctx_t* ctx, const char *fragment, char *out, size_t *out_size);

// 传统 URL 编码函数（向后兼容）
void url_encode(const char *in, char *out);

// 传统 URL 解码函数（向后兼容）
void url_decode(const char *in, char *out);

// 获取最后一次错误
url_codec_error_t url_codec_get_last_error(url_codec_ctx_t* ctx);

// 获取错误信息
const char* url_codec_strerror(url_codec_error_t error);

#endif // C_UTILS_URL_CODEC_H
