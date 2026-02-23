#include "ini.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char *section;
    char *key;
    char *value;
} ini_entry;

struct ini_s {
    ini_entry *entries;
    int count;
};

static char* trim(char *s) {
    while (isspace(*s)) s++;
    if (*s == 0) return s;
    char *end = s + strlen(s) - 1;
    while (end > s && isspace(*end)) end--;
    *(end + 1) = 0;
    return s;
}

static char* trim_copy(const char *s) {
    while (isspace(*s)) s++;
    size_t len = strlen(s);
    while (len > 0 && isspace(s[len - 1])) len--;
    char *result = malloc(len + 1);
    if (result) {
        memcpy(result, s, len);
        result[len] = '\0';
    }
    return result;
}

ini_t* ini_load(const char *filepath) {
    FILE *f = fopen(filepath, "r");
    if (!f) return NULL;

    ini_t *ini = malloc(sizeof(ini_t));
    if (!ini) { fclose(f); return NULL; }
    ini->entries = NULL;
    ini->count = 0;

    char line[256];
    char section[64] = "";

    while (fgets(line, sizeof(line), f)) {
        char *s = trim(line);
        if (*s == ';' || *s == '#' || *s == 0) continue;

        if (*s == '[' && s[strlen(s)-1] == ']') {
            strncpy(section, s + 1, strlen(s) - 2);
            section[strlen(s)-2] = 0;
        } else {
            char *eq = strchr(s, '=');
            if (eq) {
                *eq = 0;
                ini->entries = realloc(ini->entries, sizeof(ini_entry) * (ini->count + 1));
                ini->entries[ini->count].section = strdup(section);
                ini->entries[ini->count].key = strdup(trim(s));
                ini->entries[ini->count].value = strdup(trim(eq + 1));
                ini->count++;
            }
        }
    }
    fclose(f);
    return ini;
}

ini_t* ini_create(void) {
    ini_t *ini = malloc(sizeof(ini_t));
    if (!ini) return NULL;
    ini->entries = NULL;
    ini->count = 0;
    return ini;
}

void ini_free(ini_t *ini) {
    if (!ini) return;
    for (int i = 0; i < ini->count; i++) {
        free(ini->entries[i].section);
        free(ini->entries[i].key);
        free(ini->entries[i].value);
    }
    free(ini->entries);
    free(ini);
}

const char* ini_get(const ini_t *ini, const char *section, const char *key) {
    if (!ini || !section || !key) return NULL;
    for (int i = 0; i < ini->count; i++) {
        if (strcmp(ini->entries[i].section, section) == 0 &&
            strcmp(ini->entries[i].key, key) == 0) {
            return ini->entries[i].value;
        }
    }
    return NULL;
}

int ini_get_int(const ini_t *ini, const char *section, const char *key, int default_val) {
    const char *val = ini_get(ini, section, key);
    return val ? atoi(val) : default_val;
}

bool ini_get_bool(const ini_t *ini, const char *section, const char *key, bool default_val) {
    const char *val = ini_get(ini, section, key);
    if (!val) return default_val;
    if (strcmp(val, "true") == 0 || strcmp(val, "1") == 0 || 
        strcmp(val, "yes") == 0 || strcmp(val, "on") == 0) {
        return true;
    }
    return false;
}

double ini_get_double(const ini_t *ini, const char *section, const char *key, double default_val) {
    const char *val = ini_get(ini, section, key);
    return val ? atof(val) : default_val;
}

bool ini_set(ini_t *ini, const char *section, const char *key, const char *value) {
    if (!ini || !section || !key || !value) return false;
    
    for (int i = 0; i < ini->count; i++) {
        if (strcmp(ini->entries[i].section, section) == 0 &&
            strcmp(ini->entries[i].key, key) == 0) {
            free(ini->entries[i].value);
            ini->entries[i].value = strdup(value);
            return true;
        }
    }
    
    ini->entries = realloc(ini->entries, sizeof(ini_entry) * (ini->count + 1));
    ini->entries[ini->count].section = strdup(section);
    ini->entries[ini->count].key = strdup(key);
    ini->entries[ini->count].value = strdup(value);
    ini->count++;
    return true;
}

