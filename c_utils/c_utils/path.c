#include "path.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef _WIN32
    #define PATH_SEP '\\'
#else
    #define PATH_SEP '/'
#endif

path_config_t path_default_config(void) {
    path_config_t config = {
        .normalize = true,
        .resolve_symlinks = false,
        .case_sensitive = true,
        .separator = PATH_SEP
    };
    return config;
}

char path_get_separator(void) {
    return PATH_SEP;
}

char* path_join(const char *p1, const char *p2) {
    if (!p1 && !p2) return NULL;
    if (!p1) return strdup(p2);
    if (!p2) return strdup(p1);
    
    size_t l1 = strlen(p1);
    size_t l2 = strlen(p2);
    bool need_sep = (l1 > 0 && p1[l1-1] != PATH_SEP && p2[0] != PATH_SEP);
    
    char *res = malloc(l1 + l2 + (need_sep ? 2 : 1));
    if (!res) return NULL;
    
    strcpy(res, p1);
    if (need_sep) {
        res[l1] = PATH_SEP;
        strcpy(res + l1 + 1, p2);
    } else {
        strcpy(res + l1, p2);
    }
    return res;
}

char* path_join_ex(const char *p1, const char *p2, const path_config_t *config, path_error_t *error) {
    if (error) *error = PATH_OK;
    char* result = path_join(p1, p2);
    if (!result && error) *error = PATH_ERROR_MEMORY_ALLOC;
    return result;
}

char* path_join_multiple(const char **paths, size_t count, path_error_t *error) {
    if (!paths || count == 0) {
        if (error) *error = PATH_ERROR_NULL_PTR;
        return NULL;
    }
    
    char* result = strdup(paths[0]);
    if (!result) {
        if (error) *error = PATH_ERROR_MEMORY_ALLOC;
        return NULL;
    }
    
    for (size_t i = 1; i < count; i++) {
        char* temp = path_join(result, paths[i]);
        free(result);
        if (!temp) {
            if (error) *error = PATH_ERROR_MEMORY_ALLOC;
            return NULL;
        }
        result = temp;
    }
    
    if (error) *error = PATH_OK;
    return result;
}

const char* path_basename(const char *path) {
    if (!path) return NULL;
    
    const char *p = strrchr(path, PATH_SEP);
#ifdef _WIN32
    const char *p2 = strrchr(path, '/');
    if (p2 > p) p = p2;
#endif
    return p ? p + 1 : path;
}

char* path_basename_no_ext(const char *path, path_error_t *error) {
    const char* base = path_basename(path);
    if (!base) {
        if (error) *error = PATH_ERROR_NULL_PTR;
        return NULL;
    }
    
    const char* ext = strrchr(base, '.');
    if (!ext || ext == base) {
        if (error) *error = PATH_OK;
        return strdup(base);
    }
    
    size_t len = ext - base;
    char* result = malloc(len + 1);
    if (!result) {
        if (error) *error = PATH_ERROR_MEMORY_ALLOC;
        return NULL;
    }
    
    memcpy(result, base, len);
    result[len] = '\0';
    if (error) *error = PATH_OK;
    return result;
}

char* path_dirname(const char *path) {
    if (!path) return NULL;
    
    const char *base = path_basename(path);
    if (base == path) return strdup(".");
    
    size_t len = base - path;
    while (len > 1 && (path[len-1] == PATH_SEP || path[len-1] == '/')) len--;
    
    char *res = malloc(len + 1);
    if (!res) return NULL;
    
    memcpy(res, path, len);
    res[len] = '\0';
    return res;
}

char* path_dirname_ex(const char *path, const path_config_t *config, path_error_t *error) {
    (void)config;
    if (error) *error = PATH_OK;
    char* result = path_dirname(path);
    if (!result && error) *error = PATH_ERROR_MEMORY_ALLOC;
    return result;
}

bool path_is_absolute(const char *path) {
    if (!path || !*path) return false;
#ifdef _WIN32
    return (path[0] != '\0' && path[1] == ':');
#else
    return path[0] == '/';
#endif
}

bool path_is_relative(const char *path) {
    return !path_is_absolute(path);
}

