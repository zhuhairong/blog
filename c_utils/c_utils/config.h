#ifndef C_UTILS_CONFIG_H
#define C_UTILS_CONFIG_H

#include <stddef.h>
#include <stdbool.h>

// 配置错误码
typedef enum {
    CONFIG_OK = 0,
    CONFIG_ERROR_FILE_OPEN,
    CONFIG_ERROR_FILE_READ,
    CONFIG_ERROR_FILE_WRITE,
    CONFIG_ERROR_PARSE,
    CONFIG_ERROR_INVALID_SECTION,
    CONFIG_ERROR_INVALID_KEY,
    CONFIG_ERROR_INVALID_VALUE,
    CONFIG_ERROR_MEMORY_ALLOC,
    CONFIG_ERROR_UNSUPPORTED_FORMAT
} config_error_t;

// 配置格式
typedef enum {
    CONFIG_FORMAT_INI,
    CONFIG_FORMAT_JSON,
    CONFIG_FORMAT_YAML,
    CONFIG_FORMAT_AUTO
} config_format_t;

// 配置类型
typedef struct config_s config_t;

// 创建配置
// 返回: 成功返回配置指针，失败返回 NULL
config_t* config_create(void);

// 从文件加载配置
// filename: 配置文件路径
// format: 配置格式
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回配置指针，失败返回 NULL
config_t* config_load(const char *filename, config_format_t format, config_error_t *error);

// 保存配置到文件
// cfg: 配置
// filename: 配置文件路径
// format: 配置格式
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool config_save(const config_t *cfg, const char *filename, config_format_t format, config_error_t *error);

// 释放配置
// cfg: 配置
void config_free(config_t *cfg);

// 获取字符串配置值
// cfg: 配置
// section: 节名（可为 NULL 表示默认节）
// key: 键名
// default_val: 默认值
// 返回: 配置值或默认值
const char* config_get_string(const config_t *cfg, const char *section, const char *key, const char *default_val);

// 获取整数配置值
// cfg: 配置
// section: 节名（可为 NULL 表示默认节）
// key: 键名
// default_val: 默认值
// 返回: 配置值或默认值
int config_get_int(const config_t *cfg, const char *section, const char *key, int default_val);

// 获取浮点数配置值
// cfg: 配置
// section: 节名（可为 NULL 表示默认节）
// key: 键名
// default_val: 默认值
// 返回: 配置值或默认值
double config_get_double(const config_t *cfg, const char *section, const char *key, double default_val);

// 获取布尔值配置值
// cfg: 配置
// section: 节名（可为 NULL 表示默认节）
// key: 键名
// default_val: 默认值
// 返回: 配置值或默认值
bool config_get_bool(const config_t *cfg, const char *section, const char *key, bool default_val);

// 设置字符串配置值
// cfg: 配置
// section: 节名（可为 NULL 表示默认节）
// key: 键名
// value: 值
// 返回: 成功返回 true，失败返回 false
bool config_set_string(config_t *cfg, const char *section, const char *key, const char *value);

// 设置整数配置值
// cfg: 配置
// section: 节名（可为 NULL 表示默认节）
// key: 键名
// value: 值
// 返回: 成功返回 true，失败返回 false
bool config_set_int(config_t *cfg, const char *section, const char *key, int value);

// 设置浮点数配置值
// cfg: 配置
// section: 节名（可为 NULL 表示默认节）
// key: 键名
// value: 值
// 返回: 成功返回 true，失败返回 false
bool config_set_double(config_t *cfg, const char *section, const char *key, double value);

// 设置布尔值配置值
// cfg: 配置
// section: 节名（可为 NULL 表示默认节）
// key: 键名
// value: 值
// 返回: 成功返回 true，失败返回 false
bool config_set_bool(config_t *cfg, const char *section, const char *key, bool value);

// 删除配置项
// cfg: 配置
// section: 节名（可为 NULL 表示默认节）
// key: 键名
// 返回: 成功返回 true，失败返回 false
bool config_delete(config_t *cfg, const char *section, const char *key);

// 检查配置项是否存在
// cfg: 配置
// section: 节名（可为 NULL 表示默认节）
// key: 键名
// 返回: 存在返回 true，不存在返回 false
bool config_exists(const config_t *cfg, const char *section, const char *key);

// 获取错误信息
// error: 错误码
// 返回: 错误信息字符串
const char* config_strerror(config_error_t error);

#endif // C_UTILS_CONFIG_H
