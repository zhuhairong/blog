#include "utf8.h"

bool utf8_is_valid(const char *str) {
    if (str == NULL) return false;
    const unsigned char *s = (const unsigned char *)str;
    while (*s) {
        if (s[0] < 0x80) s++;
        else if ((s[0] & 0xE0) == 0xC0) {
            if ((s[1] & 0xC0) != 0x80) return false;
            s += 2;
        } else if ((s[0] & 0xF0) == 0xE0) {
            if ((s[1] & 0xC0) != 0x80 || (s[2] & 0xC0) != 0x80) return false;
            s += 3;
        } else if ((s[0] & 0xF8) == 0xF0) {
            if ((s[1] & 0xC0) != 0x80 || (s[2] & 0xC0) != 0x80 || (s[3] & 0xC0) != 0x80) return false;
            s += 4;
        } else return false;
    }
    return true;
}

size_t utf8_strlen(const char *str) {
    if (str == NULL) return 0;
    size_t len = 0;
    const unsigned char *s = (const unsigned char *)str;
    while (*s) {
        if ((*s & 0xC0) != 0x80) len++;
        s++;
    }
    return len;
}
