#include "rate_limiter_leaky.h"
#include <string.h>

leaky_bucket_config_t leaky_bucket_default_config(void) {
    leaky_bucket_config_t config = {
        .capacity = 100.0,
        .rate = 10.0,
        .initial_water = 0.0,
        .use_monotonic_time = true,
        .allow_burst = true
    };
    return config;
}

void leaky_init(leaky_bucket_t *lb, double capacity, double rate) {
    if (!lb) return;
    
    lb->capacity = capacity;
    lb->rate = rate;
    lb->water = 0;
    lb->last_time = time(NULL);
    
    lb->config = leaky_bucket_default_config();
    lb->config.capacity = capacity;
    lb->config.rate = rate;
}

bool leaky_init_ex(leaky_bucket_t *lb, const leaky_bucket_config_t *config, leaky_bucket_error_t *error) {
    if (!lb) {
        if (error) *error = LEAKY_BUCKET_ERROR_NULL_PTR;
        return false;
    }
    
    if (!config) {
        if (error) *error = LEAKY_BUCKET_ERROR_NULL_PTR;
        return false;
    }
    
    if (config->capacity <= 0) {
        if (error) *error = LEAKY_BUCKET_ERROR_CAPACITY_TOO_SMALL;
        return false;
    }
    
    if (config->rate <= 0) {
        if (error) *error = LEAKY_BUCKET_ERROR_RATE_TOO_SLOW;
        return false;
    }
    
    lb->capacity = config->capacity;
    lb->rate = config->rate;
    lb->water = config->initial_water;
    lb->last_time = time(NULL);
    lb->config = *config;
    
    if (error) *error = LEAKY_BUCKET_OK;
    return true;
}

static void update_water(leaky_bucket_t *lb) {
    time_t now = time(NULL);
    double elapsed = (double)(now - lb->last_time);
    lb->water -= elapsed * lb->rate;
    if (lb->water < 0) lb->water = 0;
    lb->last_time = now;
}

bool leaky_consume(leaky_bucket_t *lb, double amount) {
    if (!lb) return false;
    
    update_water(lb);
    
    if (lb->water + amount <= lb->capacity) {
        lb->water += amount;
        return true;
    }
    
    return false;
}

bool leaky_consume_ex(leaky_bucket_t *lb, double amount, leaky_bucket_error_t *error) {
    if (!lb) {
        if (error) *error = LEAKY_BUCKET_ERROR_NULL_PTR;
        return false;
    }
    
    if (amount < 0) {
        if (error) *error = LEAKY_BUCKET_ERROR_INVALID_ARGS;
        return false;
    }
    
    if (amount > lb->capacity) {
        if (error) *error = LEAKY_BUCKET_ERROR_AMOUNT_TOO_LARGE;
        return false;
    }
    
    update_water(lb);
    
    if (lb->water + amount <= lb->capacity) {
        lb->water += amount;
        if (error) *error = LEAKY_BUCKET_OK;
        return true;
    }
    
    if (error) *error = LEAKY_BUCKET_OK;
    return false;
}

bool leaky_consume_with_wait(leaky_bucket_t *lb, double amount, int max_wait_ms, int *actual_wait_ms, leaky_bucket_error_t *error) {
    if (!lb) {
        if (error) *error = LEAKY_BUCKET_ERROR_NULL_PTR;
        return false;
    }
    
    if (amount < 0) {
        if (error) *error = LEAKY_BUCKET_ERROR_INVALID_ARGS;
        return false;
    }
    
    if (amount > lb->capacity) {
        if (error) *error = LEAKY_BUCKET_ERROR_AMOUNT_TOO_LARGE;
        return false;
    }
    
    update_water(lb);
    
    if (lb->water + amount <= lb->capacity) {
        lb->water += amount;
        if (actual_wait_ms) *actual_wait_ms = 0;
        if (error) *error = LEAKY_BUCKET_OK;
        return true;
    }
    
    double needed = (lb->water + amount) - lb->capacity;
    int wait_time = (int)((needed / lb->rate) * 1000);
    
    if (wait_time > max_wait_ms) {
        if (actual_wait_ms) *actual_wait_ms = -1;
        if (error) *error = LEAKY_BUCKET_OK;
        return false;
    }
    
    if (actual_wait_ms) *actual_wait_ms = wait_time;
    
    if (error) *error = LEAKY_BUCKET_OK;
    return false;
}

bool leaky_get_state(const leaky_bucket_t *lb, leaky_bucket_state_t *state, leaky_bucket_error_t *error) {
    if (!lb || !state) {
        if (error) *error = LEAKY_BUCKET_ERROR_NULL_PTR;
        return false;
    }
    
    leaky_bucket_t temp = *lb;
    update_water(&temp);
    
    state->current_water = temp.water;
    state->available_capacity = temp.capacity - temp.water;
    state->fill_level = temp.water / temp.capacity;
    state->last_update_time = temp.last_time;
    state->is_full = temp.water >= temp.capacity;
    state->is_empty = temp.water <= 0;
    
    if (error) *error = LEAKY_BUCKET_OK;
    return true;
}

