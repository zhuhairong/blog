#include "json.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

static void* (*json_malloc)(size_t) = malloc;
static void (*json_free_func)(void*) = free;

json_value_t* json_get_object_item(const json_value_t *obj, const char *key) {
    if (!obj || obj->type != JSON_OBJECT) return NULL;
    for (size_t i = 0; i < obj->u.object.count; i++) {
        if (strcmp(obj->u.object.keys[i], key) == 0) return obj->u.object.values[i];
    }
    return NULL;
}

void json_free(json_value_t *v) {
    if (!v) return;
    switch (v->type) {
        case JSON_STRING: json_free_func(v->u.string); break;
        case JSON_ARRAY:
            for (size_t i = 0; i < v->u.array.count; i++) json_free(v->u.array.items[i]);
            json_free_func(v->u.array.items);
            break;
        case JSON_OBJECT:
            for (size_t i = 0; i < v->u.object.count; i++) {
                json_free_func(v->u.object.keys[i]);
                json_free(v->u.object.values[i]);
            }
            json_free_func(v->u.object.keys);
            json_free_func(v->u.object.values);
            break;
        default: break;
    }
    json_free_func(v);
}

// 极简解析器实现 (仅演示核心逻辑)
static const char* parse_value(json_value_t *v, const char *p);

static const char* skip_whitespace(const char *p) {
    while (p && *p && isspace(*p)) p++;
    return p;
}

static const char* parse_string(json_value_t *v, const char *p) {
    p++; // skip "
    const char *end = strchr(p, '"');
    if (!end) return NULL;
    size_t len = end - p;
    v->type = JSON_STRING;
    v->u.string = json_malloc(len + 1);
    memcpy(v->u.string, p, len);
    v->u.string[len] = '\0';
    return end + 1;
}

static const char* parse_number(json_value_t *v, const char *p) {
    const char *start = p;
    char *end;
    v->type = JSON_NUMBER;
    v->u.number = strtod(p, &end);
    // 检查是否实际解析了数字
    if (end == p) return NULL;
    // 检查严格的 JSON 数字格式
    // 不允许前导零（除非就是 0）
    if (*start == '0' && isdigit(start[1])) return NULL;
    return end;
}

static const char* parse_literal(json_value_t *v, const char *p) {
    if (strncmp(p, "true", 4) == 0) { v->type = JSON_BOOL; v->u.boolean = true; return p + 4; }
    if (strncmp(p, "false", 5) == 0) { v->type = JSON_BOOL; v->u.boolean = false; return p + 5; }
    if (strncmp(p, "null", 4) == 0) { v->type = JSON_NULL; return p + 4; }
    return NULL;
}

static const char* parse_value(json_value_t *v, const char *p) {
    p = skip_whitespace(p);
    if (!*p) return NULL;
    if (*p == '"') return parse_string(v, p);
    if (*p == '{') {
        v->type = JSON_OBJECT;
        v->u.object.count = 0;
        v->u.object.keys = NULL;
        v->u.object.values = NULL;
        p++;
        p = skip_whitespace(p);
        if (*p == '}') return p + 1;
        while (1) {
            json_value_t key_v;
            p = parse_string(&key_v, p);
            if (!p) return NULL;
            p = skip_whitespace(p);
            if (*p != ':') { json_free_func(key_v.u.string); return NULL; }
            p++;
            json_value_t *val = json_malloc(sizeof(json_value_t));
            p = parse_value(val, p);
            if (!p) { json_free_func(key_v.u.string); json_free_func(val); return NULL; }
            v->u.object.keys = realloc(v->u.object.keys, sizeof(char*) * (v->u.object.count + 1));
            v->u.object.values = realloc(v->u.object.values, sizeof(json_value_t*) * (v->u.object.count + 1));
            v->u.object.keys[v->u.object.count] = key_v.u.string;
            v->u.object.values[v->u.object.count] = val;
            v->u.object.count++;
            p = skip_whitespace(p);
            if (*p == '}') return p + 1;
            if (*p != ',') return NULL;
            p++;
            p = skip_whitespace(p);
        }
    }
    if (*p == '[') {
        v->type = JSON_ARRAY;
        v->u.array.count = 0;
        v->u.array.items = NULL;
        p++;
        p = skip_whitespace(p);
        if (*p == ']') return p + 1;
        while (1) {
            json_value_t *item = json_malloc(sizeof(json_value_t));
            p = parse_value(item, p);
            if (!p) { json_free_func(item); return NULL; }
            v->u.array.items = realloc(v->u.array.items, sizeof(json_value_t*) * (v->u.array.count + 1));
            v->u.array.items[v->u.array.count++] = item;
            p = skip_whitespace(p);
            if (*p == ']') return p + 1;
            if (*p != ',') return NULL;
            p++;
            p = skip_whitespace(p);
        }
    }
    if (isdigit(*p) || *p == '-') return parse_number(v, p);
    return parse_literal(v, p);
}

