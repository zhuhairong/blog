#include "varint.h"

size_t varint_encode(uint64_t val, uint8_t *buf) {
    size_t i = 0;
    while (val >= 0x80) {
        buf[i++] = (uint8_t)(val | 0x80);
        val >>= 7;
    }
    buf[i++] = (uint8_t)val;
    return i;
}

size_t varint_decode(const uint8_t *buf, uint64_t *val) {
    size_t i = 0;
    uint64_t v = 0;
    int shift = 0;
    while (1) {
        uint8_t b = buf[i++];
        v |= (uint64_t)(b & 0x7F) << shift;
        if (!(b & 0x80)) break;
        shift += 7;
    }
    *val = v;
    return i;
}
