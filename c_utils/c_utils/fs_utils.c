#include "fs_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>
#include <errno.h>

// 获取错误码
static fs_error_t fs_error_from_errno(void) {
    switch (errno) {
        case ENOENT: return FS_ERROR_FILE_NOT_FOUND;
        case EACCES: return FS_ERROR_PERMISSION_DENIED;
        case ENOMEM: return FS_ERROR_MEMORY_ALLOC;
        case EINVAL: return FS_ERROR_INVALID_PARAM;
        default: return FS_ERROR_FILE_OPEN;
    }
}

fs_options_t fs_default_options(void) {
    fs_options_t opts = {
        .follow_symlinks = false,
        .create_dirs = false,
        .atomic_write = false,
        .secure_permissions = false,
        .buffer_size = 4096
    };
    return opts;
}

char* fs_read_all(const char *filepath, size_t *out_size, fs_error_t *error) {
    if (!filepath) {
        if (error) *error = FS_ERROR_INVALID_PARAM;
        return NULL;
    }

    FILE *f = fopen(filepath, "rb");
    if (!f) {
        if (error) *error = fs_error_from_errno();
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buf = malloc(size + 1);
    if (!buf) {
        fclose(f);
        if (error) *error = FS_ERROR_MEMORY_ALLOC;
        return NULL;
    }

    size_t read_size = fread(buf, 1, size, f);
    fclose(f);
    buf[read_size] = '\0';

    if (out_size) *out_size = read_size;
    if (error) *error = FS_OK;
    return buf;
}

bool fs_write_all(const char *filepath, const void *data, size_t size, const fs_options_t *options, fs_error_t *error) {
    if (!filepath || !data) {
        if (error) *error = FS_ERROR_INVALID_PARAM;
        return false;
    }

    FILE *f = fopen(filepath, "wb");
    if (!f) {
        if (error) *error = fs_error_from_errno();
        return false;
    }
    
    size_t written = fwrite(data, 1, size, f);
    fclose(f);
    
    if (written != size) {
        if (error) *error = FS_ERROR_FILE_WRITE;
        return false;
    }
    
    if (error) *error = FS_OK;
    return true;
}

bool fs_exists(const char *filepath, fs_error_t *error) {
    if (!filepath) {
        if (error) *error = FS_ERROR_INVALID_PARAM;
        return false;
    }

    struct stat st;
    bool exists = (stat(filepath, &st) == 0);
    if (error) *error = exists ? FS_OK : FS_ERROR_FILE_NOT_FOUND;
    return exists;
}

long fs_file_size(const char *filepath, fs_error_t *error) {
    if (!filepath) {
        if (error) *error = FS_ERROR_INVALID_PARAM;
        return -1;
    }

    struct stat st;
    if (stat(filepath, &st) != 0) {
        if (error) *error = fs_error_from_errno();
        return -1;
    }
    
    if (error) *error = FS_OK;
    return st.st_size;
}

const char* fs_extname(const char *filepath, fs_error_t *error) {
    if (!filepath) {
        if (error) *error = FS_ERROR_INVALID_PARAM;
        return NULL;
    }

    const char *dot = strrchr(filepath, '.');
    if (!dot || dot == filepath) {
        if (error) *error = FS_OK;
        return "";
    }
    
    if (error) *error = FS_OK;
    return dot;
}

const char* fs_basename(const char *filepath, fs_error_t *error) {
    if (!filepath) {
        if (error) *error = FS_ERROR_INVALID_PARAM;
        return NULL;
    }

    const char *slash = strrchr(filepath, '/');
    if (error) *error = FS_OK;
    return slash ? slash + 1 : filepath;
}

const char* fs_dirname(const char *filepath, fs_error_t *error) {
    if (!filepath) {
        if (error) *error = FS_ERROR_INVALID_PARAM;
        return NULL;
    }

    static char buf[1024];
    strncpy(buf, filepath, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';
    
    char *dir = dirname(buf);
    if (error) *error = FS_OK;
    return dir;
}

bool fs_mkdir(const char *dirpath, bool recursive, fs_error_t *error) {
    if (!dirpath) {
        if (error) *error = FS_ERROR_INVALID_PARAM;
        return false;
    }

    int ret;
    if (recursive) {
        char tmp[1024];
        strncpy(tmp, dirpath, sizeof(tmp) - 1);
        tmp[sizeof(tmp) - 1] = '\0';
        
        for (char *p = tmp + 1; *p; p++) {
            if (*p == '/') {
                *p = '\0';
                mkdir(tmp, 0755);
                *p = '/';
            }
        }
        ret = mkdir(tmp, 0755);
    } else {
        ret = mkdir(dirpath, 0755);
    }
    
    if (ret != 0 && errno != EEXIST) {
        if (error) *error = fs_error_from_errno();
        return false;
    }
    
    if (error) *error = FS_OK;
    return true;
}

bool fs_rmdir(const char *dirpath, bool recursive, fs_error_t *error) {
    if (!dirpath) {
        if (error) *error = FS_ERROR_INVALID_PARAM;
        return false;
    }

    (void)recursive; // 简化实现，忽略recursive参数
    
    if (rmdir(dirpath) != 0) {
        if (error) *error = fs_error_from_errno();
        return false;
    }
    
    if (error) *error = FS_OK;
    return true;
}

bool fs_remove(const char *filepath, fs_error_t *error) {
    if (!filepath) {
        if (error) *error = FS_ERROR_INVALID_PARAM;
        return false;
    }

    if (remove(filepath) != 0) {
        if (error) *error = fs_error_from_errno();
        return false;
    }
    
    if (error) *error = FS_OK;
    return true;
}

// fs_unlink 是 fs_remove 的别名
bool fs_unlink(const char *filepath, fs_error_t *error) {
    return fs_remove(filepath, error);
}

bool fs_rename(const char *oldpath, const char *newpath, fs_error_t *error) {
    if (!oldpath || !newpath) {
        if (error) *error = FS_ERROR_INVALID_PARAM;
        return false;
    }

    if (rename(oldpath, newpath) != 0) {
        if (error) *error = fs_error_from_errno();
        return false;
    }
    
    if (error) *error = FS_OK;
    return true;
}

bool fs_copy(const char *srcpath, const char *dstpath, const fs_options_t *options, fs_error_t *error) {
    if (!srcpath || !dstpath) {
        if (error) *error = FS_ERROR_INVALID_PARAM;
        return false;
    }

    (void)options; // 简化实现，忽略options参数

    char *data = fs_read_all(srcpath, NULL, error);
    if (!data) return false;

    bool result = fs_write_all(dstpath, data, strlen(data), NULL, error);
    free(data);
    return result;
}
