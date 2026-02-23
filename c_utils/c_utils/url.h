#ifndef C_UTILS_URL_H
#define C_UTILS_URL_H

#include <stddef.h>
#include <stdbool.h>

// URL 错误码
typedef enum {
    URL_OK = 0,
    URL_INVALID_PARAMS = -1,
    URL_MEMORY_ERROR = -2,
    URL_INVALID_URL = -3,
    URL_ENCODING_ERROR = -4,
    URL_DECODING_ERROR = -5,
    URL_PARSE_ERROR = -6,
    URL_BUFFER_TOO_SMALL = -7
} url_error_t;

// URL 配置选项
typedef struct {
    bool strict_mode;        // 是否使用严格模式
    size_t max_url_length;   // 最大 URL 长度
    size_t max_query_params; // 最大查询参数数量
    bool allow_fragment;     // 是否允许片段标识符
} url_config_t;

// URL 结构体
typedef struct {
    char *scheme;
    char *host;
    int port;
    char *path;
    char *query;
    char *fragment;
} url_t;

// URL 查询参数
typedef struct {
    char *key;
    char *value;
} url_query_param_t;

// URL 解析结果
typedef struct {
    url_t url;
    url_query_param_t *params;
    size_t param_count;
    url_error_t last_error;
} url_parse_result_t;

// URL 上下文
typedef struct {
    url_config_t config;
    url_error_t last_error;
} url_ctx_t;

// 创建 URL 上下文
url_error_t url_create(url_ctx_t** ctx, const url_config_t* config);

// 销毁 URL 上下文
void url_destroy(url_ctx_t* ctx);

// URL 编码
// 返回 URL_OK 表示成功，其他值表示错误
// 如果 out 为 NULL，则返回所需的缓冲区大小
url_error_t url_encode(url_ctx_t* ctx, const char *in, char *out, size_t *out_size);

// URL 解码
// 返回 URL_OK 表示成功，其他值表示错误
// 如果 out 为 NULL，则返回所需的缓冲区大小
url_error_t url_decode(url_ctx_t* ctx, const char *in, char *out, size_t *out_size);

// URL 解析
// 返回 URL_OK 表示成功，其他值表示错误
url_error_t url_parse(url_ctx_t* ctx, const char *url_str, url_parse_result_t** result);

// 销毁 URL 解析结果
void url_parse_result_destroy(url_parse_result_t* result);

// 构建 URL
// 返回 URL_OK 表示成功，其他值表示错误
// 如果 out 为 NULL，则返回所需的缓冲区大小
url_error_t url_build(url_ctx_t* ctx, const url_t* url, char *out, size_t *out_size);

// 验证 URL
// 返回 URL_OK 表示成功，其他值表示错误
url_error_t url_validate(url_ctx_t* ctx, const char *url_str);

// 获取查询参数
// 返回 URL_OK 表示成功，其他值表示错误
url_error_t url_get_query_param(const url_parse_result_t* result, const char *key, char **value);

// 添加查询参数
// 返回 URL_OK 表示成功，其他值表示错误
url_error_t url_add_query_param(url_parse_result_t* result, const char *key, const char *value);

// 移除查询参数
// 返回 URL_OK 表示成功，其他值表示错误
url_error_t url_remove_query_param(url_parse_result_t* result, const char *key);

// 传统 URL 编码函数（向后兼容）
// 返回编码后所需的长度 (不包括 \0)
size_t url_encode_legacy(const char *in, char *out);

// 传统 URL 解码函数（向后兼容）
// 返回解码后的长度
size_t url_decode_legacy(const char *in, char *out);

// 获取最后一次错误
url_error_t url_get_last_error(url_ctx_t* ctx);

// 获取错误信息
const char* url_strerror(url_error_t error);

#endif // C_UTILS_URL_H