json_value_t* json_parse(const char *json) {
    if (!json) return NULL;
    json_value_t *v = json_malloc(sizeof(json_value_t));
    if (parse_value(v, json)) return v;
    json_free(v);
    return NULL;
}

// 序列化辅助函数
static void serialize_value(const json_value_t *v, char **buf, size_t *len, size_t *cap);

static void ensure_space(char **buf, size_t *len, size_t *cap, size_t needed) {
    while (*len + needed >= *cap) {
        *cap *= 2;
        *buf = realloc(*buf, *cap);
    }
}

static void append_string(char **buf, size_t *len, size_t *cap, const char *str) {
    size_t str_len = strlen(str);
    ensure_space(buf, len, cap, str_len + 1);
    memcpy(*buf + *len, str, str_len);
    *len += str_len;
    (*buf)[*len] = '\0';
}

static void serialize_string(const char *str, char **buf, size_t *len, size_t *cap) {
    ensure_space(buf, len, cap, 3);
    (*buf)[(*len)++] = '"';
    (*buf)[*len] = '\0';
    
    for (const char *p = str; *p; p++) {
        switch (*p) {
            case '"': ensure_space(buf, len, cap, 3); (*buf)[(*len)++] = '\\'; (*buf)[(*len)++] = '"'; break;
            case '\\': ensure_space(buf, len, cap, 3); (*buf)[(*len)++] = '\\'; (*buf)[(*len)++] = '\\'; break;
            case '\b': ensure_space(buf, len, cap, 3); (*buf)[(*len)++] = '\\'; (*buf)[(*len)++] = 'b'; break;
            case '\f': ensure_space(buf, len, cap, 3); (*buf)[(*len)++] = '\\'; (*buf)[(*len)++] = 'f'; break;
            case '\n': ensure_space(buf, len, cap, 3); (*buf)[(*len)++] = '\\'; (*buf)[(*len)++] = 'n'; break;
            case '\r': ensure_space(buf, len, cap, 3); (*buf)[(*len)++] = '\\'; (*buf)[(*len)++] = 'r'; break;
            case '\t': ensure_space(buf, len, cap, 3); (*buf)[(*len)++] = '\\'; (*buf)[(*len)++] = 't'; break;
            default:
                if ((unsigned char)*p < 0x20) {
                    ensure_space(buf, len, cap, 7);
                    sprintf(*buf + *len, "\\u%04x", (unsigned char)*p);
                    *len += 6;
                } else {
                    ensure_space(buf, len, cap, 2);
                    (*buf)[(*len)++] = *p;
                }
        }
    }
    
    ensure_space(buf, len, cap, 2);
    (*buf)[(*len)++] = '"';
    (*buf)[*len] = '\0';
}

static void serialize_value(const json_value_t *v, char **buf, size_t *len, size_t *cap) {
    if (!v) {
        append_string(buf, len, cap, "null");
        return;
    }
    
    char num_buf[64];
    switch (v->type) {
        case JSON_NULL:
            append_string(buf, len, cap, "null");
            break;
        case JSON_BOOL:
            append_string(buf, len, cap, v->u.boolean ? "true" : "false");
            break;
        case JSON_NUMBER:
            snprintf(num_buf, sizeof(num_buf), "%g", v->u.number);
            append_string(buf, len, cap, num_buf);
            break;
        case JSON_STRING:
            serialize_string(v->u.string, buf, len, cap);
            break;
        case JSON_ARRAY:
            ensure_space(buf, len, cap, 2);
            (*buf)[(*len)++] = '[';
            (*buf)[*len] = '\0';
            for (size_t i = 0; i < v->u.array.count; i++) {
                if (i > 0) append_string(buf, len, cap, ",");
                serialize_value(v->u.array.items[i], buf, len, cap);
            }
            ensure_space(buf, len, cap, 2);
            (*buf)[(*len)++] = ']';
            (*buf)[*len] = '\0';
            break;
        case JSON_OBJECT:
            ensure_space(buf, len, cap, 2);
            (*buf)[(*len)++] = '{';
            (*buf)[*len] = '\0';
            for (size_t i = 0; i < v->u.object.count; i++) {
                if (i > 0) append_string(buf, len, cap, ",");
                serialize_string(v->u.object.keys[i], buf, len, cap);
                append_string(buf, len, cap, ":");
                serialize_value(v->u.object.values[i], buf, len, cap);
            }
            ensure_space(buf, len, cap, 2);
            (*buf)[(*len)++] = '}';
            (*buf)[*len] = '\0';
            break;
    }
}

char* json_print(const json_value_t *v) {
    if (!v) return NULL;
    size_t cap = 256;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    buf[0] = '\0';
    serialize_value(v, &buf, &len, &cap);
    return buf;
}
