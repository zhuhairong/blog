#include "regex_tiny.h"

static bool match_here(const char *pat, const char *txt);
static bool match_star(int c, const char *pat, const char *txt);

bool regex_match(const char *pattern, const char *text) {
    if (pattern[0] == '^') return match_here(pattern + 1, text);
    do { if (match_here(pattern, text)) return true; } while (*text++ != '\0');
    return false;
}

static bool match_here(const char *pat, const char *txt) {
    if (pat[0] == '\0') return true;
    if (pat[1] == '*') return match_star(pat[0], pat + 2, txt);
    if (pat[0] == '$' && pat[1] == '\0') return *txt == '\0';
    if (*txt != '\0' && (pat[0] == '.' || pat[0] == *txt)) return match_here(pat + 1, txt + 1);
    return false;
}

static bool match_star(int c, const char *pat, const char *txt) {
    do { if (match_here(pat, txt)) return true; } while (*txt != '\0' && (*txt++ == c || c == '.'));
    return false;
}
