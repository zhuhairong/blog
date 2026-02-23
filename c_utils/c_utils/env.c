#include "env.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

// 默认环境变量选项
env_options_t env_default_options(void) {
    env_options_t opts = {
        .expand_variables = false,
        .trim_whitespace = true,
        .case_insensitive = false,
        .thread_safe = false
    };
    return opts;
}

// 获取环境变量值
const char* env_get(const char *name, const char *default_val, const env_options_t *options, env_error_t *error) {
    if (!name) {
        if (error) *error = ENV_ERROR_INVALID_PARAM;
        return default_val;
    }
    
    const char *val = getenv(name);
    if (!val) {
        if (error) *error = ENV_OK;
        return default_val;
    }
    
    // 处理选项
    static char buffer[4096];
    const char *result = val;
    
    if (options && options->trim_whitespace) {
        // 去除前后空格
        const char *start = val;
        while (isspace(*start)) start++;
        
        const char *end = val + strlen(val) - 1;
        while (end > start && isspace(*end)) end--;
        
        size_t len = end - start + 1;
        if (len >= sizeof(buffer)) {
            len = sizeof(buffer) - 1;
        }
        memcpy(buffer, start, len);
        buffer[len] = '\0';
        result = buffer;
    }
    
    if (error) *error = ENV_OK;
    return result;
}

// 获取整数环境变量
int env_get_int(const char *name, int default_val, const env_options_t *options, env_error_t *error) {
    const char *val = env_get(name, NULL, options, error);
    if (!val) {
        return default_val;
    }
    
    char *endptr;
    int result = (int)strtol(val, &endptr, 10);
    
    if (*endptr != '\0') {
        // 转换失败，返回默认值
        if (error) *error = ENV_OK;
        return default_val;
    }
    
    if (error) *error = ENV_OK;
    return result;
}

// 获取浮点数环境变量
double env_get_double(const char *name, double default_val, const env_options_t *options, env_error_t *error) {
    const char *val = env_get(name, NULL, options, error);
    if (!val) {
        return default_val;
    }
    
    char *endptr;
    double result = strtod(val, &endptr);
    
    if (*endptr != '\0') {
        // 转换失败，返回默认值
        if (error) *error = ENV_OK;
        return default_val;
    }
    
    if (error) *error = ENV_OK;
    return result;
}

// 获取布尔值环境变量
bool env_get_bool(const char *name, bool default_val, const env_options_t *options, env_error_t *error) {
    const char *val = env_get(name, NULL, options, error);
    if (!val) {
        return default_val;
    }
    
    // 检查常见的布尔值表示
    if (strcasecmp(val, "true") == 0 ||
        strcasecmp(val, "yes") == 0 ||
        strcasecmp(val, "1") == 0 ||
        strcasecmp(val, "on") == 0) {
        if (error) *error = ENV_OK;
        return true;
    }
    
    if (strcasecmp(val, "false") == 0 ||
        strcasecmp(val, "no") == 0 ||
        strcasecmp(val, "0") == 0 ||
        strcasecmp(val, "off") == 0) {
        if (error) *error = ENV_OK;
        return false;
    }
    
    // 无法识别，返回默认值
    if (error) *error = ENV_OK;
    return default_val;
}

// 设置环境变量
bool env_set(const char *name, const char *value, bool overwrite, const env_options_t *options, env_error_t *error) {
    (void)options; // 暂时未使用
    
    if (!name || !value) {
        if (error) *error = ENV_ERROR_INVALID_PARAM;
        return false;
    }
    
#ifdef _WIN32
    if (!overwrite && getenv(name)) {
        if (error) *error = ENV_OK;
        return true;
    }
    int result = _putenv_s(name, value);
#else
    int result = setenv(name, value, overwrite ? 1 : 0);
#endif
    
    if (result != 0) {
        if (error) *error = ENV_ERROR_SET_FAILED;
        return false;
    }
    
    if (error) *error = ENV_OK;
    return true;
}

// 删除环境变量
bool env_unset(const char *name, const env_options_t *options, env_error_t *error) {
    (void)options; // 暂时未使用
    
    if (!name) {
        if (error) *error = ENV_ERROR_INVALID_PARAM;
        return false;
    }
    
#ifdef _WIN32
    char buf[256];
    snprintf(buf, sizeof(buf), "%s=", name);
    int result = _putenv(buf);
#else
    int result = unsetenv(name);
#endif
    
    if (result != 0) {
        if (error) *error = ENV_ERROR_UNSET_FAILED;
        return false;
    }
    
    if (error) *error = ENV_OK;
    return true;
}

// 检查环境变量是否存在
bool env_has(const char *name, const env_options_t *options, env_error_t *error) {
    (void)options; // 暂时未使用
    
    if (!name) {
        if (error) *error = ENV_ERROR_INVALID_PARAM;
        return false;
    }
    
    bool exists = getenv(name) != NULL;
    if (error) *error = ENV_OK;
    return exists;
}

// 获取错误信息
const char* env_strerror(env_error_t error) {
    switch (error) {
        case ENV_OK:
            return "Success";
        case ENV_ERROR_INVALID_PARAM:
            return "Invalid parameter";
        case ENV_ERROR_SET_FAILED:
            return "Failed to set environment variable";
        case ENV_ERROR_UNSET_FAILED:
            return "Failed to unset environment variable";
        case ENV_ERROR_MEMORY_ALLOC:
            return "Memory allocation failed";
        case ENV_ERROR_BUFFER_TOO_SMALL:
            return "Buffer too small";
        case ENV_ERROR_PLATFORM_UNSUPPORTED:
            return "Platform not supported";
        default:
            return "Unknown error";
    }
}
