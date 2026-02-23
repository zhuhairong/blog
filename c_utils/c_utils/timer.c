#include "timer.h"
#include <sys/time.h>
#include <time.h>

// 获取当前时间（毫秒）
static uint64_t time_now_ms(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

void timer_set(cutils_timer_t *t, uint32_t delay_ms, timer_callback_t cb, void *data) {
    if (!t) return;
    t->target_ms = time_now_ms() + delay_ms;
    t->cb = cb;
    t->data = data;
    t->active = true;
}

timer_error_t timer_set_ex(cutils_timer_t *t, uint32_t delay_ms, timer_callback_t cb, void *data,
                         const timer_config_t *config,
                         timer_state_t *state) {
    (void)config;
    if (!t) return TIMER_ERROR_TIMER_NULL;
    if (!cb) return TIMER_ERROR_CALLBACK_NULL;
    if (delay_ms == 0) return TIMER_ERROR_DELAY_ZERO;
    
    timer_set(t, delay_ms, cb, data);
    
    if (state) {
        state->last_error = TIMER_OK;
        state->timer_sets++;
        state->active_timers++;
    }
    
    return TIMER_OK;
}

void timer_update(cutils_timer_t *t) {
    if (!t || !t->active) return;
    if (time_now_ms() >= t->target_ms) {
        t->active = false;
        if (t->cb) t->cb(t->data);
    }
}

timer_error_t timer_update_ex(cutils_timer_t *t,
                           const timer_config_t *config,
                           timer_state_t *state) {
    (void)config;
    if (!t) return TIMER_ERROR_TIMER_NULL;
    if (!t->active) return TIMER_ERROR_NOT_ACTIVE;
    
    timer_update(t);
    
    if (state) {
        state->last_error = TIMER_OK;
        if (!t->active) {
            state->timer_triggers++;
            state->active_timers--;
        }
    }
    
    return TIMER_OK;
}

timer_error_t timer_cancel(cutils_timer_t *t, timer_state_t *state) {
    if (!t) return TIMER_ERROR_TIMER_NULL;
    if (!t->active) return TIMER_ERROR_NOT_ACTIVE;
    
    t->active = false;
    
    if (state) {
        state->last_error = TIMER_OK;
        state->timer_cancels++;
        state->active_timers--;
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
    if (!t || !remaining_ms) return TIMER_ERROR_INVALID_PARAMS;
    if (!t->active) return TIMER_ERROR_NOT_ACTIVE;
    
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
    (void)config;
    if (!t) return TIMER_ERROR_TIMER_NULL;
    if (!cb) return TIMER_ERROR_CALLBACK_NULL;
    
    t->target_ms = time_now_ms() + delay_ms;
    t->start_ms = time_now_ms();
    t->delay_ms = delay_ms;
    t->cb = cb;
    t->data = data;
    t->active = true;
    t->repeating = true;
    t->repeat_interval = interval_ms;
    t->repeat_count = 0;
    t->max_repeats = max_repeats;
    
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
    if (!t) return TIMER_ERROR_TIMER_NULL;
    if (!t->active) return TIMER_ERROR_NOT_ACTIVE;
    
    uint64_t now = time_now_ms();
    if (now >= t->target_ms) {
        if (t->cb) t->cb(t->data);
        t->repeat_count++;
        
        if (t->repeating && (t->max_repeats == 0 || t->repeat_count < t->max_repeats)) {
            t->target_ms = now + t->repeat_interval;
        } else {
            t->active = false;
            if (state) {
                state->active_timers--;
            }
        }
        
        if (state) {
            state->timer_triggers++;
        }
    }
    
    if (state) {
        state->last_error = TIMER_OK;
    }
    
    return TIMER_OK;
}

timer_error_t timer_cancel_extended(timer_extended_t *t,
                                 timer_state_t *state) {
    if (!t) return TIMER_ERROR_TIMER_NULL;
    if (!t->active) return TIMER_ERROR_NOT_ACTIVE;
    
    t->active = false;
    t->repeating = false;
    
    if (state) {
        state->last_error = TIMER_OK;
        state->timer_cancels++;
        state->active_timers--;
    }
    
    return TIMER_OK;
}
