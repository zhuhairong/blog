#include "levenshtein.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

static bool char_equal(char c1, char c2, bool case_sensitive) {
    if (case_sensitive) {
        return c1 == c2;
    } else {
        return tolower((unsigned char)c1) == tolower((unsigned char)c2);
    }
}

static size_t levenshtein_naive(const char *s1, size_t s1_len, const char *s2, size_t s2_len, 
                               const levenshtein_config_t *config) {
    size_t **d = (size_t **)malloc((s1_len + 1) * sizeof(size_t *));
    if (!d) {
        return (size_t)-1;
    }

    for (size_t i = 0; i <= s1_len; i++) {
        d[i] = (size_t *)malloc((s2_len + 1) * sizeof(size_t));
        if (!d[i]) {
            for (size_t j = 0; j < i; j++) {
                free(d[j]);
            }
            free(d);
            return (size_t)-1;
        }
        d[i][0] = i;
    }

    for (size_t j = 0; j <= s2_len; j++) {
        d[0][j] = j;
    }

    for (size_t i = 1; i <= s1_len; i++) {
        for (size_t j = 1; j <= s2_len; j++) {
            size_t cost = char_equal(s1[i - 1], s2[j - 1], config->case_sensitive) ? 0 : 1;
            size_t deletion = config->include_deletion ? d[i - 1][j] + 1 : (size_t)-1;
            size_t insertion = config->include_insertion ? d[i][j - 1] + 1 : (size_t)-1;
            size_t substitution = config->include_substitution ? d[i - 1][j - 1] + cost : (size_t)-1;

            size_t min_val = (size_t)-1;
            if (deletion != (size_t)-1 && (min_val == (size_t)-1 || deletion < min_val)) {
                min_val = deletion;
            }
            if (insertion != (size_t)-1 && (min_val == (size_t)-1 || insertion < min_val)) {
                min_val = insertion;
            }
            if (substitution != (size_t)-1 && (min_val == (size_t)-1 || substitution < min_val)) {
                min_val = substitution;
            }

            d[i][j] = min_val;

            // 早停优化
            if (config->max_distance > 0 && d[i][j] > config->max_distance) {
                for (size_t x = 0; x <= s1_len; x++) {
                    free(d[x]);
                }
                free(d);
                return config->max_distance + 1;
            }
        }
    }

    size_t result = d[s1_len][s2_len];

    for (size_t i = 0; i <= s1_len; i++) {
        free(d[i]);
    }
    free(d);

    return result;
}

static size_t levenshtein_optimized(const char *s1, size_t s1_len, const char *s2, size_t s2_len, 
                                   const levenshtein_config_t *config) {
    if (s1_len < s2_len) {
        return levenshtein_optimized(s2, s2_len, s1, s1_len, config);
    }

    size_t *prev_row = (size_t *)malloc((s2_len + 1) * sizeof(size_t));
    size_t *curr_row = (size_t *)malloc((s2_len + 1) * sizeof(size_t));
    if (!prev_row || !curr_row) {
        free(prev_row);
        free(curr_row);
        return (size_t)-1;
    }

    for (size_t j = 0; j <= s2_len; j++) {
        prev_row[j] = j;
    }

    for (size_t i = 1; i <= s1_len; i++) {
        curr_row[0] = i;
        for (size_t j = 1; j <= s2_len; j++) {
            size_t cost = char_equal(s1[i - 1], s2[j - 1], config->case_sensitive) ? 0 : 1;
            size_t deletion = config->include_deletion ? prev_row[j] + 1 : (size_t)-1;
            size_t insertion = config->include_insertion ? curr_row[j - 1] + 1 : (size_t)-1;
            size_t substitution = config->include_substitution ? prev_row[j - 1] + cost : (size_t)-1;

            size_t min_val = (size_t)-1;
            if (deletion != (size_t)-1 && (min_val == (size_t)-1 || deletion < min_val)) {
                min_val = deletion;
            }
            if (insertion != (size_t)-1 && (min_val == (size_t)-1 || insertion < min_val)) {
                min_val = insertion;
            }
            if (substitution != (size_t)-1 && (min_val == (size_t)-1 || substitution < min_val)) {
                min_val = substitution;
            }

            curr_row[j] = min_val;

            // 早停优化
            if (config->max_distance > 0 && curr_row[j] > config->max_distance) {
                free(prev_row);
                free(curr_row);
                return config->max_distance + 1;
            }
        }

        size_t *temp = prev_row;
        prev_row = curr_row;
        curr_row = temp;
    }

    size_t result = prev_row[s2_len];

    free(prev_row);
    free(curr_row);

    return result;
}

