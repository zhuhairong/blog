#include "thread_local_storage.h"
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

static tls_state_t g_state = {0};
static bool g_initialized = false;

void tls_config_init(tls_config_t *config) {
    if (!config) return;
    memset(config, 0, sizeof(tls_config_t));
    config->enable_statistics = true;
    config->enable_destructors = true;
    config->enable_validation = true;
    config->enable_thread_safety = true;
    config->max_keys = 1024;
    config->initial_capacity = 16;
    config->use_malloc = true;
    config->auto_cleanup = false;
}

void tls_state_init(tls_state_t *state) {
    if (!state) return;
    memset(state, 0, sizeof(tls_state_t));
    state->is_initialized = true;
}

int tls_key_create(tls_key_t *key, void (*destructor)(void*)) {
    if (!key) return TLS_ERROR_KEY_NULL;
    return pthread_key_create(key, destructor);
}

tls_error_t tls_key_create_ex(tls_key_t *key, void (*destructor)(void*),
                             const tls_config_t *config,
                             tls_state_t *state) {
    if (!key) return TLS_ERROR_KEY_NULL;
    
    int result = pthread_key_create(key, destructor);
    if (result != 0) {
        return TLS_ERROR_KEY_CREATE;
    }
    
    if (!g_initialized) {
        tls_state_init(&g_state);
        g_initialized = true;
    }
    
    g_state.key_creations++;
    g_state.active_keys++;
    
    if (state) {
        *state = g_state;
    }
    
    return TLS_OK;
}

int tls_set_value(tls_key_t key, const void *value) {
    return pthread_setspecific(key, value);
}

tls_error_t tls_set_value_ex(tls_key_t key, const void *value,
                            const tls_config_t *config,
                            tls_state_t *state) {
    int result = pthread_setspecific(key, value);
    if (result != 0) {
        return TLS_ERROR_SET_VALUE;
    }
    
    g_state.value_sets++;
    
    if (state) {
        *state = g_state;
    }
    
    return TLS_OK;
}

void* tls_get_value(tls_key_t key) {
    return pthread_getspecific(key);
}

tls_error_t tls_get_value_ex(tls_key_t key, void **value,
                            const tls_config_t *config,
                            tls_state_t *state) {
    if (!value) return TLS_ERROR_VALUE_NULL;
    
    *value = pthread_getspecific(key);
    
    g_state.value_gets++;
    
    if (state) {
        *state = g_state;
    }
    
    return TLS_OK;
}

tls_error_t tls_key_delete(tls_key_t key, tls_state_t *state) {
    int result = pthread_key_delete(key);
    if (result != 0) {
        return TLS_ERROR_KEY_DELETE;
    }
    
    g_state.key_deletions++;
    if (g_state.active_keys > 0) {
        g_state.active_keys--;
    }
    
    if (state) {
        *state = g_state;
    }
    
    return TLS_OK;
}

tls_error_t tls_batch_key_create(tls_key_t *keys, size_t count,
                               void (*destructor)(void*),
                               const tls_config_t *config,
                               tls_state_t *state) {
    if (!keys) return TLS_ERROR_KEY_NULL;
    
    for (size_t i = 0; i < count; i++) {
        tls_error_t err = tls_key_create_ex(&keys[i], destructor, config, NULL);
        if (err != TLS_OK) {
            for (size_t j = 0; j < i; j++) {
                pthread_key_delete(keys[j]);
            }
            return err;
        }
    }
    
    if (state) {
        *state = g_state;
    }
    
    return TLS_OK;
}

tls_error_t tls_batch_key_delete(const tls_key_t *keys, size_t count,
                               tls_state_t *state) {
    if (!keys) return TLS_ERROR_KEY_NULL;
    
    for (size_t i = 0; i < count; i++) {
        pthread_key_delete(keys[i]);
        g_state.key_deletions++;
        if (g_state.active_keys > 0) {
            g_state.active_keys--;
        }
    }
    
    if (state) {
        *state = g_state;
    }
    
    return TLS_OK;
}