char* path_normalize(const char *path, path_error_t *error) {
    if (!path) {
        if (error) *error = PATH_ERROR_NULL_PTR;
        return NULL;
    }
    
    size_t len = strlen(path);
    char* result = malloc(len + 1);
    if (!result) {
        if (error) *error = PATH_ERROR_MEMORY_ALLOC;
        return NULL;
    }
    
    const char* src = path;
    char* dst = result;
    
    while (*src) {
        if (*src == '.' && *(src + 1) == '.' && (*(src + 2) == PATH_SEP || *(src + 2) == '/' || *(src + 2) == '\0')) {
            if (dst > result) {
                dst--;
                while (dst > result && *(dst - 1) != PATH_SEP && *(dst - 1) != '/') dst--;
            }
            src += 2;
        } else if (*src == '.' && (*(src + 1) == PATH_SEP || *(src + 1) == '/' || *(src + 1) == '\0')) {
            src += 1;
        } else if (*src == PATH_SEP || *src == '/') {
            if (dst == result || *(dst - 1) != PATH_SEP) {
                *dst++ = PATH_SEP;
            }
            src++;
        } else {
            *dst++ = *src++;
        }
    }
    
    if (dst > result && *(dst - 1) == PATH_SEP && len > 0 && path[len - 1] != PATH_SEP && path[len - 1] != '/') {
        dst--;
    }
    
    *dst = '\0';
    if (error) *error = PATH_OK;
    return result;
}

bool path_parse(const char *path, path_info_t *info, path_error_t *error) {
    if (!path || !info) {
        if (error) *error = PATH_ERROR_NULL_PTR;
        return false;
    }
    
    memset(info, 0, sizeof(path_info_t));
    
    info->directory = path_dirname(path);
    info->filename = strdup(path_basename(path));
    info->is_absolute = path_is_absolute(path);
    
    const char* ext = path_get_extension(path);
    if (ext && *ext) {
        info->extension = strdup(ext);
    }
    
    if (error) *error = PATH_OK;
    return true;
}

void path_free_info(path_info_t *info) {
    if (!info) return;
    free(info->directory);
    free(info->filename);
    free(info->extension);
    memset(info, 0, sizeof(path_info_t));
}

const char* path_get_extension(const char *path) {
    if (!path) return "";
    
    const char* base = path_basename(path);
    const char* ext = strrchr(base, '.');
    
    if (!ext || ext == base) return "";
    return ext;
}

char* path_add_extension(const char *path, const char *extension, path_error_t *error) {
    if (!path) {
        if (error) *error = PATH_ERROR_NULL_PTR;
        return NULL;
    }
    
    const char* existing = path_get_extension(path);
    if (existing && *existing) {
        if (error) *error = PATH_OK;
        return strdup(path);
    }
    
    size_t path_len = strlen(path);
    size_t ext_len = extension ? strlen(extension) : 0;
    bool need_dot = (extension && extension[0] != '.');
    
    char* result = malloc(path_len + ext_len + (need_dot ? 2 : 1));
    if (!result) {
        if (error) *error = PATH_ERROR_MEMORY_ALLOC;
        return NULL;
    }
    
    strcpy(result, path);
    if (need_dot) {
        result[path_len] = '.';
        strcpy(result + path_len + 1, extension);
    } else if (extension) {
        strcpy(result + path_len, extension);
    } else {
        result[path_len] = '\0';
    }
    
    if (error) *error = PATH_OK;
    return result;
}

char* path_remove_extension(const char *path, path_error_t *error) {
    if (!path) {
        if (error) *error = PATH_ERROR_NULL_PTR;
        return NULL;
    }
    
    const char* ext = path_get_extension(path);
    if (!ext || !*ext) {
        if (error) *error = PATH_OK;
        return strdup(path);
    }
    
    size_t len = ext - path;
    char* result = malloc(len + 1);
    if (!result) {
        if (error) *error = PATH_ERROR_MEMORY_ALLOC;
        return NULL;
    }
    
    memcpy(result, path, len);
    result[len] = '\0';
    if (error) *error = PATH_OK;
    return result;
}
