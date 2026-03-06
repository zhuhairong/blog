#include "timer.h"
#include <string.h>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/time.h>
    #include <time.h>
#endif

static uint64_t time_now_ms(void) {
#ifdef _WIN32
    FILETIME ft;
    ULARGE_INTEGER uli;
    GetSystemTimeAsFileTime(&ft);
    uli.LowPart = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;
    return (uli.QuadPart / 10000ULL) - 11644473600000ULL;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000ULL + (uint64_t)tv.tv_usec / 1000ULL;
#endif
}

const char* timer_strerror(const timer_state_t *state) {
    if (!state) {
        return "NULL state pointer";
    }
    
    switch (state->last_error) {
        case TIMER_OK:                   return "Success";
        case TIMER_ERROR_INVALID_PARAMS: return "Invalid parameters";
        case TIMER_ERROR_SET:            return "Failed to set timer";
        case TIMER_ERROR_UPDATE:         return "Failed to update timer";
        case TIMER_ERROR_CANCEL:         return "Failed to cancel timer";
        case TIMER_ERROR_DELETE:         return "Failed to delete timer";
        case TIMER_ERROR_MEMORY:         return "Memory allocation failed";
        case TIMER_ERROR_CALLBACK_NULL:  return "Callback function is NULL";
        case TIMER_ERROR_TIMER_NULL:     return "Timer pointer is NULL";
        case TIMER_ERROR_DELAY_ZERO:     return "Delay time cannot be zero";
        case TIMER_ERROR_NOT_ACTIVE:     return "Timer is not active";
        case TIMER_ERROR_ALREADY_ACTIVE: return "Timer is already active";
        case TIMER_ERROR_FILE_OPEN:      return "Failed to open file";
        case TIMER_ERROR_FILE_READ:      return "Failed to read file";
        case TIMER_ERROR_FILE_WRITE:     return "Failed to write file";
        case TIMER_ERROR_STATISTICS:     return "Failed to get statistics";
        case TIMER_ERROR_BATCH_OPERATION: return "Batch operation failed";
        default:                         return "Unknown error";
    }
}

void timer_config_init(timer_config_t *config) {
    if (!config) return;
    
    config->enable_statistics = false;
    config->enable_validation = true;
    config->enable_thread_safety = false;
    config->auto_cleanup = false;
    config->max_timers = 1024;
    config->buffer_size = 4096;
    config->max_delay = 86400000ULL;
    config->use_high_resolution = false;
}

void timer_state_init(timer_state_t *state) {
    if (!state) return;
    
    state->last_error = TIMER_OK;
    state->timer_sets = 0;
    state->timer_triggers = 0;
    state->timer_cancels = 0;
    state->is_initialized = true;
    state->active_timers = 0;
    state->average_delay = 0;
}

void timer_reset_state(timer_state_t *state) {
    if (!state) return;
    
    state->last_error = TIMER_OK;
    state->timer_sets = 0;
    state->timer_triggers = 0;
    state->timer_cancels = 0;
    state->active_timers = 0;
    state->average_delay = 0;
}

timer_error_t timer_validate(const cutils_timer_t *t, bool *valid, timer_state_t *state) {
    if (!t || !valid) {
        if (state) state->last_error = TIMER_ERROR_INVALID_PARAMS;
        return TIMER_ERROR_INVALID_PARAMS;
    }
    
    *valid = true;
    
    if (t->active && t->cb == NULL) {
        *valid = false;
    }
    
    if (state) {
        state->last_error = TIMER_OK;
    }
    
    return TIMER_OK;
}

timer_error_t timer_get_statistics(timer_state_t *state) {
    if (!state) {
        return TIMER_ERROR_INVALID_PARAMS;
    }
    
    if (!state->is_initialized) {
        state->last_error = TIMER_ERROR_STATISTICS;
        return TIMER_ERROR_STATISTICS;
    }
    
    state->last_error = TIMER_OK;
    return TIMER_OK;
}

