#include "lockfile.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

static lockfile_error_t get_lockfile_error(int errno_val) {
    switch (errno_val) {
        case EACCES:
        case EPERM:
            return LOCKFILE_PERMISSION_DENIED;
        case EEXIST:
            return LOCKFILE_ALREADY_LOCKED;
        case EBUSY:
            return LOCKFILE_ALREADY_LOCKED;
        case EWOULDBLOCK:
            return LOCKFILE_ALREADY_LOCKED;
        case ENOENT:
        case EIO:
        case EINTR:
            return LOCKFILE_FILE_ERROR;
        default:
            return LOCKFILE_SYSTEM_ERROR;
    }
}

static int do_lock(int fd, bool exclusive, bool non_blocking, lockfile_error_t *error) {
    int lock_type = exclusive ? LOCK_EX : LOCK_SH;
    if (non_blocking) {
        lock_type |= LOCK_NB;
    }

    int result = flock(fd, lock_type);
    if (result == -1) {
        if (error) {
            *error = get_lockfile_error(errno);
        }
        return -1;
    }

    if (error) {
        *error = LOCKFILE_OK;
    }
    return fd;
}

int lockfile_lock(const char *filename) {
    lockfile_config_t config;
    lockfile_get_default_config(&config);
    lockfile_error_t error;
    return lockfile_lock_ex(filename, &config, &error);
}

void lockfile_unlock(int fd) {
    if (fd >= 0) {
        flock(fd, LOCK_UN);
        close(fd);
    }
}

int lockfile_lock_ex(const char *filename, const lockfile_config_t *config, lockfile_error_t *error) {
    if (!filename || !config) {
        if (error) *error = LOCKFILE_INVALID_INPUT;
        return -1;
    }

    int open_flags = O_RDWR;
    if (config->create_file) {
        open_flags |= O_CREAT;
    }

    int fd = open(filename, open_flags, config->file_mode);
    if (fd == -1) {
        if (error) *error = LOCKFILE_FILE_ERROR;
        return -1;
    }

    if (config->blocking && config->timeout_ms > 0) {
        int start_time = (int)(time(NULL) * 1000);
        while (1) {
            int result = do_lock(fd, config->exclusive, true, NULL);
            if (result != -1) {
                if (error) *error = LOCKFILE_OK;
                return fd;
            }

            int current_time = (int)(time(NULL) * 1000);
            if (current_time - start_time >= config->timeout_ms) {
                close(fd);
                if (error) *error = LOCKFILE_TIMEOUT;
                return -1;
            }

            usleep(10000); // 10ms
        }
    } else {
        int result = do_lock(fd, config->exclusive, config->non_blocking, error);
        if (result == -1) {
            close(fd);
            return -1;
        }
        return fd;
    }
}

int lockfile_try_lock(const char *filename, lockfile_error_t *error) {
    lockfile_config_t config;
    lockfile_get_default_config(&config);
    config.non_blocking = true;
    config.timeout_ms = 0;
    return lockfile_lock_ex(filename, &config, error);
}

bool lockfile_is_locked(const char *filename, lockfile_error_t *error) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        if (error) *error = LOCKFILE_FILE_ERROR;
        return false;
    }

    int result = flock(fd, LOCK_EX | LOCK_NB);
    bool is_locked = (result == -1 && (errno == EWOULDBLOCK || errno == EBUSY));

    if (!is_locked) {
        flock(fd, LOCK_UN);
    }

    close(fd);

    if (error) *error = LOCKFILE_OK;
    return is_locked;
}

lockfile_error_t lockfile_get_status(const char *filename, bool *is_locked, pid_t *lock_owner) {
    if (!filename || !is_locked) {
        return LOCKFILE_INVALID_INPUT;
    }

    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        return LOCKFILE_FILE_ERROR;
    }

    int result = flock(fd, LOCK_EX | LOCK_NB);
    *is_locked = (result == -1 && (errno == EWOULDBLOCK || errno == EBUSY));

    if (!*is_locked) {
        flock(fd, LOCK_UN);
    }

    if (lock_owner) {
        // 注意：flock 不支持获取锁的所有者 PID
        *lock_owner = -1;
    }

    close(fd);
    return LOCKFILE_OK;
}

lockfile_error_t lockfile_force_unlock(const char *filename) {
    int fd = open(filename, O_RDWR);
    if (fd == -1) {
        return LOCKFILE_FILE_ERROR;
    }

    int result = flock(fd, LOCK_UN);
    close(fd);

    if (result == -1) {
        return LOCKFILE_UNLOCK_ERROR;
    }

    return LOCKFILE_OK;
}

void lockfile_get_default_config(lockfile_config_t *config) {
    if (config) {
        config->blocking = true;
        config->exclusive = true;
        config->non_blocking = false;
        config->timeout_ms = 0;
        config->create_file = true;
        config->file_mode = 0666;
    }
}
