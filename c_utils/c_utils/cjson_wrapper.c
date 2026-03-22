#include "cjson_wrapper.h"
#include "../thirdparty/cjson/cJSON.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char* g_last_error = NULL;

const char* cjson_error_string(cjson_wrap_error_t error) {
    switch (error) {
        case CJSON_WRAP_OK: return "Success";
        case CJSON_WRAP_ERR_NULL_PTR: return "Null pointer";
        case CJSON_WRAP_ERR_MEMORY: return "Memory allocation failed";
        case CJSON_WRAP_ERR_PARSE: return "Parse error";
        case CJSON_WRAP_ERR_TYPE: return "Type mismatch";
        case CJSON_WRAP_ERR_NOT_FOUND: return "Key not found";
        case CJSON_WRAP_ERR_BUFFER: return "Buffer error";
        default: return "Unknown error";
    }
}

cjson_value_t* cjson_parse(const char* json_string) {
    if (!json_string) {
        g_last_error = "Null input string";
        return NULL;
    }
    
    cJSON* json = cJSON_Parse(json_string);
    if (!json) {
        g_last_error = cJSON_GetErrorPtr();
        return NULL;
    }
    
    return (cjson_value_t*)json;
}

cjson_value_t* cjson_parse_with_length(const char* json_string, size_t length) {
    if (!json_string) {
        g_last_error = "Null input string";
        return NULL;
    }
    
    cJSON* json = cJSON_ParseWithLength(json_string, length);
    if (!json) {
        g_last_error = cJSON_GetErrorPtr();
        return NULL;
    }
    
    return (cjson_value_t*)json;
}

cjson_value_t* cjson_parse_file(const char* filename) {
    if (!filename) {
        g_last_error = "Null filename";
        return NULL;
    }
    
    FILE* file = fopen(filename, "rb");
    if (!file) {
        g_last_error = "Failed to open file";
        return NULL;
    }
    
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    if (size <= 0) {
        fclose(file);
        g_last_error = "Empty file";
        return NULL;
    }
    
    char* buffer = (char*)malloc(size + 1);
    if (!buffer) {
        fclose(file);
        g_last_error = "Memory allocation failed";
        return NULL;
    }
    
    size_t read_size = fread(buffer, 1, size, file);
    fclose(file);
    
    buffer[read_size] = '\0';
    
    cJSON* json = cJSON_Parse(buffer);
    free(buffer);
    
    if (!json) {
        g_last_error = cJSON_GetErrorPtr();
        return NULL;
    }
    
    return (cjson_value_t*)json;
}

const char* cjson_get_error(void) {
    return g_last_error;
}

void cjson_free(cjson_value_t* value) {
    if (value) {
        cJSON_Delete((cJSON*)value);
    }
}

char* cjson_print(const cjson_value_t* value) {
    if (!value) return NULL;
    return cJSON_Print((const cJSON*)value);
}

char* cjson_print_unformatted(const cjson_value_t* value) {
    if (!value) return NULL;
    return cJSON_PrintUnformatted((const cJSON*)value);
}

int cjson_print_to_file(const cjson_value_t* value, const char* filename) {
    if (!value || !filename) return CJSON_WRAP_ERR_NULL_PTR;
    
    char* json_str = cJSON_Print((const cJSON*)value);
    if (!json_str) return CJSON_WRAP_ERR_MEMORY;
    
    FILE* file = fopen(filename, "w");
    if (!file) {
        free(json_str);
        return CJSON_WRAP_ERR_BUFFER;
    }
    
    size_t len = strlen(json_str);
    size_t written = fwrite(json_str, 1, len, file);
    fclose(file);
    free(json_str);
    
    return (written == len) ? CJSON_WRAP_OK : CJSON_WRAP_ERR_BUFFER;
}

cjson_wrap_type_t cjson_get_type(const cjson_value_t* value) {
    if (!value) return CJSON_WRAP_TYPE_INVALID;
    
    cJSON* json = (cJSON*)value;
    
    if (cJSON_IsInvalid(json)) return CJSON_WRAP_TYPE_INVALID;
    if (cJSON_IsNull(json)) return CJSON_WRAP_TYPE_NULL;
    if (cJSON_IsBool(json)) return CJSON_WRAP_TYPE_BOOL;
    if (cJSON_IsNumber(json)) return CJSON_WRAP_TYPE_NUMBER;
    if (cJSON_IsString(json)) return CJSON_WRAP_TYPE_STRING;
    if (cJSON_IsArray(json)) return CJSON_WRAP_TYPE_ARRAY;
    if (cJSON_IsObject(json)) return CJSON_WRAP_TYPE_OBJECT;
    
    return CJSON_WRAP_TYPE_INVALID;
}

bool cjson_is_null(const cjson_value_t* value) {
    return value ? cJSON_IsNull((const cJSON*)value) : false;
}

