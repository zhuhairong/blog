#include "csv.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// 默认CSV配置
csv_config_t csv_default_config(void) {
    csv_config_t config = {
        .delimiter = ',',
        .quote = '"',
        .escape = '\\',
        .trim_whitespace = false,
        .ignore_empty_lines = true,
        .header_row = false
    };
    return config;
}

// 创建CSV表格
csv_t* csv_create(const csv_config_t *config, csv_error_t *error) {
    csv_t *csv = calloc(1, sizeof(csv_t));
    if (!csv) {
        if (error) *error = CSV_ERROR_MEMORY_ALLOC;
        return NULL;
    }
    
    csv->config = config ? *config : csv_default_config();
    csv->capacity = 16;
    csv->data = malloc(sizeof(char**) * csv->capacity);
    if (!csv->data) {
        free(csv);
        if (error) *error = CSV_ERROR_MEMORY_ALLOC;
        return NULL;
    }
    
    if (error) *error = CSV_OK;
    return csv;
}

// 释放CSV表格
void csv_free(csv_t *csv) {
    if (!csv) return;
    
    for (size_t i = 0; i < csv->rows; i++) {
        for (size_t j = 0; j < csv->cols; j++) {
            free(csv->data[i][j]);
        }
        free(csv->data[i]);
    }
    free(csv->data);
    free(csv);
}

// 解析一行CSV
csv_row_t* csv_parse_line(const char *line, const csv_config_t *config, csv_error_t *error) {
    if (!line) {
        if (error) *error = CSV_ERROR_INVALID_PARAM;
        return NULL;
    }
    
    csv_config_t cfg = config ? *config : csv_default_config();
    csv_row_t *row = calloc(1, sizeof(csv_row_t));
    if (!row) {
        if (error) *error = CSV_ERROR_MEMORY_ALLOC;
        return NULL;
    }
    
    const char *p = line;
    
    while (*p) {
        // 扩展字段数组
        char **new_fields = realloc(row->fields, sizeof(char*) * (row->count + 1));
        if (!new_fields) {
            csv_row_free(row);
            if (error) *error = CSV_ERROR_MEMORY_ALLOC;
            return NULL;
        }
        row->fields = new_fields;
        
        const char *start = p;
        size_t len = 0;
        
        if (*p == cfg.quote) {
            // 引号字段
            p++;
            start = p;
            while (*p && *p != cfg.quote) {
                if (*p == cfg.escape && *(p+1) == cfg.quote) {
                    p += 2;
                } else {
                    p++;
                }
            }
            len = p - start;
            
            row->fields[row->count] = malloc(len + 1);
            if (!row->fields[row->count]) {
                csv_row_free(row);
                if (error) *error = CSV_ERROR_MEMORY_ALLOC;
                return NULL;
            }
            memcpy(row->fields[row->count], start, len);
            row->fields[row->count][len] = '\0';
            row->count++;
            
            if (*p == cfg.quote) p++;
            if (*p == cfg.delimiter) p++;
        } else {
            // 普通字段
            const char *delim = strchr(p, cfg.delimiter);
            const char *newline = strchr(p, '\n');
            
            const char *end = delim;
            if (!end || (newline && newline < end)) {
                end = newline;
            }
            if (!end) {
                end = p + strlen(p);
            }
            
            len = end - p;
            
            // 去除尾部换行符
            if (len > 0 && p[len-1] == '\r') {
                len--;
            }
            
            row->fields[row->count] = malloc(len + 1);
            if (!row->fields[row->count]) {
                csv_row_free(row);
                if (error) *error = CSV_ERROR_MEMORY_ALLOC;
                return NULL;
            }
            memcpy(row->fields[row->count], p, len);
            row->fields[row->count][len] = '\0';
            row->count++;
            
            p = end;
            if (*p == cfg.delimiter) p++;
        }
    }
    
    if (error) *error = CSV_OK;
    return row;
}

