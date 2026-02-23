#include "dstr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    size_t len;
    size_t cap;
    char buf[];
} dstr_hdr;

#define DSTR_HDR(s) ((dstr_hdr*)((s) - offsetof(dstr_hdr, buf)))

dstr dstr_new_len(const void *init, size_t init_len) {
    dstr_hdr *hdr = malloc(sizeof(dstr_hdr) + init_len + 1);
    if (!hdr) return NULL;
    hdr->len = init_len;
    hdr->cap = init_len;
    if (init && init_len) {
        memcpy(hdr->buf, init, init_len);
    }
    hdr->buf[init_len] = '\0';
    return hdr->buf;
}

dstr dstr_new(const char *init) {
    size_t len = init ? strlen(init) : 0;
    return dstr_new_len(init, len);
}

dstr dstr_new_empty(void) {
    return dstr_new_len("", 0);
}

void dstr_free(dstr s) {
    if (s) free(DSTR_HDR(s));
}

size_t dstr_len(const dstr s) {
    return s ? DSTR_HDR(s)->len : 0;
}

size_t dstr_avail(const dstr s) {
    if (!s) return 0;
    dstr_hdr *hdr = DSTR_HDR(s);
    return hdr->cap - hdr->len;
}

size_t dstr_capacity(const dstr s) {
    return s ? DSTR_HDR(s)->cap : 0;
}

bool dstr_is_empty(const dstr s) {
    return dstr_len(s) == 0;
}

static dstr dstr_make_room(dstr s, size_t add_len) {
    if (!s) return NULL;
    dstr_hdr *hdr = DSTR_HDR(s);
    if (hdr->cap - hdr->len >= add_len) return s;

    size_t new_cap = (hdr->len + add_len) * 2;
    if (new_cap < 16) new_cap = 16; // 最小容量
    
    dstr_hdr *new_hdr = realloc(hdr, sizeof(dstr_hdr) + new_cap + 1);
    if (!new_hdr) return NULL;
    new_hdr->cap = new_cap;
    return new_hdr->buf;
}

dstr dstr_append_len(dstr s, const void *t, size_t len) {
    if (!s) return NULL;
    if (!t || len == 0) return s;
    
    s = dstr_make_room(s, len);
    if (!s) return NULL;
    dstr_hdr *hdr = DSTR_HDR(s);
    memcpy(hdr->buf + hdr->len, t, len);
    hdr->len += len;
    hdr->buf[hdr->len] = '\0';
    return s;
}

dstr dstr_append(dstr s, const char *t) {
    if (!s) return NULL;
    return t ? dstr_append_len(s, t, strlen(t)) : s;
}

dstr dstr_append_fmt(dstr s, const char *fmt, ...) {
    if (!s || !fmt) return s;
    
    va_list ap;
    va_start(ap, fmt);
    va_list cpy;
    va_copy(cpy, ap);
    int len = vsnprintf(NULL, 0, fmt, cpy);
    va_end(cpy);
    
    if (len < 0) { va_end(ap); return s; }
    
    s = dstr_make_room(s, (size_t)len);
    if (!s) { va_end(ap); return NULL; }
    
    dstr_hdr *hdr = DSTR_HDR(s);
    vsnprintf(hdr->buf + hdr->len, (size_t)len + 1, fmt, ap);
    hdr->len += (size_t)len;
    va_end(ap);
    return s;
}

dstr dstr_append_char(dstr s, char c) {
    return dstr_append_len(s, &c, 1);
}

dstr dstr_prepend_len(dstr s, const void *t, size_t len) {
    if (!s) return NULL;
    if (!t || len == 0) return s;
    
    dstr_hdr *hdr = DSTR_HDR(s);
    size_t old_len = hdr->len;
    
    s = dstr_make_room(s, len);
    if (!s) return NULL;
    hdr = DSTR_HDR(s);
    
    // 移动原有内容
    memmove(hdr->buf + len, hdr->buf, old_len + 1);
    // 复制新内容到开头
    memcpy(hdr->buf, t, len);
    hdr->len = old_len + len;
    
    return s;
}

dstr dstr_prepend(dstr s, const char *t) {
    if (!s) return NULL;
    return t ? dstr_prepend_len(s, t, strlen(t)) : s;
}

void dstr_clear(dstr s) {
    if (!s) return;
    dstr_hdr *hdr = DSTR_HDR(s);
    hdr->len = 0;
    hdr->buf[0] = '\0';
}

dstr dstr_substr(const dstr s, size_t start, size_t len) {
    if (!s) return NULL;
    size_t s_len = dstr_len(s);
    
    if (start >= s_len) return dstr_new_empty();
    if (start + len > s_len) len = s_len - start;
    
    return dstr_new_len(s + start, len);
}

dstr dstr_dup(const dstr s) {
    if (!s) return NULL;
    return dstr_new_len(s, dstr_len(s));
}

int dstr_cmp(const dstr s1, const dstr s2) {
    if (!s1 && !s2) return 0;
    if (!s1) return -1;
    if (!s2) return 1;
    return strcmp(s1, s2);
}

