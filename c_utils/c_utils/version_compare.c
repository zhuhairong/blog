#include "version_compare.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

version_compare_error_t version_compare_create(version_compare_ctx_t** ctx, const version_compare_config_t* config) {
    if (!ctx) return VERSION_COMPARE_INVALID_PARAMS;
    
    version_compare_ctx_t* c = (version_compare_ctx_t*)malloc(sizeof(version_compare_ctx_t));
    if (!c) return VERSION_COMPARE_MEMORY_ERROR;
    
    if (config) {
        c->config = *config;
    } else {
        c->config.strict_mode = false;
        c->config.allow_pre_release = true;
        c->config.allow_build_metadata = true;
        c->config.use_semver_rules = true;
        c->config.max_version_length = 256;
    }
    c->last_error = VERSION_COMPARE_OK;
    c->compare_count = 0;
    c->parse_count = 0;
    
    *ctx = c;
    return VERSION_COMPARE_OK;
}

void version_compare_destroy(version_compare_ctx_t* ctx) {
    if (ctx) free(ctx);
}

static int parse_version(const char* v, int* major, int* minor, int* patch, char* prerelease, size_t prerelease_size) {
    *major = 0;
    *minor = 0;
    *patch = 0;
    if (prerelease && prerelease_size > 0) prerelease[0] = '\0';
    
    if (!v) return -1;
    
    const char* p = v;
    while (*p && !isdigit((unsigned char)*p)) p++;
    
    int count = sscanf(p, "%d.%d.%d", major, minor, patch);
    if (count < 1) return -1;
    if (count < 2) *minor = 0;
    if (count < 3) *patch = 0;
    
    if (prerelease && prerelease_size > 0) {
        const char* dash = strchr(v, '-');
        if (dash) {
            size_t len = strlen(dash + 1);
            if (len >= prerelease_size) len = prerelease_size - 1;
            strncpy(prerelease, dash + 1, len);
            prerelease[len] = '\0';
            
            char* plus = strchr(prerelease, '+');
            if (plus) *plus = '\0';
        }
    }
    
    return 0;
}

static int compare_prerelease(const char* pre1, const char* pre2) {
    if (!pre1[0] && !pre2[0]) return 0;
    if (!pre1[0]) return 1;
    if (!pre2[0]) return -1;
    
    return strcmp(pre1, pre2);
}

int version_compare_str(const char *v1, const char *v2) {
    int major1 = 0, minor1 = 0, patch1 = 0;
    int major2 = 0, minor2 = 0, patch2 = 0;
    
    parse_version(v1, &major1, &minor1, &patch1, NULL, 0);
    parse_version(v2, &major2, &minor2, &patch2, NULL, 0);
    
    if (major1 != major2) return (major1 < major2) ? -1 : 1;
    if (minor1 != minor2) return (minor1 < minor2) ? -1 : 1;
    if (patch1 != patch2) return (patch1 < patch2) ? -1 : 1;
    return 0;
}

version_compare_error_t version_compare_str_safe(version_compare_ctx_t* ctx, const char *v1, const char *v2, int* result) {
    if (!ctx || !v1 || !v2 || !result) {
        if (ctx) ctx->last_error = VERSION_COMPARE_INVALID_PARAMS;
        return VERSION_COMPARE_INVALID_PARAMS;
    }
    
    if (strlen(v1) > ctx->config.max_version_length || strlen(v2) > ctx->config.max_version_length) {
        ctx->last_error = VERSION_COMPARE_INVALID_FORMAT;
        return VERSION_COMPARE_INVALID_FORMAT;
    }
    
    int major1, minor1, patch1;
    int major2, minor2, patch2;
    char pre1[64] = {0}, pre2[64] = {0};
    
    if (parse_version(v1, &major1, &minor1, &patch1, pre1, sizeof(pre1)) < 0 ||
        parse_version(v2, &major2, &minor2, &patch2, pre2, sizeof(pre2)) < 0) {
        ctx->last_error = VERSION_COMPARE_INVALID_FORMAT;
        return VERSION_COMPARE_INVALID_FORMAT;
    }
    
    if (major1 != major2) *result = (major1 < major2) ? -1 : 1;
    else if (minor1 != minor2) *result = (minor1 < minor2) ? -1 : 1;
    else if (patch1 != patch2) *result = (patch1 < patch2) ? -1 : 1;
    else *result = compare_prerelease(pre1, pre2);
    
    ctx->compare_count++;
    ctx->last_error = VERSION_COMPARE_OK;
    return VERSION_COMPARE_OK;
}

