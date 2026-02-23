#include "endian_utils.h"
#include <stdbool.h>

bool endian_is_big(void) {
    union { uint32_t i; char c[4]; } bint = {0x01020304};
    return bint.c[0] == 1;
}

uint16_t endian_swap16(uint16_t v) {
    return (v << 8) | (v >> 8);
}

uint32_t endian_swap32(uint32_t v) {
    return ((v & 0x000000FF) << 24) |
           ((v & 0x0000FF00) << 8)  |
           ((v & 0x00FF0000) >> 8)  |
           ((v & 0xFF000000) >> 24);
}

uint64_t endian_swap64(uint64_t v) {
    v = ((v & 0x00000000FFFFFFFFULL) << 32) | ((v & 0xFFFFFFFF00000000ULL) >> 32);
    v = ((v & 0x0000FFFF0000FFFFULL) << 16) | ((v & 0xFFFF0000FFFF0000ULL) >> 16);
    v = ((v & 0x00FF00FF00FF00FFULL) << 8)  | ((v & 0xFF00FF00FF00FF00ULL) >> 8);
    return v;
}

uint16_t host_to_net16(uint16_t host) {
    return endian_is_big() ? host : endian_swap16(host);
}

uint32_t host_to_net32(uint32_t host) {
    return endian_is_big() ? host : endian_swap32(host);
}

uint64_t host_to_net64(uint64_t host) {
    return endian_is_big() ? host : endian_swap64(host);
}

uint16_t net_to_host16(uint16_t net) {
    return endian_is_big() ? net : endian_swap16(net);
}

uint32_t net_to_host32(uint32_t net) {
    return endian_is_big() ? net : endian_swap32(net);
}

uint64_t net_to_host64(uint64_t net) {
    return endian_is_big() ? net : endian_swap64(net);
}
