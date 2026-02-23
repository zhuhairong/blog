#ifndef C_UTILS_DSTR_H
#define C_UTILS_DSTR_H

#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>
#include <sys/types.h>

/* 
 * dstr (Dynamic String)
 * 指针直接指向字符串数据，头部隐藏了长度和容量信息。
 * 兼容标准 C 字符串函数，如 printf("%s", s);
 */
typedef char* dstr;

// 创建与销毁
dstr dstr_new(const char *init);
dstr dstr_new_len(const void *init, size_t init_len);
dstr dstr_new_empty(void);
void dstr_free(dstr s);
void dstr_clear(dstr s);

// 属性获取
size_t dstr_len(const dstr s);
size_t dstr_avail(const dstr s);
size_t dstr_capacity(const dstr s);
bool   dstr_is_empty(const dstr s);

// 修改与拼接
dstr dstr_append(dstr s, const char *t);
dstr dstr_append_len(dstr s, const void *t, size_t len);
dstr dstr_append_fmt(dstr s, const char *fmt, ...);
dstr dstr_append_char(dstr s, char c);
dstr dstr_prepend(dstr s, const char *t);
dstr dstr_prepend_len(dstr s, const void *t, size_t len);

// 子串操作
dstr dstr_substr(const dstr s, size_t start, size_t len);
dstr dstr_dup(const dstr s);
int  dstr_cmp(const dstr s1, const dstr s2);
int  dstr_casecmp(const dstr s1, const dstr s2);
bool dstr_starts_with(const dstr s, const char *prefix);
bool dstr_ends_with(const dstr s, const char *suffix);
bool dstr_contains(const dstr s, const char *substr);

// 查找与替换
ssize_t dstr_find(const dstr s, const char *substr);
ssize_t dstr_rfind(const dstr s, const char *substr);
dstr    dstr_replace(dstr s, const char *old, const char *new_str);

// 实用工具
void dstr_trim(dstr s, const char *cset);
void dstr_trim_left(dstr s, const char *cset);
void dstr_trim_right(dstr s, const char *cset);
void dstr_tolower(dstr s);
void dstr_toupper(dstr s);
void dstr_reverse(dstr s);

#endif // C_UTILS_DSTR_H
