#include "xml_parse.h"
#include <string.h>
#include <stdio.h>

const char* xml_get_tag_content(const char *xml, const char *tag, char *out, int max_len) {
    char start_tag[64], end_tag[64];
    snprintf(start_tag, sizeof(start_tag), "<%s>", tag);
    snprintf(end_tag, sizeof(end_tag), "</%s>", tag);
    
    const char *s = strstr(xml, start_tag);
    if (!s) return NULL;
    s += strlen(start_tag);
    const char *e = strstr(s, end_tag);
    if (!e) return NULL;
    
    int len = (int)(e - s);
    if (len >= max_len) len = max_len - 1;
    memcpy(out, s, len);
    out[len] = '\0';
    return out;
}
