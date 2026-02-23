#include "toml_parse.h"
#include <string.h>
#include <stdio.h>

const char* toml_get_string(const char *toml, const char *key, char *out, int max_len) {
    const char *line = strstr(toml, key);
    if (!line) return NULL;
    const char *eq = strchr(line, '=');
    if (!eq) return NULL;
    const char *q1 = strchr(eq, '"');
    if (!q1) return NULL;
    const char *q2 = strchr(q1 + 1, '"');
    if (!q2) return NULL;
    
    int len = (int)(q2 - q1 - 1);
    if (len >= max_len) len = max_len - 1;
    memcpy(out, q1 + 1, len);
    out[len] = '\0';
    return out;
}
