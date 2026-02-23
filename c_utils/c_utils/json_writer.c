#include "json_writer.h"
#include <stdlib.h>
#include <string.h>

// 传统初始化函数
void json_writer_init(json_writer_t *jw, FILE *f) {
    memset(jw, 0, sizeof(json_writer_t));
    jw->output_type = JSON_WRITER_OUTPUT_FILE;
    jw->output.file = f;
    jw->depth = 0;
    jw->first[0] = true;
    jw->config.pretty_print = false;
    jw->config.indent_size = 2;
    jw->config.escape_unicode = false;
    jw->config.check_types = true;
    jw->config.use_buffering = true;
    jw->config.max_depth = 32;
    jw->config.max_buffer_size = 1024 * 1024;
}

// 创建 JSON writer (文件输出)
json_writer_error_t json_writer_create_file(json_writer_t** writer, FILE *file, const json_writer_config_t* config) {
    if (!writer || !file) {
        return JSON_WRITER_INVALID_PARAMS;
    }
    
    *writer = (json_writer_t*)malloc(sizeof(json_writer_t));
    if (!*writer) {
        return JSON_WRITER_MEMORY_ERROR;
    }
    
    memset(*writer, 0, sizeof(json_writer_t));
    
    (*writer)->output_type = JSON_WRITER_OUTPUT_FILE;
    (*writer)->output.file = file;
    (*writer)->depth = 0;
    (*writer)->first[0] = true;
    
    if (config) {
        (*writer)->config = *config;
    } else {
        (*writer)->config.pretty_print = false;
        (*writer)->config.indent_size = 2;
        (*writer)->config.escape_unicode = false;
        (*writer)->config.check_types = true;
        (*writer)->config.use_buffering = true;
        (*writer)->config.max_depth = 32;
        (*writer)->config.max_buffer_size = 1024 * 1024;
    }
    
    (*writer)->last_error = JSON_WRITER_OK;
    
    return JSON_WRITER_OK;
}

// 创建 JSON writer (缓冲区输出)
json_writer_error_t json_writer_create_buffer(json_writer_t** writer, size_t buffer_size, const json_writer_config_t* config) {
    if (!writer) {
        return JSON_WRITER_INVALID_PARAMS;
    }
    
    *writer = (json_writer_t*)malloc(sizeof(json_writer_t));
    if (!*writer) {
        return JSON_WRITER_MEMORY_ERROR;
    }
    
    memset(*writer, 0, sizeof(json_writer_t));
    
    (*writer)->output_type = JSON_WRITER_OUTPUT_BUFFER;
    (*writer)->output.buffer.buffer = (char*)malloc(buffer_size);
    if (!(*writer)->output.buffer.buffer) {
        free(*writer);
        return JSON_WRITER_MEMORY_ERROR;
    }
    
    (*writer)->output.buffer.size = buffer_size;
    (*writer)->output.buffer.used = 0;
    (*writer)->output.buffer.buffer[0] = '\0';
    (*writer)->depth = 0;
    (*writer)->first[0] = true;
    
    if (config) {
        (*writer)->config = *config;
    } else {
        (*writer)->config.pretty_print = false;
        (*writer)->config.indent_size = 2;
        (*writer)->config.escape_unicode = false;
        (*writer)->config.check_types = true;
        (*writer)->config.use_buffering = true;
        (*writer)->config.max_depth = 32;
        (*writer)->config.max_buffer_size = buffer_size;
    }
    
    (*writer)->last_error = JSON_WRITER_OK;
    
    return JSON_WRITER_OK;
}

// 创建 JSON writer (自定义输出)
json_writer_error_t json_writer_create_custom(json_writer_t** writer, json_writer_output_callback callback, void* user_data, const json_writer_config_t* config) {
    if (!writer || !callback) {
        return JSON_WRITER_INVALID_PARAMS;
    }
    
    *writer = (json_writer_t*)malloc(sizeof(json_writer_t));
    if (!*writer) {
        return JSON_WRITER_MEMORY_ERROR;
    }
    
    memset(*writer, 0, sizeof(json_writer_t));
    
    (*writer)->output_type = JSON_WRITER_OUTPUT_CUSTOM;
    (*writer)->output.custom.callback = callback;
    (*writer)->output.custom.user_data = user_data;
    (*writer)->depth = 0;
    (*writer)->first[0] = true;
    
    if (config) {
        (*writer)->config = *config;
    } else {
        (*writer)->config.pretty_print = false;
        (*writer)->config.indent_size = 2;
        (*writer)->config.escape_unicode = false;
        (*writer)->config.check_types = true;
        (*writer)->config.use_buffering = true;
        (*writer)->config.max_depth = 32;
        (*writer)->config.max_buffer_size = 1024 * 1024;
    }
    
    (*writer)->last_error = JSON_WRITER_OK;
    
    return JSON_WRITER_OK;
}