tls_error_t tls_batch_set_value(const tls_key_t *keys, const void **values,
                               size_t count,
                               const tls_config_t *config,
                               tls_state_t *state) {
    if (!keys || !values) return TLS_ERROR_INVALID_PARAMS;
    
    for (size_t i = 0; i < count; i++) {
        int result = pthread_setspecific(keys[i], values[i]);
        if (result != 0) {
            return TLS_ERROR_BATCH_OPERATION;
        }
        g_state.value_sets++;
    }
    
    if (state) {
        *state = g_state;
    }
    
    return TLS_OK;
}

tls_error_t tls_batch_get_value(const tls_key_t *keys, void **values,
                               size_t count,
                               const tls_config_t *config,
                               tls_state_t *state) {
    if (!keys || !values) return TLS_ERROR_INVALID_PARAMS;
    
    for (size_t i = 0; i < count; i++) {
        values[i] = pthread_getspecific(keys[i]);
        g_state.value_gets++;
    }
    
    if (state) {
        *state = g_state;
    }
    
    return TLS_OK;
}

tls_error_t tls_validate_key(tls_key_t key, bool *valid,
                            tls_state_t *state) {
    if (!valid) return TLS_ERROR_INVALID_PARAMS;
    
    void *value = pthread_getspecific(key);
    *valid = true;
    
    if (state) {
        *state = g_state;
    }
    
    return TLS_OK;
}

tls_error_t tls_get_thread_id(uint64_t *thread_id,
                             tls_state_t *state) {
    if (!thread_id) return TLS_ERROR_INVALID_PARAMS;
    
    *thread_id = (uint64_t)(uintptr_t)pthread_self();
    
    if (state) {
        *state = g_state;
    }
    
    return TLS_OK;
}

tls_error_t tls_cleanup_thread(tls_state_t *state) {
    g_state.active_threads = 0;
    
    if (state) {
        *state = g_state;
    }
    
    return TLS_OK;
}

tls_error_t tls_load_config(const char *filename,
                           tls_config_t *config,
                           tls_state_t *state) {
    if (!filename || !config) return TLS_ERROR_INVALID_PARAMS;
    
    tls_config_init(config);
    
    if (state) {
        *state = g_state;
    }
    
    return TLS_OK;
}

tls_error_t tls_save_config(const char *filename,
                           const tls_config_t *config,
                           tls_state_t *state) {
    if (!filename || !config) return TLS_ERROR_INVALID_PARAMS;
    
    if (state) {
        *state = g_state;
    }
    
    return TLS_OK;
}

tls_error_t tls_save_state(const char *filename,
                          const tls_state_t *state) {
    if (!filename || !state) return TLS_ERROR_INVALID_PARAMS;
    return TLS_OK;
}

tls_error_t tls_get_statistics(tls_state_t *state) {
    if (!state) return TLS_ERROR_STATISTICS;
    
    *state = g_state;
    return TLS_OK;
}

void tls_reset_state(tls_state_t *state) {
    if (state) {
        tls_state_init(state);
    }
    tls_state_init(&g_state);
}

const char* tls_strerror(const tls_state_t *state) {
    if (!state) return "Invalid state";
    
    switch (state->last_error) {
        case TLS_OK: return "Success";
        case TLS_ERROR_INVALID_PARAMS: return "Invalid parameters";
        case TLS_ERROR_KEY_CREATE: return "Failed to create key";
        case TLS_ERROR_KEY_DELETE: return "Failed to delete key";
        case TLS_ERROR_SET_VALUE: return "Failed to set value";
        case TLS_ERROR_GET_VALUE: return "Failed to get value";
        case TLS_ERROR_MEMORY: return "Memory allocation error";
        case TLS_ERROR_DESTRUCTOR_NULL: return "Destructor is null";
        case TLS_ERROR_KEY_NULL: return "Key is null";
        case TLS_ERROR_VALUE_NULL: return "Value is null";
        case TLS_ERROR_THREAD_NOT_FOUND: return "Thread not found";
        case TLS_ERROR_FILE_OPEN: return "Failed to open file";
        case TLS_ERROR_FILE_READ: return "Failed to read file";
        case TLS_ERROR_FILE_WRITE: return "Failed to write file";
        case TLS_ERROR_BATCH_OPERATION: return "Batch operation failed";
        case TLS_ERROR_STATISTICS: return "Statistics error";
        default: return "Unknown error";
    }
}
