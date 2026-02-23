#include "sem.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

sem_handle_t sem_create(const char *name, unsigned int value) {
    int flags = O_CREAT | O_RDWR;
    mode_t mode = 0666;
    return sem_open(name, flags, mode, value);
}

sem_handle_t sem_create_ex(const char *name, unsigned int value, const sem_config_t *config, sem_state_t *state) {
    int flags = O_RDWR;
    mode_t mode = 0666;
    
    if (config) {
        if (config->create_if_not_exists) {
            flags |= O_CREAT;
        }
        if (config->exclusive) {
            flags |= O_EXCL;
        }
        mode = config->max_value > 0 ? 0666 : 0666;
    } else {
        flags |= O_CREAT;
    }
    
    sem_handle_t sem = sem_open(name, flags, mode, value);
    
    if (state) {
        if (sem == SEM_FAILED) {
            state->last_error = SEM_ERROR_CREATE;
            state->error_code = errno;
            state->is_initialized = false;
        } else {
            state->last_error = SEM_OK;
            state->error_code = 0;
            state->is_initialized = true;
            state->is_named = (name != NULL);
            
            int val;
            if (sem_getvalue(sem, &val) == 0) {
                state->current_value = (unsigned int)val;
            }
        }
    }
    
    return sem;
}

sem_handle_t sem_open_ex(const char *name, sem_state_t *state) {
    sem_handle_t sem = sem_open(name, 0);
    
    if (state) {
        if (sem == SEM_FAILED) {
            state->last_error = SEM_ERROR_OPEN;
            state->error_code = errno;
            state->is_initialized = false;
        } else {
            state->last_error = SEM_OK;
            state->error_code = 0;
            state->is_initialized = true;
            state->is_named = true;
            
            int val;
            if (sem_getvalue(sem, &val) == 0) {
                state->current_value = (unsigned int)val;
            }
        }
    }
    
    return sem;
}

void sem_close_delete(sem_handle_t sem, const char *name) {
    if (sem != SEM_FAILED && sem != NULL) {
        sem_close(sem);
        if (name) {
            sem_unlink(name);
        }
    }
}

sem_error_t sem_close_delete_ex(sem_handle_t sem, const char *name, sem_state_t *state) {
    if (sem == SEM_FAILED || sem == NULL) {
        if (state) {
            state->last_error = SEM_ERROR_INVALID_PARAMS;
        }
        return SEM_ERROR_INVALID_PARAMS;
    }
    
    if (sem_close(sem) != 0) {
        if (state) {
            state->last_error = SEM_ERROR_CLOSE;
            state->error_code = errno;
        }
        return SEM_ERROR_CLOSE;
    }
    
    if (name) {
        if (sem_unlink(name) != 0) {
            if (state) {
                state->last_error = SEM_ERROR_UNLINK;
                state->error_code = errno;
            }
            return SEM_ERROR_UNLINK;
        }
    }
    
    if (state) {
        state->last_error = SEM_OK;
        state->is_initialized = false;
    }
    
    return SEM_OK;
}

void sem_p(sem_handle_t sem) { 
    if (sem != SEM_FAILED && sem != NULL) {
        sem_wait(sem); 
    }
}

void sem_v(sem_handle_t sem) { 
    if (sem != SEM_FAILED && sem != NULL) {
        sem_post(sem); 
    }
}
