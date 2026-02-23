#include "kmp.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

static void compute_lps(const char *pat, int m, int *lps) {
    int len = 0;
    lps[0] = 0;
    int i = 1;
    while (i < m) {
        if (pat[i] == pat[len]) {
            len++;
            lps[i] = len;
            i++;
        } else {
            if (len != 0) len = lps[len - 1];
            else { lps[i] = 0; i++; }
        }
    }
}

static void compute_lps_case_insensitive(const char *pat, int m, int *lps) {
    int len = 0;
    lps[0] = 0;
    int i = 1;
    while (i < m) {
        if (tolower((unsigned char)pat[i]) == tolower((unsigned char)pat[len])) {
            len++;
            lps[i] = len;
            i++;
        } else {
            if (len != 0) len = lps[len - 1];
            else { lps[i] = 0; i++; }
        }
    }
}

int kmp_search(const char *text, const char *pattern) {
    if (!text || !pattern) {
        return -1;
    }
    size_t text_len = strlen(text);
    size_t pattern_len = strlen(pattern);
    kmp_error_t error;
    return kmp_search_ex(text, text_len, pattern, pattern_len, &error);
}

int kmp_search_ex(const char *text, size_t text_len, const char *pattern, size_t pattern_len, 
                  kmp_error_t *error) {
    if (!text || !pattern) {
        if (error) *error = KMP_INVALID_INPUT;
        return -1;
    }

    if (pattern_len == 0) {
        if (error) *error = KMP_PATTERN_TOO_SHORT;
        return -1;
    }

    if (text_len < pattern_len) {
        if (error) *error = KMP_TEXT_TOO_SHORT;
        return -1;
    }

    int m = (int)pattern_len;
    int *lps = malloc(m * sizeof(int));
    if (!lps) {
        if (error) *error = KMP_MEMORY_ERROR;
        return -1;
    }

    compute_lps(pattern, m, lps);

    int i = 0, j = 0;
    int res = -1;
    while (i < (int)text_len) {
        if (pattern[j] == text[i]) {
            i++;
            j++;
        }
        if (j == m) {
            res = i - j;
            break;
        } else if (i < (int)text_len && pattern[j] != text[i]) {
            if (j != 0) {
                j = lps[j - 1];
            } else {
                i++;
            }
        }
    }

    free(lps);
    if (error) *error = KMP_OK;
    return res;
}

size_t kmp_search_all(const char *text, size_t text_len, const char *pattern, size_t pattern_len, 
                      size_t *matches, size_t max_matches, kmp_error_t *error) {
    if (!text || !pattern || !matches) {
        if (error) *error = KMP_INVALID_INPUT;
        return 0;
    }

    if (pattern_len == 0) {
        if (error) *error = KMP_PATTERN_TOO_SHORT;
        return 0;
    }

    if (text_len < pattern_len) {
        if (error) *error = KMP_TEXT_TOO_SHORT;
        return 0;
    }

    int m = (int)pattern_len;
    int *lps = malloc(m * sizeof(int));
    if (!lps) {
        if (error) *error = KMP_MEMORY_ERROR;
        return 0;
    }

    compute_lps(pattern, m, lps);

    int i = 0, j = 0;
    size_t match_count = 0;
    while (i < (int)text_len && match_count < max_matches) {
        if (pattern[j] == text[i]) {
            i++;
            j++;
        }
        if (j == m) {
            matches[match_count++] = i - j;
            j = lps[j - 1];
        } else if (i < (int)text_len && pattern[j] != text[i]) {
            if (j != 0) {
                j = lps[j - 1];
            } else {
                i++;
            }
        }
    }

    free(lps);
    if (error) *error = KMP_OK;
    return match_count;
}

int kmp_search_with_config(const char *text, size_t text_len, const char *pattern, size_t pattern_len, 
                          const kmp_config_t *config, kmp_error_t *error) {
    if (!text || !pattern || !config) {
        if (error) *error = KMP_INVALID_INPUT;
        return -1;
    }

    if (pattern_len == 0) {
        if (error) *error = KMP_PATTERN_TOO_SHORT;
        return -1;
    }

    if (text_len < pattern_len) {
        if (error) *error = KMP_TEXT_TOO_SHORT;
        return -1;
    }

    int m = (int)pattern_len;
    int *lps = malloc(m * sizeof(int));
    if (!lps) {
        if (error) *error = KMP_MEMORY_ERROR;
        return -1;
    }

    if (config->case_sensitive) {
        compute_lps(pattern, m, lps);
    } else {
        compute_lps_case_insensitive(pattern, m, lps);
    }

    int i = 0, j = 0;
    int res = -1;
    while (i < (int)text_len) {
        bool match;
        if (config->case_sensitive) {
            match = (pattern[j] == text[i]);
        } else {
            match = (tolower((unsigned char)pattern[j]) == tolower((unsigned char)text[i]));
        }

        if (match) {
            i++;
            j++;
        }
        if (j == m) {
            res = i - j;
            if (!config->find_all) {
                break;
            }
            j = lps[j - 1];
        } else if (i < (int)text_len && !match) {
            if (j != 0) {
                j = lps[j - 1];
            } else {
                i++;
            }
        }
    }

    free(lps);
    if (error) *error = KMP_OK;
    return res;
}

int kmp_build_lps(const char *pattern, size_t pattern_len, int *lps, kmp_error_t *error) {
    if (!pattern || !lps) {
        if (error) *error = KMP_INVALID_INPUT;
        return -1;
    }

    if (pattern_len == 0) {
        if (error) *error = KMP_PATTERN_TOO_SHORT;
        return -1;
    }

    compute_lps(pattern, (int)pattern_len, lps);
    if (error) *error = KMP_OK;
    return 0;
}

void kmp_get_default_config(kmp_config_t *config) {
    if (config) {
        config->case_sensitive = true;
        config->find_all = false;
        config->max_matches = 1000;
    }
}