bool cjson_is_bool(const cjson_value_t* value) {
    return value ? cJSON_IsBool((const cJSON*)value) : false;
}

bool cjson_is_number(const cjson_value_t* value) {
    return value ? cJSON_IsNumber((const cJSON*)value) : false;
}

bool cjson_is_string(const cjson_value_t* value) {
    return value ? cJSON_IsString((const cJSON*)value) : false;
}

bool cjson_is_array(const cjson_value_t* value) {
    return value ? cJSON_IsArray((const cJSON*)value) : false;
}

bool cjson_is_object(const cjson_value_t* value) {
    return value ? cJSON_IsObject((const cJSON*)value) : false;
}

cjson_value_t* cjson_create_null(void) {
    return (cjson_value_t*)cJSON_CreateNull();
}

cjson_value_t* cjson_create_bool(bool value) {
    return (cjson_value_t*)cJSON_CreateBool(value);
}

cjson_value_t* cjson_create_number(double value) {
    return (cjson_value_t*)cJSON_CreateNumber(value);
}

cjson_value_t* cjson_create_string(const char* value) {
    if (!value) return NULL;
    return (cjson_value_t*)cJSON_CreateString(value);
}

cjson_value_t* cjson_create_array(void) {
    return (cjson_value_t*)cJSON_CreateArray();
}

cjson_value_t* cjson_create_object(void) {
    return (cjson_value_t*)cJSON_CreateObject();
}

bool cjson_get_bool(const cjson_value_t* value) {
    if (!value || !cJSON_IsBool((const cJSON*)value)) return false;
    return cJSON_IsTrue((const cJSON*)value);
}

double cjson_get_number(const cjson_value_t* value) {
    if (!value || !cJSON_IsNumber((const cJSON*)value)) return 0.0;
    return ((const cJSON*)value)->valuedouble;
}

const char* cjson_get_string(const cjson_value_t* value) {
    if (!value || !cJSON_IsString((const cJSON*)value)) return NULL;
    return ((const cJSON*)value)->valuestring;
}

size_t cjson_array_size(const cjson_value_t* array) {
    if (!array || !cJSON_IsArray((const cJSON*)array)) return 0;
    return cJSON_GetArraySize((const cJSON*)array);
}

cjson_value_t* cjson_array_get(const cjson_value_t* array, size_t index) {
    if (!array || !cJSON_IsArray((const cJSON*)array)) return NULL;
    return (cjson_value_t*)cJSON_GetArrayItem((const cJSON*)array, (int)index);
}

int cjson_array_add(cjson_value_t* array, cjson_value_t* item) {
    if (!array || !item) return CJSON_WRAP_ERR_NULL_PTR;
    if (!cJSON_IsArray((cJSON*)array)) return CJSON_WRAP_ERR_TYPE;
    
    cJSON_AddItemToArray((cJSON*)array, (cJSON*)item);
    return CJSON_WRAP_OK;
}

int cjson_array_set(cjson_value_t* array, size_t index, cjson_value_t* item) {
    if (!array || !item) return CJSON_WRAP_ERR_NULL_PTR;
    if (!cJSON_IsArray((cJSON*)array)) return CJSON_WRAP_ERR_TYPE;
    
    cJSON_ReplaceItemInArray((cJSON*)array, (int)index, (cJSON*)item);
    return CJSON_WRAP_OK;
}

int cjson_array_remove(cjson_value_t* array, size_t index) {
    if (!array) return CJSON_WRAP_ERR_NULL_PTR;
    if (!cJSON_IsArray((cJSON*)array)) return CJSON_WRAP_ERR_TYPE;
    
    cJSON_DeleteItemFromArray((cJSON*)array, (int)index);
    return CJSON_WRAP_OK;
}

size_t cjson_object_size(const cjson_value_t* object) {
    if (!object || !cJSON_IsObject((const cJSON*)object)) return 0;
    return cJSON_GetArraySize((const cJSON*)object);
}

cjson_value_t* cjson_object_get(const cjson_value_t* object, const char* key) {
    if (!object || !key) return NULL;
    if (!cJSON_IsObject((const cJSON*)object)) return NULL;
    
    return (cjson_value_t*)cJSON_GetObjectItemCaseSensitive((const cJSON*)object, key);
}

bool cjson_object_has(const cjson_value_t* object, const char* key) {
    if (!object || !key) return false;
    if (!cJSON_IsObject((const cJSON*)object)) return false;
    
    return cJSON_GetObjectItemCaseSensitive((const cJSON*)object, key) != NULL;
}

int cjson_object_set(cjson_value_t* object, const char* key, cjson_value_t* value) {
    if (!object || !key || !value) return CJSON_WRAP_ERR_NULL_PTR;
    if (!cJSON_IsObject((cJSON*)object)) return CJSON_WRAP_ERR_TYPE;
    
    if (cJSON_HasObjectItem((cJSON*)object, key)) {
        cJSON_ReplaceItemInObjectCaseSensitive((cJSON*)object, key, (cJSON*)value);
    } else {
        cJSON_AddItemToObject((cJSON*)object, key, (cJSON*)value);
    }
    return CJSON_WRAP_OK;
}

