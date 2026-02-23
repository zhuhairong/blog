#ifndef C_UTILS_JSON_H
#define C_UTILS_JSON_H

#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

typedef enum {
    JSON_NULL,
    JSON_BOOL,
    JSON_NUMBER,
    JSON_STRING,
    JSON_ARRAY,
    JSON_OBJECT
} json_type_t;

typedef struct json_value_s json_value_t;

struct json_value_s {
    json_type_t type;
    union {
        bool boolean;
        double number;
        char *string;
        struct { json_value_t **items; size_t count; } array;
        struct { char **keys; json_value_t **values; size_t count; } object;
    } u;
};

// 解析与释放
json_value_t* json_parse(const char *json);
void          json_free(json_value_t *v);

// 类型获取
static inline json_type_t json_type(const json_value_t *v) {
    return v ? v->type : JSON_NULL;
}

// 值获取
static inline bool json_as_bool(const json_value_t *v) {
    return v && v->type == JSON_BOOL ? v->u.boolean : false;
}

static inline double json_as_number(const json_value_t *v) {
    return v && v->type == JSON_NUMBER ? v->u.number : 0.0;
}

static inline const char* json_as_string(const json_value_t *v) {
    return v && v->type == JSON_STRING ? v->u.string : "";
}

// 数组操作
static inline size_t json_array_size(const json_value_t *v) {
    return v && v->type == JSON_ARRAY ? v->u.array.count : 0;
}

static inline json_value_t* json_array_get(const json_value_t *v, size_t index) {
    if (!v || v->type != JSON_ARRAY || index >= v->u.array.count) return NULL;
    return v->u.array.items[index];
}

// 对象操作
json_value_t* json_get_object_item(const json_value_t *obj, const char *key);

static inline json_value_t* json_object_get(const json_value_t *obj, const char *key) {
    return json_get_object_item(obj, key);
}

// 序列化
char* json_print(const json_value_t *v);

static inline bool json_serialize(const json_value_t *v, char *buffer, size_t size) {
    char *result = json_print(v);
    if (!result) return false;
    strncpy(buffer, result, size - 1);
    buffer[size - 1] = '\0';
    free(result);
    return true;
}

#endif // C_UTILS_JSON_H
