#include "fs_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef _WIN32
    #include <windows.h>
    #include <direct.h>
    #include <io.h>
    #define PATH_SEP '\\'
    #define PATH_SEP_STR "\\"
    #define mkdir(path, mode) _mkdir(path)
    #define rmdir _rmdir
    #define getcwd _getcwd
    #define chdir _chdir
    #define access _access
    #define F_OK 0
#else
    #include <sys/stat.h>
    #include <unistd.h>
    #include <dirent.h>
    #include <libgen.h>
    #include <limits.h>
    #define PATH_SEP '/'
    #define PATH_SEP_STR "/"
#endif

static fs_error_t fs_error_from_errno(void) {
    switch (errno) {
        case ENOENT: return FS_ERROR_FILE_NOT_FOUND;
        case EACCES: return FS_ERROR_PERMISSION_DENIED;
        case ENOMEM: return FS_ERROR_MEMORY_ALLOC;
        case EINVAL: return FS_ERROR_INVALID_PARAM;
        default: return FS_ERROR_FILE_OPEN;
    }
}

const char* fs_strerror(fs_error_t error) {
    switch (error) {
        case FS_OK: return "Success";
        case FS_ERROR_INVALID_PARAM: return "Invalid parameter";
        case FS_ERROR_FILE_NOT_FOUND: return "File not found";
        case FS_ERROR_FILE_OPEN: return "Failed to open file";
        case FS_ERROR_FILE_READ: return "Failed to read file";
        case FS_ERROR_FILE_WRITE: return "Failed to write file";
        case FS_ERROR_FILE_CREATE: return "Failed to create file";
        case FS_ERROR_FILE_DELETE: return "Failed to delete file";
        case FS_ERROR_DIR_OPEN: return "Failed to open directory";
        case FS_ERROR_DIR_CREATE: return "Failed to create directory";
        case FS_ERROR_DIR_DELETE: return "Failed to delete directory";
        case FS_ERROR_DIR_READ: return "Failed to read directory";
        case FS_ERROR_PERMISSION_DENIED: return "Permission denied";
        case FS_ERROR_MEMORY_ALLOC: return "Memory allocation failed";
        case FS_ERROR_BUFFER_TOO_SMALL: return "Buffer too small";
        case FS_ERROR_PLATFORM_UNSUPPORTED: return "Platform not supported";
        case FS_ERROR_INVALID_PATH: return "Invalid path";
        default: return "Unknown error";
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

    if (size < 0) {
        fclose(f);
        if (error) *error = FS_ERROR_FILE_READ;
        return NULL;
    }

    char *buf = malloc((size_t)size + 1);
    if (!buf) {
        fclose(f);
        if (error) *error = FS_ERROR_MEMORY_ALLOC;
        return NULL;
    }

    size_t read_size = fread(buf, 1, (size_t)size, f);
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

    (void)options;

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

#ifdef _WIN32
    struct _stat st;
    bool exists = (_stat(filepath, &st) == 0);
#else
    struct stat st;
    bool exists = (stat(filepath, &st) == 0);
#endif
    if (error) *error = exists ? FS_OK : FS_ERROR_FILE_NOT_FOUND;
    return exists;
}

long fs_file_size(const char *filepath, fs_error_t *error) {
    if (!filepath) {
        if (error) *error = FS_ERROR_INVALID_PARAM;
        return -1;
    }

#ifdef _WIN32
    struct _stat st;
    if (_stat(filepath, &st) != 0) {
#else
    struct stat st;
    if (stat(filepath, &st) != 0) {
#endif
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
    
#ifdef _WIN32
    const char *slash = strrchr(filepath, '\\');
    const char *fslash = strrchr(filepath, '/');
    const char *last_sep = (slash > fslash) ? slash : fslash;
#else
    const char *last_sep = strrchr(filepath, '/');
#endif
    if (last_sep && dot < last_sep) {
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

    const char *result = filepath;
    
#ifdef _WIN32
    const char *slash = strrchr(filepath, '\\');
    const char *fslash = strrchr(filepath, '/');
    const char *last_sep = (slash > fslash) ? slash : fslash;
#else
    const char *last_sep = strrchr(filepath, '/');
#endif
    if (last_sep) {
        result = last_sep + 1;
    }
    
    if (error) *error = FS_OK;
    return result;
}

char* fs_dirname(const char *filepath, fs_error_t *error) {
    if (!filepath) {
        if (error) *error = FS_ERROR_INVALID_PARAM;
        return NULL;
    }

    size_t len = strlen(filepath);
    if (len == 0) {
        char *result = strdup(".");
        if (!result) {
            if (error) *error = FS_ERROR_MEMORY_ALLOC;
            return NULL;
        }
        if (error) *error = FS_OK;
        return result;
    }

    char *buf = strdup(filepath);
    if (!buf) {
        if (error) *error = FS_ERROR_MEMORY_ALLOC;
        return NULL;
    }

    char *end = buf + len - 1;
    while (end > buf && (*end == '/' 
#ifdef _WIN32
        || *end == '\\'
#endif
    )) {
        end--;
    }
    *(end + 1) = '\0';

    char *last_sep = strrchr(buf, '/'
#ifdef _WIN32
    );
    char *last_sep_win = strrchr(buf, '\\');
    if (last_sep_win > last_sep) last_sep = last_sep_win;
#else
    );
#endif

    if (!last_sep) {
        free(buf);
        char *result = strdup(".");
        if (!result) {
            if (error) *error = FS_ERROR_MEMORY_ALLOC;
            return NULL;
        }
        if (error) *error = FS_OK;
        return result;
    }

    if (last_sep == buf) {
        *(last_sep + 1) = '\0';
    } else {
        *last_sep = '\0';
    }

    if (error) *error = FS_OK;
    return buf;
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
        
        char *p = tmp;
#ifdef _WIN32
        if (tmp[0] && tmp[1] == ':') {
            p = tmp + 2;
            if (*p == '\\' || *p == '/') p++;
        } else if (tmp[0] == '\\' || tmp[0] == '/') {
            p++;
        }
#else
        if (tmp[0] == '/') p++;
#endif
        
        for (; *p; p++) {
            if (*p == '/' 
#ifdef _WIN32
                || *p == '\\'
#endif
            ) {
                char saved = *p;
                *p = '\0';
                mkdir(tmp, 0755);
                *p = saved;
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

static bool fs_remove_recursive(const char *dirpath, fs_error_t *error) {
#ifdef _WIN32
    WIN32_FIND_DATAA find_data;
    char search_path[MAX_PATH];
    snprintf(search_path, sizeof(search_path), "%s\\*", dirpath);
    
    HANDLE h_find = FindFirstFileA(search_path, &find_data);
    if (h_find == INVALID_HANDLE_VALUE) {
        if (error) *error = fs_error_from_errno();
        return false;
    }
    
    bool success = true;
    do {
        if (strcmp(find_data.cFileName, ".") == 0 || 
            strcmp(find_data.cFileName, "..") == 0) {
            continue;
        }
        
        char full_path[MAX_PATH];
        snprintf(full_path, sizeof(full_path), "%s\\%s", dirpath, find_data.cFileName);
        
        if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (!fs_remove_recursive(full_path, error)) {
                success = false;
                break;
            }
        } else {
            if (!DeleteFileA(full_path)) {
                if (error) *error = FS_ERROR_FILE_DELETE;
                success = false;
                break;
            }
        }
    } while (FindNextFileA(h_find, &find_data));
    
    FindClose(h_find);
    
    if (success) {
        if (!RemoveDirectoryA(dirpath)) {
            if (error) *error = FS_ERROR_DIR_DELETE;
            return false;
        }
    }
    
    return success;
#else
    DIR *dir = opendir(dirpath);
    if (!dir) {
        if (error) *error = fs_error_from_errno();
        return false;
    }
    
    struct dirent *entry;
    bool success = true;
    
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || 
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", dirpath, entry->d_name);
        
        struct stat st;
        if (stat(full_path, &st) != 0) {
            if (error) *error = fs_error_from_errno();
            success = false;
            break;
        }
        
        if (S_ISDIR(st.st_mode)) {
            if (!fs_remove_recursive(full_path, error)) {
                success = false;
                break;
            }
        } else {
            if (remove(full_path) != 0) {
                if (error) *error = FS_ERROR_FILE_DELETE;
                success = false;
                break;
            }
        }
    }
    
    closedir(dir);
    
    if (success) {
        if (rmdir(dirpath) != 0) {
            if (error) *error = fs_error_from_errno();
            return false;
        }
    }
    
    return success;
#endif
}

bool fs_rmdir(const char *dirpath, bool recursive, fs_error_t *error) {
    if (!dirpath) {
        if (error) *error = FS_ERROR_INVALID_PARAM;
        return false;
    }

    if (recursive) {
        return fs_remove_recursive(dirpath, error);
    }
    
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

    (void)options;

    size_t size;
    char *data = fs_read_all(srcpath, &size, error);
    if (!data) return false;

    bool result = fs_write_all(dstpath, data, size, NULL, error);
    free(data);
    return result;
}

bool fs_stat(const char *filepath, fs_file_info_t *info, fs_error_t *error) {
    if (!filepath || !info) {
        if (error) *error = FS_ERROR_INVALID_PARAM;
        return false;
    }

    memset(info, 0, sizeof(*info));

#ifdef _WIN32
    struct _stat st;
    if (_stat(filepath, &st) != 0) {
#else
    struct stat st;
    if (stat(filepath, &st) != 0) {
#endif
        if (error) *error = fs_error_from_errno();
        return false;
    }

    info->path = strdup(filepath);
    if (!info->path) {
        if (error) *error = FS_ERROR_MEMORY_ALLOC;
        return false;
    }

    info->size = (size_t)st.st_size;
    info->mtime = st.st_mtime;
    info->atime = st.st_atime;
    info->ctime = st.st_ctime;
    info->mode = st.st_mode;
    info->uid = st.st_uid;
    info->gid = st.st_gid;

#ifdef _WIN32
    if (st.st_mode & _S_IFDIR) {
        info->type = FS_TYPE_DIRECTORY;
    } else if (st.st_mode & _S_IFREG) {
        info->type = FS_TYPE_REGULAR;
    } else if (st.st_mode & _S_IFCHR) {
        info->type = FS_TYPE_CHAR_DEVICE;
    } else {
        info->type = FS_TYPE_UNKNOWN;
    }
#else
    if (S_ISREG(st.st_mode)) {
        info->type = FS_TYPE_REGULAR;
    } else if (S_ISDIR(st.st_mode)) {
        info->type = FS_TYPE_DIRECTORY;
    } else if (S_ISLNK(st.st_mode)) {
        info->type = FS_TYPE_SYMLINK;
    } else if (S_ISCHR(st.st_mode)) {
        info->type = FS_TYPE_CHAR_DEVICE;
    } else if (S_ISBLK(st.st_mode)) {
        info->type = FS_TYPE_BLOCK_DEVICE;
    } else if (S_ISFIFO(st.st_mode)) {
        info->type = FS_TYPE_FIFO;
    } else if (S_ISSOCK(st.st_mode)) {
        info->type = FS_TYPE_SOCKET;
    } else {
        info->type = FS_TYPE_UNKNOWN;
    }
#endif

    if (error) *error = FS_OK;
    return true;
}

bool fs_read_dir(const char *dirpath, char ***entries, size_t *count, fs_error_t *error) {
    if (!dirpath || !entries || !count) {
        if (error) *error = FS_ERROR_INVALID_PARAM;
        return false;
    }

    *entries = NULL;
    *count = 0;

#ifdef _WIN32
    WIN32_FIND_DATAA find_data;
    char search_path[MAX_PATH];
    snprintf(search_path, sizeof(search_path), "%s\\*", dirpath);
    
    HANDLE h_find = FindFirstFileA(search_path, &find_data);
    if (h_find == INVALID_HANDLE_VALUE) {
        if (error) *error = FS_ERROR_DIR_OPEN;
        return false;
    }
    
    size_t capacity = 16;
    char **result = malloc(capacity * sizeof(char*));
    if (!result) {
        FindClose(h_find);
        if (error) *error = FS_ERROR_MEMORY_ALLOC;
        return false;
    }
    
    size_t n = 0;
    bool success = true;
    
    do {
        if (strcmp(find_data.cFileName, ".") == 0 || 
            strcmp(find_data.cFileName, "..") == 0) {
            continue;
        }
        
        if (n >= capacity) {
            capacity *= 2;
            char **new_result = realloc(result, capacity * sizeof(char*));
            if (!new_result) {
                success = false;
                break;
            }
            result = new_result;
        }
        
        result[n] = strdup(find_data.cFileName);
        if (!result[n]) {
            success = false;
            break;
        }
        n++;
    } while (FindNextFileA(h_find, &find_data));
    
    FindClose(h_find);
    
    if (!success) {
        for (size_t i = 0; i < n; i++) {
            free(result[i]);
        }
        free(result);
        if (error) *error = FS_ERROR_MEMORY_ALLOC;
        return false;
    }
    
    *entries = result;
    *count = n;
#else
    DIR *dir = opendir(dirpath);
    if (!dir) {
        if (error) *error = fs_error_from_errno();
        return false;
    }
    
    size_t capacity = 16;
    char **result = malloc(capacity * sizeof(char*));
    if (!result) {
        closedir(dir);
        if (error) *error = FS_ERROR_MEMORY_ALLOC;
        return false;
    }
    
    size_t n = 0;
    struct dirent *entry;
    bool success = true;
    
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || 
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        if (n >= capacity) {
            capacity *= 2;
            char **new_result = realloc(result, capacity * sizeof(char*));
            if (!new_result) {
                success = false;
                break;
            }
            result = new_result;
        }
        
        result[n] = strdup(entry->d_name);
        if (!result[n]) {
            success = false;
            break;
        }
        n++;
    }
    
    closedir(dir);
    
    if (!success) {
        for (size_t i = 0; i < n; i++) {
            free(result[i]);
        }
        free(result);
        if (error) *error = FS_ERROR_MEMORY_ALLOC;
        return false;
    }
    
    *entries = result;
    *count = n;
#endif

    if (error) *error = FS_OK;
    return true;
}

void fs_free_dir_entries(char ***entries, size_t count) {
    if (!entries || !*entries) return;
    
    char **arr = *entries;
    for (size_t i = 0; i < count; i++) {
        free(arr[i]);
    }
    free(arr);
    *entries = NULL;
}

char* fs_getcwd(char *buffer, size_t size, fs_error_t *error) {
    if (!buffer || size == 0) {
        if (error) *error = FS_ERROR_INVALID_PARAM;
        return NULL;
    }

    if (getcwd(buffer, size) == NULL) {
        if (error) *error = fs_error_from_errno();
        return NULL;
    }
    
    if (error) *error = FS_OK;
    return buffer;
}

bool fs_chdir(const char *dirpath, fs_error_t *error) {
    if (!dirpath) {
        if (error) *error = FS_ERROR_INVALID_PARAM;
        return false;
    }

    if (chdir(dirpath) != 0) {
        if (error) *error = fs_error_from_errno();
        return false;
    }
    
    if (error) *error = FS_OK;
    return true;
}

bool fs_is_absolute_path(const char *path) {
    if (!path || path[0] == '\0') {
        return false;
    }

#ifdef _WIN32
    if (path[0] == '\\' || path[0] == '/') {
        return true;
    }
    if (path[0] && path[1] == ':') {
        return true;
    }
    return false;
#else
    return path[0] == '/';
#endif
}

char* fs_realpath(const char *path, char *buffer, size_t size, fs_error_t *error) {
    if (!path || !buffer || size == 0) {
        if (error) *error = FS_ERROR_INVALID_PARAM;
        return NULL;
    }

#ifdef _WIN32
    char full_path[MAX_PATH];
    char *file_part;
    DWORD result_len = GetFullPathNameA(path, MAX_PATH, full_path, &file_part);
    
    if (result_len == 0) {
        if (error) *error = FS_ERROR_INVALID_PATH;
        return NULL;
    }
    
    if (result_len >= size) {
        if (error) *error = FS_ERROR_BUFFER_TOO_SMALL;
        return NULL;
    }
    
    strncpy(buffer, full_path, size);
    buffer[size - 1] = '\0';
    
    DWORD attrs = GetFileAttributesA(buffer);
    if (attrs == INVALID_FILE_ATTRIBUTES) {
        if (error) *error = FS_ERROR_FILE_NOT_FOUND;
        return NULL;
    }
#else
    char *resolved = realpath(path, NULL);
    if (!resolved) {
        if (error) *error = fs_error_from_errno();
        return NULL;
    }
    
    if (strlen(resolved) >= size) {
        free(resolved);
        if (error) *error = FS_ERROR_BUFFER_TOO_SMALL;
        return NULL;
    }
    
    strncpy(buffer, resolved, size);
    buffer[size - 1] = '\0';
    free(resolved);
#endif

    if (error) *error = FS_OK;
    return buffer;
}
