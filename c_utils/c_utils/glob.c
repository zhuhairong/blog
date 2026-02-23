#include "glob.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

// 默认 Glob 配置
glob_config_t glob_default_config(void) {
    glob_config_t config = {
        .flags = GLOB_FLAG_NONE,
        .max_matches = 1000,
        .recursive = false,
        .follow_symlinks = false
    };
    return config;
}

// 字符串匹配
bool glob_match(const char *pattern, const char *text, glob_flags_t flags, glob_error_t *error) {
    if (!pattern || !text) {
        if (error) *error = GLOB_ERROR_INVALID_PARAM;
        return false;
    }

    const char *pat = pattern;
    const char *txt = text;

    while (*pat) {
        if (*pat == '*') {
            while (*pat == '*') pat++;
            if (!*pat) {
                if (error) *error = GLOB_OK;
                return true;
            }
            while (*txt) {
                if (glob_match(pat, txt, flags, NULL)) {
                    if (error) *error = GLOB_OK;
                    return true;
                }
                txt++;
            }
            if (error) *error = GLOB_OK;
            return false;
        } else if (*pat == '?') {
            if (!*txt) {
                if (error) *error = GLOB_OK;
                return false;
            }
            pat++;
            txt++;
        } else if (*pat == '[') {
            // 字符类匹配
            pat++;
            bool negate = false;
            if (*pat == '!') {
                negate = true;
                pat++;
            }
            bool matched = false;
            while (*pat && *pat != ']') {
                if (*(pat + 1) == '-' && *(pat + 2) && *(pat + 2) != ']') {
                    char start = *pat;
                    char end = *(pat + 2);
                    if ((*txt >= start && *txt <= end) ||
                        ((flags & GLOB_FLAG_CASE_INSENSITIVE) &&
                         (tolower((unsigned char)*txt) >= tolower((unsigned char)start) &&
                          tolower((unsigned char)*txt) <= tolower((unsigned char)end)))) {
                        matched = true;
                    }
                    pat += 3;
                } else {
                    if (*pat == *txt ||
                        ((flags & GLOB_FLAG_CASE_INSENSITIVE) &&
                         tolower((unsigned char)*pat) == tolower((unsigned char)*txt))) {
                        matched = true;
                    }
                    pat++;
                }
            }
            if (*pat == ']') pat++;
            if (matched == negate) {
                if (error) *error = GLOB_OK;
                return false;
            }
            txt++;
        } else {
            char pc = *pat;
            char tc = *txt;

            if (flags & GLOB_FLAG_CASE_INSENSITIVE) {
                pc = tolower((unsigned char)pc);
                tc = tolower((unsigned char)tc);
            }

            if (pc != tc) {
                if (error) *error = GLOB_OK;
                return false;
            }
            pat++;
            txt++;
        }
    }

    if (error) *error = GLOB_OK;
    return !*txt;
}

// 辅助函数：添加匹配结果
static bool glob_add_result(glob_t *result, const char *path, glob_error_t *error) {
    if (!result || !path) {
        if (error) *error = GLOB_ERROR_INVALID_PARAM;
        return false;
    }

    char **new_pathv = realloc(result->gl_pathv, sizeof(char*) * (result->gl_pathc + 1));
    if (!new_pathv) {
        if (error) *error = GLOB_ERROR_MEMORY_ALLOC;
        return false;
    }

    result->gl_pathv = new_pathv;
    result->gl_pathv[result->gl_pathc] = strdup(path);
    if (!result->gl_pathv[result->gl_pathc]) {
        if (error) *error = GLOB_ERROR_MEMORY_ALLOC;
        return false;
    }
    result->gl_pathc++;

    if (error) *error = GLOB_OK;
    return true;
}