int dstr_casecmp(const dstr s1, const dstr s2) {
    if (!s1 && !s2) return 0;
    if (!s1) return -1;
    if (!s2) return 1;
    
    size_t len1 = dstr_len(s1);
    size_t len2 = dstr_len(s2);
    size_t min_len = len1 < len2 ? len1 : len2;
    
    for (size_t i = 0; i < min_len; i++) {
        int c1 = tolower((unsigned char)s1[i]);
        int c2 = tolower((unsigned char)s2[i]);
        if (c1 != c2) return c1 - c2;
    }
    
    if (len1 < len2) return -1;
    if (len1 > len2) return 1;
    return 0;
}

bool dstr_starts_with(const dstr s, const char *prefix) {
    if (!s || !prefix) return false;
    size_t s_len = dstr_len(s);
    size_t prefix_len = strlen(prefix);
    if (prefix_len > s_len) return false;
    return strncmp(s, prefix, prefix_len) == 0;
}

bool dstr_ends_with(const dstr s, const char *suffix) {
    if (!s || !suffix) return false;
    size_t s_len = dstr_len(s);
    size_t suffix_len = strlen(suffix);
    if (suffix_len > s_len) return false;
    return strncmp(s + s_len - suffix_len, suffix, suffix_len) == 0;
}

bool dstr_contains(const dstr s, const char *substr) {
    if (!s || !substr) return false;
    return strstr(s, substr) != NULL;
}

ssize_t dstr_find(const dstr s, const char *substr) {
    if (!s || !substr) return -1;
    const char *p = strstr(s, substr);
    if (!p) return -1;
    return p - s;
}

ssize_t dstr_rfind(const dstr s, const char *substr) {
    if (!s || !substr) return -1;
    size_t s_len = dstr_len(s);
    size_t sub_len = strlen(substr);
    if (sub_len > s_len) return -1;
    
    for (size_t i = s_len - sub_len + 1; i > 0; i--) {
        if (strncmp(s + i - 1, substr, sub_len) == 0) {
            return (ssize_t)(i - 1);
        }
    }
    return -1;
}

dstr dstr_replace(dstr s, const char *old, const char *new_str) {
    if (!s || !old || !new_str) return s;
    
    size_t old_len = strlen(old);
    size_t new_len = strlen(new_str);
    
    if (old_len == 0) return s;
    
    // 计算替换次数
    size_t count = 0;
    const char *tmp = s;
    while ((tmp = strstr(tmp, old)) != NULL) {
        count++;
        tmp += old_len;
    }
    
    if (count == 0) return s;
    
    // 创建新字符串
    size_t s_len = dstr_len(s);
    size_t result_len = s_len + count * (new_len - old_len);
    dstr result = dstr_new_len(NULL, result_len);
    if (!result) return s;
    
    // 执行替换
    char *dst = result;
    const char *src = s;
    const char *match;
    
    while ((match = strstr(src, old)) != NULL) {
        size_t prefix_len = match - src;
        memcpy(dst, src, prefix_len);
        dst += prefix_len;
        memcpy(dst, new_str, new_len);
        dst += new_len;
        src = match + old_len;
    }
    // 复制剩余部分
    strcpy(dst, src);
    
    dstr_free(s);
    return result;
}

void dstr_trim(dstr s, const char *cset) {
    if (!s) return;
    if (!cset || !*cset) cset = " \t\n\r\f\v";
    
    dstr_hdr *hdr = DSTR_HDR(s);
    char *start = hdr->buf;
    char *end = hdr->buf + hdr->len - 1;

    while (start <= end && strchr(cset, *start)) start++;
    while (end > start && strchr(cset, *end)) end--;

    size_t len = (start > end) ? 0 : (size_t)(end - start + 1);
    if (start != hdr->buf) memmove(hdr->buf, start, len);
    hdr->buf[len] = '\0';
    hdr->len = len;
}

void dstr_trim_left(dstr s, const char *cset) {
    if (!s) return;
    if (!cset || !*cset) cset = " \t\n\r\f\v";
    
    dstr_hdr *hdr = DSTR_HDR(s);
    char *start = hdr->buf;
    char *end = hdr->buf + hdr->len;

    while (start < end && strchr(cset, *start)) start++;

    size_t len = (size_t)(end - start);
    if (start != hdr->buf) memmove(hdr->buf, start, len);
    hdr->buf[len] = '\0';
    hdr->len = len;
}

void dstr_trim_right(dstr s, const char *cset) {
    if (!s) return;
    if (!cset || !*cset) cset = " \t\n\r\f\v";
    
    dstr_hdr *hdr = DSTR_HDR(s);
    char *end = hdr->buf + hdr->len - 1;

    while (end >= hdr->buf && strchr(cset, *end)) end--;

    size_t len = (end < hdr->buf) ? 0 : (size_t)(end - hdr->buf + 1);
    hdr->buf[len] = '\0';
    hdr->len = len;
}

void dstr_tolower(dstr s) {
    if (!s) return;
    size_t len = dstr_len(s);
    for (size_t i = 0; i < len; i++) s[i] = (char)tolower((unsigned char)s[i]);
}

void dstr_toupper(dstr s) {
    if (!s) return;
    size_t len = dstr_len(s);
    for (size_t i = 0; i < len; i++) s[i] = (char)toupper((unsigned char)s[i]);
}

void dstr_reverse(dstr s) {
    if (!s) return;
    size_t len = dstr_len(s);
    for (size_t i = 0; i < len / 2; i++) {
        char tmp = s[i];
        s[i] = s[len - 1 - i];
        s[len - 1 - i] = tmp;
    }
}
