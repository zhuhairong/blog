#include "cron.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// 解析单个字段
static bool parse_field(const char *field, bool *values, int min, int max, cron_error_t *error) {
    if (!field || !values) {
        if (error) *error = CRON_ERROR_INVALID_FIELD;
        return false;
    }
    
    // 初始化所有值为false
    for (int i = min; i <= max; i++) {
        values[i] = false;
    }
    
    // 处理 "*" (所有值)
    if (strcmp(field, "*") == 0) {
        for (int i = min; i <= max; i++) {
            values[i] = true;
        }
        return true;
    }
    
    // 处理范围 (例如 "1-5")
    char *dash = strchr(field, '-');
    if (dash) {
        int start = atoi(field);
        int end = atoi(dash + 1);
        if (start < min || end > max || start > end) {
            if (error) *error = CRON_ERROR_INVALID_RANGE;
            return false;
        }
        for (int i = start; i <= end; i++) {
            values[i] = true;
        }
        return true;
    }
    
    // 处理单个值
    int val = atoi(field);
    if (val < min || val > max) {
        if (error) *error = CRON_ERROR_INVALID_RANGE;
        return false;
    }
    values[val] = true;
    return true;
}

// 解析 Cron 表达式
bool cron_parse(const char *expr, cron_expr_t *cron) {
    if (!expr || !cron) {
        return false;
    }
    
    memset(cron, 0, sizeof(cron_expr_t));
    
    char buf[256];
    strncpy(buf, expr, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';
    
    char *min = strtok(buf, " ");
    char *hour = strtok(NULL, " ");
    char *dom = strtok(NULL, " ");
    char *mon = strtok(NULL, " ");
    char *dow = strtok(NULL, " ");
    
    if (!min || !hour || !dom || !mon || !dow) {
        cron->has_error = true;
        cron->error = CRON_ERROR_INVALID_EXPRESSION;
        snprintf(cron->error_msg, sizeof(cron->error_msg), "Invalid expression format");
        return false;
    }
    
    cron_error_t error;
    if (!parse_field(min, cron->minute, 0, 59, &error)) {
        cron->has_error = true;
        cron->error = error;
        snprintf(cron->error_msg, sizeof(cron->error_msg), "Invalid minute field");
        return false;
    }
    
    if (!parse_field(hour, cron->hour, 0, 23, &error)) {
        cron->has_error = true;
        cron->error = error;
        snprintf(cron->error_msg, sizeof(cron->error_msg), "Invalid hour field");
        return false;
    }
    
    if (!parse_field(dom, cron->day_of_month, 1, 31, &error)) {
        cron->has_error = true;
        cron->error = error;
        snprintf(cron->error_msg, sizeof(cron->error_msg), "Invalid day of month field");
        return false;
    }
    
    if (!parse_field(mon, cron->month, 1, 12, &error)) {
        cron->has_error = true;
        cron->error = error;
        snprintf(cron->error_msg, sizeof(cron->error_msg), "Invalid month field");
        return false;
    }
    
    if (!parse_field(dow, cron->day_of_week, 0, 6, &error)) {
        cron->has_error = true;
        cron->error = error;
        snprintf(cron->error_msg, sizeof(cron->error_msg), "Invalid day of week field");
        return false;
    }
    
    cron->has_error = false;
    cron->error = CRON_OK;
    return true;
}

// 匹配 Cron 表达式
bool cron_match(const cron_expr_t *cron, const struct tm *t) {
    if (!cron || !t || cron->has_error) {
        return false;
    }
    
    return cron->minute[t->tm_min] &&
           cron->hour[t->tm_hour] &&
           cron->day_of_month[t->tm_mday] &&
           cron->month[t->tm_mon + 1] &&
           cron->day_of_week[t->tm_wday];
}

// 匹配 Cron 表达式（直接使用字符串）
bool cron_match_str(const char *expr, const struct tm *t, cron_error_t *error) {
    if (!expr || !t) {
        if (error) *error = CRON_ERROR_INVALID_EXPRESSION;
        return false;
    }
    
    cron_expr_t cron;
    if (!cron_parse(expr, &cron)) {
        if (error) *error = cron.error;
        return false;
    }
    
    if (error) *error = CRON_OK;
    return cron_match(&cron, t);
}

// 获取下一次执行时间
bool cron_next(const cron_expr_t *cron, const struct tm *current, struct tm *next, cron_error_t *error) {
    if (!cron || !current || !next || cron->has_error) {
        if (error) *error = CRON_ERROR_INVALID_EXPRESSION;
        return false;
    }
    
    time_t start = mktime((struct tm *)current);
    if (start == (time_t)-1) {
        if (error) *error = CRON_ERROR_INVALID_EXPRESSION;
        return false;
    }
    
    // 从下一分钟开始搜索
    time_t t = start + 60;
    
    // 最多搜索一年
    for (int i = 0; i < 525600; i++) {
        struct tm *tm = localtime(&t);
        if (!tm) {
            if (error) *error = CRON_ERROR_INVALID_EXPRESSION;
            return false;
        }
        
        if (cron_match(cron, tm)) {
            memcpy(next, tm, sizeof(struct tm));
            if (error) *error = CRON_OK;
            return true;
        }
        
        t += 60; // 下一分钟
    }
    
    if (error) *error = CRON_ERROR_INVALID_EXPRESSION;
    return false;
}

// 获取错误信息
const char* cron_strerror(cron_error_t error) {
    switch (error) {
        case CRON_OK:
            return "Success";
        case CRON_ERROR_INVALID_EXPRESSION:
            return "Invalid cron expression";
        case CRON_ERROR_INVALID_FIELD:
            return "Invalid field";
        case CRON_ERROR_INVALID_RANGE:
            return "Invalid range";
        case CRON_ERROR_INVALID_STEP:
            return "Invalid step";
        case CRON_ERROR_UNSUPPORTED_FEATURE:
            return "Unsupported feature";
        case CRON_ERROR_MEMORY_ALLOC:
            return "Memory allocation failed";
        default:
            return "Unknown error";
    }
}