// 辅助函数：递归扫描目录
static bool glob_scan_dir(const char *dir_path, const char *pattern, const glob_config_t *config,
                          glob_t *result, glob_error_t *error) {
    DIR *dir = opendir(dir_path);
    if (!dir) {
        if (error) *error = GLOB_OK; // 目录不存在不算错误
        return true;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // 检查最大匹配数
        if (config->max_matches > 0 && result->gl_pathc >= config->max_matches) {
            break;
        }

        char full_path[4096];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

        struct stat st;
        if (stat(full_path, &st) != 0) {
            continue;
        }

        bool is_dir = S_ISDIR(st.st_mode);

        // 匹配文件名
        if (glob_match(pattern, entry->d_name, config->flags, NULL)) {
            if (!(config->flags & GLOB_FLAG_ONLYDIR) || is_dir) {
                char result_path[4096];
                if (config->flags & GLOB_FLAG_MARK && is_dir) {
                    snprintf(result_path, sizeof(result_path), "%s/", full_path);
                } else {
                    strncpy(result_path, full_path, sizeof(result_path) - 1);
                    result_path[sizeof(result_path) - 1] = '\0';
                }
                glob_add_result(result, result_path, NULL);
            }
        }

        // 递归处理子目录
        if (is_dir && config->recursive) {
            if (config->follow_symlinks || !S_ISLNK(st.st_mode)) {
                glob_scan_dir(full_path, pattern, config, result, NULL);
            }
        }
    }

    closedir(dir);
    if (error) *error = GLOB_OK;
    return true;
}

// 文件系统匹配
bool glob(const char *pattern, const glob_config_t *config, glob_t *result, glob_error_t *error) {
    if (!pattern || !result) {
        if (error) *error = GLOB_ERROR_INVALID_PARAM;
        return false;
    }

    glob_config_t cfg = config ? *config : glob_default_config();

    // 初始化结果
    if (!(cfg.flags & GLOB_FLAG_APPEND)) {
        memset(result, 0, sizeof(glob_t));
    }

    // 解析模式，提取目录和文件名模式
    char dir_part[4096] = ".";
    char file_pattern[256];

    const char *last_slash = strrchr(pattern, '/');
    if (last_slash) {
        size_t dir_len = last_slash - pattern;
        if (dir_len > 0) {
            strncpy(dir_part, pattern, dir_len);
            dir_part[dir_len] = '\0';
        } else {
            strcpy(dir_part, "/");
        }
        strncpy(file_pattern, last_slash + 1, sizeof(file_pattern) - 1);
        file_pattern[sizeof(file_pattern) - 1] = '\0';
    } else {
        strncpy(file_pattern, pattern, sizeof(file_pattern) - 1);
        file_pattern[sizeof(file_pattern) - 1] = '\0';
    }

    // 扫描目录
    glob_scan_dir(dir_part, file_pattern, &cfg, result, error);

    if (error) *error = GLOB_OK;
    return true;
}

// 释放 Glob 结果
void glob_free(glob_t *result) {
    if (!result) return;

    for (size_t i = 0; i < result->gl_pathc; i++) {
        free(result->gl_pathv[i]);
    }
    free(result->gl_pathv);

    result->gl_pathv = NULL;
    result->gl_pathc = 0;
}

// 批量匹配
bool glob_match_batch(const char **patterns, const char *text, size_t count, glob_flags_t flags, bool *matches, glob_error_t *error) {
    if (!patterns || !text || !matches || count == 0) {
        if (error) *error = GLOB_ERROR_INVALID_PARAM;
        return false;
    }

    for (size_t i = 0; i < count; i++) {
        matches[i] = glob_match(patterns[i], text, flags, NULL);
    }

    if (error) *error = GLOB_OK;
    return true;
}

// 检查模式是否有效
bool glob_validate_pattern(const char *pattern, glob_flags_t flags, glob_error_t *error) {
    (void)flags;
    if (!pattern) {
        if (error) *error = GLOB_ERROR_INVALID_PARAM;
        return false;
    }

    // 简单验证：检查括号是否匹配
    int bracket_depth = 0;
    const char *p = pattern;

    while (*p) {
        if (*p == '[') {
            bracket_depth++;
        } else if (*p == ']') {
            bracket_depth--;
            if (bracket_depth < 0) {
                if (error) *error = GLOB_ERROR_INVALID_PATTERN;
                return false;
            }
        }
        p++;
    }

    if (bracket_depth != 0) {
        if (error) *error = GLOB_ERROR_INVALID_PATTERN;
        return false;
    }

    if (error) *error = GLOB_OK;
    return true;
}

// 获取错误信息
const char* glob_strerror(glob_error_t error) {
    switch (error) {
        case GLOB_OK: return "Success";
        case GLOB_ERROR_INVALID_PARAM: return "Invalid parameter";
        case GLOB_ERROR_INVALID_PATTERN: return "Invalid pattern";
        case GLOB_ERROR_MEMORY_ALLOC: return "Memory allocation failed";
        case GLOB_ERROR_BUFFER_TOO_SMALL: return "Buffer too small";
        default: return "Unknown error";
    }
}