// 销毁 JSON writer
void json_writer_destroy(json_writer_t* writer) {
    if (writer) {
        if (writer->output_type == JSON_WRITER_OUTPUT_BUFFER && writer->output.buffer.buffer) {
            free(writer->output.buffer.buffer);
        }
        free(writer);
    }
}

// 内部辅助函数：写入数据
static void json_writer_write_raw(json_writer_t *jw, const char *data) {
    if (jw->output_type == JSON_WRITER_OUTPUT_FILE) {
        fputs(data, jw->output.file);
    } else if (jw->output_type == JSON_WRITER_OUTPUT_BUFFER) {
        size_t len = strlen(data);
        if (jw->output.buffer.used + len < jw->output.buffer.size) {
            strcpy(jw->output.buffer.buffer + jw->output.buffer.used, data);
            jw->output.buffer.used += len;
        }
    } else if (jw->output_type == JSON_WRITER_OUTPUT_CUSTOM) {
        jw->output.custom.callback(jw->output.custom.user_data, data, strlen(data));
    }
    jw->write_count++;
}

// 内部辅助函数：写入字符
static void json_writer_write_char(json_writer_t *jw, char c) {
    char str[2] = {c, '\0'};
    json_writer_write_raw(jw, str);
}

// 内部辅助函数：处理逗号
static void json_writer_comma(json_writer_t *jw) {
    if (!jw->first[jw->depth]) {
        json_writer_write_char(jw, ',');
    }
    jw->first[jw->depth] = false;
}

// 开始对象
json_writer_error_t json_writer_begin_object(json_writer_t *jw) {
    if (!jw) {
        return JSON_WRITER_INVALID_PARAMS;
    }
    
    if (jw->config.max_depth > 0 && jw->depth >= (int)jw->config.max_depth) {
        return JSON_WRITER_DEPTH_ERROR;
    }
    
    json_writer_comma(jw);
    json_writer_write_char(jw, '{');
    jw->depth++;
    jw->first[jw->depth] = true;
    jw->object_count++;
    
    return JSON_WRITER_OK;
}

// 结束对象
json_writer_error_t json_writer_end_object(json_writer_t *jw) {
    if (!jw) {
        return JSON_WRITER_INVALID_PARAMS;
    }
    
    if (jw->depth <= 0) {
        return JSON_WRITER_FORMAT_ERROR;
    }
    
    json_writer_write_char(jw, '}');
    jw->depth--;
    
    return JSON_WRITER_OK;
}

// 开始数组
json_writer_error_t json_writer_begin_array(json_writer_t *jw) {
    if (!jw) {
        return JSON_WRITER_INVALID_PARAMS;
    }
    
    if (jw->config.max_depth > 0 && jw->depth >= (int)jw->config.max_depth) {
        return JSON_WRITER_DEPTH_ERROR;
    }
    
    json_writer_comma(jw);
    json_writer_write_char(jw, '[');
    jw->depth++;
    jw->first[jw->depth] = true;
    jw->array_count++;
    
    return JSON_WRITER_OK;
}

// 结束数组
json_writer_error_t json_writer_end_array(json_writer_t *jw) {
    if (!jw) {
        return JSON_WRITER_INVALID_PARAMS;
    }
    
    if (jw->depth <= 0) {
        return JSON_WRITER_FORMAT_ERROR;
    }
    
    json_writer_write_char(jw, ']');
    jw->depth--;
    
    return JSON_WRITER_OK;
}

// 写入键
json_writer_error_t json_writer_key(json_writer_t *jw, const char *key) {
    if (!jw || !key) {
        return JSON_WRITER_INVALID_PARAMS;
    }
    
    json_writer_comma(jw);
    json_writer_write_char(jw, '"');
    json_writer_write_raw(jw, key);
    json_writer_write_raw(jw, "\":");
    jw->first[jw->depth] = true;
    
    return JSON_WRITER_OK;
}

// 写入字符串
json_writer_error_t json_writer_string(json_writer_t *jw, const char *val) {
    if (!jw || !val) {
        return JSON_WRITER_INVALID_PARAMS;
    }
    
    json_writer_comma(jw);
    json_writer_write_char(jw, '"');
    json_writer_write_raw(jw, val);
    json_writer_write_char(jw, '"');
    
    return JSON_WRITER_OK;
}