static bool timer_validate_params(const cutils_timer_t *t, timer_callback_t cb, 
                                   uint32_t delay_ms, const timer_config_t *config) {
    (void)config;
    
    if (!t) return false;
    if (!cb) return false;
    if (delay_ms == 0) return false;
    
    return true;
}

static timer_error_t timer_validate_delay(uint32_t delay_ms, const timer_config_t *config) {
    if (delay_ms == 0) {
        return TIMER_ERROR_DELAY_ZERO;
    }
    
    if (config && config->max_delay > 0 && delay_ms > config->max_delay) {
        return TIMER_ERROR_INVALID_PARAMS;
    }
    
    return TIMER_OK;
}

void timer_set(cutils_timer_t *t, uint32_t delay_ms, timer_callback_t cb, void *data) {
    if (!t) return;
    if (!cb) return;
    if (delay_ms == 0) return;
    
    uint64_t now = time_now_ms();
    t->target_ms = now + delay_ms;
    t->cb = cb;
    t->data = data;
    t->active = true;
}

timer_error_t timer_set_ex(cutils_timer_t *t, uint32_t delay_ms, timer_callback_t cb, void *data,
                         const timer_config_t *config,
                         timer_state_t *state) {
    if (!t) {
        if (state) state->last_error = TIMER_ERROR_TIMER_NULL;
        return TIMER_ERROR_TIMER_NULL;
    }
    if (!cb) {
        if (state) state->last_error = TIMER_ERROR_CALLBACK_NULL;
        return TIMER_ERROR_CALLBACK_NULL;
    }
    
    timer_error_t delay_err = timer_validate_delay(delay_ms, config);
    if (delay_err != TIMER_OK) {
        if (state) state->last_error = delay_err;
        return delay_err;
    }
    
    if (config && config->enable_validation) {
        if (!timer_validate_params(t, cb, delay_ms, config)) {
            if (state) state->last_error = TIMER_ERROR_INVALID_PARAMS;
            return TIMER_ERROR_INVALID_PARAMS;
        }
    }
    
    if (t->active && config && config->enable_validation) {
        if (state) state->last_error = TIMER_ERROR_ALREADY_ACTIVE;
        return TIMER_ERROR_ALREADY_ACTIVE;
    }
    
    uint64_t now = time_now_ms();
    t->target_ms = now + delay_ms;
    t->cb = cb;
    t->data = data;
    t->active = true;
    
    if (state) {
        state->last_error = TIMER_OK;
        state->timer_sets++;
        state->active_timers++;
        
        if (state->timer_sets > 0) {
            state->average_delay = 
                (state->average_delay * (state->timer_sets - 1) + delay_ms) / state->timer_sets;
        }
    }
    
    return TIMER_OK;
}

void timer_update(cutils_timer_t *t) {
    if (!t || !t->active) return;
    
    uint64_t now = time_now_ms();
    if (now >= t->target_ms) {
        t->active = false;
        if (t->cb) t->cb(t->data);
    }
}

timer_error_t timer_update_ex(cutils_timer_t *t,
                           const timer_config_t *config,
                           timer_state_t *state) {
    (void)config;
    
    if (!t) {
        if (state) state->last_error = TIMER_ERROR_TIMER_NULL;
        return TIMER_ERROR_TIMER_NULL;
    }
    if (!t->active) {
        if (state) state->last_error = TIMER_ERROR_NOT_ACTIVE;
        return TIMER_ERROR_NOT_ACTIVE;
    }
    
    uint64_t now = time_now_ms();
    bool triggered = false;
    
    if (now >= t->target_ms) {
        t->active = false;
        triggered = true;
        if (t->cb) t->cb(t->data);
    }
    
    if (state) {
        state->last_error = TIMER_OK;
        if (triggered) {
            state->timer_triggers++;
            if (state->active_timers > 0) {
                state->active_timers--;
            }
        }
    }
    
    return TIMER_OK;
}

