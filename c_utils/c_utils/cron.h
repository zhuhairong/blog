#ifndef C_UTILS_CRON_H
#define C_UTILS_CRON_H

#include <stdbool.h>
#include <time.h>
#include <stddef.h>

// Cron 错误码
typedef enum {
    CRON_OK = 0,
    CRON_ERROR_INVALID_EXPRESSION,
    CRON_ERROR_INVALID_FIELD,
    CRON_ERROR_INVALID_RANGE,
    CRON_ERROR_INVALID_STEP,
    CRON_ERROR_UNSUPPORTED_FEATURE,
    CRON_ERROR_MEMORY_ALLOC
} cron_error_t;

// Cron 字段类型
typedef enum {
    CRON_FIELD_MINUTE,
    CRON_FIELD_HOUR,
    CRON_FIELD_DAY_OF_MONTH,
    CRON_FIELD_MONTH,
    CRON_FIELD_DAY_OF_WEEK,
    CRON_FIELD_COUNT
} cron_field_type_t;

// Cron 表达式解析结果
typedef struct {
    bool minute[60];
    bool hour[24];
    bool day_of_month[32];
    bool month[13];
    bool day_of_week[7];
    bool has_error;
    cron_error_t error;
    char error_msg[256];
} cron_expr_t;

// 解析 Cron 表达式
// expr: Cron 表达式字符串
// cron: 解析结果（输出参数）
// 返回: 成功返回 true，失败返回 false
bool cron_parse(const char *expr, cron_expr_t *cron);

// 匹配 Cron 表达式
// cron: 解析后的 Cron 表达式
// t: 时间结构
// 返回: 匹配返回 true，不匹配返回 false
bool cron_match(const cron_expr_t *cron, const struct tm *t);

// 匹配 Cron 表达式（直接使用字符串）
// expr: Cron 表达式字符串
// t: 时间结构
// error: 错误码（输出参数，可为 NULL）
// 返回: 匹配返回 true，不匹配返回 false
bool cron_match_str(const char *expr, const struct tm *t, cron_error_t *error);

// 获取下一次执行时间
// cron: 解析后的 Cron 表达式
// current: 当前时间
// next: 下一次执行时间（输出参数）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool cron_next(const cron_expr_t *cron, const struct tm *current, struct tm *next, cron_error_t *error);

// 获取下一次执行时间（直接使用字符串）
// expr: Cron 表达式字符串
// current: 当前时间
// next: 下一次执行时间（输出参数）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool cron_next_str(const char *expr, const struct tm *current, struct tm *next, cron_error_t *error);

// 验证 Cron 表达式
// expr: Cron 表达式字符串
// error: 错误码（输出参数，可为 NULL）
// 返回: 有效返回 true，无效返回 false
bool cron_validate(const char *expr, cron_error_t *error);

// 获取错误信息
// error: 错误码
// 返回: 错误信息字符串
const char* cron_strerror(cron_error_t error);

#endif // C_UTILS_CRON_H
