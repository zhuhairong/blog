#include "manacher.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

static char *preprocess_string(const char *s, size_t s_len, bool case_sensitive, size_t *out_len) {
    size_t m = 2 * s_len + 3;
    char *t = (char *)malloc(m * sizeof(char));
    if (!t) {
        return NULL;
    }

    t[0] = '$';
    t[1] = '#';
    for (size_t i = 0; i < s_len; i++) {
        t[2 * i + 2] = case_sensitive ? s[i] : tolower((unsigned char)s[i]);
        t[2 * i + 3] = '#';
    }
    t[m - 1] = '@';
    *out_len = m;
    return t;
}

static manacher_error_t manacher_core(const char *s, size_t s_len, const manacher_config_t *config, 
                                     size_t *length, size_t *start, int **p_out, size_t *p_len) {
    if (!s || s_len == 0) {
        return MANACHER_EMPTY_STRING;
    }

    if (s_len > config->max_string_length) {
        return MANACHER_STRING_TOO_LONG;
    }

    size_t t_len;
    char *t = preprocess_string(s, s_len, config->case_sensitive, &t_len);
    if (!t) {
        return MANACHER_MEMORY_ERROR;
    }

    int *p = (int *)calloc(t_len, sizeof(int));
    if (!p) {
        free(t);
        return MANACHER_MEMORY_ERROR;
    }

    int center = 0, right = 0, max_len = 0, max_center = 0;
    for (int i = 1; i < (int)t_len - 1; i++) {
        if (i < right) {
            p[i] = (p[2 * center - i] < right - i) ? p[2 * center - i] : right - i;
        }

        while (t[i + p[i] + 1] == t[i - p[i] - 1]) {
            p[i]++;
        }

        if (i + p[i] > right) {
            center = i;
            right = i + p[i];
        }

        if (p[i] > max_len) {
            max_len = p[i];
            max_center = i;
        }
    }

    if (length) {
        *length = (size_t)max_len;
    }

    if (start && max_len > 0) {
        *start = (size_t)((max_center - max_len) / 2);
    }

    if (p_out) {
        *p_out = p;
        *p_len = t_len;
    } else {
        free(p);
    }

    free(t);
    return MANACHER_OK;
}

int manacher_longest_palindrome(const char *s) {
    if (!s) {
        return 0;
    }
    size_t s_len = strlen(s);
    size_t length, start;
    manacher_config_t config;
    manacher_get_default_config(&config);
    manacher_error_t error = manacher_longest_palindrome_ex(s, s_len, &length, &start, &config);
    return error == MANACHER_OK ? (int)length : 0;
}

manacher_error_t manacher_longest_palindrome_ex(const char *s, size_t s_len, 
                                              size_t *length, size_t *start, 
                                              const manacher_config_t *config) {
    if (!s || !config) {
        return MANACHER_INVALID_INPUT;
    }

    if (length) {
        *length = 0;
    }

    if (start) {
        *start = 0;
    }

    if (s_len == 0) {
        return MANACHER_EMPTY_STRING;
    }

    return manacher_core(s, s_len, config, length, start, NULL, NULL);
}

static bool char_equal(char a, char b, bool case_sensitive) {
    if (case_sensitive) {
        return a == b;
    } else {
        return tolower((unsigned char)a) == tolower((unsigned char)b);
    }
}

bool manacher_is_palindrome(const char *s, size_t s_len, bool case_sensitive) {
    if (!s || s_len == 0) {
        return true;
    }

    for (size_t i = 0; i < s_len / 2; i++) {
        if (!char_equal(s[i], s[s_len - i - 1], case_sensitive)) {
            return false;
        }
    }

    return true;
}

static size_t find_all_palindromes(const char *s, size_t s_len, bool case_sensitive, 
                                  size_t **positions, size_t **lengths, 
                                  size_t max_results, manacher_error_t *error) {
    if (!s || s_len == 0) {
        if (error) *error = MANACHER_EMPTY_STRING;
        return 0;
    }

    if (positions) {
        *positions = (size_t *)malloc(max_results * sizeof(size_t));
        if (!*positions) {
            if (error) *error = MANACHER_MEMORY_ERROR;
            return 0;
        }
    }

    if (lengths) {
        *lengths = (size_t *)malloc(max_results * sizeof(size_t));
        if (!*lengths) {
            if (positions) free(*positions);
            if (error) *error = MANACHER_MEMORY_ERROR;
            return 0;
        }
    }

    size_t count = 0;
    for (size_t i = 0; i < s_len && count < max_results; i++) {
        for (size_t j = i; j < s_len && count < max_results; j++) {
            size_t len = j - i + 1;
            bool is_pal = true;
            for (size_t k = 0; k < len / 2; k++) {
                if (!char_equal(s[i + k], s[j - k], case_sensitive)) {
                    is_pal = false;
                    break;
                }
            }
            if (is_pal) {
                if (positions) {
                    (*positions)[count] = i;
                }
                if (lengths) {
                    (*lengths)[count] = len;
                }
                count++;
            }
        }
    }

    if (error) {
        *error = MANACHER_OK;
    }
    return count;
}

size_t manacher_find_all(const char *s, size_t s_len, 
                        size_t **positions, size_t **lengths, 
                        size_t max_results, manacher_error_t *error) {
    if (!s || !positions || !lengths) {
        if (error) *error = MANACHER_INVALID_INPUT;
        return 0;
    }

    manacher_config_t config;
    manacher_get_default_config(&config);
    return find_all_palindromes(s, s_len, config.case_sensitive, positions, lengths, max_results, error);
}

size_t manacher_find_all_struct(const char *s, size_t s_len, 
                               manacher_palindrome_t *results, 
                               size_t max_results, manacher_error_t *error) {
    if (!s || !results) {
        if (error) *error = MANACHER_INVALID_INPUT;
        return 0;
    }

    manacher_config_t config;
    manacher_get_default_config(&config);

    size_t *positions = (size_t *)malloc(max_results * sizeof(size_t));
    size_t *lengths = (size_t *)malloc(max_results * sizeof(size_t));
    if (!positions || !lengths) {
        free(positions);
        free(lengths);
        if (error) *error = MANACHER_MEMORY_ERROR;
        return 0;
    }

    size_t count = find_all_palindromes(s, s_len, config.case_sensitive, &positions, &lengths, max_results, error);

    for (size_t i = 0; i < count; i++) {
        results[i].start = positions[i];
        results[i].length = lengths[i];
    }

    free(positions);
    free(lengths);
    return count;
}

void manacher_get_default_config(manacher_config_t *config) {
    if (config) {
        config->case_sensitive = true;
        config->find_all = false;
        config->include_single_char = true;
        config->max_string_length = 10000;
        config->enable_stats = false;
    }
}
