#include "bitstream.h"

void bitstream_init(bitstream_t *bs, const uint8_t *buf, size_t len) {
    bs->buf = buf; bs->len = len; bs->pos = 0;
}

uint32_t bitstream_read(bitstream_t *bs, int bits) {
    uint32_t val = 0;
    for (int i = 0; i < bits; i++) {
        size_t byte_pos = bs->pos / 8;
        int bit_off = 7 - (int)(bs->pos % 8);
        if (byte_pos < bs->len) {
            val = (val << 1) | ((bs->buf[byte_pos] >> bit_off) & 1);
        }
        bs->pos++;
    }
    return val;
}
