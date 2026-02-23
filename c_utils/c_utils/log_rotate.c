#include "log_rotate.h"
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

static log_rotate_error_t get_file_size(const char *path, size_t *size) {
    struct stat st;
    if (stat(path, &st) != 0) {
        if (errno == ENOENT) {
            *size = 0;
            return LOG_ROTATE_OK;
        }
        return LOG_ROTATE_STAT_ERROR;
    }
    *size = (size_t)st.st_size;
    return LOG_ROTATE_OK;
}

static log_rotate_error_t rotate_backups(const char *path, int max_backups) {
    for (int i = max_backups - 1; i >= 1; i--) {
        char old_name[512], new_name[512];
        snprintf(old_name, sizeof(old_name), "%s.%d", path, i);
        snprintf(new_name, sizeof(new_name), "%s.%d", path, i + 1);

        if (access(old_name, F_OK) == 0) {
            if (rename(old_name, new_name) != 0) {
                return LOG_ROTATE_RENAME_ERROR;
            }
        }
    }
    return LOG_ROTATE_OK;
}

static log_rotate_error_t create_empty_file(const char *path, mode_t mode) {
    FILE *fp = fopen(path, "w");
    if (!fp) {
        return LOG_ROTATE_OPEN_ERROR;
    }
    if (fclose(fp) != 0) {
        return LOG_ROTATE_CLOSE_ERROR;
    }
    if (chmod(path, mode) != 0) {
        return LOG_ROTATE_PERMISSION_DENIED;
    }
    return LOG_ROTATE_OK;
}

void log_rotate(const char *path, size_t max_size, int max_backups) {
    log_rotate_config_t config;
    log_rotate_get_default_config(&config);
    config.max_size = max_size;
    config.max_backups = max_backups;
    bool rotated;
    log_rotate_ex(path, &config, LOG_ROTATE_TRIGGER_SIZE, &rotated);
}

log_rotate_error_t log_rotate_ex(const char *path, const log_rotate_config_t *config, 
                                log_rotate_trigger_t trigger, bool *rotated) {
    if (!path || !config) {
        return LOG_ROTATE_INVALID_INPUT;
    }

    if (rotated) {
        *rotated = false;
    }

    bool needs_rotation = false;
    if (trigger == LOG_ROTATE_TRIGGER_SIZE || trigger == LOG_ROTATE_TRIGGER_TIME) {
        size_t file_size;
        log_rotate_error_t error = get_file_size(path, &file_size);
        if (error != LOG_ROTATE_OK) {
            return error;
        }
        needs_rotation = (file_size >= config->max_size);
    } else if (trigger == LOG_ROTATE_TRIGGER_FORCE || config->force_rotate) {
        needs_rotation = true;
    }

    if (!needs_rotation) {
        return LOG_ROTATE_OK;
    }

    // 滚动备份文件
    log_rotate_error_t error = rotate_backups(path, config->max_backups);
    if (error != LOG_ROTATE_OK) {
        return error;
    }

    // 重命名当前文件为第一个备份
    char first_backup[512];
    snprintf(first_backup, sizeof(first_backup), "%s.1", path);
    if (rename(path, first_backup) != 0) {
        return LOG_ROTATE_RENAME_ERROR;
    }

    // 创建新的空日志文件
    if (config->create_file) {
        error = create_empty_file(path, config->file_mode);
        if (error != LOG_ROTATE_OK) {
            return error;
        }
    }

    if (rotated) {
        *rotated = true;
    }

    return LOG_ROTATE_OK;
}

bool log_rotate_needs_rotation(const char *path, const log_rotate_config_t *config, 
                              log_rotate_error_t *error) {
    if (!path || !config) {
        if (error) *error = LOG_ROTATE_INVALID_INPUT;
        return false;
    }

    size_t file_size;
    log_rotate_error_t err = get_file_size(path, &file_size);
    if (error) *error = err;
    if (err != LOG_ROTATE_OK) {
        return false;
    }

    return file_size >= config->max_size;
}

log_rotate_error_t log_rotate_force(const char *path, const log_rotate_config_t *config) {
    if (!path || !config) {
        return LOG_ROTATE_INVALID_INPUT;
    }

    log_rotate_config_t force_config = *config;
    force_config.force_rotate = true;
    bool rotated;
    return log_rotate_ex(path, &force_config, LOG_ROTATE_TRIGGER_FORCE, &rotated);
}

log_rotate_error_t log_rotate_cleanup(const char *path, int max_backups, 
                                     log_rotate_error_t *error) {
    if (!path || max_backups < 0) {
        if (error) *error = LOG_ROTATE_INVALID_INPUT;
        return LOG_ROTATE_INVALID_INPUT;
    }

    for (int i = max_backups + 1; ; i++) {
        char backup_name[512];
        snprintf(backup_name, sizeof(backup_name), "%s.%d", path, i);
        if (access(backup_name, F_OK) != 0) {
            break;
        }
        if (unlink(backup_name) != 0) {
            if (error) *error = LOG_ROTATE_FILE_ERROR;
            return LOG_ROTATE_FILE_ERROR;
        }
    }

    if (error) *error = LOG_ROTATE_OK;
    return LOG_ROTATE_OK;
}

log_rotate_error_t log_rotate_get_file_size(const char *path, size_t *size) {
    if (!path || !size) {
        return LOG_ROTATE_INVALID_INPUT;
    }
    return get_file_size(path, size);
}

void log_rotate_get_default_config(log_rotate_config_t *config) {
    if (config) {
        config->max_size = 10 * 1024 * 1024; // 10MB
        config->max_backups = 5;
        config->compress = false;
        config->create_file = true;
        config->verbose = false;
        config->check_size = true;
        config->file_mode = 0644;
        config->rotate_interval = 0;
        config->force_rotate = false;
    }
}