// 释放CSV行
void csv_row_free(csv_row_t *row) {
    if (!row) return;
    
    for (size_t i = 0; i < row->count; i++) {
        free(row->fields[i]);
    }
    free(row->fields);
    free(row);
}

// 加载整个CSV文件
csv_t* csv_load(const char *filepath, const csv_config_t *config, csv_error_t *error) {
    if (!filepath) {
        if (error) *error = CSV_ERROR_INVALID_PARAM;
        return NULL;
    }
    
    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        if (error) *error = CSV_ERROR_FILE_OPEN;
        return NULL;
    }
    
    csv_t *csv = csv_create(config, error);
    if (!csv) {
        fclose(fp);
        return NULL;
    }
    
    char *line = NULL;
    size_t line_size = 0;
    ssize_t read;
    
    while ((read = getline(&line, &line_size, fp)) != -1) {
        // 跳过空行
        if (csv->config.ignore_empty_lines) {
            bool is_empty = true;
            for (char *p = line; *p; p++) {
                if (*p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') {
                    is_empty = false;
                    break;
                }
            }
            if (is_empty) continue;
        }
        
        csv_row_t *row = csv_parse_line(line, config, error);
        if (!row) {
            csv_free(csv);
            free(line);
            fclose(fp);
            return NULL;
        }
        
        // 扩展数据数组
        if (csv->rows >= csv->capacity) {
            csv->capacity *= 2;
            char ***new_data = realloc(csv->data, sizeof(char**) * csv->capacity);
            if (!new_data) {
                csv_row_free(row);
                csv_free(csv);
                free(line);
                fclose(fp);
                if (error) *error = CSV_ERROR_MEMORY_ALLOC;
                return NULL;
            }
            csv->data = new_data;
        }
        
        // 转换为行数组
        csv->data[csv->rows] = row->fields;
        if (csv->rows == 0) {
            csv->cols = row->count;
        }
        csv->rows++;
        
        // 释放行结构（保留字段）
        free(row);
    }
    
    free(line);
    fclose(fp);
    
    if (error) *error = CSV_OK;
    return csv;
}

// 保存CSV文件
bool csv_save(const csv_t *csv, const char *filepath, csv_error_t *error) {
    if (!csv || !filepath) {
        if (error) *error = CSV_ERROR_INVALID_PARAM;
        return false;
    }
    
    FILE *fp = fopen(filepath, "w");
    if (!fp) {
        if (error) *error = CSV_ERROR_FILE_OPEN;
        return false;
    }
    
    for (size_t i = 0; i < csv->rows; i++) {
        for (size_t j = 0; j < csv->cols; j++) {
            if (j > 0) fputc(csv->config.delimiter, fp);
            
            const char *field = csv->data[i][j];
            bool needs_quote = false;
            
            // 检查是否需要引号
            for (const char *p = field; *p; p++) {
                if (*p == csv->config.delimiter || *p == '"' || *p == '\n' || *p == '\r') {
                    needs_quote = true;
                    break;
                }
            }
            
            if (needs_quote) {
                fputc('"', fp);
                for (const char *p = field; *p; p++) {
                    if (*p == '"') {
                        fputc('"', fp);
                        fputc('"', fp);
                    } else {
                        fputc(*p, fp);
                    }
                }
                fputc('"', fp);
            } else {
                fputs(field, fp);
            }
        }
        fputc('\n', fp);
    }
    
    fclose(fp);
    if (error) *error = CSV_OK;
    return true;
}

// 获取CSV行数
size_t csv_row_count(const csv_t *csv) {
    return csv ? csv->rows : 0;
}

// 获取CSV列数
size_t csv_col_count(const csv_t *csv) {
    return csv ? csv->cols : 0;
}

// 获取单元格
const char* csv_get_cell(const csv_t *csv, size_t row, size_t col, csv_error_t *error) {
    if (!csv) {
        if (error) *error = CSV_ERROR_INVALID_PARAM;
        return NULL;
    }
    
    if (row >= csv->rows || col >= csv->cols) {
        if (error) *error = CSV_ERROR_OUT_OF_BOUNDS;
        return NULL;
    }
    
    if (error) *error = CSV_OK;
    return csv->data[row][col];
}

