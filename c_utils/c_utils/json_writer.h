#ifndef C_UTILS_JSON_WRITER_H
#define C_UTILS_JSON_WRITER_H

#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

// JSON writer 错误码
typedef enum {
    JSON_WRITER_OK = 0,
    JSON_WRITER_INVALID_PARAMS = -1,
    JSON_WRITER_FILE_ERROR = -2,
    JSON_WRITER_MEMORY_ERROR = -3,
    JSON_WRITER_BUFFER_TOO_SMALL = -4,
    JSON_WRITER_DEPTH_ERROR = -5,
    JSON_WRITER_FORMAT_ERROR = -6,
    JSON_WRITER_ENCODING_ERROR = -7
} json_writer_error_t;

// JSON writer 配置选项
typedef struct {
    bool pretty_print;          // 是否美化输出
    int indent_size;            // 缩进大小
    bool escape_unicode;        // 是否转义 Unicode
    bool check_types;           // 是否检查类型
    bool use_buffering;         // 是否使用缓冲
    size_t max_depth;           // 最大嵌套深度
    size_t max_buffer_size;     // 最大缓冲区大小
} json_writer_config_t;

// JSON writer 输出类型
typedef enum {
    JSON_WRITER_OUTPUT_FILE,
    JSON_WRITER_OUTPUT_BUFFER,
    JSON_WRITER_OUTPUT_CUSTOM
} json_writer_output_type_t;

// JSON writer 自定义输出回调
typedef json_writer_error_t (*json_writer_output_callback)(void* user_data, const char* data, size_t len);

// JSON writer 结构体
typedef struct {
    json_writer_output_type_t output_type;
    union {
        FILE *file;
        struct {
            char *buffer;
            size_t size;
            size_t used;
        } buffer;
        struct {
            json_writer_output_callback callback;
            void *user_data;
        } custom;
    } output;
    bool first[32];
    int depth;
    json_writer_config_t config;
    json_writer_error_t last_error;
    size_t write_count;
    size_t object_count;
    size_t array_count;
} json_writer_t;

// 创建 JSON writer (文件输出)
// 返回 JSON_WRITER_OK 表示成功，其他值表示错误
json_writer_error_t json_writer_create_file(json_writer_t** writer, FILE *file, const json_writer_config_t* config);

// 创建 JSON writer (缓冲区输出)
// 返回 JSON_WRITER_OK 表示成功，其他值表示错误
json_writer_error_t json_writer_create_buffer(json_writer_t** writer, size_t buffer_size, const json_writer_config_t* config);

// 创建 JSON writer (自定义输出)
// 返回 JSON_WRITER_OK 表示成功，其他值表示错误
json_writer_error_t json_writer_create_custom(json_writer_t** writer, json_writer_output_callback callback, void* user_data, const json_writer_config_t* config);

// 销毁 JSON writer
void json_writer_destroy(json_writer_t* writer);

// 初始化 JSON writer (向后兼容)
void json_writer_init(json_writer_t *jw, FILE *f);

// 开始对象
// 返回 JSON_WRITER_OK 表示成功，其他值表示错误
json_writer_error_t json_writer_begin_object(json_writer_t *jw);

// 结束对象
// 返回 JSON_WRITER_OK 表示成功，其他值表示错误
json_writer_error_t json_writer_end_object(json_writer_t *jw);

// 开始数组
// 返回 JSON_WRITER_OK 表示成功，其他值表示错误
json_writer_error_t json_writer_begin_array(json_writer_t *jw);

// 结束数组
// 返回 JSON_WRITER_OK 表示成功，其他值表示错误
json_writer_error_t json_writer_end_array(json_writer_t *jw);

// 写入键
// 返回 JSON_WRITER_OK 表示成功，其他值表示错误
json_writer_error_t json_writer_key(json_writer_t *jw, const char *key);

// 写入字符串
// 返回 JSON_WRITER_OK 表示成功，其他值表示错误
json_writer_error_t json_writer_string(json_writer_t *jw, const char *val);

// 写入整数
// 返回 JSON_WRITER_OK 表示成功，其他值表示错误
json_writer_error_t json_writer_int(json_writer_t *jw, int val);

// 写入 64 位整数
// 返回 JSON_WRITER_OK 表示成功，其他值表示错误
json_writer_error_t json_writer_int64(json_writer_t *jw, int64_t val);

// 写入无符号整数
// 返回 JSON_WRITER_OK 表示成功，其他值表示错误
json_writer_error_t json_writer_uint(json_writer_t *jw, unsigned int val);

// 写入 64 位无符号整数
// 返回 JSON_WRITER_OK 表示成功，其他值表示错误
json_writer_error_t json_writer_uint64(json_writer_t *jw, uint64_t val);

// 写入浮点数
// 返回 JSON_WRITER_OK 表示成功，其他值表示错误
json_writer_error_t json_writer_double(json_writer_t *jw, double val);

// 写入布尔值
// 返回 JSON_WRITER_OK 表示成功，其他值表示错误
json_writer_error_t json_writer_bool(json_writer_t *jw, bool val);

// 写入 null
// 返回 JSON_WRITER_OK 表示成功，其他值表示错误
json_writer_error_t json_writer_null(json_writer_t *jw);

// 刷新输出
// 返回 JSON_WRITER_OK 表示成功，其他值表示错误
json_writer_error_t json_writer_flush(json_writer_t *jw);

// 获取缓冲区内容（仅适用于缓冲区输出）
// 返回 JSON_WRITER_OK 表示成功，其他值表示错误
json_writer_error_t json_writer_get_buffer_content(json_writer_t *jw, const char** content, size_t* size);

// 获取最后一次错误
json_writer_error_t json_writer_get_last_error(json_writer_t *jw);

// 获取错误信息
const char* json_writer_strerror(json_writer_error_t error);

#endif // C_UTILS_JSON_WRITER_H
