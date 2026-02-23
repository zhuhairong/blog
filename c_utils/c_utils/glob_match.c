#include "glob_match.h"
#include <ctype.h>

// 默认配置
glob_match_config_t glob_match_default_config(void) {
    glob_match_config_t config = {
        .flags = GLOB_MATCH_FLAG_NONE,
        .wildcard_single = '?',
        .wildcard_any = '*',
        .escape_char = '\\'
    };
    return config;
}

// 简单通配符匹配
bool glob_match_simple(const char *pattern, const char *text, glob_match_flags_t flags, glob_match_error_t *error) {
    if (!pattern || !text) {
        if (error) *error = GLOB_MATCH_ERROR_INVALID_PARAM;
        return false;
    }
    
    const char *pat = pattern;
    const char *txt = text;
    
    while (*pat) {
        if (*pat == '*') {
            while (*pat == '*') pat++;
            if (!*pat) {
                if (error) *error = GLOB_MATCH_OK;
                return true;
            }
            while (*txt) {
                if (glob_match_simple(pat, txt, flags, NULL)) {
                    if (error) *error = GLOB_MATCH_OK;
                    return true;
                }
                txt++;
            }
            if (error) *error = GLOB_MATCH_OK;
            return false;
        } else if (*pat == '?') {
            if (!*txt) {
                if (error) *error = GLOB_MATCH_OK;
                return false;
            }
            pat++;
            txt++;
        } else {
            char pc = *pat;
            char tc = *txt;
            
            if (flags & GLOB_MATCH_FLAG_CASE_INSENSITIVE) {
                pc = tolower(pc);
                tc = tolower(tc);
            }
            
            if (pc != tc) {
                if (error) *error = GLOB_MATCH_OK;
                return false;
            }
            pat++;
            txt++;
        }
    }
    
    if (error) *error = GLOB_MATCH_OK;
    return !*txt;
}

// 配置化通配符匹配
bool glob_match_config(const char *pattern, const char *text, const glob_match_config_t *config, glob_match_error_t *error) {
    if (!pattern || !text) {
        if (error) *error = GLOB_MATCH_ERROR_INVALID_PARAM;
        return false;
    }
    
    glob_match_config_t cfg = config ? *config : glob_match_default_config();
    
    const char *pat = pattern;
    const char *txt = text;
    
    while (*pat) {
        if (*pat == cfg.wildcard_any) {
            while (*pat == cfg.wildcard_any) pat++;
            if (!*pat) {
                if (error) *error = GLOB_MATCH_OK;
                return true;
            }
            while (*txt) {
                if (glob_match_config(pat, txt, &cfg, NULL)) {
                    if (error) *error = GLOB_MATCH_OK;
                    return true;
                }
                txt++;
            }
            if (error) *error = GLOB_MATCH_OK;
            return false;
        } else if (*pat == cfg.wildcard_single) {
            if (!*txt) {
                if (error) *error = GLOB_MATCH_OK;
                return false;
            }
            pat++;
            txt++;
        } else if (*pat == cfg.escape_char && *(pat+1)) {
            pat++;
            char pc = *pat;
            char tc = *txt;
            
            if (cfg.flags & GLOB_MATCH_FLAG_CASE_INSENSITIVE) {
                pc = tolower(pc);
                tc = tolower(tc);
            }
            
            if (pc != tc) {
                if (error) *error = GLOB_MATCH_OK;
                return false;
            }
            pat++;
            txt++;
        } else {
            char pc = *pat;
            char tc = *txt;
            
            if (cfg.flags & GLOB_MATCH_FLAG_CASE_INSENSITIVE) {
                pc = tolower(pc);
                tc = tolower(tc);
            }
            
            if (pc != tc) {
                if (error) *error = GLOB_MATCH_OK;
                return false;
            }
            pat++;
            txt++;
        }
    }
    
    if (error) *error = GLOB_MATCH_OK;
    return !*txt;
}

// 批量通配符匹配
bool glob_match_batch(const char **patterns, const char *text, size_t count, glob_match_flags_t flags, bool *matches, glob_match_error_t *error) {
    if (!patterns || !text || !matches || count == 0) {
        if (error) *error = GLOB_MATCH_ERROR_INVALID_PARAM;
        return false;
    }
    
    for (size_t i = 0; i < count; i++) {
        matches[i] = glob_match_simple(patterns[i], text, flags, NULL);
    }
    
    if (error) *error = GLOB_MATCH_OK;
    return true;
}

// 获取错误信息
const char* glob_match_strerror(glob_match_error_t error) {
    switch (error) {
        case GLOB_MATCH_OK: return "Success";
        case GLOB_MATCH_ERROR_INVALID_PARAM: return "Invalid parameter";
        case GLOB_MATCH_ERROR_INVALID_PATTERN: return "Invalid pattern";
        case GLOB_MATCH_ERROR_MEMORY_ALLOC: return "Memory allocation failed";
        default: return "Unknown error";
    }
}
