#include "otp.h"
#include "hmac.h"
#include "base32.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

otp_totp_config_t otp_default_totp_config(void) {
    otp_totp_config_t config = {
        .algorithm = OTP_ALGORITHM_SHA1,
        .digits = 6,
        .time_step = 30,
        .epoch = 0
    };
    return config;
}

otp_hotp_config_t otp_default_hotp_config(void) {
    otp_hotp_config_t config = {
        .algorithm = OTP_ALGORITHM_SHA1,
        .digits = 6
    };
    return config;
}

static int hmac_hash(otp_algorithm_t algorithm, const uint8_t *key, size_t key_len,
                     const uint8_t *msg, size_t msg_len, uint8_t *hash_out) {
    switch (algorithm) {
        case OTP_ALGORITHM_SHA1:
            hmac_sha1(key, key_len, msg, msg_len, hash_out, NULL);
            return 20;
        case OTP_ALGORITHM_SHA256:
            hmac_sha256(key, key_len, msg, msg_len, hash_out, NULL);
            return 32;
        case OTP_ALGORITHM_SHA512:
            return -1;
        default:
            return -1;
    }
}

static uint32_t dynamic_truncate(const uint8_t *hash, int hash_len, uint32_t digits) {
    int offset = hash[hash_len - 1] & 0x0f;
    
    uint32_t bin_code = ((hash[offset] & 0x7f) << 24) |
                        ((hash[offset + 1] & 0xff) << 16) |
                        ((hash[offset + 2] & 0xff) << 8) |
                        (hash[offset + 3] & 0xff);
    
    uint32_t modulus = 1;
    for (uint32_t i = 0; i < digits; i++) {
        modulus *= 10;
    }
    
    return bin_code % modulus;
}

uint32_t otp_generate_totp(const uint8_t *secret, size_t secret_len, uint64_t time_sec) {
    return otp_generate_totp_ex(secret, secret_len, time_sec, NULL, NULL);
}

uint32_t otp_generate_totp_ex(const uint8_t *secret, size_t secret_len, uint64_t time_sec, 
                              const otp_totp_config_t *config, otp_error_t *error) {
    if (!secret || secret_len == 0) {
        if (error) *error = OTP_ERROR_NULL_PTR;
        return 0;
    }
    
    otp_totp_config_t cfg = config ? *config : otp_default_totp_config();
    
    if (cfg.digits < 6 || cfg.digits > 8) {
        if (error) *error = OTP_ERROR_INVALID_ARGS;
        return 0;
    }
    
    uint64_t counter = (time_sec - cfg.epoch) / cfg.time_step;
    
    uint8_t msg[8];
    for (int i = 7; i >= 0; i--) {
        msg[i] = (uint8_t)(counter & 0xFF);
        counter >>= 8;
    }
    
    uint8_t hash[64];
    int hash_len = hmac_hash(cfg.algorithm, secret, secret_len, msg, 8, hash);
    
    if (hash_len < 0) {
        if (error) *error = OTP_ERROR_HMAC_FAILED;
        return 0;
    }
    
    if (error) *error = OTP_OK;
    return dynamic_truncate(hash, hash_len, cfg.digits);
}

uint32_t otp_generate_hotp(const uint8_t *secret, size_t secret_len, uint64_t counter, 
                           uint32_t digits, otp_error_t *error) {
    otp_hotp_config_t config = otp_default_hotp_config();
    config.digits = digits;
    return otp_generate_hotp_ex(secret, secret_len, counter, &config, error);
}

uint32_t otp_generate_hotp_ex(const uint8_t *secret, size_t secret_len, uint64_t counter,
                              const otp_hotp_config_t *config, otp_error_t *error) {
    if (!secret || secret_len == 0) {
        if (error) *error = OTP_ERROR_NULL_PTR;
        return 0;
    }
    
    otp_hotp_config_t cfg = config ? *config : otp_default_hotp_config();
    
    if (cfg.digits < 6 || cfg.digits > 8) {
        if (error) *error = OTP_ERROR_INVALID_ARGS;
        return 0;
    }
    
    uint8_t msg[8];
    uint64_t c = counter;
    for (int i = 7; i >= 0; i--) {
        msg[i] = (uint8_t)(c & 0xFF);
        c >>= 8;
    }
    
    uint8_t hash[64];
    int hash_len = hmac_hash(cfg.algorithm, secret, secret_len, msg, 8, hash);
    
    if (hash_len < 0) {
        if (error) *error = OTP_ERROR_HMAC_FAILED;
        return 0;
    }
    
    if (error) *error = OTP_OK;
    return dynamic_truncate(hash, hash_len, cfg.digits);
}

bool otp_verify_totp(const uint8_t *secret, size_t secret_len, uint64_t time_sec,
                     uint32_t code, uint32_t window, otp_error_t *error) {
    return otp_verify_totp_ex(secret, secret_len, time_sec, code, window, NULL, error);
}

