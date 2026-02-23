#include "bitset_compressed.h"
#include <stdlib.h>
#include <string.h>

void bitset_comp_init(bitset_compressed_t *bc) {
    if (!bc) return;
    bc->runs = NULL;
    bc->count = 0;
    bc->capacity = 0;
    bc->total_bits = 0;
}

bitset_compressed_t* bitset_comp_create(size_t estimated_size) {
    bitset_compressed_t *bc = (bitset_compressed_t*)malloc(sizeof(bitset_compressed_t));
    if (!bc) return NULL;
    
    bitset_comp_init(bc);
    
    if (estimated_size > 0) {
        bc->capacity = estimated_size * 2;
        bc->runs = (uint32_t*)calloc(bc->capacity, sizeof(uint32_t));
        if (!bc->runs) {
            free(bc);
            return NULL;
        }
    }
    
    return bc;
}

bitset_compressed_t* bitset_comp_clone(const bitset_compressed_t *bc) {
    if (!bc) return NULL;
    
    bitset_compressed_t *clone = bitset_comp_create(bc->count / 2);
    if (!clone) return NULL;
    
    if (bc->count > 0 && bc->runs) {
        memcpy(clone->runs, bc->runs, bc->count * sizeof(uint32_t));
    }
    clone->count = bc->count;
    clone->total_bits = bc->total_bits;
    
    return clone;
}

void bitset_comp_free(bitset_compressed_t *bc) {
    if (!bc) return;
    free(bc->runs);
    free(bc);
}

static bool ensure_capacity(bitset_compressed_t *bc, size_t needed) {
    if (bc->capacity >= needed) return true;
    
    size_t new_capacity = bc->capacity == 0 ? 16 : bc->capacity * 2;
    while (new_capacity < needed) new_capacity *= 2;
    
    uint32_t *new_runs = (uint32_t*)realloc(bc->runs, new_capacity * sizeof(uint32_t));
    if (!new_runs) return false;
    
    bc->runs = new_runs;
    bc->capacity = new_capacity;
    return true;
}

void bitset_comp_add_run(bitset_compressed_t *bc, uint32_t start, uint32_t len) {
    if (!bc || len == 0) return;
    
    if (!ensure_capacity(bc, bc->count + 2)) return;
    
    bc->runs[bc->count++] = start;
    bc->runs[bc->count++] = len;
    
    if (start + len > bc->total_bits) {
        bc->total_bits = start + len;
    }
}

bool bitset_comp_add_bit(bitset_compressed_t *bc, size_t bit, bool value) {
    if (!bc) return false;
    
    if (!ensure_capacity(bc, bc->count + 2)) return false;
    
    bc->runs[bc->count++] = (uint32_t)bit;
    bc->runs[bc->count++] = value ? 1 : 0;
    
    if (bit + 1 > bc->total_bits) {
        bc->total_bits = bit + 1;
    }
    
    return true;
}

bool bitset_comp_test(const bitset_compressed_t *bc, size_t bit) {
    if (!bc || !bc->runs || bit >= bc->total_bits) return false;
    
    for (size_t i = 0; i < bc->count; i += 2) {
        uint32_t start = bc->runs[i];
        uint32_t len = bc->runs[i + 1];
        
        if (bit >= start && bit < start + len) {
            return true;
        }
    }
    
    return false;
}

bool bitset_comp_set(bitset_compressed_t *bc, size_t bit) {
    if (!bc) return false;
    
    if (bitset_comp_test(bc, bit)) return true;
    
    if (!ensure_capacity(bc, bc->count + 2)) return false;
    
    bc->runs[bc->count++] = (uint32_t)bit;
    bc->runs[bc->count++] = 1;
    
    if (bit + 1 > bc->total_bits) {
        bc->total_bits = bit + 1;
    }
    
    return true;
}

