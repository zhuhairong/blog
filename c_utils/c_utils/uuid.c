#include "uuid.h"
#include "random.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uuid_error_t uuid_create(uuid_ctx_t** ctx, const uuid_config_t* config) {
    if (!ctx) return UUID_INVALID_PARAMS;
    
    *ctx = malloc(sizeof(uuid_ctx_t));
    if (!*ctx) return UUID_MEMORY_ERROR;
    
    if (config) {
        (*ctx)->config = *config;
    } else {
        (*ctx)->config.use_hyphens = true;
        (*ctx)->config.uppercase = false;
        (*ctx)->config.strict_validation = false;
        (*ctx)->config.max_string_length = 64;
    }
    
    (*ctx)->last_error = UUID_OK;
    return UUID_OK;
}

void uuid_destroy(uuid_ctx_t* ctx) {
    free(ctx);
}

uuid_error_t uuid_generate_v4(uuid_ctx_t* ctx, uuid_t* uuid) {
    if (!ctx || !uuid) return UUID_INVALID_PARAMS;
    
    uint64_t r1 = random_u64();
    uint64_t r2 = random_u64();
    
    for (int i = 0; i < 8; i++) {
        uuid->data[i] = (r1 >> (i * 8)) & 0xFF;
        uuid->data[i + 8] = (r2 >> (i * 8)) & 0xFF;
    }
    
    uuid->data[6] = (uuid->data[6] & 0x0F) | 0x40;
    uuid->data[8] = (uuid->data[8] & 0x3F) | 0x80;
    
    return UUID_OK;
}

uuid_error_t uuid_generate_v4_string(uuid_ctx_t* ctx, char* out) {
    if (!ctx || !out) return UUID_INVALID_PARAMS;
    
    uuid_t uuid;
    uuid_error_t err = uuid_generate_v4(ctx, &uuid);
    if (err != UUID_OK) return err;
    
    return uuid_to_string(ctx, &uuid, out);
}

uuid_error_t uuid_parse(uuid_ctx_t* ctx, const char* str, uuid_t* uuid) {
    if (!ctx || !str || !uuid) return UUID_INVALID_PARAMS;
    
    memset(uuid->data, 0, UUID_BIN_LEN);
    
    unsigned int data[16];
    int parsed = sscanf(str,
        "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
        &data[0], &data[1], &data[2], &data[3],
        &data[4], &data[5], &data[6], &data[7],
        &data[8], &data[9], &data[10], &data[11],
        &data[12], &data[13], &data[14], &data[15]);
    
    if (parsed != 16) {
        parsed = sscanf(str,
            "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
            &data[0], &data[1], &data[2], &data[3],
            &data[4], &data[5], &data[6], &data[7],
            &data[8], &data[9], &data[10], &data[11],
            &data[12], &data[13], &data[14], &data[15]);
        if (parsed != 16) return UUID_INVALID_FORMAT;
    }
    
    for (int i = 0; i < 16; i++) {
        uuid->data[i] = (unsigned char)data[i];
    }
    
    return UUID_OK;
}

uuid_error_t uuid_to_string(uuid_ctx_t* ctx, const uuid_t* uuid, char* out) {
    if (!ctx || !uuid || !out) return UUID_INVALID_PARAMS;
    
    const char* fmt = ctx->config.uppercase ?
        "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X" :
        "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x";
    
    sprintf(out, fmt,
        uuid->data[0], uuid->data[1], uuid->data[2], uuid->data[3],
        uuid->data[4], uuid->data[5], uuid->data[6], uuid->data[7],
        uuid->data[8], uuid->data[9], uuid->data[10], uuid->data[11],
        uuid->data[12], uuid->data[13], uuid->data[14], uuid->data[15]);
    
    return UUID_OK;
}

bool uuid_is_valid(const char* str) {
    if (!str) return false;
    
    size_t len = strlen(str);
    if (len != 36 && len != 32) return false;
    
    for (size_t i = 0; i < len; i++) {
        char c = str[i];
        if (len == 36 && (i == 8 || i == 13 || i == 18 || i == 23)) {
            if (c != '-') return false;
        } else {
            if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) {
                return false;
            }
        }
    }
    
    return true;
}

int uuid_get_version(const uuid_t* uuid) {
    if (!uuid) return 0;
    return (uuid->data[6] >> 4) & 0x0F;
}

int uuid_compare(const uuid_t* uuid1, const uuid_t* uuid2) {
    if (!uuid1 || !uuid2) return 0;
    return memcmp(uuid1->data, uuid2->data, UUID_BIN_LEN);
}

void uuid_copy(uuid_t* dest, const uuid_t* src) {
    if (!dest || !src) return;
    memcpy(dest->data, src->data, UUID_BIN_LEN);
}

bool uuid_is_nil(const uuid_t* uuid) {
    if (!uuid) return true;
    for (int i = 0; i < UUID_BIN_LEN; i++) {
        if (uuid->data[i] != 0) return false;
    }
    return true;
}

void uuid_v4(char *out) {
    uint64_t r1 = random_u64();
    uint64_t r2 = random_u64();
    
    r1 = (r1 & 0xFFFFFFFFFFFF0FFFULL) | 0x0000000000004000ULL;
    r2 = (r2 & 0x3FFFFFFFFFFFFFFFULL) | 0x8000000000000000ULL;
    
    sprintf(out, "%08x-%04x-%04x-%04x-%012llx",
            (uint32_t)(r1 >> 32),
            (uint16_t)(r1 >> 16),
            (uint16_t)r1,
            (uint16_t)(r2 >> 48),
            (unsigned long long)(r2 & 0xFFFFFFFFFFFFULL));
}

uuid_error_t uuid_get_last_error(uuid_ctx_t* ctx) {
    return ctx ? ctx->last_error : UUID_INVALID_PARAMS;
}

const char* uuid_strerror(uuid_error_t error) {
    switch (error) {
        case UUID_OK: return "Success";
        case UUID_INVALID_PARAMS: return "Invalid parameters";
        case UUID_INVALID_FORMAT: return "Invalid format";
        case UUID_INVALID_VERSION: return "Invalid version";
        case UUID_MEMORY_ERROR: return "Memory error";
        case UUID_GENERATION_ERROR: return "Generation error";
        default: return "Unknown error";
    }
}
