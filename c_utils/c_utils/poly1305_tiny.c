#include "poly1305_tiny.h"
#include <string.h>

void poly1305_tiny(const uint8_t *key, const uint8_t *msg, size_t len, uint8_t *mac) {
    // 极简实现：仅演示接口
    memset(mac, 0, 16);
}