bool bitset_comp_clear(bitset_compressed_t *bc, size_t bit) {
    if (!bc || !bc->runs) return false;
    
    for (size_t i = 0; i < bc->count; i += 2) {
        uint32_t start = bc->runs[i];
        uint32_t len = bc->runs[i + 1];
        
        if (bit >= start && bit < start + len) {
            if (len == 1) {
                memmove(&bc->runs[i], &bc->runs[i + 2], (bc->count - i - 2) * sizeof(uint32_t));
                bc->count -= 2;
            } else if (bit == start) {
                bc->runs[i] = start + 1;
                bc->runs[i + 1] = len - 1;
            } else if (bit == start + len - 1) {
                bc->runs[i + 1] = len - 1;
            } else {
                if (!ensure_capacity(bc, bc->count + 2)) return false;
                
                memmove(&bc->runs[i + 4], &bc->runs[i + 2], (bc->count - i - 2) * sizeof(uint32_t));
                
                bc->runs[i + 1] = (uint32_t)(bit - start);
                bc->runs[i + 2] = (uint32_t)(bit + 1);
                bc->runs[i + 3] = (uint32_t)(start + len - bit - 1);
                bc->count += 2;
            }
            return true;
        }
    }
    
    return true;
}

bool bitset_comp_flip(bitset_compressed_t *bc, size_t bit) {
    if (!bc) return false;
    
    if (bitset_comp_test(bc, bit)) {
        return bitset_comp_clear(bc, bit);
    } else {
        return bitset_comp_set(bc, bit);
    }
}

bool bitset_comp_set_range(bitset_compressed_t *bc, size_t start, size_t end, bool value) {
    if (!bc || start > end) return false;
    
    if (value) {
        if (!ensure_capacity(bc, bc->count + 2)) return false;
        
        bc->runs[bc->count++] = (uint32_t)start;
        bc->runs[bc->count++] = (uint32_t)(end - start + 1);
        
        if (end + 1 > bc->total_bits) {
            bc->total_bits = end + 1;
        }
    }
    
    return true;
}

size_t bitset_comp_count(const bitset_compressed_t *bc, bool value) {
    if (!bc || !bc->runs) return 0;
    
    size_t total = 0;
    
    for (size_t i = 0; i < bc->count; i += 2) {
        total += bc->runs[i + 1];
    }
    
    return value ? total : (bc->total_bits - total);
}

size_t bitset_comp_count_range(const bitset_compressed_t *bc, size_t start, size_t end, bool value) {
    if (!bc || !bc->runs || start > end) return 0;
    
    size_t total = 0;
    
    for (size_t i = 0; i < bc->count; i += 2) {
        uint32_t run_start = bc->runs[i];
        uint32_t run_len = bc->runs[i + 1];
        uint32_t run_end = run_start + run_len - 1;
        
        if (run_end >= start && run_start <= end) {
            uint32_t overlap_start = run_start > start ? run_start : start;
            uint32_t overlap_end = run_end < end ? run_end : end;
            total += overlap_end - overlap_start + 1;
        }
    }
    
    return value ? total : ((end - start + 1) - total);
}

size_t bitset_comp_find_first(const bitset_compressed_t *bc, bool value, size_t start) {
    if (!bc || !bc->runs) return (size_t)-1;
    
    if (value) {
        for (size_t i = 0; i < bc->count; i += 2) {
            uint32_t run_start = bc->runs[i];
            uint32_t run_len = bc->runs[i + 1];
            
            if (run_start + run_len > start) {
                return run_start > start ? run_start : start;
            }
        }
    } else {
        size_t pos = start;
        for (size_t i = 0; i < bc->count; i += 2) {
            uint32_t run_start = bc->runs[i];
            
            if (pos < run_start) {
                return pos;
            }
            pos = run_start + bc->runs[i + 1];
        }
        if (pos < bc->total_bits) {
            return pos;
        }
    }
    
    return (size_t)-1;
}

size_t bitset_comp_find_last(const bitset_compressed_t *bc, bool value) {
    if (!bc || !bc->runs) return (size_t)-1;
    
    if (value && bc->count >= 2) {
        uint32_t run_start = bc->runs[bc->count - 2];
        uint32_t run_len = bc->runs[bc->count - 1];
        return run_start + run_len - 1;
    }
    
    return (size_t)-1;
}

bool bitset_comp_is_empty(const bitset_compressed_t *bc) {
    if (!bc) return true;
    return bc->count == 0;
}

size_t bitset_comp_size(const bitset_compressed_t *bc) {
    if (!bc) return 0;
    return bc->total_bits;
}

size_t bitset_comp_run_count(const bitset_compressed_t *bc) {
    if (!bc) return 0;
    return bc->count / 2;
}

