#ifndef C_UTILS_UTF8_H
#define C_UTILS_UTF8_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

// UTF-8 错误码
typedef enum {
    UTF8_OK = 0,
    UTF8_INVALID_PARAMS = -1,
    UTF8_INVALID_SEQUENCE = -2,
    UTF8_OVERFLOW = -3,
    UTF8_UNDERFLOW = -4,
    UTF8_MEMORY_ERROR = -5
} utf8_error_t;

// UTF-8 配置选项
typedef struct {
    bool strict_validation;     // 是否使用严格验证
    bool allow_surrogates;      // 是否允许代理对
    bool allow_overlong;        // 是否允许过长编码
    size_t max_string_length;   // 最大字符串长度
} utf8_config_t;

// UTF-8 字符迭代器
typedef struct {
    const char *str;
    size_t pos;
    size_t len;
    utf8_error_t last_error;
} utf8_iterator_t;

// UTF-8 上下文
typedef struct {
    utf8_config_t config;
    utf8_error_t last_error;
} utf8_ctx_t;

// 创建 UTF-8 上下文
utf8_error_t utf8_create(utf8_ctx_t** ctx, const utf8_config_t* config);

// 销毁 UTF-8 上下文
void utf8_destroy(utf8_ctx_t* ctx);

// 检查是否为合法的 UTF-8 字符串
// 返回 true 表示有效，false 表示无效
bool utf8_is_valid(const char *str);

// 计算字符数 (非字节数)
size_t utf8_strlen(const char *str);

// 验证 UTF-8 字符串
// 返回 UTF8_OK 表示成功，其他值表示错误
utf8_error_t utf8_validate(utf8_ctx_t* ctx, const char *str, size_t len);

// 获取字符数
// 返回 UTF8_OK 表示成功，其他值表示错误
utf8_error_t utf8_get_length(utf8_ctx_t* ctx, const char *str, size_t len, size_t* out_length);

// 获取指定位置的字符
// 返回 UTF8_OK 表示成功，其他值表示错误
utf8_error_t utf8_get_char(utf8_ctx_t* ctx, const char *str, size_t pos, uint32_t* out_char, size_t* out_bytes);

// 查找字符
// 返回 UTF8_OK 表示成功，其他值表示错误
utf8_error_t utf8_find_char(utf8_ctx_t* ctx, const char *str, uint32_t c, size_t* out_pos);

// 转换为大写
// 返回 UTF8_OK 表示成功，其他值表示错误
utf8_error_t utf8_to_upper(utf8_ctx_t* ctx, const char *str, char *out, size_t* out_len);

// 转换为小写
// 返回 UTF8_OK 表示成功，其他值表示错误
utf8_error_t utf8_to_lower(utf8_ctx_t* ctx, const char *str, char *out, size_t* out_len);

// 创建迭代器
utf8_error_t utf8_iterator_create(utf8_iterator_t* iter, const char *str);

// 获取下一个字符
utf8_error_t utf8_iterator_next(utf8_iterator_t* iter, uint32_t* out_char);

// 检查是否结束
bool utf8_iterator_end(const utf8_iterator_t* iter);

// 获取最后一次错误
utf8_error_t utf8_get_last_error(utf8_ctx_t* ctx);

// 获取错误信息
const char* utf8_strerror(utf8_error_t error);

#endif // C_UTILS_UTF8_H
