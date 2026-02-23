#include "mmap.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

mmap_config_t mmap_default_config(void) {
    mmap_config_t config = {
        .read_only = true,
        .private = true,
        .populate = false,
        .offset = 0,
        .length = 0
    };
    return config;
}

void* mmap_file(const char *filename, size_t *size) {
    return mmap_file_ex(filename, size, NULL, NULL);
}

void* mmap_file_ex(const char *filename, size_t *size, const mmap_config_t *config, mmap_error_t *error) {
    if (!filename || !size) {
        if (error) *error = MMAP_ERROR_NULL_PTR;
        return NULL;
    }
    
    mmap_config_t cfg = config ? *config : mmap_default_config();
    
    int flags = cfg.read_only ? O_RDONLY : O_RDWR;
    int fd = open(filename, flags);
    if (fd == -1) {
        if (error) *error = MMAP_ERROR_OPEN_FILE;
        return NULL;
    }
    
    struct stat st;
    if (fstat(fd, &st) == -1) {
        close(fd);
        if (error) *error = MMAP_ERROR_FILE_SIZE;
        return NULL;
    }
    
    size_t file_size = st.st_size;
    size_t map_size = cfg.length > 0 ? cfg.length : file_size - cfg.offset;
    
    if (cfg.offset >= file_size) {
        close(fd);
        if (error) *error = MMAP_ERROR_INVALID_ARGS;
        return NULL;
    }
    
    int prot = PROT_READ;
    if (!cfg.read_only) {
        prot |= PROT_WRITE;
    }
    
    int map_flags = cfg.private ? MAP_PRIVATE : MAP_SHARED;
    if (cfg.populate) {
        map_flags |= MAP_POPULATE;
    }
    
    void *ptr = mmap(NULL, map_size, prot, map_flags, fd, cfg.offset);
    close(fd);
    
    if (ptr == MAP_FAILED) {
        if (error) *error = MMAP_ERROR_MMAP;
        return NULL;
    }
    
    *size = map_size;
    if (error) *error = MMAP_OK;
    return ptr;
}

bool mmap_file_info(const char *filename, const mmap_config_t *config, mmap_info_t *info, mmap_error_t *error) {
    if (!filename || !info) {
        if (error) *error = MMAP_ERROR_NULL_PTR;
        return false;
    }
    
    mmap_config_t cfg = config ? *config : mmap_default_config();
    
    int flags = cfg.read_only ? O_RDONLY : O_RDWR;
    int fd = open(filename, flags);
    if (fd == -1) {
        if (error) *error = MMAP_ERROR_OPEN_FILE;
        return false;
    }
    
    struct stat st;
    if (fstat(fd, &st) == -1) {
        close(fd);
        if (error) *error = MMAP_ERROR_FILE_SIZE;
        return false;
    }
    
    size_t file_size = st.st_size;
    size_t map_size = cfg.length > 0 ? cfg.length : file_size - cfg.offset;
    
    if (cfg.offset >= file_size) {
        close(fd);
        if (error) *error = MMAP_ERROR_INVALID_ARGS;
        return false;
    }
    
    int prot = PROT_READ;
    if (!cfg.read_only) {
        prot |= PROT_WRITE;
    }
    
    int map_flags = cfg.private ? MAP_PRIVATE : MAP_SHARED;
    if (cfg.populate) {
        map_flags |= MAP_POPULATE;
    }
    
    void *ptr = mmap(NULL, map_size, prot, map_flags, fd, cfg.offset);
    close(fd);
    
    if (ptr == MAP_FAILED) {
        if (error) *error = MMAP_ERROR_MMAP;
        return false;
    }
    
    info->addr = ptr;
    info->size = map_size;
    info->file_size = file_size;
    info->read_only = cfg.read_only;
    info->private = cfg.private;
    info->filename = filename;
    
    if (error) *error = MMAP_OK;
    return true;
}

void mmap_free(void *ptr, size_t size) {
    mmap_free_ex(ptr, size, NULL);
}

bool mmap_free_ex(void *ptr, size_t size, mmap_error_t *error) {
    if (!ptr) {
        if (error) *error = MMAP_ERROR_NULL_PTR;
        return false;
    }
    
    if (munmap(ptr, size) == -1) {
        if (error) *error = MMAP_ERROR_UNMAP;
        return false;
    }
    
    if (error) *error = MMAP_OK;
    return true;
}

bool mmap_sync(void *ptr, size_t size, mmap_error_t *error) {
    if (!ptr) {
        if (error) *error = MMAP_ERROR_NULL_PTR;
        return false;
    }
    
    if (msync(ptr, size, MS_SYNC) == -1) {
        if (error) *error = MMAP_ERROR_UNMAP;
        return false;
    }
    
    if (error) *error = MMAP_OK;
    return true;
}

void* mmap_anonymous(size_t size, bool read_only, mmap_error_t *error) {
    if (size == 0) {
        if (error) *error = MMAP_ERROR_INVALID_ARGS;
        return NULL;
    }
    
    int prot = PROT_READ;
    if (!read_only) {
        prot |= PROT_WRITE;
    }
    
    void *ptr = mmap(NULL, size, prot, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    
    if (ptr == MAP_FAILED) {
        if (error) *error = MMAP_ERROR_MMAP;
        return NULL;
    }
    
    if (error) *error = MMAP_OK;
    return ptr;
}

const char* mmap_error_string(mmap_error_t error) {
    switch (error) {
        case MMAP_OK: return "Success";
        case MMAP_ERROR_NULL_PTR: return "Null pointer error";
        case MMAP_ERROR_OPEN_FILE: return "Failed to open file";
        case MMAP_ERROR_FILE_SIZE: return "Failed to get file size";
        case MMAP_ERROR_MMAP: return "Memory mapping failed";
        case MMAP_ERROR_INVALID_ARGS: return "Invalid arguments";
        case MMAP_ERROR_UNMAP: return "Unmap failed";
        default: return "Unknown error";
    }
}