timer_error_t timer_cancel(cutils_timer_t *t, timer_state_t *state) {
    if (!t) {
        if (state) state->last_error = TIMER_ERROR_TIMER_NULL;
        return TIMER_ERROR_TIMER_NULL;
    }
    if (!t->active) {
        if (state) state->last_error = TIMER_ERROR_NOT_ACTIVE;
        return TIMER_ERROR_NOT_ACTIVE;
    }
    
    t->active = false;
    
    if (state) {
        state->last_error = TIMER_OK;
        state->timer_cancels++;
        if (state->active_timers > 0) {
            state->active_timers--;
        }
    }
    
    return TIMER_OK;
}

timer_error_t timer_is_active(const cutils_timer_t *t, bool *active) {
    if (!t || !active) return TIMER_ERROR_INVALID_PARAMS;
    
    *active = t->active;
    return TIMER_OK;
}

timer_error_t timer_get_remaining(const cutils_timer_t *t, uint32_t *remaining_ms,
                               timer_state_t *state) {
    if (!t || !remaining_ms) {
        if (state) state->last_error = TIMER_ERROR_INVALID_PARAMS;
        return TIMER_ERROR_INVALID_PARAMS;
    }
    if (!t->active) {
        if (state) state->last_error = TIMER_ERROR_NOT_ACTIVE;
        return TIMER_ERROR_NOT_ACTIVE;
    }
    
    uint64_t now = time_now_ms();
    if (now >= t->target_ms) {
        *remaining_ms = 0;
    } else {
        *remaining_ms = (uint32_t)(t->target_ms - now);
    }
    
    if (state) {
        state->last_error = TIMER_OK;
    }
    
    return TIMER_OK;
}

timer_error_t timer_set_repeating(timer_extended_t *t, uint32_t delay_ms,
                                uint32_t interval_ms, size_t max_repeats,
                                timer_callback_t cb, void *data,
                                const timer_config_t *config,
                                timer_state_t *state) {
    if (!t) {
        if (state) state->last_error = TIMER_ERROR_TIMER_NULL;
        return TIMER_ERROR_TIMER_NULL;
    }
    if (!cb) {
        if (state) state->last_error = TIMER_ERROR_CALLBACK_NULL;
        return TIMER_ERROR_CALLBACK_NULL;
    }
    
    timer_error_t delay_err = timer_validate_delay(delay_ms, config);
    if (delay_err != TIMER_OK) {
        if (state) state->last_error = delay_err;
        return delay_err;
    }
    
    if (interval_ms == 0) {
        if (state) state->last_error = TIMER_ERROR_DELAY_ZERO;
        return TIMER_ERROR_DELAY_ZERO;
    }
    
    uint64_t now = time_now_ms();
    t->target_ms = now + delay_ms;
    t->start_ms = now;
    t->delay_ms = delay_ms;
    t->cb = cb;
    t->data = data;
    t->active = true;
    t->repeating = true;
    t->repeat_interval = interval_ms;
    t->repeat_count = 0;
    t->max_repeats = max_repeats;
    
    if (config) {
        t->config = *config;
    } else {
        timer_config_init(&t->config);
    }
    
    if (state) {
        state->last_error = TIMER_OK;
        state->timer_sets++;
        state->active_timers++;
    }
    
    return TIMER_OK;
}

timer_error_t timer_update_extended(timer_extended_t *t,
                                 const timer_config_t *config,
                                 timer_state_t *state) {
    (void)config;
    
    if (!t) {
        if (state) state->last_error = TIMER_ERROR_TIMER_NULL;
        return TIMER_ERROR_TIMER_NULL;
    }
    if (!t->active) {
        if (state) state->last_error = TIMER_ERROR_NOT_ACTIVE;
        return TIMER_ERROR_NOT_ACTIVE;
    }
    
    uint64_t now = time_now_ms();
    bool triggered = false;
    
    if (now >= t->target_ms) {
        if (t->cb) t->cb(t->data);
        t->repeat_count++;
        triggered = true;
        
        if (t->repeating && (t->max_repeats == 0 || t->repeat_count < t->max_repeats)) {
            t->target_ms = now + t->repeat_interval;
        } else {
            t->active = false;
            if (state && state->active_timers > 0) {
                state->active_timers--;
            }
        }
    }
    
    if (state) {
        state->last_error = TIMER_OK;
        if (triggered) {
            state->timer_triggers++;
        }
    }
    
    return TIMER_OK;
}

