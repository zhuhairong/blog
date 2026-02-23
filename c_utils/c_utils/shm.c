#include "shm.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

void* shm_open_map(const char *name, size_t size) {
    int fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    if (fd == -1) return NULL;
    if (ftruncate(fd, size) == -1) { close(fd); return NULL; }
    void *ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    return ptr == MAP_FAILED ? NULL : ptr;
}

void* shm_open_map_ex(const char *name, size_t size, const shm_config_t *config, shm_state_t *state) {
    if (!name || size == 0) {
        if (state) {
            state->last_error = SHM_ERROR_INVALID_PARAMS;
        }
        return NULL;
    }
    
    int flags = O_RDWR;
    mode_t mode = 0666;
    
    if (config) {
        if (config->create_if_not_exists) {
            flags |= O_CREAT;
        }
        if (config->exclusive) {
            flags |= O_EXCL;
        }
        if (config->read_only) {
            flags = O_RDONLY;
        }
        mode = config->permissions;
    } else {
        flags |= O_CREAT;
    }
    
    int fd = shm_open(name, flags, mode);
    if (fd == -1) {
        if (state) {
            state->last_error = SHM_ERROR_OPEN;
            state->error_code = errno;
        }
        return NULL;
    }
    
    if (ftruncate(fd, size) == -1) {
        close(fd);
        if (state) {
            state->last_error = SHM_ERROR_FTRUNCATE;
            state->error_code = errno;
        }
        return NULL;
    }
    
    int prot = (config && config->read_only) ? PROT_READ : (PROT_READ | PROT_WRITE);
    void *ptr = mmap(0, size, prot, MAP_SHARED, fd, 0);
    close(fd);
    
    if (ptr == MAP_FAILED) {
        if (state) {
            state->last_error = SHM_ERROR_MAP;
            state->error_code = errno;
        }
        return NULL;
    }
    
    if (state) {
        state->last_error = SHM_OK;
        state->error_code = 0;
        state->actual_size = size;
        state->is_mapped = true;
        state->is_read_only = (config && config->read_only);
        state->is_created = (flags & O_CREAT) ? true : false;
    }
    
    return ptr;
}

void shm_close_unmap(void *ptr, const char *name, size_t size) {
    if (ptr && ptr != MAP_FAILED) {
        munmap(ptr, size);
    }
    if (name) {
        shm_unlink(name);
    }
}

shm_error_t shm_close_unmap_ex(void *ptr, const char *name, size_t size, const shm_config_t *config, shm_state_t *state) {
    (void)config;
    
    if (!ptr || ptr == MAP_FAILED) {
        if (state) {
            state->last_error = SHM_ERROR_INVALID_PARAMS;
        }
        return SHM_ERROR_INVALID_PARAMS;
    }
    
    if (munmap(ptr, size) != 0) {
        if (state) {
            state->last_error = SHM_ERROR_UNMAP;
            state->error_code = errno;
        }
        return SHM_ERROR_UNMAP;
    }
    
    if (name && (!config || config->unlink_on_close)) {
        if (shm_unlink(name) != 0) {
            if (state) {
                state->last_error = SHM_ERROR_UNLINK;
                state->error_code = errno;
            }
            return SHM_ERROR_UNLINK;
        }
    }
    
    if (state) {
        state->last_error = SHM_OK;
        state->is_mapped = false;
    }
    
    return SHM_OK;
}
