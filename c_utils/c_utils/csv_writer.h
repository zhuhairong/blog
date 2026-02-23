#ifndef C_UTILS_CSV_WRITER_H
#define C_UTILS_CSV_WRITER_H

#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>

// CSV 写入器错误码
typedef enum {
    CSV_WRITER_OK = 0,
    CSV_WRITER_ERROR_FILE_OPEN,
    CSV_WRITER_ERROR_FILE_WRITE,
    CSV_WRITER_ERROR_INVALID_PARAM,
    CSV_WRITER_ERROR_MEMORY_ALLOC,
    CSV_WRITER_ERROR_BUFFER_FULL
} csv_writer_error_t;

// CSV 写入器配置
typedef struct {
    char delimiter;
    char quote;
    char escape;
    bool quote_all_fields;
    bool trim_whitespace;
    size_t buffer_size;
} csv_writer_config_t;

// CSV 写入器
typedef struct {
    FILE *file;
    csv_writer_config_t config;
    char *buffer;
    size_t buffer_pos;
    size_t buffer_size;
    bool has_error;
    csv_writer_error_t error;
    char error_msg[256];
} csv_writer_t;

// 默认 CSV 写入器配置
// 返回: 默认配置
csv_writer_config_t csv_writer_default_config(void);

// 创建 CSV 写入器（从文件路径）
// filepath: 文件路径
// mode: 文件打开模式
// config: 配置（可为 NULL 使用默认配置）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回写入器指针，失败返回 NULL
csv_writer_t* csv_writer_create(const char *filepath, const char *mode, const csv_writer_config_t *config, csv_writer_error_t *error);

// 创建 CSV 写入器（从 FILE 指针）
// file: 文件指针
// config: 配置（可为 NULL 使用默认配置）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回写入器指针，失败返回 NULL
csv_writer_t* csv_writer_create_from_file(FILE *file, const csv_writer_config_t *config, csv_writer_error_t *error);

// 释放 CSV 写入器
// writer: CSV 写入器
void csv_writer_free(csv_writer_t *writer);

// 写入字段
// writer: CSV 写入器
// field: 字段值
// is_last: 是否为行的最后一个字段
// 返回: 成功返回 true，失败返回 false
bool csv_writer_write_field(csv_writer_t *writer, const char *field, bool is_last);

// 写入一行
// writer: CSV 写入器
// fields: 字段数组
// count: 字段数量
// 返回: 成功返回 true，失败返回 false
bool csv_writer_write_row(csv_writer_t *writer, const char **fields, size_t count);

// 写入一行（可变参数）
// writer: CSV 写入器
// ...: 字段值列表，以 NULL 结束
// 返回: 成功返回 true，失败返回 false
bool csv_writer_write_row_va(csv_writer_t *writer, ...);

// 刷新缓冲区
// writer: CSV 写入器
// 返回: 成功返回 true，失败返回 false
bool csv_writer_flush(csv_writer_t *writer);

// 检查写入器是否有错误
// writer: CSV 写入器
// error: 错误码（输出参数，可为 NULL）
// error_msg: 错误信息（输出参数，可为 NULL）
// 返回: 有错误返回 true，无错误返回 false
bool csv_writer_has_error(const csv_writer_t *writer, csv_writer_error_t *error, const char **error_msg);

// 获取错误信息
// error: 错误码
// 返回: 错误信息字符串
const char* csv_writer_strerror(csv_writer_error_t error);

#endif // C_UTILS_CSV_WRITER_H