timer_error_t timer_cancel_extended(timer_extended_t *t,
                                 timer_state_t *state) {
    if (!t) {
        if (state) state->last_error = TIMER_ERROR_TIMER_NULL;
        return TIMER_ERROR_TIMER_NULL;
    }
    if (!t->active) {
        if (state) state->last_error = TIMER_ERROR_NOT_ACTIVE;
        return TIMER_ERROR_NOT_ACTIVE;
    }
    
    t->active = false;
    t->repeating = false;
    
    if (state) {
        state->last_error = TIMER_OK;
        state->timer_cancels++;
        if (state->active_timers > 0) {
            state->active_timers--;
        }
    }
    
    return TIMER_OK;
}

timer_error_t timer_batch_set(cutils_timer_t *timers, size_t count,
                            const uint32_t *delay_ms,
                            const timer_callback_t *cbs,
                            void **datas,
                            const timer_config_t *config,
                            timer_state_t *state) {
    if (!timers || !delay_ms || !cbs || count == 0) {
        if (state) state->last_error = TIMER_ERROR_INVALID_PARAMS;
        return TIMER_ERROR_INVALID_PARAMS;
    }
    
    if (config && config->max_timers > 0 && count > config->max_timers) {
        if (state) state->last_error = TIMER_ERROR_BATCH_OPERATION;
        return TIMER_ERROR_BATCH_OPERATION;
    }
    
    size_t success_count = 0;
    timer_error_t last_error = TIMER_OK;
    
    for (size_t i = 0; i < count; i++) {
        void *data = datas ? datas[i] : NULL;
        timer_error_t err = timer_set_ex(&timers[i], delay_ms[i], cbs[i], data, config, NULL);
        
        if (err != TIMER_OK) {
            last_error = err;
        } else {
            success_count++;
        }
    }
    
    if (state) {
        state->last_error = (success_count == count) ? TIMER_OK : TIMER_ERROR_BATCH_OPERATION;
        state->timer_sets += success_count;
        state->active_timers += success_count;
    }
    
    return (success_count == count) ? TIMER_OK : TIMER_ERROR_BATCH_OPERATION;
}

timer_error_t timer_batch_update(cutils_timer_t *timers, size_t count,
                               const timer_config_t *config,
                               timer_state_t *state) {
    if (!timers || count == 0) {
        if (state) state->last_error = TIMER_ERROR_INVALID_PARAMS;
        return TIMER_ERROR_INVALID_PARAMS;
    }
    
    size_t triggered_count = 0;
    timer_error_t last_error = TIMER_OK;
    
    for (size_t i = 0; i < count; i++) {
        timer_error_t err = timer_update_ex(&timers[i], config, NULL);
        if (err != TIMER_OK && err != TIMER_ERROR_NOT_ACTIVE) {
            last_error = err;
        }
    }
    
    if (state) {
        state->last_error = last_error;
    }
    
    return TIMER_OK;
}

timer_error_t timer_batch_cancel(cutils_timer_t *timers, size_t count,
                               timer_state_t *state) {
    if (!timers || count == 0) {
        if (state) state->last_error = TIMER_ERROR_INVALID_PARAMS;
        return TIMER_ERROR_INVALID_PARAMS;
    }
    
    size_t success_count = 0;
    
    for (size_t i = 0; i < count; i++) {
        if (timers[i].active) {
            timers[i].active = false;
            success_count++;
        }
    }
    
    if (state) {
        state->last_error = TIMER_OK;
        state->timer_cancels += success_count;
        if (state->active_timers >= success_count) {
            state->active_timers -= success_count;
        } else {
            state->active_timers = 0;
        }
    }
    
    return TIMER_OK;
}
