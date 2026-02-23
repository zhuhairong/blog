#include "z_algorithm.h"
#include <stdlib.h>
#include <string.h>

z_algorithm_error_t z_algorithm_create(z_algorithm_ctx_t** ctx, const z_algorithm_config_t* config) {
    if (!ctx) return Z_ALGORITHM_INVALID_PARAMS;
    
    z_algorithm_ctx_t* c = (z_algorithm_ctx_t*)malloc(sizeof(z_algorithm_ctx_t));
    if (!c) return Z_ALGORITHM_MEMORY_ERROR;
    
    if (config) {
        c->config = *config;
    } else {
        c->config.use_optimization = true;
        c->config.check_bounds = true;
        c->config.allow_empty_string = false;
        c->config.max_string_length = 1024 * 1024;
        c->config.max_pattern_length = 1024;
    }
    c->last_error = Z_ALGORITHM_OK;
    c->compute_count = 0;
    c->match_count = 0;
    
    *ctx = c;
    return Z_ALGORITHM_OK;
}

void z_algorithm_destroy(z_algorithm_ctx_t* ctx) {
    if (ctx) free(ctx);
}

void z_algorithm_compute(const char *s, int *z) {
    if (!s || !z) return;
    
    int n = (int)strlen(s);
    int l = 0, r = 0;
    z[0] = 0;
    
    for (int i = 1; i < n; i++) {
        if (i <= r) {
            z[i] = (z[i - l] < r - i + 1) ? z[i - l] : r - i + 1;
        } else {
            z[i] = 0;
        }
        while (i + z[i] < n && s[z[i]] == s[i + z[i]]) {
            z[i]++;
        }
        if (i + z[i] - 1 > r) {
            l = i;
            r = i + z[i] - 1;
        }
    }
}

z_algorithm_error_t z_algorithm_compute_safe(z_algorithm_ctx_t* ctx, const char *s, size_t s_len, int *z, size_t z_size) {
    if (!ctx || !s || !z) {
        if (ctx) ctx->last_error = Z_ALGORITHM_INVALID_PARAMS;
        return Z_ALGORITHM_INVALID_PARAMS;
    }
    
    if (!ctx->config.allow_empty_string && s_len == 0) {
        ctx->last_error = Z_ALGORITHM_INVALID_PARAMS;
        return Z_ALGORITHM_INVALID_PARAMS;
    }
    
    if (s_len > ctx->config.max_string_length) {
        ctx->last_error = Z_ALGORITHM_STRING_TOO_LONG;
        return Z_ALGORITHM_STRING_TOO_LONG;
    }
    
    if (z_size < s_len) {
        ctx->last_error = Z_ALGORITHM_BUFFER_TOO_SMALL;
        return Z_ALGORITHM_BUFFER_TOO_SMALL;
    }
    
    int l = 0, r = 0;
    z[0] = 0;
    
    for (size_t i = 1; i < s_len; i++) {
        if ((int)i <= r) {
            z[i] = (z[i - l] < r - (int)i + 1) ? z[i - l] : r - (int)i + 1;
        } else {
            z[i] = 0;
        }
        while (i + z[i] < s_len && s[z[i]] == s[i + z[i]]) {
            z[i]++;
        }
        if ((int)i + z[i] - 1 > r) {
            l = (int)i;
            r = (int)i + z[i] - 1;
        }
    }
    
    ctx->compute_count++;
    ctx->last_error = Z_ALGORITHM_OK;
    return Z_ALGORITHM_OK;
}

z_algorithm_error_t z_algorithm_match(z_algorithm_ctx_t* ctx, const char *text, size_t text_len, 
                                       const char *pattern, size_t pattern_len,
                                       size_t *matches, size_t max_matches, size_t *match_count) {
    if (!ctx || !text || !pattern || !matches || !match_count) {
        if (ctx) ctx->last_error = Z_ALGORITHM_INVALID_PARAMS;
        return Z_ALGORITHM_INVALID_PARAMS;
    }
    
    *match_count = 0;
    
    if (pattern_len == 0 || text_len == 0 || pattern_len > text_len) {
        ctx->last_error = Z_ALGORITHM_MATCH_NOT_FOUND;
        return Z_ALGORITHM_MATCH_NOT_FOUND;
    }
    
    if (pattern_len > ctx->config.max_pattern_length) {
        ctx->last_error = Z_ALGORITHM_STRING_TOO_LONG;
        return Z_ALGORITHM_STRING_TOO_LONG;
    }
    
    size_t concat_len = pattern_len + 1 + text_len;
    char* concat = (char*)malloc(concat_len + 1);
    if (!concat) {
        ctx->last_error = Z_ALGORITHM_MEMORY_ERROR;
        return Z_ALGORITHM_MEMORY_ERROR;
    }
    
    memcpy(concat, pattern, pattern_len);
    concat[pattern_len] = '$';
    memcpy(concat + pattern_len + 1, text, text_len);
    concat[concat_len] = '\0';
    
    int* z = (int*)malloc(concat_len * sizeof(int));
    if (!z) {
        free(concat);
        ctx->last_error = Z_ALGORITHM_MEMORY_ERROR;
        return Z_ALGORITHM_MEMORY_ERROR;
    }
    
    z_algorithm_compute(concat, z);
    
    for (size_t i = pattern_len + 1; i < concat_len; i++) {
        if ((size_t)z[i] == pattern_len) {
            if (*match_count < max_matches) {
                matches[*match_count] = i - pattern_len - 1;
            }
            (*match_count)++;
        }
    }
    
    free(z);
    free(concat);
    
    ctx->match_count += *match_count;
    ctx->last_error = (*match_count > 0) ? Z_ALGORITHM_OK : Z_ALGORITHM_MATCH_NOT_FOUND;
    return ctx->last_error;
}

