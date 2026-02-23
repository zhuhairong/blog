#include "rle.h"

size_t rle_encode(const unsigned char *in, size_t in_len, unsigned char *out) {
    size_t j = 0;
    for (size_t i = 0; i < in_len; i++) {
        unsigned char count = 1;
        while (i + 1 < in_len && in[i] == in[i+1] && count < 255) {
            count++;
            i++;
        }
        out[j++] = count;
        out[j++] = in[i];
    }
    return j;
}

size_t rle_decode(const unsigned char *in, size_t in_len, unsigned char *out) {
    size_t j = 0;
    for (size_t i = 0; i < in_len; i += 2) {
        unsigned char count = in[i];
        unsigned char val = in[i+1];
        for (int k = 0; k < count; k++) {
            out[j++] = val;
        }
    }
    return j;
}
