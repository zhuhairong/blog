#ifndef C_UTILS_CJSON_WRAPPER_H
#define C_UTILS_CJSON_WRAPPER_H

#include <stddef.h>
#include <stdbool.h>

typedef enum {
    CJSON_WRAP_OK = 0,
    CJSON_WRAP_ERR_NULL_PTR = -1,
    CJSON_WRAP_ERR_MEMORY = -2,
    CJSON_WRAP_ERR_PARSE = -3,
    CJSON_WRAP_ERR_TYPE = -4,
    CJSON_WRAP_ERR_NOT_FOUND = -5,
    CJSON_WRAP_ERR_BUFFER = -6,
} cjson_wrap_error_t;

typedef enum {
    CJSON_WRAP_TYPE_INVALID = 0,
    CJSON_WRAP_TYPE_NULL,
    CJSON_WRAP_TYPE_BOOL,
    CJSON_WRAP_TYPE_NUMBER,
    CJSON_WRAP_TYPE_STRING,
    CJSON_WRAP_TYPE_ARRAY,
    CJSON_WRAP_TYPE_OBJECT,
} cjson_wrap_type_t;

typedef struct cjson_value cjson_value_t;

const char* cjson_error_string(cjson_wrap_error_t error);

cjson_value_t* cjson_parse(const char* json_string);
cjson_value_t* cjson_parse_with_length(const char* json_string, size_t length);
cjson_value_t* cjson_parse_file(const char* filename);
const char* cjson_get_error(void);

void cjson_free(cjson_value_t* value);

char* cjson_print(const cjson_value_t* value);
char* cjson_print_unformatted(const cjson_value_t* value);
int cjson_print_to_file(const cjson_value_t* value, const char* filename);

cjson_wrap_type_t cjson_get_type(const cjson_value_t* value);
bool cjson_is_null(const cjson_value_t* value);
bool cjson_is_bool(const cjson_value_t* value);
bool cjson_is_number(const cjson_value_t* value);
bool cjson_is_string(const cjson_value_t* value);
bool cjson_is_array(const cjson_value_t* value);
bool cjson_is_object(const cjson_value_t* value);

cjson_value_t* cjson_create_null(void);
cjson_value_t* cjson_create_bool(bool value);
cjson_value_t* cjson_create_number(double value);
cjson_value_t* cjson_create_string(const char* value);
cjson_value_t* cjson_create_array(void);
cjson_value_t* cjson_create_object(void);

bool cjson_get_bool(const cjson_value_t* value);
double cjson_get_number(const cjson_value_t* value);
const char* cjson_get_string(const cjson_value_t* value);

size_t cjson_array_size(const cjson_value_t* array);
cjson_value_t* cjson_array_get(const cjson_value_t* array, size_t index);
int cjson_array_add(cjson_value_t* array, cjson_value_t* item);
int cjson_array_set(cjson_value_t* array, size_t index, cjson_value_t* item);
int cjson_array_remove(cjson_value_t* array, size_t index);

size_t cjson_object_size(const cjson_value_t* object);
cjson_value_t* cjson_object_get(const cjson_value_t* object, const char* key);
bool cjson_object_has(const cjson_value_t* object, const char* key);
int cjson_object_set(cjson_value_t* object, const char* key, cjson_value_t* value);
int cjson_object_remove(cjson_value_t* object, const char* key);

bool cjson_object_get_bool(const cjson_value_t* obj, const char* key, bool default_val);
double cjson_object_get_number(const cjson_value_t* obj, const char* key, double default_val);
const char* cjson_object_get_string(const cjson_value_t* obj, const char* key, const char* default_val);
cjson_value_t* cjson_object_get_array(const cjson_value_t* obj, const char* key);
cjson_value_t* cjson_object_get_object(const cjson_value_t* obj, const char* key);

int cjson_object_set_null(cjson_value_t* obj, const char* key);
int cjson_object_set_bool(cjson_value_t* obj, const char* key, bool value);
int cjson_object_set_number(cjson_value_t* obj, const char* key, double value);
int cjson_object_set_string(cjson_value_t* obj, const char* key, const char* value);

cjson_value_t* cjson_deep_copy(const cjson_value_t* value);

bool cjson_equal(const cjson_value_t* a, const cjson_value_t* b);

#endif
