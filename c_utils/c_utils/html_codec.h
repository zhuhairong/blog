#ifndef C_UTILS_HTML_CODEC_H
#define C_UTILS_HTML_CODEC_H

#include <stddef.h>
#include <stdbool.h>

// HTML 编解码错误码
typedef enum {
    HTML_CODEC_OK = 0,
    HTML_CODEC_ERROR_INVALID_PARAM,
    HTML_CODEC_ERROR_INVALID_ENTITY,
    HTML_CODEC_ERROR_INVALID_CHARACTER,
    HTML_CODEC_ERROR_MEMORY_ALLOC,
    HTML_CODEC_ERROR_BUFFER_TOO_SMALL
} html_codec_error_t;

// HTML 编解码标志
typedef enum {
    HTML_CODEC_FLAG_NONE = 0,
    HTML_CODEC_FLAG_ENCODE_ALL = (1 << 0),
    HTML_CODEC_FLAG_ENCODE_NON_ASCII = (1 << 1),
    HTML_CODEC_FLAG_DECODE_NUMERIC = (1 << 2),
    HTML_CODEC_FLAG_DECODE_NAMED = (1 << 3),
    HTML_CODEC_FLAG_SKIP_INVALID = (1 << 4)
} html_codec_flags_t;

// HTML 编解码配置
typedef struct {
    html_codec_flags_t flags;
    bool encode_quotations;
    bool encode_apostrophes;
    bool encode_less_than;
    bool encode_greater_than;
    bool encode_ampersand;
    size_t max_entity_length;
} html_codec_config_t;

// 默认 HTML 编解码配置
// 返回: 默认配置
html_codec_config_t html_codec_default_config(void);

// HTML 编码
// in: 输入字符串
// out: 输出缓冲区
// out_size: 输出缓冲区大小
// flags: 标志
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回写入的字符数，失败返回 0
size_t html_encode(const char *in, char *out, size_t out_size, html_codec_flags_t flags, html_codec_error_t *error);

// HTML 解码
// in: 输入字符串
// out: 输出缓冲区
// out_size: 输出缓冲区大小
// flags: 标志
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回写入的字符数，失败返回 0
size_t html_decode(const char *in, char *out, size_t out_size, html_codec_flags_t flags, html_codec_error_t *error);

// 配置化 HTML 编码
// in: 输入字符串
// out: 输出缓冲区
// out_size: 输出缓冲区大小
// config: 配置（可为 NULL 使用默认配置）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回写入的字符数，失败返回 0
size_t html_encode_config(const char *in, char *out, size_t out_size, const html_codec_config_t *config, html_codec_error_t *error);

// 配置化 HTML 解码
// in: 输入字符串
// out: 输出缓冲区
// out_size: 输出缓冲区大小
// config: 配置（可为 NULL 使用默认配置）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回写入的字符数，失败返回 0
size_t html_decode_config(const char *in, char *out, size_t out_size, const html_codec_config_t *config, html_codec_error_t *error);

// 计算 HTML 编码所需的最大缓冲区大小
// in: 输入字符串
// flags: 标志
// 返回: 最大缓冲区大小
size_t html_encode_max_size(const char *in, html_codec_flags_t flags);

// 检查字符是否需要 HTML 编码
// c: 字符
// flags: 标志
// 返回: 需要编码返回 true，否则返回 false
bool html_needs_encoding(char c, html_codec_flags_t flags);

// 获取错误信息
// error: 错误码
// 返回: 错误信息字符串
const char* html_codec_strerror(html_codec_error_t error);

#endif // C_UTILS_HTML_CODEC_H
