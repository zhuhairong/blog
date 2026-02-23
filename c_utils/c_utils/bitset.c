#include "bitset.h"
#include <stdlib.h>
#include <string.h>

struct bitset_s {
    uint8_t *bits;
    size_t nbits;
};

// 计算需要的字节数
static size_t bits_to_bytes(size_t nbits) {
    return (nbits + 7) / 8;
}

// 创建与销毁
bitset_t* bitset_create(size_t nbits) {
    bitset_t *bs = malloc(sizeof(bitset_t));
    if (!bs) return NULL;
    
    bs->nbits = nbits > 0 ? nbits : 1;
    bs->bits = calloc(bits_to_bytes(bs->nbits), 1);
    if (!bs->bits) {
        free(bs);
        return NULL;
    }
    if (nbits == 0) bs->nbits = 0;
    return bs;
}

bitset_t* bitset_create_from_bytes(const uint8_t *bytes, size_t nbytes, size_t nbits) {
    if (!bytes || nbytes == 0 || nbits == 0) return NULL;
    
    bitset_t *bs = bitset_create(nbits);
    if (!bs) return NULL;
    
    size_t copy_bytes = bits_to_bytes(nbits);
    if (copy_bytes > nbytes) copy_bytes = nbytes;
    memcpy(bs->bits, bytes, copy_bytes);
    return bs;
}

bitset_t* bitset_clone(const bitset_t *bs) {
    if (!bs) return NULL;
    return bitset_create_from_bytes(bs->bits, bits_to_bytes(bs->nbits), bs->nbits);
}

void bitset_free(bitset_t *bs) {
    if (bs) {
        free(bs->bits);
        free(bs);
    }
}

// 基本操作
void bitset_set(bitset_t *bs, size_t bit) {
    if (!bs || bit >= bs->nbits) return;
    bs->bits[bit / 8] |= (1 << (bit % 8));
}

void bitset_clear(bitset_t *bs, size_t bit) {
    if (!bs || bit >= bs->nbits) return;
    bs->bits[bit / 8] &= ~(1 << (bit % 8));
}

bool bitset_test(const bitset_t *bs, size_t bit) {
    if (!bs || bit >= bs->nbits) return false;
    return (bs->bits[bit / 8] & (1 << (bit % 8))) != 0;
}

void bitset_flip(bitset_t *bs, size_t bit) {
    if (!bs || bit >= bs->nbits) return;
    bs->bits[bit / 8] ^= (1 << (bit % 8));
}

// 批量操作
void bitset_set_all(bitset_t *bs) {
    if (!bs) return;
    memset(bs->bits, 0xFF, bits_to_bytes(bs->nbits));
}

void bitset_clear_all(bitset_t *bs) {
    if (!bs) return;
    memset(bs->bits, 0, bits_to_bytes(bs->nbits));
}

void bitset_flip_all(bitset_t *bs) {
    if (!bs) return;
    size_t nbytes = bits_to_bytes(bs->nbits);
    for (size_t i = 0; i < nbytes; i++) {
        bs->bits[i] ^= 0xFF;
    }
}

void bitset_set_range(bitset_t *bs, size_t start, size_t end) {
    if (!bs || start >= bs->nbits || end < start) return;
    if (end >= bs->nbits) end = bs->nbits - 1;
    for (size_t i = start; i <= end; i++) {
        bitset_set(bs, i);
    }
}

void bitset_clear_range(bitset_t *bs, size_t start, size_t end) {
    if (!bs || start >= bs->nbits || end < start) return;
    if (end >= bs->nbits) end = bs->nbits - 1;
    for (size_t i = start; i <= end; i++) {
        bitset_clear(bs, i);
    }
}

void bitset_flip_range(bitset_t *bs, size_t start, size_t end) {
    if (!bs || start >= bs->nbits || end < start) return;
    if (end >= bs->nbits) end = bs->nbits - 1;
    for (size_t i = start; i <= end; i++) {
        bitset_flip(bs, i);
    }
}

// 位运算
bitset_t* bitset_and(const bitset_t *a, const bitset_t *b) {
    if (!a || !b) return NULL;
    size_t nbits = a->nbits < b->nbits ? a->nbits : b->nbits;
    bitset_t *result = bitset_create(nbits);
    if (!result) return NULL;
    size_t nbytes = bits_to_bytes(nbits);
    for (size_t i = 0; i < nbytes; i++) {
        result->bits[i] = a->bits[i] & b->bits[i];
    }
    return result;
}

