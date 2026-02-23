#ifndef C_UTILS_CSV_H
#define C_UTILS_CSV_H

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

// CSV 错误码
typedef enum {
    CSV_OK = 0,
    CSV_ERROR_FILE_OPEN,
    CSV_ERROR_FILE_READ,
    CSV_ERROR_FILE_WRITE,
    CSV_ERROR_PARSE,
    CSV_ERROR_INVALID_PARAM,
    CSV_ERROR_MEMORY_ALLOC,
    CSV_ERROR_COLUMN_MISMATCH,
    CSV_ERROR_OUT_OF_BOUNDS
} csv_error_t;

// CSV 配置选项
typedef struct {
    char delimiter;
    char quote;
    char escape;
    bool trim_whitespace;
    bool ignore_empty_lines;
    bool header_row;
} csv_config_t;

// 单行解析
typedef struct {
    char **fields;
    size_t count;
    size_t capacity;
} csv_row_t;

// 完整 CSV 表格
typedef struct {
    char ***data;
    size_t rows;
    size_t cols;
    size_t capacity;
    csv_config_t config;
    bool has_error;
    csv_error_t error;
    char error_msg[256];
} csv_t;

// 默认 CSV 配置
// 返回: 默认配置
csv_config_t csv_default_config(void);

// 创建 CSV 表格
// config: CSV 配置（可为 NULL 使用默认配置）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 CSV 表格指针，失败返回 NULL
csv_t* csv_create(const csv_config_t *config, csv_error_t *error);

// 释放 CSV 表格
// csv: CSV 表格
void csv_free(csv_t *csv);

// 解析一行 CSV
// line: CSV 行字符串
// config: CSV 配置（可为 NULL 使用默认配置）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 CSV 行指针，失败返回 NULL
csv_row_t* csv_parse_line(const char *line, const csv_config_t *config, csv_error_t *error);

// 释放 CSV 行
// row: CSV 行
void csv_row_free(csv_row_t *row);

// 加载 CSV 文件
// filepath: 文件路径
// config: CSV 配置（可为 NULL 使用默认配置）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 CSV 表格指针，失败返回 NULL
csv_t* csv_load(const char *filepath, const csv_config_t *config, csv_error_t *error);

// 保存 CSV 文件
// csv: CSV 表格
// filepath: 文件路径
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool csv_save(const csv_t *csv, const char *filepath, csv_error_t *error);

// 添加行到 CSV 表格
// csv: CSV 表格
// fields: 字段数组
// count: 字段数量
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool csv_add_row(csv_t *csv, const char **fields, size_t count, csv_error_t *error);

// 获取 CSV 表格中的值
// csv: CSV 表格
// row: 行索引
// col: 列索引
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回值字符串，失败返回 NULL
const char* csv_get(const csv_t *csv, size_t row, size_t col, csv_error_t *error);

// 设置 CSV 表格中的值
// csv: CSV 表格
// row: 行索引
// col: 列索引
// value: 值字符串
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool csv_set(csv_t *csv, size_t row, size_t col, const char *value, csv_error_t *error);

// 获取 CSV 表格的行数
// csv: CSV 表格
// 返回: 行数
size_t csv_get_rows(const csv_t *csv);

// 获取 CSV 表格的列数
// csv: CSV 表格
// 返回: 列数
size_t csv_get_cols(const csv_t *csv);

// 检查 CSV 表格是否有错误
// csv: CSV 表格
// error: 错误码（输出参数，可为 NULL）
// error_msg: 错误信息（输出参数，可为 NULL）
// 返回: 有错误返回 true，无错误返回 false
bool csv_has_error(const csv_t *csv, csv_error_t *error, const char **error_msg);

// 获取错误信息
// error: 错误码
// 返回: 错误信息字符串
const char* csv_strerror(csv_error_t error);

#endif // C_UTILS_CSV_H
