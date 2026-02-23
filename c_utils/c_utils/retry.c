#include "retry.h"
#include <stdlib.h>
#include <time.h>

// 简单的毫秒级睡眠函数（跨平台）
static void sleep_ms(int ms) {
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

retry_config_t retry_default_config(void) {
    retry_config_t config = {
        .max_attempts = 3,
        .base_delay_ms = 100,
        .max_delay_ms = 10000,
        .max_total_time_ms = 60000,
        .delay_strategy = RETRY_DELAY_EXPONENTIAL,
        .jitter_factor = 0.1,
        .backoff_factor = 2.0,
        .exponential_backoff = true,
        .use_monotonic_time = true
    };
    return config;
}

void retry_init(retry_policy_t *policy, int max_attempts, int base_delay_ms) {
    if (!policy) return;
    
    policy->config = retry_default_config();
    policy->config.max_attempts = max_attempts;
    policy->config.base_delay_ms = base_delay_ms;
    
    policy->state.current_attempt = 0;
    policy->state.last_delay_ms = 0;
    policy->state.next_delay_ms = policy->config.base_delay_ms;
    policy->state.total_delay_ms = 0;
    policy->state.exhausted = false;
    policy->state.timed_out = false;
    policy->start_time = time(NULL);
}

bool retry_init_ex(retry_policy_t *policy, const retry_config_t *config, retry_error_t *error) {
    if (!policy) {
        if (error) *error = RETRY_ERROR_NULL_PTR;
        return false;
    }
    
    if (config) {
        policy->config = *config;
    } else {
        policy->config = retry_default_config();
    }
    
    policy->state.current_attempt = 0;
    policy->state.last_delay_ms = 0;
    policy->state.next_delay_ms = policy->config.base_delay_ms;
    policy->state.total_delay_ms = 0;
    policy->state.exhausted = false;
    policy->state.timed_out = false;
    policy->start_time = time(NULL);
    
    if (error) *error = RETRY_OK;
    return true;
}

bool retry_should_retry(retry_policy_t *policy, retry_error_t *error) {
    if (!policy) {
        if (error) *error = RETRY_ERROR_NULL_PTR;
        return false;
    }
    
    // 检查是否已达到最大尝试次数
    if (policy->state.current_attempt >= policy->config.max_attempts) {
        policy->state.exhausted = true;
        if (error) *error = RETRY_ERROR_MAX_ATTEMPTS_REACHED;
        return false;
    }
    
    // 检查是否超时
    if (policy->config.max_total_time_ms > 0) {
        time_t elapsed = (time(NULL) - policy->start_time) * 1000;
        if (elapsed >= policy->config.max_total_time_ms) {
            policy->state.timed_out = true;
            if (error) *error = RETRY_ERROR_TIMEOUT;
            return false;
        }
    }
    
    if (error) *error = RETRY_OK;
    return true;
}

retry_error_t retry_wait(retry_policy_t *policy) {
    if (!policy) return RETRY_ERROR_NULL_PTR;
    
    if (policy->state.current_attempt > 0) {
        int delay = policy->state.next_delay_ms;
        
        // 应用抖动
        if (policy->config.jitter_factor > 0) {
            int jitter = (int)(delay * policy->config.jitter_factor * (rand() % 100 - 50) / 50.0);
            delay += jitter;
        }
        
        // 限制最大延迟
        if (delay > policy->config.max_delay_ms) {
            delay = policy->config.max_delay_ms;
        }
        
        sleep_ms(delay);
        
        policy->state.last_delay_ms = delay;
        policy->state.total_delay_ms += delay;
        
        // 计算下次延迟
        if (policy->config.exponential_backoff) {
            policy->state.next_delay_ms = (int)(policy->state.next_delay_ms * policy->config.backoff_factor);
            if (policy->state.next_delay_ms > policy->config.max_delay_ms) {
                policy->state.next_delay_ms = policy->config.max_delay_ms;
            }
        }
    }
    
    policy->state.current_attempt++;
    return RETRY_OK;
}

bool retry_execute(retry_policy_t *policy, retry_callback_t callback, void *user_data, retry_error_t *error) {
    if (!policy || !callback) {
        if (error) *error = RETRY_ERROR_NULL_PTR;
        return false;
    }
    
    retry_error_t err;
    
    while (retry_should_retry(policy, &err)) {
        if (callback(user_data, policy->state.current_attempt, &err)) {
            if (error) *error = RETRY_OK;
            return true;
        }
        
        if (err != RETRY_OK) {
            // 回调返回错误，等待后重试
            retry_wait(policy);
        } else {
            // 回调成功
            if (error) *error = RETRY_OK;
            return true;
        }
    }
    
    if (error) *error = err;
    return false;
}

bool retry_reset(retry_policy_t *policy, retry_error_t *error) {
    if (!policy) {
        if (error) *error = RETRY_ERROR_NULL_PTR;
        return false;
    }
    
    policy->state.current_attempt = 0;
    policy->state.last_delay_ms = 0;
    policy->state.next_delay_ms = policy->config.base_delay_ms;
    policy->state.total_delay_ms = 0;
    policy->state.exhausted = false;
    policy->state.timed_out = false;
    policy->start_time = time(NULL);
    
    if (error) *error = RETRY_OK;
    return true;
}

int retry_get_attempts(const retry_policy_t *policy) {
    return policy ? policy->state.current_attempt : 0;
}

int retry_get_total_delay_ms(const retry_policy_t *policy) {
    return policy ? policy->state.total_delay_ms : 0;
}

bool retry_is_exhausted(const retry_policy_t *policy) {
    return policy ? policy->state.exhausted : true;
}

const char* retry_strerror(retry_error_t error) {
    switch (error) {
        case RETRY_OK: return "Success";
        case RETRY_ERROR_NULL_PTR: return "Null pointer";
        case RETRY_ERROR_INVALID_ARGS: return "Invalid arguments";
        case RETRY_ERROR_MAX_ATTEMPTS_REACHED: return "Max attempts reached";
        case RETRY_ERROR_DELAY_TOO_LONG: return "Delay too long";
        case RETRY_ERROR_TIMEOUT: return "Timeout";
        default: return "Unknown error";
    }
}