bitset_t* bitset_or(const bitset_t *a, const bitset_t *b) {
    if (!a || !b) return NULL;
    size_t nbits = a->nbits > b->nbits ? a->nbits : b->nbits;
    bitset_t *result = bitset_create(nbits);
    if (!result) return NULL;
    size_t nbytes = bits_to_bytes(nbits);
    for (size_t i = 0; i < nbytes; i++) {
        if (i < bits_to_bytes(a->nbits) && i < bits_to_bytes(b->nbits)) {
            result->bits[i] = a->bits[i] | b->bits[i];
        } else if (i < bits_to_bytes(a->nbits)) {
            result->bits[i] = a->bits[i];
        } else {
            result->bits[i] = b->bits[i];
        }
    }
    return result;
}

bitset_t* bitset_xor(const bitset_t *a, const bitset_t *b) {
    if (!a || !b) return NULL;
    size_t nbits = a->nbits > b->nbits ? a->nbits : b->nbits;
    bitset_t *result = bitset_create(nbits);
    if (!result) return NULL;
    size_t nbytes = bits_to_bytes(nbits);
    for (size_t i = 0; i < nbytes; i++) {
        if (i < bits_to_bytes(a->nbits) && i < bits_to_bytes(b->nbits)) {
            result->bits[i] = a->bits[i] ^ b->bits[i];
        } else if (i < bits_to_bytes(a->nbits)) {
            result->bits[i] = a->bits[i];
        } else {
            result->bits[i] = b->bits[i];
        }
    }
    return result;
}

bitset_t* bitset_not(const bitset_t *bs) {
    if (!bs) return NULL;
    bitset_t *result = bitset_clone(bs);
    if (!result) return NULL;
    bitset_flip_all(result);
    return result;
}

// 计数与查找
size_t bitset_count(const bitset_t *bs) {
    if (!bs) return 0;
    size_t count = 0;
    size_t nbytes = bits_to_bytes(bs->nbits);
    for (size_t i = 0; i < nbytes; i++) {
        uint8_t byte = bs->bits[i];
        while (byte) {
            byte &= byte - 1;
            count++;
        }
    }
    return count;
}

size_t bitset_count_range(const bitset_t *bs, size_t start, size_t end) {
    if (!bs || start >= bs->nbits || end < start) return 0;
    if (end >= bs->nbits) end = bs->nbits - 1;
    size_t count = 0;
    for (size_t i = start; i <= end; i++) {
        if (bitset_test(bs, i)) count++;
    }
    return count;
}

size_t bitset_find_first_set(const bitset_t *bs, size_t start) {
    if (!bs) return (size_t)-1;
    for (size_t i = start; i < bs->nbits; i++) {
        if (bitset_test(bs, i)) return i;
    }
    return (size_t)-1;
}

size_t bitset_find_first_clear(const bitset_t *bs, size_t start) {
    if (!bs) return (size_t)-1;
    for (size_t i = start; i < bs->nbits; i++) {
        if (!bitset_test(bs, i)) return i;
    }
    return (size_t)-1;
}

size_t bitset_find_last_set(const bitset_t *bs) {
    if (!bs) return (size_t)-1;
    for (size_t i = bs->nbits - 1; i >= 0; i--) {
        if (bitset_test(bs, i)) return i;
        if (i == 0) break;
    }
    return (size_t)-1;
}

// 状态查询
bool bitset_is_empty(const bitset_t *bs) {
    return bitset_count(bs) == 0;
}

bool bitset_is_all_set(const bitset_t *bs) {
    if (!bs) return false;
    return bitset_count(bs) == bs->nbits;
}

bool bitset_equals(const bitset_t *a, const bitset_t *b) {
    if (!a || !b || a->nbits != b->nbits) return false;
    size_t nbytes = bits_to_bytes(a->nbits);
    return memcmp(a->bits, b->bits, nbytes) == 0;
}

size_t bitset_size(const bitset_t *bs) {
    return bs ? bs->nbits : 0;
}

size_t bitset_byte_size(const bitset_t *bs) {
    return bs ? bits_to_bytes(bs->nbits) : 0;
}

// 调整大小
bool bitset_resize(bitset_t *bs, size_t new_size) {
    if (!bs) return false;
    size_t old_bytes = bits_to_bytes(bs->nbits);
    size_t new_bytes = bits_to_bytes(new_size);
    uint8_t *new_bits = realloc(bs->bits, new_bytes);
    if (!new_bits) return false;
    if (new_bytes > old_bytes) {
        memset(new_bits + old_bytes, 0, new_bytes - old_bytes);
    }
    bs->bits = new_bits;
    bs->nbits = new_size;
    return true;
}

// 序列化
bool bitset_to_bytes(const bitset_t *bs, uint8_t *out, size_t out_size) {
    if (!bs || !out) return false;
    size_t nbytes = bits_to_bytes(bs->nbits);
    if (out_size < nbytes) return false;
    memcpy(out, bs->bits, nbytes);
    return true;
}
