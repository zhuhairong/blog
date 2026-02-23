#ifndef C_UTILS_HTTP_H
#define C_UTILS_HTTP_H

#include <stddef.h>
#include <stdbool.h>

// HTTP 错误码
typedef enum {
    HTTP_OK = 0,
    HTTP_ERROR_INVALID_PARAM,
    HTTP_ERROR_INVALID_REQUEST,
    HTTP_ERROR_INVALID_RESPONSE,
    HTTP_ERROR_MEMORY_ALLOC,
    HTTP_ERROR_BUFFER_TOO_SMALL,
    HTTP_ERROR_PARSE_FAILED,
    HTTP_ERROR_UNSUPPORTED_METHOD,
    HTTP_ERROR_UNSUPPORTED_VERSION
} http_error_t;

// HTTP 方法
typedef enum {
    HTTP_METHOD_GET,
    HTTP_METHOD_POST,
    HTTP_METHOD_PUT,
    HTTP_METHOD_DELETE,
    HTTP_METHOD_HEAD,
    HTTP_METHOD_OPTIONS,
    HTTP_METHOD_PATCH,
    HTTP_METHOD_TRACE,
    HTTP_METHOD_CONNECT,
    HTTP_METHOD_CUSTOM
} http_method_t;

// HTTP 请求
typedef struct {
    char *method;
    char *path;
    char *query;
    char *fragment;
    char *version;
    char **headers;
    char **values;
    size_t header_count;
    char *body;
    size_t body_len;
    bool has_error;
    http_error_t error;
    char error_msg[256];
} http_req_t;

// HTTP 响应
typedef struct {
    char *version;
    int status_code;
    char *status_message;
    char **headers;
    char **values;
    size_t header_count;
    char *body;
    size_t body_len;
    bool has_error;
    http_error_t error;
    char error_msg[256];
} http_res_t;

// HTTP 配置
typedef struct {
    bool parse_query;
    bool parse_fragment;
    bool preserve_case;
    size_t max_headers;
    size_t max_header_size;
    size_t max_body_size;
} http_config_t;

// 默认 HTTP 配置
// 返回: 默认配置
http_config_t http_default_config(void);

// 解析 HTTP 请求
// raw: 原始 HTTP 请求数据
// config: 配置（可为 NULL 使用默认配置）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回请求对象，失败返回 NULL
http_req_t* http_parse_request(const char *raw, const http_config_t *config, http_error_t *error);

// 解析 HTTP 响应
// raw: 原始 HTTP 响应数据
// config: 配置（可为 NULL 使用默认配置）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回响应对象，失败返回 NULL
http_res_t* http_parse_response(const char *raw, const http_config_t *config, http_error_t *error);

// 释放 HTTP 请求
// req: HTTP 请求
void http_req_free(http_req_t *req);

// 释放 HTTP 响应
// res: HTTP 响应
void http_res_free(http_res_t *res);

// 获取 HTTP 请求头部
// req: HTTP 请求
// name: 头部名称
// error: 错误码（输出参数，可为 NULL）
// 返回: 头部值，失败返回 NULL
const char* http_req_get_header(const http_req_t *req, const char *name, http_error_t *error);

// 获取 HTTP 响应头部
// res: HTTP 响应
// name: 头部名称
// error: 错误码（输出参数，可为 NULL）
// 返回: 头部值，失败返回 NULL
const char* http_res_get_header(const http_res_t *res, const char *name, http_error_t *error);

// 添加 HTTP 请求头部
// req: HTTP 请求
// name: 头部名称
// value: 头部值
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool http_req_add_header(http_req_t *req, const char *name, const char *value, http_error_t *error);

// 添加 HTTP 响应头部
// res: HTTP 响应
// name: 头部名称
// value: 头部值
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool http_res_add_header(http_res_t *res, const char *name, const char *value, http_error_t *error);

// 删除 HTTP 请求头部
// req: HTTP 请求
// name: 头部名称
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool http_req_delete_header(http_req_t *req, const char *name, http_error_t *error);

// 删除 HTTP 响应头部
// res: HTTP 响应
// name: 头部名称
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool http_res_delete_header(http_res_t *res, const char *name, http_error_t *error);

// 构建 HTTP 请求
// method: HTTP 方法
// path: 请求路径
// version: HTTP 版本
// headers: 头部数组（格式为 "Name: Value"）
// header_count: 头部数量
// body: 请求体
// body_len: 请求体长度
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 HTTP 请求字符串，失败返回 NULL
char* http_build_request(const char *method, const char *path, const char *version, const char **headers, size_t header_count, const char *body, size_t body_len, http_error_t *error);

// 构建 HTTP 响应
// version: HTTP 版本
// status_code: 状态码
// status_message: 状态消息
// headers: 头部数组（格式为 "Name: Value"）
// header_count: 头部数量
// body: 响应体
// body_len: 响应体长度
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 HTTP 响应字符串，失败返回 NULL
char* http_build_response(const char *version, int status_code, const char *status_message, const char **headers, size_t header_count, const char *body, size_t body_len, http_error_t *error);

// 检查 HTTP 请求是否有错误
// req: HTTP 请求
// error: 错误码（输出参数，可为 NULL）
// error_msg: 错误信息（输出参数，可为 NULL）
// 返回: 有错误返回 true，无错误返回 false
bool http_req_has_error(const http_req_t *req, http_error_t *error, const char **error_msg);

// 检查 HTTP 响应是否有错误
// res: HTTP 响应
// error: 错误码（输出参数，可为 NULL）
// error_msg: 错误信息（输出参数，可为 NULL）
// 返回: 有错误返回 true，无错误返回 false
bool http_res_has_error(const http_res_t *res, http_error_t *error, const char **error_msg);

// 获取错误信息
// error: 错误码
// 返回: 错误信息字符串
const char* http_strerror(http_error_t error);

#endif // C_UTILS_HTTP_H