// 设置单元格
bool csv_set_cell(csv_t *csv, size_t row, size_t col, const char *value, csv_error_t *error) {
    if (!csv || !value) {
        if (error) *error = CSV_ERROR_INVALID_PARAM;
        return false;
    }
    
    if (row >= csv->rows || col >= csv->cols) {
        if (error) *error = CSV_ERROR_OUT_OF_BOUNDS;
        return false;
    }
    
    char *new_value = strdup(value);
    if (!new_value) {
        if (error) *error = CSV_ERROR_MEMORY_ALLOC;
        return false;
    }
    
    free(csv->data[row][col]);
    csv->data[row][col] = new_value;
    
    if (error) *error = CSV_OK;
    return true;
}

// 添加行
bool csv_add_row(csv_t *csv, const char **fields, size_t count, csv_error_t *error) {
    if (!csv || !fields) {
        if (error) *error = CSV_ERROR_INVALID_PARAM;
        return false;
    }
    
    // 扩展数据数组
    if (csv->rows >= csv->capacity) {
        csv->capacity *= 2;
        char ***new_data = realloc(csv->data, sizeof(char**) * csv->capacity);
        if (!new_data) {
            if (error) *error = CSV_ERROR_MEMORY_ALLOC;
            return false;
        }
        csv->data = new_data;
    }
    
    // 分配新行
    csv->data[csv->rows] = malloc(sizeof(char*) * count);
    if (!csv->data[csv->rows]) {
        if (error) *error = CSV_ERROR_MEMORY_ALLOC;
        return false;
    }
    
    for (size_t i = 0; i < count; i++) {
        csv->data[csv->rows][i] = strdup(fields[i]);
        if (!csv->data[csv->rows][i]) {
            // 回滚
            for (size_t j = 0; j < i; j++) {
                free(csv->data[csv->rows][j]);
            }
            free(csv->data[csv->rows]);
            if (error) *error = CSV_ERROR_MEMORY_ALLOC;
            return false;
        }
    }
    
    if (csv->rows == 0) {
        csv->cols = count;
    } else if (count != csv->cols) {
        // 列数不匹配
        for (size_t i = 0; i < count; i++) {
            free(csv->data[csv->rows][i]);
        }
        free(csv->data[csv->rows]);
        if (error) *error = CSV_ERROR_COLUMN_MISMATCH;
        return false;
    }
    
    csv->rows++;
    
    if (error) *error = CSV_OK;
    return true;
}

// 兼容函数 - 获取单元格（别名）
const char* csv_get(const csv_t *csv, size_t row, size_t col, csv_error_t *error) {
    return csv_get_cell(csv, row, col, error);
}

// 兼容函数 - 获取行数（别名）
size_t csv_get_rows(const csv_t *csv) {
    return csv_row_count(csv);
}

// 兼容函数 - 获取列数（别名）
size_t csv_get_cols(const csv_t *csv) {
    return csv_col_count(csv);
}

// 获取错误信息
const char* csv_strerror(csv_error_t error) {
    switch (error) {
        case CSV_OK: return "Success";
        case CSV_ERROR_FILE_OPEN: return "Failed to open file";
        case CSV_ERROR_FILE_READ: return "Failed to read file";
        case CSV_ERROR_FILE_WRITE: return "Failed to write file";
        case CSV_ERROR_PARSE: return "Parse error";
        case CSV_ERROR_INVALID_PARAM: return "Invalid parameter";
        case CSV_ERROR_MEMORY_ALLOC: return "Memory allocation failed";
        case CSV_ERROR_COLUMN_MISMATCH: return "Column count mismatch";
        case CSV_ERROR_OUT_OF_BOUNDS: return "Index out of bounds";
        default: return "Unknown error";
    }
}
