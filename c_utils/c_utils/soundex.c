#include "soundex.h"
#include <ctype.h>
#include <string.h>

static char get_code(char c) {
    c = (char)toupper(c);
    if (strchr("BFPV", c)) return '1';
    if (strchr("CGJKQSXZ", c)) return '2';
    if (strchr("DT", c)) return '3';
    if (c == 'L') return '4';
    if (strchr("MN", c)) return '5';
    if (c == 'R') return '6';
    return '0';
}

void soundex_encode(const char *s, char *res) {
    if (!s || !*s) { strcpy(res, "0000"); return; }
    res[0] = (char)toupper(s[0]);
    int idx = 1;
    char prev_code = get_code(s[0]);
    for (int i = 1; s[i] && idx < 4; i++) {
        char code = get_code(s[i]);
        if (code != '0' && code != prev_code) {
            res[idx++] = code;
        }
        prev_code = code;
    }
    while (idx < 4) res[idx++] = '0';
    res[4] = '\0';
}