size_t bitset_comp_byte_size(const bitset_compressed_t *bc) {
    if (!bc) return 0;
    return sizeof(bitset_compressed_t) + bc->count * sizeof(uint32_t);
}

bool bitset_comp_to_bytes(const bitset_compressed_t *bc, uint8_t *out, size_t out_size) {
    if (!bc || !out) return false;
    
    size_t needed = bc->count * sizeof(uint32_t) + sizeof(size_t);
    if (out_size < needed) return false;
    
    memcpy(out, &bc->count, sizeof(size_t));
    memcpy(out + sizeof(size_t), bc->runs, bc->count * sizeof(uint32_t));
    
    return true;
}

bitset_compressed_t* bitset_comp_from_bytes(const uint8_t *in, size_t in_size) {
    if (!in || in_size < sizeof(size_t)) return NULL;
    
    size_t count;
    memcpy(&count, in, sizeof(size_t));
    
    if (in_size < sizeof(size_t) + count * sizeof(uint32_t)) return NULL;
    
    bitset_compressed_t *bc = bitset_comp_create(count / 2);
    if (!bc) return NULL;
    
    bc->count = count;
    memcpy(bc->runs, in + sizeof(size_t), count * sizeof(uint32_t));
    
    return bc;
}

bool bitset_comp_equals(const bitset_compressed_t *a, const bitset_compressed_t *b) {
    if (!a || !b) return a == b;
    if (a->count != b->count) return false;
    
    return memcmp(a->runs, b->runs, a->count * sizeof(uint32_t)) == 0;
}

bitset_compressed_t* bitset_comp_and(const bitset_compressed_t *a, const bitset_compressed_t *b) {
    if (!a || !b) return NULL;
    
    bitset_compressed_t *result = bitset_comp_create((a->count + b->count) / 4);
    if (!result) return NULL;
    
    for (size_t i = 0; i < a->count; i += 2) {
        for (size_t j = 0; j < b->count; j += 2) {
            uint32_t a_start = a->runs[i];
            uint32_t a_end = a_start + a->runs[i + 1];
            uint32_t b_start = b->runs[j];
            uint32_t b_end = b_start + b->runs[j + 1];
            
            if (a_start < b_end && b_start < a_end) {
                uint32_t start = a_start > b_start ? a_start : b_start;
                uint32_t end = a_end < b_end ? a_end : b_end;
                bitset_comp_add_run(result, start, end - start);
            }
        }
    }
    
    return result;
}

bitset_compressed_t* bitset_comp_or(const bitset_compressed_t *a, const bitset_compressed_t *b) {
    if (!a) return bitset_comp_clone(b);
    if (!b) return bitset_comp_clone(a);
    
    bitset_compressed_t *result = bitset_comp_clone(a);
    if (!result) return NULL;
    
    for (size_t i = 0; i < b->count; i += 2) {
        bitset_comp_add_run(result, b->runs[i], b->runs[i + 1]);
    }
    
    return result;
}

bitset_compressed_t* bitset_comp_xor(const bitset_compressed_t *a, const bitset_compressed_t *b) {
    if (!a || !b) return NULL;
    
    bitset_compressed_t *result = bitset_comp_clone(a);
    if (!result) return NULL;
    
    for (size_t i = 0; i < b->count; i += 2) {
        for (uint32_t bit = b->runs[i]; bit < b->runs[i] + b->runs[i + 1]; bit++) {
            bitset_comp_flip(result, bit);
        }
    }
    
    return result;
}

bitset_compressed_t* bitset_comp_not(const bitset_compressed_t *bc) {
    if (!bc) return NULL;
    
    bitset_compressed_t *result = bitset_comp_create(bc->count / 2);
    if (!result) return NULL;
    
    uint32_t pos = 0;
    
    for (size_t i = 0; i < bc->count; i += 2) {
        uint32_t start = bc->runs[i];
        
        if (pos < start) {
            bitset_comp_add_run(result, pos, start - pos);
        }
        
        pos = start + bc->runs[i + 1];
    }
    
    if (pos < bc->total_bits) {
        bitset_comp_add_run(result, pos, bc->total_bits - pos);
    }
    
    result->total_bits = bc->total_bits;
    return result;
}