size_t levenshtein_distance(const char *s1, const char *s2) {
    if (!s1 || !s2) {
        return (size_t)-1;
    }
    size_t s1_len = strlen(s1);
    size_t s2_len = strlen(s2);
    size_t distance;
    levenshtein_config_t config;
    levenshtein_get_default_config(&config);
    levenshtein_distance_ex(s1, s1_len, s2, s2_len, &distance, &config);
    return distance;
}

levenshtein_error_t levenshtein_distance_ex(const char *s1, size_t s1_len, const char *s2, size_t s2_len, 
                                           size_t *distance, const levenshtein_config_t *config) {
    if (!s1 || !s2 || !distance || !config) {
        return LEVENSHTEIN_INVALID_INPUT;
    }

    size_t result;
    if (config->use_optimized) {
        result = levenshtein_optimized(s1, s1_len, s2, s2_len, config);
    } else {
        result = levenshtein_naive(s1, s1_len, s2, s2_len, config);
    }

    if (result == (size_t)-1) {
        return LEVENSHTEIN_MEMORY_ERROR;
    }

    *distance = result;
    return LEVENSHTEIN_OK;
}

double levenshtein_similarity(const char *s1, const char *s2) {
    if (!s1 || !s2) {
        return 0.0;
    }
    size_t s1_len = strlen(s1);
    size_t s2_len = strlen(s2);
    size_t distance = levenshtein_distance(s1, s2);
    size_t max_len = s1_len > s2_len ? s1_len : s2_len;
    return max_len > 0 ? 1.0 - (double)distance / (double)max_len : 1.0;
}

double levenshtein_similarity_ex(const char *s1, size_t s1_len, const char *s2, size_t s2_len, 
                                const levenshtein_config_t *config, levenshtein_error_t *error) {
    if (!s1 || !s2 || !config) {
        if (error) *error = LEVENSHTEIN_INVALID_INPUT;
        return 0.0;
    }

    size_t distance;
    levenshtein_error_t err = levenshtein_distance_ex(s1, s1_len, s2, s2_len, &distance, config);
    if (error) *error = err;
    if (err != LEVENSHTEIN_OK) {
        return 0.0;
    }

    size_t max_len = s1_len > s2_len ? s1_len : s2_len;
    if (max_len == 0) {
        return 1.0;
    }

    double similarity = 1.0 - (double)distance / (double)max_len;
    return similarity > 0.0 ? similarity : 0.0;
}

size_t levenshtein_distance_optimized(const char *s1, const char *s2) {
    if (!s1 || !s2) {
        return (size_t)-1;
    }
    size_t s1_len = strlen(s1);
    size_t s2_len = strlen(s2);
    size_t distance;
    levenshtein_config_t config;
    levenshtein_get_default_config(&config);
    config.use_optimized = true;
    levenshtein_distance_ex(s1, s1_len, s2, s2_len, &distance, &config);
    return distance;
}

size_t levenshtein_distance_batch(const char *target, const char **strings, size_t count, 
                                 size_t *distances) {
    if (!target || !strings || !distances) {
        return 0;
    }

    size_t target_len = strlen(target);
    levenshtein_config_t config;
    levenshtein_get_default_config(&config);

    size_t calculated = 0;
    for (size_t i = 0; i < count; i++) {
        if (strings[i]) {
            size_t str_len = strlen(strings[i]);
            levenshtein_error_t error = levenshtein_distance_ex(target, target_len, 
                                                              strings[i], str_len, 
                                                              &distances[i], &config);
            if (error == LEVENSHTEIN_OK) {
                calculated++;
            } else {
                distances[i] = (size_t)-1;
            }
        } else {
            distances[i] = (size_t)-1;
        }
    }

    return calculated;
}

void levenshtein_get_default_config(levenshtein_config_t *config) {
    if (config) {
        config->case_sensitive = true;
        config->use_optimized = false;
        config->include_substitution = true;
        config->include_insertion = true;
        config->include_deletion = true;
        config->max_distance = 0;
    }
}
