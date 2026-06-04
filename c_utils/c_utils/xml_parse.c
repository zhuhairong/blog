#include "xml_parse.h"
#include <string.h>
#include <stdio.h>

const char* xml_get_tag_content(const char *xml, const char *tag, char *out, int max_len) {
    char start_pattern[64], end_tag[64];
    size_t tag_len;

    snprintf(start_pattern, sizeof(start_pattern), "<%s", tag);
    snprintf(end_tag, sizeof(end_tag), "</%s>", tag);
    tag_len = strlen(tag);

    const char *s = strstr(xml, start_pattern);
    if (!s) return NULL;

    /* Ensure we matched a full tag name, not a prefix (e.g. <item> not <items>) */
    const char *after_tag = s + 1 + tag_len;  /* skip past '<' and tag name */
    if (*after_tag != '>' && *after_tag != ' ' && *after_tag != '\t' && *after_tag != '\n') {
        /* Not a tag boundary — this is a different tag that starts with our tag name.
         * Search again from after this position. */
        s = strstr(after_tag, start_pattern);
        if (!s) return NULL;
        after_tag = s + 1 + tag_len;
        if (*after_tag != '>' && *after_tag != ' ' && *after_tag != '\t' && *after_tag != '\n') {
            return NULL;
        }
    }

    /* Find the closing '>' of the start tag (may have attributes) */
    s = strchr(s, '>');
    if (!s) return NULL;
    s++; /* skip past '>' */

    const char *e = strstr(s, end_tag);
    if (!e) return NULL;

    int len = (int)(e - s);
    if (len >= max_len) len = max_len - 1;
    memcpy(out, s, len);
    out[len] = '\0';
    return out;
}