version_compare_error_t version_compare_batch(version_compare_ctx_t* ctx, const char** versions, size_t count, int* results) {
    if (!ctx || !versions || !results || count < 2) {
        if (ctx) ctx->last_error = VERSION_COMPARE_INVALID_PARAMS;
        return VERSION_COMPARE_INVALID_PARAMS;
    }
    
    for (size_t i = 0; i < count - 1; i++) {
        version_compare_error_t err = version_compare_str_safe(ctx, versions[i], versions[i + 1], &results[i]);
        if (err != VERSION_COMPARE_OK) return err;
    }
    
    return VERSION_COMPARE_OK;
}

bool version_compare_in_range(version_compare_ctx_t* ctx, const char* version, const char* range) {
    if (!ctx || !version || !range) return false;
    
    char range_copy[256];
    strncpy(range_copy, range, sizeof(range_copy) - 1);
    range_copy[sizeof(range_copy) - 1] = '\0';
    
    char* dash = strchr(range_copy, '-');
    if (dash) {
        *dash = '\0';
        int cmp1, cmp2;
        if (version_compare_str_safe(ctx, version, range_copy, &cmp1) != VERSION_COMPARE_OK) return false;
        if (version_compare_str_safe(ctx, version, dash + 1, &cmp2) != VERSION_COMPARE_OK) return false;
        return cmp1 >= 0 && cmp2 <= 0;
    }
    
    if (range_copy[0] == '>' || range_copy[0] == '<' || range_copy[0] == '=') {
        char op = range_copy[0];
        const char* v = range_copy + 1;
        if (range_copy[1] == '=') { v++; }
        
        int cmp;
        if (version_compare_str_safe(ctx, version, v, &cmp) != VERSION_COMPARE_OK) return false;
        
        switch (op) {
            case '>': return cmp > 0;
            case '<': return cmp < 0;
            case '=': return cmp == 0;
        }
    }
    
    int cmp;
    if (version_compare_str_safe(ctx, version, range_copy, &cmp) != VERSION_COMPARE_OK) return false;
    return cmp == 0;
}

bool version_compare_validate(version_compare_ctx_t* ctx, const char* version) {
    if (!ctx || !version) return false;
    
    if (strlen(version) > ctx->config.max_version_length) return false;
    
    int major, minor, patch;
    return parse_version(version, &major, &minor, &patch, NULL, 0) == 0;
}

static int compare_strings(const void* a, const void* b) {
    return version_compare_str(*(const char**)a, *(const char**)b);
}

static int compare_strings_desc(const void* a, const void* b) {
    return version_compare_str(*(const char**)b, *(const char**)a);
}

version_compare_error_t version_compare_sort(version_compare_ctx_t* ctx, char** versions, size_t count, bool ascending) {
    if (!ctx || !versions || count == 0) {
        if (ctx) ctx->last_error = VERSION_COMPARE_INVALID_PARAMS;
        return VERSION_COMPARE_INVALID_PARAMS;
    }
    
    qsort(versions, count, sizeof(char*), ascending ? compare_strings : compare_strings_desc);
    
    ctx->last_error = VERSION_COMPARE_OK;
    return VERSION_COMPARE_OK;
}

version_compare_error_t version_compare_get_last_error(version_compare_ctx_t* ctx) {
    return ctx ? ctx->last_error : VERSION_COMPARE_INVALID_PARAMS;
}

const char* version_compare_strerror(version_compare_error_t error) {
    switch (error) {
        case VERSION_COMPARE_OK: return "Success";
        case VERSION_COMPARE_INVALID_PARAMS: return "Invalid parameters";
        case VERSION_COMPARE_INVALID_FORMAT: return "Invalid version format";
        case VERSION_COMPARE_MEMORY_ERROR: return "Memory allocation error";
        case VERSION_COMPARE_UNSUPPORTED_FORMAT: return "Unsupported version format";
        default: return "Unknown error";
    }
}
