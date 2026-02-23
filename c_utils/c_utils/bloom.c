#include "bloom.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>

struct bloom_s {
    uint8_t *bits;
    size_t   nbits;
    size_t   nhash;
    size_t   nexpected;
    double   fp_rate;
    size_t   nadded;
};

static uint32_t murmurhash3(const void *key, size_t len, uint32_t seed) {
    const uint8_t *data = (const uint8_t*)key;
    uint32_t h = seed;
    for (size_t i = 0; i < len; i++) {
        h ^= data[i];
        h *= 0x5bd1e995;
        h ^= h >> 15;
    }
    return h;
}

bool bloom_validate_params(size_t n, double p) {
    if (n == 0) return false;
    if (p <= 0.0 || p >= 1.0) return false;
    return true;
}

bloom_t* bloom_create(size_t n, double p) {
    if (!bloom_validate_params(n, p)) return NULL;
    
    bloom_t *b = malloc(sizeof(bloom_t));
    if (!b) return NULL;

    b->nbits = (size_t)(-(double)n * log(p) / (log(2) * log(2)));
    if (b->nbits == 0) b->nbits = 1;
    
    b->nhash = (size_t)((double)b->nbits / (double)n * log(2));
    if (b->nhash == 0) b->nhash = 1;

    size_t bytes = (b->nbits + 7) / 8;
    b->bits = calloc(bytes, 1);
    if (!b->bits) {
        free(b);
        return NULL;
    }
    
    b->nexpected = n;
    b->fp_rate = p;
    b->nadded = 0;
    
    return b;
}

void bloom_free(bloom_t *b) {
    if (b) {
        free(b->bits);
        free(b);
    }
}

bool bloom_add(bloom_t *b, const void *data, size_t len) {
    if (!b || !data) return false;
    for (size_t i = 0; i < b->nhash; i++) {
        uint32_t h = murmurhash3(data, len, (uint32_t)i);
        size_t bit = h % b->nbits;
        b->bits[bit / 8] |= (1 << (bit % 8));
    }
    b->nadded++;
    return true;
}

bool bloom_check(const bloom_t *b, const void *data, size_t len) {
    if (!b || !data) return false;
    for (size_t i = 0; i < b->nhash; i++) {
        uint32_t h = murmurhash3(data, len, (uint32_t)i);
        size_t bit = h % b->nbits;
        if (!(b->bits[bit / 8] & (1 << (bit % 8)))) return false;
    }
    return true;
}

bool bloom_reset(bloom_t *b) {
    if (!b) return false;
    size_t bytes = (b->nbits + 7) / 8;
    memset(b->bits, 0, bytes);
    b->nadded = 0;
    return true;
}

bool bloom_stats(const bloom_t *b, size_t *estimated_elements, double *false_positive_rate) {
    if (!b) return false;
    
    if (estimated_elements) {
        size_t set_bits = 0;
        size_t bytes = (b->nbits + 7) / 8;
        for (size_t i = 0; i < bytes; i++) {
            uint8_t byte = b->bits[i];
            while (byte) {
                set_bits += byte & 1;
                byte >>= 1;
            }
        }
        *estimated_elements = b->nadded;
    }
    
    if (false_positive_rate) {
        *false_positive_rate = b->fp_rate;
    }
    
    return true;
}

bool bloom_serialize(const bloom_t *b, uint8_t *buf, size_t buf_size, size_t *written) {
    if (!b || !buf || buf_size == 0) return false;
    
    size_t bytes_needed = sizeof(size_t) * 4 + sizeof(double) + (b->nbits + 7) / 8;
    if (buf_size < bytes_needed) return false;
    
    size_t offset = 0;
    
    memcpy(buf + offset, &b->nbits, sizeof(size_t));
    offset += sizeof(size_t);
    
    memcpy(buf + offset, &b->nhash, sizeof(size_t));
    offset += sizeof(size_t);
    
    memcpy(buf + offset, &b->nexpected, sizeof(size_t));
    offset += sizeof(size_t);
    
    memcpy(buf + offset, &b->nadded, sizeof(size_t));
    offset += sizeof(size_t);
    
    memcpy(buf + offset, &b->fp_rate, sizeof(double));
    offset += sizeof(double);
    
    size_t bits_bytes = (b->nbits + 7) / 8;
    memcpy(buf + offset, b->bits, bits_bytes);
    offset += bits_bytes;
    
    if (written) *written = offset;
    
    return true;
}

bloom_t* bloom_deserialize(const uint8_t *buf, size_t buf_size) {
    if (!buf || buf_size < sizeof(size_t) * 4 + sizeof(double)) return NULL;
    
    size_t offset = 0;
    
    bloom_t *b = malloc(sizeof(bloom_t));
    if (!b) return NULL;
    
    memcpy(&b->nbits, buf + offset, sizeof(size_t));
    offset += sizeof(size_t);
    
    memcpy(&b->nhash, buf + offset, sizeof(size_t));
    offset += sizeof(size_t);
    
    memcpy(&b->nexpected, buf + offset, sizeof(size_t));
    offset += sizeof(size_t);
    
    memcpy(&b->nadded, buf + offset, sizeof(size_t));
    offset += sizeof(size_t);
    
    memcpy(&b->fp_rate, buf + offset, sizeof(double));
    offset += sizeof(double);
    
    size_t bits_bytes = (b->nbits + 7) / 8;
    if (buf_size < offset + bits_bytes) {
        free(b);
        return NULL;
    }
    
    b->bits = malloc(bits_bytes);
    if (!b->bits) {
        free(b);
        return NULL;
    }
    
    memcpy(b->bits, buf + offset, bits_bytes);
    
    return b;
}
