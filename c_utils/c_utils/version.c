#include "version.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

version_error_t version_create(version_ctx_t** ctx, const version_config_t* config) {
    if (!ctx) return VERSION_INVALID_PARAMS;
    
    *ctx = malloc(sizeof(version_ctx_t));
    if (!*ctx) return VERSION_MEMORY_ERROR;
    
    if (config) {
        (*ctx)->config = *config;
    } else {
        (*ctx)->config.allow_pre_release = true;
        (*ctx)->config.allow_build_metadata = true;
        (*ctx)->config.strict_mode = false;
        (*ctx)->config.max_version_length = 256;
    }
    
    (*ctx)->last_error = VERSION_OK;
    (*ctx)->parse_count = 0;
    (*ctx)->compare_count = 0;
    
    return VERSION_OK;
}

void version_destroy(version_ctx_t* ctx) {
    free(ctx);
}

version_error_t version_parse(version_ctx_t* ctx, const char *v_str, version_t* out) {
    if (!ctx || !v_str || !out) return VERSION_INVALID_PARAMS;
    
    // 初始化输出
    memset(out, 0, sizeof(version_t));
    
    // 解析版本号
    int parsed = sscanf(v_str, "%d.%d.%d", &out->major, &out->minor, &out->patch);
    if (parsed < 2) {
        return VERSION_INVALID_FORMAT;
    }
    
    // 如果只解析了2个数字，patch设为0
    if (parsed == 2) {
        out->patch = 0;
    }
    
    ctx->parse_count++;
    return VERSION_OK;
}

int version_compare(version_t v1, version_t v2) {
    if (v1.major != v2.major) return v1.major > v2.major ? 1 : -1;
    if (v1.minor != v2.minor) return v1.minor > v2.minor ? 1 : -1;
    if (v1.patch != v2.patch) return v1.patch > v2.patch ? 1 : -1;
    return 0;
}

version_error_t version_compare_safe(version_ctx_t* ctx, version_t v1, version_t v2, int* result) {
    if (!ctx || !result) return VERSION_INVALID_PARAMS;
    
    *result = version_compare(v1, v2);
    ctx->compare_count++;
    return VERSION_OK;
}

version_error_t version_to_string(version_ctx_t* ctx, const version_t* v, char* out, size_t out_size) {
    if (!ctx || !v || !out || out_size == 0) return VERSION_INVALID_PARAMS;
    
    int written = snprintf(out, out_size, "%d.%d.%d", v->major, v->minor, v->patch);
    if (written < 0 || (size_t)written >= out_size) {
        return VERSION_MEMORY_ERROR;
    }
    
    return VERSION_OK;
}

version_error_t version_increment(version_ctx_t* ctx, version_t* v, int part) {
    if (!ctx || !v) return VERSION_INVALID_PARAMS;
    
    switch (part) {
        case 0: // major
            v->major++;
            v->minor = 0;
            v->patch = 0;
            break;
        case 1: // minor
            v->minor++;
            v->patch = 0;
            break;
        case 2: // patch
            v->patch++;
            break;
        default:
            return VERSION_INVALID_PARAMS;
    }
    
    return VERSION_OK;
}

bool version_is_valid(const version_t* v) {
    if (!v) return false;
    return v->major >= 0 && v->minor >= 0 && v->patch >= 0;
}

version_error_t version_copy(version_t* dest, const version_t* src) {
    if (!dest || !src) return VERSION_INVALID_PARAMS;
    
    *dest = *src;
    
    // 复制动态分配的字符串
    if (src->pre_release) {
        dest->pre_release = strdup(src->pre_release);
        if (!dest->pre_release) return VERSION_MEMORY_ERROR;
    }
    
    if (src->build_metadata) {
        dest->build_metadata = strdup(src->build_metadata);
        if (!dest->build_metadata) {
            free(dest->pre_release);
            return VERSION_MEMORY_ERROR;
        }
    }
    
    return VERSION_OK;
}

void version_free(version_t* v) {
    if (!v) return;
    free(v->pre_release);
    free(v->build_metadata);
    v->pre_release = NULL;
    v->build_metadata = NULL;
}

version_error_t version_get_last_error(version_ctx_t* ctx) {
    return ctx ? ctx->last_error : VERSION_INVALID_PARAMS;
}

const char* version_strerror(version_error_t error) {
    switch (error) {
        case VERSION_OK: return "Success";
        case VERSION_INVALID_PARAMS: return "Invalid parameters";
        case VERSION_INVALID_FORMAT: return "Invalid format";
        case VERSION_OUT_OF_RANGE: return "Out of range";
        case VERSION_MEMORY_ERROR: return "Memory error";
        case VERSION_COMPARE_ERROR: return "Compare error";
        default: return "Unknown error";
    }
}
