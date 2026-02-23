#include "file_watcher.h"
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

file_watcher_options_t file_watcher_default_options(void) {
    file_watcher_options_t opts = {
        .recursive = false,
        .follow_symlinks = false,
        .watch_hidden = false,
        .use_polling = true,
        .poll_interval_ms = 1000
    };
    return opts;
}

file_watcher_t* file_watcher_create(const file_watcher_options_t *options, file_watcher_callback_t callback, void *user_data, file_watcher_error_t *error) {
    file_watcher_t *fw = calloc(1, sizeof(file_watcher_t));
    if (!fw) {
        if (error) *error = FILE_WATCHER_ERROR_MEMORY_ALLOC;
        return NULL;
    }
    
    if (options) {
        fw->recursive = options->recursive;
    }
    fw->callback = callback;
    fw->user_data = user_data;
    
    if (error) *error = FILE_WATCHER_OK;
    return fw;
}

void file_watcher_free(file_watcher_t *fw) {
    if (!fw) return;
    
    file_watcher_item_s *item = fw->items;
    while (item) {
        file_watcher_item_s *next = item->next;
        free(item->path);
        free(item);
        item = next;
    }
    
    free(fw);
}

bool file_watcher_add(file_watcher_t *fw, const char *path, file_watcher_error_t *error) {
    if (!fw || !path) {
        if (error) *error = FILE_WATCHER_ERROR_INVALID_PARAM;
        return false;
    }
    
    file_watcher_item_s *item = calloc(1, sizeof(file_watcher_item_s));
    if (!item) {
        if (error) *error = FILE_WATCHER_ERROR_MEMORY_ALLOC;
        return false;
    }
    
    item->path = strdup(path);
    if (!item->path) {
        free(item);
        if (error) *error = FILE_WATCHER_ERROR_MEMORY_ALLOC;
        return false;
    }
    
    struct stat st;
    if (stat(path, &st) == 0) {
        item->last_mtime = st.st_mtime;
        item->is_directory = S_ISDIR(st.st_mode);
    } else {
        item->last_mtime = 0;
        item->is_directory = false;
    }
    
    item->next = fw->items;
    fw->items = item;
    fw->count++;
    
    if (error) *error = FILE_WATCHER_OK;
    return true;
}

bool file_watcher_remove(file_watcher_t *fw, const char *path, file_watcher_error_t *error) {
    if (!fw || !path) {
        if (error) *error = FILE_WATCHER_ERROR_INVALID_PARAM;
        return false;
    }
    
    file_watcher_item_s **current = &fw->items;
    while (*current) {
        if (strcmp((*current)->path, path) == 0) {
            file_watcher_item_s *to_remove = *current;
            *current = (*current)->next;
            free(to_remove->path);
            free(to_remove);
            fw->count--;
            if (error) *error = FILE_WATCHER_OK;
            return true;
        }
        current = &(*current)->next;
    }
    
    if (error) *error = FILE_WATCHER_ERROR_FILE_NOT_FOUND;
    return false;
}

bool file_watcher_check(file_watcher_t *fw, file_watcher_error_t *error) {
    if (!fw) {
        if (error) *error = FILE_WATCHER_ERROR_INVALID_PARAM;
        return false;
    }
    
    bool changed = false;
    file_watcher_item_s *item = fw->items;
    while (item) {
        struct stat st;
        if (stat(item->path, &st) == 0) {
            if (st.st_mtime != item->last_mtime) {
                item->last_mtime = st.st_mtime;
                changed = true;
                if (fw->callback) {
                    fw->callback(item->path, FILE_WATCHER_EVENT_MODIFY, fw->user_data);
                }
            }
        } else {
            if (fw->callback) {
                fw->callback(item->path, FILE_WATCHER_EVENT_DELETE, fw->user_data);
            }
        }
        item = item->next;
    }
    
    if (error) *error = FILE_WATCHER_OK;
    return changed;
}

bool file_watcher_add_batch(file_watcher_t *fw, const char **paths, size_t count, file_watcher_error_t *error) {
    if (!fw || !paths || count == 0) {
        if (error) *error = FILE_WATCHER_ERROR_INVALID_PARAM;
        return false;
    }
    
    for (size_t i = 0; i < count; i++) {
        if (!file_watcher_add(fw, paths[i], error)) {
            return false;
        }
    }
    
    if (error) *error = FILE_WATCHER_OK;
    return true;
}

size_t file_watcher_get_count(const file_watcher_t *fw) {
    return fw ? fw->count : 0;
}

bool file_watcher_has_error(const file_watcher_t *fw, file_watcher_error_t *error, const char **error_msg) {
    if (!fw) {
        if (error) *error = FILE_WATCHER_ERROR_INVALID_PARAM;
        if (error_msg) *error_msg = "Invalid file watcher";
        return true;
    }
    
    if (error) *error = fw->error;
    if (error_msg) *error_msg = fw->has_error ? fw->error_msg : NULL;
    return fw->has_error;
}

const char* file_watcher_strerror(file_watcher_error_t error) {
    switch (error) {
        case FILE_WATCHER_OK: return "Success";
        case FILE_WATCHER_ERROR_INVALID_PARAM: return "Invalid parameter";
        case FILE_WATCHER_ERROR_FILE_NOT_FOUND: return "File not found";
        case FILE_WATCHER_ERROR_MEMORY_ALLOC: return "Memory allocation failed";
        case FILE_WATCHER_ERROR_DIR_OPEN: return "Directory open failed";
        case FILE_WATCHER_ERROR_DIR_READ: return "Directory read failed";
        case FILE_WATCHER_ERROR_PLATFORM_UNSUPPORTED: return "Platform not supported";
        default: return "Unknown error";
    }
}