int cjson_object_remove(cjson_value_t* object, const char* key) {
    if (!object || !key) return CJSON_WRAP_ERR_NULL_PTR;
    if (!cJSON_IsObject((cJSON*)object)) return CJSON_WRAP_ERR_TYPE;
    
    cJSON_DeleteItemFromObjectCaseSensitive((cJSON*)object, key);
    return CJSON_WRAP_OK;
}

bool cjson_object_get_bool(const cjson_value_t* obj, const char* key, bool default_val) {
    cjson_value_t* item = cjson_object_get(obj, key);
    if (!item || !cjson_is_bool(item)) return default_val;
    return cjson_get_bool(item);
}

double cjson_object_get_number(const cjson_value_t* obj, const char* key, double default_val) {
    cjson_value_t* item = cjson_object_get(obj, key);
    if (!item || !cjson_is_number(item)) return default_val;
    return cjson_get_number(item);
}

const char* cjson_object_get_string(const cjson_value_t* obj, const char* key, const char* default_val) {
    cjson_value_t* item = cjson_object_get(obj, key);
    if (!item || !cjson_is_string(item)) return default_val;
    return cjson_get_string(item);
}

cjson_value_t* cjson_object_get_array(const cjson_value_t* obj, const char* key) {
    cjson_value_t* item = cjson_object_get(obj, key);
    if (!item || !cjson_is_array(item)) return NULL;
    return item;
}

cjson_value_t* cjson_object_get_object(const cjson_value_t* obj, const char* key) {
    cjson_value_t* item = cjson_object_get(obj, key);
    if (!item || !cjson_is_object(item)) return NULL;
    return item;
}

int cjson_object_set_null(cjson_value_t* obj, const char* key) {
    if (!obj || !key) return CJSON_WRAP_ERR_NULL_PTR;
    if (!cJSON_IsObject((cJSON*)obj)) return CJSON_WRAP_ERR_TYPE;
    
    if (cJSON_HasObjectItem((cJSON*)obj, key)) {
        cJSON_ReplaceItemInObjectCaseSensitive((cJSON*)obj, key, cJSON_CreateNull());
    } else {
        cJSON_AddNullToObject((cJSON*)obj, key);
    }
    return CJSON_WRAP_OK;
}

int cjson_object_set_bool(cjson_value_t* obj, const char* key, bool value) {
    if (!obj || !key) return CJSON_WRAP_ERR_NULL_PTR;
    if (!cJSON_IsObject((cJSON*)obj)) return CJSON_WRAP_ERR_TYPE;
    
    if (cJSON_HasObjectItem((cJSON*)obj, key)) {
        cJSON_ReplaceItemInObjectCaseSensitive((cJSON*)obj, key, cJSON_CreateBool(value));
    } else {
        cJSON_AddBoolToObject((cJSON*)obj, key, value);
    }
    return CJSON_WRAP_OK;
}

int cjson_object_set_number(cjson_value_t* obj, const char* key, double value) {
    if (!obj || !key) return CJSON_WRAP_ERR_NULL_PTR;
    if (!cJSON_IsObject((cJSON*)obj)) return CJSON_WRAP_ERR_TYPE;
    
    if (cJSON_HasObjectItem((cJSON*)obj, key)) {
        cJSON_ReplaceItemInObjectCaseSensitive((cJSON*)obj, key, cJSON_CreateNumber(value));
    } else {
        cJSON_AddNumberToObject((cJSON*)obj, key, value);
    }
    return CJSON_WRAP_OK;
}

int cjson_object_set_string(cjson_value_t* obj, const char* key, const char* value) {
    if (!obj || !key || !value) return CJSON_WRAP_ERR_NULL_PTR;
    if (!cJSON_IsObject((cJSON*)obj)) return CJSON_WRAP_ERR_TYPE;
    
    if (cJSON_HasObjectItem((cJSON*)obj, key)) {
        cJSON_ReplaceItemInObjectCaseSensitive((cJSON*)obj, key, cJSON_CreateString(value));
    } else {
        cJSON_AddStringToObject((cJSON*)obj, key, value);
    }
    return CJSON_WRAP_OK;
}

cjson_value_t* cjson_deep_copy(const cjson_value_t* value) {
    if (!value) return NULL;
    return (cjson_value_t*)cJSON_Duplicate((const cJSON*)value, 1);
}

bool cjson_equal(const cjson_value_t* a, const cjson_value_t* b) {
    if (a == b) return true;
    if (!a || !b) return false;
    
    return cJSON_Compare((const cJSON*)a, (const cJSON*)b, 1);
}