bool otp_verify_totp_ex(const uint8_t *secret, size_t secret_len, uint64_t time_sec,
                        uint32_t code, uint32_t window, const otp_totp_config_t *config,
                        otp_error_t *error) {
    if (!secret || secret_len == 0) {
        if (error) *error = OTP_ERROR_NULL_PTR;
        return false;
    }
    
    otp_totp_config_t cfg = config ? *config : otp_default_totp_config();
    
    for (uint32_t i = 0; i <= window; i++) {
        uint64_t time_forward = time_sec + (uint64_t)i * cfg.time_step;
        uint64_t time_backward = time_sec - (uint64_t)i * cfg.time_step;
        
        uint32_t code_forward = otp_generate_totp_ex(secret, secret_len, time_forward, &cfg, NULL);
        if (code_forward == code) {
            if (error) *error = OTP_OK;
            return true;
        }
        
        if (i > 0 && time_backward >= cfg.epoch) {
            uint32_t code_backward = otp_generate_totp_ex(secret, secret_len, time_backward, &cfg, NULL);
            if (code_backward == code) {
                if (error) *error = OTP_OK;
                return true;
            }
        }
    }
    
    if (error) *error = OTP_OK;
    return false;
}

bool otp_verify_hotp(const uint8_t *secret, size_t secret_len, uint64_t counter,
                     uint32_t code, uint32_t digits, otp_error_t *error) {
    otp_hotp_config_t config = otp_default_hotp_config();
    config.digits = digits;
    return otp_verify_hotp_ex(secret, secret_len, counter, code, &config, error);
}

bool otp_verify_hotp_ex(const uint8_t *secret, size_t secret_len, uint64_t counter,
                        uint32_t code, const otp_hotp_config_t *config, otp_error_t *error) {
    if (!secret || secret_len == 0) {
        if (error) *error = OTP_ERROR_NULL_PTR;
        return false;
    }
    
    uint32_t generated = otp_generate_hotp_ex(secret, secret_len, counter, config, error);
    return generated == code;
}

size_t otp_generate_uri(const otp_uri_config_t *config, char *uri_buffer, 
                        size_t buffer_size, otp_error_t *error) {
    if (!config || !uri_buffer || buffer_size == 0) {
        if (error) *error = OTP_ERROR_NULL_PTR;
        return 0;
    }
    
    char base32_secret[256];
    size_t base32_len = otp_secret_to_base32(config->secret, config->secret_len, 
                                              base32_secret, sizeof(base32_secret), error);
    if (base32_len == 0) {
        return 0;
    }
    
    int len;
    if (strcmp(config->type, "totp") == 0) {
        len = snprintf(uri_buffer, buffer_size,
                      "otpauth://totp/%s?secret=%s&issuer=%s&algorithm=SHA%d&digits=%u&period=%u",
                      config->label, base32_secret, config->issuer,
                      config->totp_config.algorithm == OTP_ALGORITHM_SHA1 ? 1 : 256,
                      config->totp_config.digits, config->totp_config.time_step);
    } else {
        len = snprintf(uri_buffer, buffer_size,
                      "otpauth://hotp/%s?secret=%s&issuer=%s&algorithm=SHA%d&digits=%u&counter=%llu",
                      config->label, base32_secret, config->issuer,
                      config->totp_config.algorithm == OTP_ALGORITHM_SHA1 ? 1 : 256,
                      config->totp_config.digits, (unsigned long long)config->counter);
    }
    
    if (len < 0 || (size_t)len >= buffer_size) {
        if (error) *error = OTP_ERROR_INVALID_ARGS;
        return 0;
    }
    
    if (error) *error = OTP_OK;
    return (size_t)len;
}

size_t otp_generate_secret(uint8_t *secret_buffer, size_t buffer_size, otp_error_t *error) {
    if (!secret_buffer || buffer_size == 0) {
        if (error) *error = OTP_ERROR_NULL_PTR;
        return 0;
    }
    
    size_t secret_len = buffer_size < 20 ? buffer_size : 20;
    
    srand((unsigned int)time(NULL));
    for (size_t i = 0; i < secret_len; i++) {
        secret_buffer[i] = (uint8_t)(rand() & 0xFF);
    }
    
    if (error) *error = OTP_OK;
    return secret_len;
}

size_t otp_secret_to_base32(const uint8_t *secret, size_t secret_len, 
                            char *base32_buffer, size_t buffer_size, otp_error_t *error) {
    if (!secret || !base32_buffer || buffer_size == 0) {
        if (error) *error = OTP_ERROR_NULL_PTR;
        return 0;
    }
    
    size_t needed = base32_encode_size(secret_len);
    if (buffer_size < needed) {
        if (error) *error = OTP_ERROR_INVALID_ARGS;
        return 0;
    }
    
    base32_encode(secret, secret_len, base32_buffer);
    size_t len = needed - 1;
    
    if (error) *error = OTP_OK;
    return len;
}

size_t otp_base32_to_secret(const char *base32_str, uint8_t *secret_buffer, 
                            size_t buffer_size, otp_error_t *error) {
    if (!base32_str || !secret_buffer || buffer_size == 0) {
        if (error) *error = OTP_ERROR_NULL_PTR;
        return 0;
    }
    
    size_t len = base32_decode(base32_str, strlen(base32_str), secret_buffer);
    
    if (error) *error = OTP_OK;
    return len;
}

const char* otp_error_string(otp_error_t error) {
    switch (error) {
        case OTP_OK: return "Success";
        case OTP_ERROR_NULL_PTR: return "Null pointer error";
        case OTP_ERROR_INVALID_ARGS: return "Invalid arguments";
        case OTP_ERROR_SECRET_TOO_SHORT: return "Secret too short";
        case OTP_ERROR_HMAC_FAILED: return "HMAC calculation failed";
        case OTP_ERROR_INVALID_ALGORITHM: return "Invalid algorithm";
        default: return "Unknown error";
    }
}
