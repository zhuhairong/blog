#include "random.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

static uint64_t global_state[4] = {
    0x123456789ABCDEF0ULL,
    0xFEDCBA9876543210ULL,
    0x0123456789ABCDEFULL,
    0xF0E1D2C3B4A59687ULL
};

static bool global_initialized = false;

random_config_t random_default_config(void) {
    random_config_t config = {
        .type = RANDOM_TYPE_XORSHIFT,
        .seed = 0,
        .use_crypto_seed = true,
        .buffer_size = 0
    };
    return config;
}

void random_seed(uint64_t seed) {
    global_state[0] = seed ? seed : (uint64_t)time(NULL);
    global_state[1] = global_state[0] ^ 0xFEDCBA9876543210ULL;
    global_state[2] = global_state[0] ^ 0x0123456789ABCDEFULL;
    global_state[3] = global_state[0] ^ 0xF0E1D2C3B4A59687ULL;
    global_initialized = true;
}

bool random_seed_with_entropy(void) {
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
        random_seed((uint64_t)time(NULL));
        return false;
    }
    
    uint64_t entropy;
    ssize_t bytes_read = read(fd, &entropy, sizeof(entropy));
    close(fd);
    
    if (bytes_read != sizeof(entropy)) {
        random_seed((uint64_t)time(NULL));
        return false;
    }
    
    random_seed(entropy);
    return true;
}

static void ensure_initialized(void) {
    if (!global_initialized) {
        random_seed_with_entropy();
        global_initialized = true;
    }
}

uint64_t random_u64(void) {
    ensure_initialized();
    
    uint64_t t = global_state[0];
    t ^= t >> 12;
    t ^= t << 25;
    t ^= t >> 27;
    global_state[0] = t * 0x2545F4914F6CDD1DULL;
    
    return global_state[0];
}

uint32_t random_u32(void) {
    return (uint32_t)random_u64();
}

uint16_t random_u16(void) {
    return (uint16_t)random_u64();
}

uint8_t random_u8(void) {
    return (uint8_t)random_u64();
}

int64_t random_i64(void) {
    return (int64_t)random_u64();
}

int32_t random_i32(void) {
    return (int32_t)random_u64();
}

int64_t random_range(int64_t min, int64_t max) {
    if (min >= max) return min;
    return min + (int64_t)(random_u64() % (uint64_t)(max - min + 1));
}

int64_t random_range_ex(int64_t min, int64_t max, random_error_t *error) {
    if (min > max) {
        if (error) *error = RANDOM_ERROR_RANGE_ERROR;
        return min;
    }
    if (error) *error = RANDOM_OK;
    return random_range(min, max);
}

double random_double(void) {
    return (double)random_u64() / (double)UINT64_MAX;
}

double random_double_range(double min, double max) {
    if (min >= max) return min;
    return min + random_double() * (max - min);
}

double random_double_range_ex(double min, double max, random_error_t *error) {
    if (min > max) {
        if (error) *error = RANDOM_ERROR_RANGE_ERROR;
        return min;
    }
    if (error) *error = RANDOM_OK;
    return random_double_range(min, max);
}

bool random_bool(double probability) {
    if (probability <= 0.0) return false;
    if (probability >= 1.0) return true;
    return random_double() < probability;
}

bool random_bytes(uint8_t *buffer, size_t size) {
    if (!buffer || size == 0) return false;
    
    for (size_t i = 0; i < size; i++) {
        buffer[i] = random_u8();
    }
    return true;
}

bool random_bytes_ex(uint8_t *buffer, size_t size, random_error_t *error) {
    if (!buffer) {
        if (error) *error = RANDOM_ERROR_NULL_PTR;
        return false;
    }
    if (size == 0) {
        if (error) *error = RANDOM_OK;
        return true;
    }
    if (error) *error = RANDOM_OK;
    return random_bytes(buffer, size);
}

bool random_init(random_ctx_t *ctx, const random_config_t *config, random_error_t *error) {
    if (!ctx) {
        if (error) *error = RANDOM_ERROR_NULL_PTR;
        return false;
    }
    
    memset(ctx, 0, sizeof(random_ctx_t));
    
    if (config) {
        ctx->config = *config;
    } else {
        ctx->config = random_default_config();
    }
    
    uint64_t seed = ctx->config.seed;
    if (seed == 0) {
        seed = (uint64_t)time(NULL) ^ (uint64_t)(uintptr_t)ctx;
    }
    
    ctx->state.xorshift.x = seed;
    ctx->state.xorshift.y = seed ^ 0xFEDCBA9876543210ULL;
    ctx->state.xorshift.z = seed ^ 0x0123456789ABCDEFULL;
    ctx->state.xorshift.w = seed ^ 0xF0E1D2C3B4A59687ULL;
    
    ctx->initialized = true;
    
    if (error) *error = RANDOM_OK;
    return true;
}

uint64_t random_u64_ctx(random_ctx_t *ctx) {
    if (!ctx || !ctx->initialized) return random_u64();
    
    uint64_t t = ctx->state.xorshift.x;
    t ^= t >> 12;
    t ^= t << 25;
    t ^= t >> 27;
    ctx->state.xorshift.x = t * 0x2545F4914F6CDD1DULL;
    
    return ctx->state.xorshift.x;
}

int64_t random_range_ctx(random_ctx_t *ctx, int64_t min, int64_t max, random_error_t *error) {
    if (!ctx) {
        if (error) *error = RANDOM_ERROR_NULL_PTR;
        return min;
    }
    if (min > max) {
        if (error) *error = RANDOM_ERROR_RANGE_ERROR;
        return min;
    }
    if (error) *error = RANDOM_OK;
    if (min >= max) return min;
    return min + (int64_t)(random_u64_ctx(ctx) % (uint64_t)(max - min + 1));
}

double random_double_ctx(random_ctx_t *ctx) {
    if (!ctx || !ctx->initialized) return random_double();
    return (double)random_u64_ctx(ctx) / (double)UINT64_MAX;
}

bool random_bytes_ctx(random_ctx_t *ctx, uint8_t *buffer, size_t size, random_error_t *error) {
    if (!ctx) {
        if (error) *error = RANDOM_ERROR_NULL_PTR;
        return false;
    }
    if (!buffer) {
        if (error) *error = RANDOM_ERROR_NULL_PTR;
        return false;
    }
    if (size == 0) {
        if (error) *error = RANDOM_OK;
        return true;
    }
    
    for (size_t i = 0; i < size; i++) {
        buffer[i] = (uint8_t)random_u64_ctx(ctx);
    }
    
    if (error) *error = RANDOM_OK;
    return true;
}

bool random_reset(random_ctx_t *ctx, uint64_t seed, random_error_t *error) {
    if (!ctx) {
        if (error) *error = RANDOM_ERROR_NULL_PTR;
        return false;
    }
    
    ctx->state.xorshift.x = seed;
    ctx->state.xorshift.y = seed ^ 0xFEDCBA9876543210ULL;
    ctx->state.xorshift.z = seed ^ 0x0123456789ABCDEFULL;
    ctx->state.xorshift.w = seed ^ 0xF0E1D2C3B4A59687ULL;
    
    if (error) *error = RANDOM_OK;
    return true;
}
