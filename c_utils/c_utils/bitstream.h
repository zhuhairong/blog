#ifndef C_UTILS_BITSTREAM_H
#define C_UTILS_BITSTREAM_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
    const uint8_t *buf;
    size_t len;
    size_t pos; // bit position
} bitstream_t;

void     bitstream_init(bitstream_t *bs, const uint8_t *buf, size_t len);
uint32_t bitstream_read(bitstream_t *bs, int bits);

#endif // C_UTILS_BITSTREAM_H