bool ini_set_int(ini_t *ini, const char *section, const char *key, int value) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", value);
    return ini_set(ini, section, key, buf);
}

bool ini_set_bool(ini_t *ini, const char *section, const char *key, bool value) {
    return ini_set(ini, section, key, value ? "true" : "false");
}

bool ini_set_double(ini_t *ini, const char *section, const char *key, double value) {
    char buf[64];
    snprintf(buf, sizeof(buf), "%g", value);
    return ini_set(ini, section, key, buf);
}

bool ini_delete(ini_t *ini, const char *section, const char *key) {
    if (!ini || !section || !key) return false;
    
    for (int i = 0; i < ini->count; i++) {
        if (strcmp(ini->entries[i].section, section) == 0 &&
            strcmp(ini->entries[i].key, key) == 0) {
            free(ini->entries[i].section);
            free(ini->entries[i].key);
            free(ini->entries[i].value);
            for (int j = i; j < ini->count - 1; j++) {
                ini->entries[j] = ini->entries[j + 1];
            }
            ini->count--;
            return true;
        }
    }
    return false;
}

bool ini_delete_section(ini_t *ini, const char *section) {
    if (!ini || !section) return false;
    
    int deleted = 0;
    for (int i = ini->count - 1; i >= 0; i--) {
        if (strcmp(ini->entries[i].section, section) == 0) {
            free(ini->entries[i].section);
            free(ini->entries[i].key);
            free(ini->entries[i].value);
            for (int j = i; j < ini->count - 1; j++) {
                ini->entries[j] = ini->entries[j + 1];
            }
            ini->count--;
            deleted++;
        }
    }
    return deleted > 0;
}

bool ini_save(const ini_t *ini, const char *filepath) {
    if (!ini || !filepath) return false;
    
    FILE *f = fopen(filepath, "w");
    if (!f) return false;
    
    const char *last_section = "";
    for (int i = 0; i < ini->count; i++) {
        if (strcmp(ini->entries[i].section, last_section) != 0) {
            if (i > 0) fprintf(f, "\n");
            fprintf(f, "[%s]\n", ini->entries[i].section);
            last_section = ini->entries[i].section;
        }
        fprintf(f, "%s = %s\n", ini->entries[i].key, ini->entries[i].value);
    }
    
    fclose(f);
    return true;
}

void ini_foreach(const ini_t *ini, ini_visit_fn visit, void *user_data) {
    if (!ini || !visit) return;
    for (int i = 0; i < ini->count; i++) {
        visit(ini->entries[i].section, ini->entries[i].key, ini->entries[i].value, user_data);
    }
}

void ini_foreach_section(const ini_t *ini, const char *section, ini_visit_fn visit, void *user_data) {
    if (!ini || !visit || !section) return;
    for (int i = 0; i < ini->count; i++) {
        if (strcmp(ini->entries[i].section, section) == 0) {
            visit(ini->entries[i].section, ini->entries[i].key, ini->entries[i].value, user_data);
        }
    }
}

bool ini_has_section(const ini_t *ini, const char *section) {
    if (!ini || !section) return false;
    for (int i = 0; i < ini->count; i++) {
        if (strcmp(ini->entries[i].section, section) == 0) {
            return true;
        }
    }
    return false;
}

bool ini_has_key(const ini_t *ini, const char *section, const char *key) {
    if (!ini || !section || !key) return false;
    for (int i = 0; i < ini->count; i++) {
        if (strcmp(ini->entries[i].section, section) == 0 &&
            strcmp(ini->entries[i].key, key) == 0) {
            return true;
        }
    }
    return false;
}