// 写入整数
json_writer_error_t json_writer_int(json_writer_t *jw, int val) {
    if (!jw) {
        return JSON_WRITER_INVALID_PARAMS;
    }
    
    json_writer_comma(jw);
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", val);
    json_writer_write_raw(jw, buf);
    
    return JSON_WRITER_OK;
}

// 写入 64 位整数
json_writer_error_t json_writer_int64(json_writer_t *jw, int64_t val) {
    if (!jw) {
        return JSON_WRITER_INVALID_PARAMS;
    }
    
    json_writer_comma(jw);
    char buf[32];
    snprintf(buf, sizeof(buf), "%lld", (long long)val);
    json_writer_write_raw(jw, buf);
    
    return JSON_WRITER_OK;
}

// 写入无符号整数
json_writer_error_t json_writer_uint(json_writer_t *jw, unsigned int val) {
    if (!jw) {
        return JSON_WRITER_INVALID_PARAMS;
    }
    
    json_writer_comma(jw);
    char buf[32];
    snprintf(buf, sizeof(buf), "%u", val);
    json_writer_write_raw(jw, buf);
    
    return JSON_WRITER_OK;
}

// 写入 64 位无符号整数
json_writer_error_t json_writer_uint64(json_writer_t *jw, uint64_t val) {
    if (!jw) {
        return JSON_WRITER_INVALID_PARAMS;
    }
    
    json_writer_comma(jw);
    char buf[32];
    snprintf(buf, sizeof(buf), "%llu", (unsigned long long)val);
    json_writer_write_raw(jw, buf);
    
    return JSON_WRITER_OK;
}

// 写入浮点数
json_writer_error_t json_writer_double(json_writer_t *jw, double val) {
    if (!jw) {
        return JSON_WRITER_INVALID_PARAMS;
    }
    
    json_writer_comma(jw);
    char buf[64];
    snprintf(buf, sizeof(buf), "%g", val);
    json_writer_write_raw(jw, buf);
    
    return JSON_WRITER_OK;
}

// 写入布尔值
json_writer_error_t json_writer_bool(json_writer_t *jw, bool val) {
    if (!jw) {
        return JSON_WRITER_INVALID_PARAMS;
    }
    
    json_writer_comma(jw);
    json_writer_write_raw(jw, val ? "true" : "false");
    
    return JSON_WRITER_OK;
}

// 写入 null
json_writer_error_t json_writer_null(json_writer_t *jw) {
    if (!jw) {
        return JSON_WRITER_INVALID_PARAMS;
    }
    
    json_writer_comma(jw);
    json_writer_write_raw(jw, "null");
    
    return JSON_WRITER_OK;
}

// 刷新输出
json_writer_error_t json_writer_flush(json_writer_t *jw) {
    if (!jw) {
        return JSON_WRITER_INVALID_PARAMS;
    }
    
    if (jw->output_type == JSON_WRITER_OUTPUT_FILE && jw->output.file) {
        fflush(jw->output.file);
    }
    
    return JSON_WRITER_OK;
}

// 获取缓冲区内容
json_writer_error_t json_writer_get_buffer_content(json_writer_t *jw, const char** content, size_t* size) {
    if (!jw || !content || !size) {
        return JSON_WRITER_INVALID_PARAMS;
    }
    
    if (jw->output_type != JSON_WRITER_OUTPUT_BUFFER) {
        return JSON_WRITER_INVALID_PARAMS;
    }
    
    *content = jw->output.buffer.buffer;
    *size = jw->output.buffer.used;
    
    return JSON_WRITER_OK;
}

// 获取最后一次错误
json_writer_error_t json_writer_get_last_error(json_writer_t *jw) {
    if (!jw) {
        return JSON_WRITER_INVALID_PARAMS;
    }
    return jw->last_error;
}

// 获取错误信息
const char* json_writer_strerror(json_writer_error_t error) {
    switch (error) {
        case JSON_WRITER_OK: return "Success";
        case JSON_WRITER_INVALID_PARAMS: return "Invalid parameters";
        case JSON_WRITER_FILE_ERROR: return "File error";
        case JSON_WRITER_MEMORY_ERROR: return "Memory error";
        case JSON_WRITER_BUFFER_TOO_SMALL: return "Buffer too small";
        case JSON_WRITER_DEPTH_ERROR: return "Depth exceeded";
        case JSON_WRITER_FORMAT_ERROR: return "Format error";
        case JSON_WRITER_ENCODING_ERROR: return "Encoding error";
        default: return "Unknown error";
    }
}
