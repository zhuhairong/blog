#ifndef C_UTILS_FS_UTILS_H
#define C_UTILS_FS_UTILS_H

#include <stddef.h>
#include <stdbool.h>
#include <time.h>
#include <sys/types.h>

// 文件系统错误码
typedef enum {
    FS_OK = 0,
    FS_ERROR_INVALID_PARAM,
    FS_ERROR_FILE_NOT_FOUND,
    FS_ERROR_FILE_OPEN,
    FS_ERROR_FILE_READ,
    FS_ERROR_FILE_WRITE,
    FS_ERROR_FILE_CREATE,
    FS_ERROR_FILE_DELETE,
    FS_ERROR_DIR_OPEN,
    FS_ERROR_DIR_CREATE,
    FS_ERROR_DIR_DELETE,
    FS_ERROR_DIR_READ,
    FS_ERROR_PERMISSION_DENIED,
    FS_ERROR_MEMORY_ALLOC,
    FS_ERROR_BUFFER_TOO_SMALL,
    FS_ERROR_PLATFORM_UNSUPPORTED,
    FS_ERROR_INVALID_PATH
} fs_error_t;

// 文件类型
typedef enum {
    FS_TYPE_UNKNOWN,
    FS_TYPE_REGULAR,
    FS_TYPE_DIRECTORY,
    FS_TYPE_SYMLINK,
    FS_TYPE_CHAR_DEVICE,
    FS_TYPE_BLOCK_DEVICE,
    FS_TYPE_FIFO,
    FS_TYPE_SOCKET
} fs_file_type_t;

// 文件信息
typedef struct {
    char *path;
    fs_file_type_t type;
    size_t size;
    time_t mtime;
    time_t atime;
    time_t ctime;
    mode_t mode;
    uid_t uid;
    gid_t gid;
} fs_file_info_t;

// 文件系统选项
typedef struct {
    bool follow_symlinks;
    bool create_dirs;
    bool atomic_write;
    bool secure_permissions;
    size_t buffer_size;
} fs_options_t;

// 默认文件系统选项
// 返回: 默认选项
fs_options_t fs_default_options(void);

// 读取整个文件
// filepath: 文件路径
// out_size: 输出文件大小（可为 NULL）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回文件内容指针，失败返回 NULL
char* fs_read_all(const char *filepath, size_t *out_size, fs_error_t *error);

// 写入整个文件
// filepath: 文件路径
// data: 数据
// size: 数据大小
// options: 选项（可为 NULL 使用默认选项）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool fs_write_all(const char *filepath, const void *data, size_t size, const fs_options_t *options, fs_error_t *error);

// 检查文件是否存在
// filepath: 文件路径
// error: 错误码（输出参数，可为 NULL）
// 返回: 存在返回 true，不存在返回 false
bool fs_exists(const char *filepath, fs_error_t *error);

// 获取文件大小
// filepath: 文件路径
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回文件大小，失败返回 -1
long fs_file_size(const char *filepath, fs_error_t *error);

// 获取文件扩展名
// filepath: 文件路径
// error: 错误码（输出参数，可为 NULL）
// 返回: 文件扩展名，失败返回 NULL
const char* fs_extname(const char *filepath, fs_error_t *error);

// 获取文件名
// filepath: 文件路径
// error: 错误码（输出参数，可为 NULL）
// 返回: 文件名，失败返回 NULL
const char* fs_basename(const char *filepath, fs_error_t *error);

// 获取目录名
// filepath: 文件路径
// error: 错误码（输出参数，可为 NULL）
// 返回: 目录名，失败返回 NULL
const char* fs_dirname(const char *filepath, fs_error_t *error);

// 创建目录
// dirpath: 目录路径
// recursive: 是否递归创建
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool fs_mkdir(const char *dirpath, bool recursive, fs_error_t *error);

// 删除目录
// dirpath: 目录路径
// recursive: 是否递归删除
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool fs_rmdir(const char *dirpath, bool recursive, fs_error_t *error);

// 删除文件
// filepath: 文件路径
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool fs_unlink(const char *filepath, fs_error_t *error);

// 重命名文件或目录
// oldpath: 旧路径
// newpath: 新路径
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool fs_rename(const char *oldpath, const char *newpath, fs_error_t *error);

// 复制文件
// src: 源文件路径
// dest: 目标文件路径
// options: 选项（可为 NULL 使用默认选项）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool fs_copy(const char *src, const char *dest, const fs_options_t *options, fs_error_t *error);

// 获取文件信息
// filepath: 文件路径
// info: 文件信息（输出参数）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool fs_stat(const char *filepath, fs_file_info_t *info, fs_error_t *error);

// 列出目录内容
// dirpath: 目录路径
// entries: 目录条目数组（输出参数）
// count: 目录条目数量（输出参数）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool fs_read_dir(const char *dirpath, char ***entries, size_t *count, fs_error_t *error);

// 释放目录条目
// entries: 目录条目数组
// count: 目录条目数量
void fs_free_dir_entries(char ***entries, size_t count);

// 获取当前工作目录
// buffer: 缓冲区
// size: 缓冲区大小
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回当前工作目录，失败返回 NULL
char* fs_getcwd(char *buffer, size_t size, fs_error_t *error);

// 设置当前工作目录
// dirpath: 目录路径
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool fs_chdir(const char *dirpath, fs_error_t *error);

// 检查路径是否为绝对路径
// path: 路径
// 返回: 是绝对路径返回 true，否则返回 false
bool fs_is_absolute_path(const char *path);

// 规范化路径
// path: 路径
// buffer: 缓冲区
// size: 缓冲区大小
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回规范化后的路径，失败返回 NULL
char* fs_realpath(const char *path, char *buffer, size_t size, fs_error_t *error);

// 获取错误信息
// error: 错误码
// 返回: 错误信息字符串
const char* fs_strerror(fs_error_t error);

#endif // C_UTILS_FS_UTILS_H