bool leaky_reset(leaky_bucket_t *lb, leaky_bucket_error_t *error) {
    if (!lb) {
        if (error) *error = LEAKY_BUCKET_ERROR_NULL_PTR;
        return false;
    }
    
    lb->water = lb->config.initial_water;
    lb->last_time = time(NULL);
    
    if (error) *error = LEAKY_BUCKET_OK;
    return true;
}

bool leaky_update(leaky_bucket_t *lb, leaky_bucket_error_t *error) {
    if (!lb) {
        if (error) *error = LEAKY_BUCKET_ERROR_NULL_PTR;
        return false;
    }
    
    update_water(lb);
    
    if (error) *error = LEAKY_BUCKET_OK;
    return true;
}

bool leaky_calculate_wait_time(const leaky_bucket_t *lb, double amount, int *wait_ms, leaky_bucket_error_t *error) {
    if (!lb || !wait_ms) {
        if (error) *error = LEAKY_BUCKET_ERROR_NULL_PTR;
        return false;
    }
    
    if (amount < 0) {
        if (error) *error = LEAKY_BUCKET_ERROR_INVALID_ARGS;
        return false;
    }
    
    leaky_bucket_t temp = *lb;
    update_water(&temp);
    
    if (temp.water + amount <= temp.capacity) {
        *wait_ms = 0;
        if (error) *error = LEAKY_BUCKET_OK;
        return true;
    }
    
    double needed = (temp.water + amount) - temp.capacity;
    *wait_ms = (int)((needed / temp.rate) * 1000);
    
    if (error) *error = LEAKY_BUCKET_OK;
    return true;
}

bool leaky_set_params(leaky_bucket_t *lb, double capacity, double rate, leaky_bucket_error_t *error) {
    if (!lb) {
        if (error) *error = LEAKY_BUCKET_ERROR_NULL_PTR;
        return false;
    }
    
    if (capacity <= 0) {
        if (error) *error = LEAKY_BUCKET_ERROR_CAPACITY_TOO_SMALL;
        return false;
    }
    
    if (rate <= 0) {
        if (error) *error = LEAKY_BUCKET_ERROR_RATE_TOO_SLOW;
        return false;
    }
    
    lb->capacity = capacity;
    lb->rate = rate;
    lb->config.capacity = capacity;
    lb->config.rate = rate;
    
    if (lb->water > capacity) {
        lb->water = capacity;
    }
    
    if (error) *error = LEAKY_BUCKET_OK;
    return true;
}

bool leaky_can_consume(const leaky_bucket_t *lb, double amount, leaky_bucket_error_t *error) {
    if (!lb) {
        if (error) *error = LEAKY_BUCKET_ERROR_NULL_PTR;
        return false;
    }
    
    if (amount < 0) {
        if (error) *error = LEAKY_BUCKET_ERROR_INVALID_ARGS;
        return false;
    }
    
    if (amount > lb->capacity) {
        if (error) *error = LEAKY_BUCKET_ERROR_AMOUNT_TOO_LARGE;
        return false;
    }
    
    leaky_bucket_t temp = *lb;
    update_water(&temp);
    
    if (error) *error = LEAKY_BUCKET_OK;
    return temp.water + amount <= temp.capacity;
}

double leaky_get_available(const leaky_bucket_t *lb, leaky_bucket_error_t *error) {
    if (!lb) {
        if (error) *error = LEAKY_BUCKET_ERROR_NULL_PTR;
        return 0;
    }
    
    leaky_bucket_t temp = *lb;
    update_water(&temp);
    
    if (error) *error = LEAKY_BUCKET_OK;
    return temp.capacity - temp.water;
}

const char* leaky_bucket_error_string(leaky_bucket_error_t error) {
    switch (error) {
        case LEAKY_BUCKET_OK: return "Success";
        case LEAKY_BUCKET_ERROR_NULL_PTR: return "Null pointer error";
        case LEAKY_BUCKET_ERROR_INVALID_ARGS: return "Invalid arguments";
        case LEAKY_BUCKET_ERROR_CAPACITY_TOO_SMALL: return "Capacity too small";
        case LEAKY_BUCKET_ERROR_RATE_TOO_SLOW: return "Rate too slow";
        case LEAKY_BUCKET_ERROR_AMOUNT_TOO_LARGE: return "Amount too large";
        case LEAKY_BUCKET_ERROR_MAX: return "Max error code";
        default: return "Unknown error";
    }
}
