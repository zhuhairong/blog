#ifndef C_UTILS_LOCKFILE_H
#define C_UTILS_LOCKFILE_H

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>
#include <errno.h>

// 锁文件错误码
typedef enum {
    LOCKFILE_OK = 0,
    LOCKFILE_INVALID_INPUT = -1,
    LOCKFILE_FILE_ERROR = -2,
    LOCKFILE_LOCK_ERROR = -3,
    LOCKFILE_UNLOCK_ERROR = -4,
    LOCKFILE_TIMEOUT = -5,
    LOCKFILE_ALREADY_LOCKED = -6,
    LOCKFILE_PERMISSION_DENIED = -7,
    LOCKFILE_SYSTEM_ERROR = -8
} lockfile_error_t;

// 锁文件配置
typedef struct {
    bool blocking;
    bool exclusive;
    bool non_blocking;
    int timeout_ms;
    bool create_file;
    mode_t file_mode;
} lockfile_config_t;

// 锁定文件 (阻塞)
int  lockfile_lock(const char *filename);

// 解锁
void lockfile_unlock(int fd);

// 带错误处理的锁定
int lockfile_lock_ex(const char *filename, const lockfile_config_t *config, lockfile_error_t *error);

// 尝试锁定（非阻塞）
int lockfile_try_lock(const char *filename, lockfile_error_t *error);

// 检查文件是否被锁定
bool lockfile_is_locked(const char *filename, lockfile_error_t *error);

// 获取锁定状态
lockfile_error_t lockfile_get_status(const char *filename, bool *is_locked, pid_t *lock_owner);

// 强制解锁（谨慎使用）
lockfile_error_t lockfile_force_unlock(const char *filename);

// 获取默认配置
void lockfile_get_default_config(lockfile_config_t *config);

#endif // C_UTILS_LOCKFILE_H