z_algorithm_error_t z_algorithm_compute_batch(z_algorithm_ctx_t* ctx, const char** strings, 
                                               size_t* lengths, size_t count,
                                               int** z_arrays, size_t* z_sizes) {
    if (!ctx || !strings || !lengths || !z_arrays || !z_sizes) {
        if (ctx) ctx->last_error = Z_ALGORITHM_INVALID_PARAMS;
        return Z_ALGORITHM_INVALID_PARAMS;
    }
    
    for (size_t i = 0; i < count; i++) {
        z_sizes[i] = lengths[i];
        z_arrays[i] = (int*)malloc(lengths[i] * sizeof(int));
        if (!z_arrays[i]) {
            for (size_t j = 0; j < i; j++) {
                free(z_arrays[j]);
            }
            ctx->last_error = Z_ALGORITHM_MEMORY_ERROR;
            return Z_ALGORITHM_MEMORY_ERROR;
        }
        
        z_algorithm_error_t err = z_algorithm_compute_safe(ctx, strings[i], lengths[i], 
                                                           z_arrays[i], z_sizes[i]);
        if (err != Z_ALGORITHM_OK) {
            for (size_t j = 0; j <= i; j++) {
                free(z_arrays[j]);
            }
            return err;
        }
    }
    
    return Z_ALGORITHM_OK;
}

size_t z_algorithm_lcp(const char *s1, const char *s2) {
    if (!s1 || !s2) return 0;
    
    size_t len = 0;
    while (s1[len] && s2[len] && s1[len] == s2[len]) {
        len++;
    }
    return len;
}

z_algorithm_error_t z_algorithm_find_all_matches(z_algorithm_ctx_t* ctx, const char *text, 
                                                  size_t text_len, const char *pattern, 
                                                  size_t pattern_len, size_t** matches, 
                                                  size_t* match_count) {
    if (!ctx || !text || !pattern || !matches || !match_count) {
        if (ctx) ctx->last_error = Z_ALGORITHM_INVALID_PARAMS;
        return Z_ALGORITHM_INVALID_PARAMS;
    }
    
    *matches = NULL;
    *match_count = 0;
    
    if (pattern_len == 0 || text_len == 0 || pattern_len > text_len) {
        ctx->last_error = Z_ALGORITHM_MATCH_NOT_FOUND;
        return Z_ALGORITHM_MATCH_NOT_FOUND;
    }
    
    size_t max_possible = text_len - pattern_len + 1;
    size_t* temp_matches = (size_t*)malloc(max_possible * sizeof(size_t));
    if (!temp_matches) {
        ctx->last_error = Z_ALGORITHM_MEMORY_ERROR;
        return Z_ALGORITHM_MEMORY_ERROR;
    }
    
    size_t count = 0;
    z_algorithm_error_t err = z_algorithm_match(ctx, text, text_len, pattern, pattern_len,
                                                 temp_matches, max_possible, &count);
    
    if (err == Z_ALGORITHM_OK && count > 0) {
        *matches = (size_t*)malloc(count * sizeof(size_t));
        if (!*matches) {
            free(temp_matches);
            ctx->last_error = Z_ALGORITHM_MEMORY_ERROR;
            return Z_ALGORITHM_MEMORY_ERROR;
        }
        memcpy(*matches, temp_matches, count * sizeof(size_t));
        *match_count = count;
    }
    
    free(temp_matches);
    return err;
}

z_algorithm_error_t z_algorithm_get_last_error(z_algorithm_ctx_t* ctx) {
    return ctx ? ctx->last_error : Z_ALGORITHM_INVALID_PARAMS;
}

const char* z_algorithm_strerror(z_algorithm_error_t error) {
    switch (error) {
        case Z_ALGORITHM_OK: return "Success";
        case Z_ALGORITHM_INVALID_PARAMS: return "Invalid parameters";
        case Z_ALGORITHM_MEMORY_ERROR: return "Memory allocation error";
        case Z_ALGORITHM_BUFFER_TOO_SMALL: return "Buffer too small";
        case Z_ALGORITHM_STRING_TOO_LONG: return "String too long";
        case Z_ALGORITHM_MATCH_NOT_FOUND: return "Match not found";
        default: return "Unknown error";
    }
}
