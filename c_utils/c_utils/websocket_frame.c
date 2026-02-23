#include "websocket_frame.h"
#include <string.h>

size_t ws_frame_encode_hdr(ws_frame_hdr_t *hdr, uint8_t *buf) {
    buf[0] = (hdr->fin << 7) | (hdr->opcode & 0x0F);
    size_t idx = 1;
    if (hdr->payload_len < 126) {
        buf[idx++] = (hdr->masked << 7) | (uint8_t)hdr->payload_len;
    } else if (hdr->payload_len < 65536) {
        buf[idx++] = (hdr->masked << 7) | 126;
        buf[idx++] = (hdr->payload_len >> 8) & 0xFF;
        buf[idx++] = hdr->payload_len & 0xFF;
    } else {
        buf[idx++] = (hdr->masked << 7) | 127;
        for (int i = 7; i >= 0; i--) buf[idx++] = (hdr->payload_len >> (i * 8)) & 0xFF;
    }
    if (hdr->masked) {
        memcpy(&buf[idx], hdr->mask, 4);
        idx += 4;
    }
    return idx;
}
