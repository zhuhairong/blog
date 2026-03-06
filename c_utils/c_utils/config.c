#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_SIZE 4096
#define MAX_SECTION_SIZE 256

typedef struct {
    char *section;
    char *key;
    char *value;
} config_entry_t;

struct config_s {
    config_entry_t *entries;
    size_t count;
    size_t capacity;
};

static char* trim(char *s) {
    while (isspace((unsigned char)*s)) s++;
    if (*s == 0) return s;
    char *end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return s;
}

static char* trim_copy(const char *s) {
    while (isspace((unsigned char)*s)) s++;
    size_t len = strlen(s);
    if (len == 0) return strdup("");
    
    const char *end = s + len - 1;
    while (end > s && isspace((unsigned char)*end)) end--;
    size_t trimmed_len = end - s + 1;
    
    char *result = malloc(trimmed_len + 1);
    if (result) {
        memcpy(result, s, trimmed_len);
        result[trimmed_len] = '\0';
    }
    return result;
}

// 创建配置
config_t* config_create(void) {
    config_t *cfg = calloc(1, sizeof(config_t));
    return cfg;
}

static config_t* config_load_ini(FILE *fp, config_error_t *error) {
    config_t *cfg = calloc(1, sizeof(config_t));
    if (!cfg) {
        if (error) *error = CONFIG_ERROR_MEMORY_ALLOC;
        return NULL;
    }

    char *line = malloc(MAX_LINE_SIZE);
    char *current_section = calloc(1, MAX_SECTION_SIZE);
    
    if (!line || !current_section) {
        free(cfg);
        free(line);
        free(current_section);
        if (error) *error = CONFIG_ERROR_MEMORY_ALLOC;
        return NULL;
    }

    while (fgets(line, MAX_LINE_SIZE, fp)) {
        char *t = trim(line);
        if (*t == ';' || *t == '#' || *t == '\0') continue;

        if (*t == '[' && t[strlen(t)-1] == ']') {
            size_t len = strlen(t);
            if (len > 2 && len - 2 < MAX_SECTION_SIZE) {
                strncpy(current_section, t + 1, len - 2);
                current_section[len - 2] = '\0';
            }
        } else {
            char *sep = strchr(t, '=');
            if (sep) {
                *sep = '\0';
                if (cfg->count >= cfg->capacity) {
                    size_t new_capacity = cfg->capacity == 0 ? 16 : cfg->capacity * 2;
                    config_entry_t *new_entries = realloc(cfg->entries, sizeof(config_entry_t) * new_capacity);
                    if (!new_entries) {
                        config_free(cfg);
                        free(line);
                        free(current_section);
                        if (error) *error = CONFIG_ERROR_MEMORY_ALLOC;
                        return NULL;
                    }
                    cfg->entries = new_entries;
                    cfg->capacity = new_capacity;
                }
                cfg->entries[cfg->count].section = strdup(current_section);
                cfg->entries[cfg->count].key = strdup(trim(t));
                cfg->entries[cfg->count].value = strdup(trim(sep + 1));
                cfg->count++;
            }
        }
    }
    
    free(line);
    free(current_section);
    
    if (error) *error = CONFIG_OK;
    return cfg;
}

static bool config_save_ini(const config_t *cfg, FILE *fp) {
    char current_section[MAX_SECTION_SIZE] = "";
    
    for (size_t i = 0; i < cfg->count; i++) {
        if (strcmp(cfg->entries[i].section, current_section) != 0) {
            strncpy(current_section, cfg->entries[i].section, MAX_SECTION_SIZE - 1);
            current_section[MAX_SECTION_SIZE - 1] = '\0';
            if (strlen(current_section) > 0) {
                fprintf(fp, "[%s]\n", current_section);
            }
        }
        fprintf(fp, "%s=%s\n", cfg->entries[i].key, cfg->entries[i].value);
    }
    return true;
}

static config_t* config_load_json(FILE *fp, config_error_t *error) {
    config_t *cfg = calloc(1, sizeof(config_t));
    if (!cfg) {
        if (error) *error = CONFIG_ERROR_MEMORY_ALLOC;
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    char *content = malloc(file_size + 1);
    if (!content) {
        config_free(cfg);
        if (error) *error = CONFIG_ERROR_MEMORY_ALLOC;
        return NULL;
    }
    
    size_t read_size = fread(content, 1, file_size, fp);
    content[read_size] = '\0';
    
    char *p = content;
    char current_section[MAX_SECTION_SIZE] = "";
    char key[256];
    char value[4096];
    int brace_depth = 0;
    
    while (*p) {
        if (*p == '{') {
            brace_depth++;
            p++;
        } else if (*p == '}') {
            brace_depth--;
            if (brace_depth == 1) {
                current_section[0] = '\0';
            }
            p++;
        } else if (*p == '"') {
            p++;
            char *key_start = p;
            while (*p && *p != '"') p++;
            size_t key_len = p - key_start;
            if (key_len > 0 && key_len < sizeof(key)) {
                strncpy(key, key_start, key_len);
                key[key_len] = '\0';
            }
            if (*p == '"') p++;
            
            while (*p && (*p == ':' || *p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')) p++;
            
            if (*p == '{') {
                strncpy(current_section, key, MAX_SECTION_SIZE - 1);
                current_section[MAX_SECTION_SIZE - 1] = '\0';
                brace_depth++;
                p++;
            } else if (*p == '"') {
                p++;
                char *val_start = p;
                while (*p && *p != '"') {
                    if (*p == '\\' && *(p+1)) p += 2;
                    else p++;
                }
                size_t val_len = p - val_start;
                if (val_len > 0 && val_len < sizeof(value)) {
                    strncpy(value, val_start, val_len);
                    value[val_len] = '\0';
                }
                if (*p == '"') p++;
                
                if (cfg->count >= cfg->capacity) {
                    size_t new_capacity = cfg->capacity == 0 ? 16 : cfg->capacity * 2;
                    config_entry_t *new_entries = realloc(cfg->entries, sizeof(config_entry_t) * new_capacity);
                    if (!new_entries) {
                        config_free(cfg);
                        free(content);
                        if (error) *error = CONFIG_ERROR_MEMORY_ALLOC;
                        return NULL;
                    }
                    cfg->entries = new_entries;
                    cfg->capacity = new_capacity;
                }
                cfg->entries[cfg->count].section = strdup(current_section);
                cfg->entries[cfg->count].key = strdup(key);
                cfg->entries[cfg->count].value = strdup(value);
                cfg->count++;
            } else if (*p && (*p == '-' || isdigit((unsigned char)*p))) {
                char *val_start = p;
                while (*p && (*p == '-' || *p == '.' || isdigit((unsigned char)*p))) p++;
                size_t val_len = p - val_start;
                if (val_len > 0 && val_len < sizeof(value)) {
                    strncpy(value, val_start, val_len);
                    value[val_len] = '\0';
                }
                
                if (cfg->count >= cfg->capacity) {
                    size_t new_capacity = cfg->capacity == 0 ? 16 : cfg->capacity * 2;
                    config_entry_t *new_entries = realloc(cfg->entries, sizeof(config_entry_t) * new_capacity);
                    if (!new_entries) {
                        config_free(cfg);
                        free(content);
                        if (error) *error = CONFIG_ERROR_MEMORY_ALLOC;
                        return NULL;
                    }
                    cfg->entries = new_entries;
                    cfg->capacity = new_capacity;
                }
                cfg->entries[cfg->count].section = strdup(current_section);
                cfg->entries[cfg->count].key = strdup(key);
                cfg->entries[cfg->count].value = strdup(value);
                cfg->count++;
            } else if (strncmp(p, "true", 4) == 0) {
                if (cfg->count >= cfg->capacity) {
                    size_t new_capacity = cfg->capacity == 0 ? 16 : cfg->capacity * 2;
                    config_entry_t *new_entries = realloc(cfg->entries, sizeof(config_entry_t) * new_capacity);
                    if (!new_entries) {
                        config_free(cfg);
                        free(content);
                        if (error) *error = CONFIG_ERROR_MEMORY_ALLOC;
                        return NULL;
                    }
                    cfg->entries = new_entries;
                    cfg->capacity = new_capacity;
                }
                cfg->entries[cfg->count].section = strdup(current_section);
                cfg->entries[cfg->count].key = strdup(key);
                cfg->entries[cfg->count].value = strdup("true");
                cfg->count++;
                p += 4;
            } else if (strncmp(p, "false", 5) == 0) {
                if (cfg->count >= cfg->capacity) {
                    size_t new_capacity = cfg->capacity == 0 ? 16 : cfg->capacity * 2;
                    config_entry_t *new_entries = realloc(cfg->entries, sizeof(config_entry_t) * new_capacity);
                    if (!new_entries) {
                        config_free(cfg);
                        free(content);
                        if (error) *error = CONFIG_ERROR_MEMORY_ALLOC;
                        return NULL;
                    }
                    cfg->entries = new_entries;
                    cfg->capacity = new_capacity;
                }
                cfg->entries[cfg->count].section = strdup(current_section);
                cfg->entries[cfg->count].key = strdup(key);
                cfg->entries[cfg->count].value = strdup("false");
                cfg->count++;
                p += 5;
            }
        } else {
            p++;
        }
    }
    
    free(content);
    if (error) *error = CONFIG_OK;
    return cfg;
}

static bool config_save_json(const config_t *cfg, FILE *fp) {
    fprintf(fp, "{\n");
    
    char **sections = NULL;
    size_t section_count = 0;
    
    for (size_t i = 0; i < cfg->count; i++) {
        bool found = false;
        for (size_t j = 0; j < section_count; j++) {
            if (strcmp(sections[j], cfg->entries[i].section) == 0) {
                found = true;
                break;
            }
        }
        if (!found) {
            char **new_sections = realloc(sections, sizeof(char*) * (section_count + 1));
            if (new_sections) {
                sections = new_sections;
                sections[section_count] = strdup(cfg->entries[i].section);
                section_count++;
            }
        }
    }
    
    for (size_t s = 0; s < section_count; s++) {
        const char *sec = sections[s];
        bool is_default = (strlen(sec) == 0);
        
        if (!is_default) {
            fprintf(fp, "  \"%s\": {\n", sec);
        }
        
        bool first = true;
        for (size_t i = 0; i < cfg->count; i++) {
            if (strcmp(cfg->entries[i].section, sec) == 0) {
                if (!first) fprintf(fp, ",\n");
                first = false;
                
                const char *val = cfg->entries[i].value;
                bool is_number = (*val == '-' || isdigit((unsigned char)*val));
                bool is_bool = (strcmp(val, "true") == 0 || strcmp(val, "false") == 0);
                
                if (is_default) {
                    fprintf(fp, "  \"%s\": ", cfg->entries[i].key);
                } else {
                    fprintf(fp, "    \"%s\": ", cfg->entries[i].key);
                }
                
                if (is_number || is_bool) {
                    fprintf(fp, "%s", val);
                } else {
                    fprintf(fp, "\"%s\"", val);
                }
            }
        }
        fprintf(fp, "\n");
        
        if (!is_default) {
            fprintf(fp, "  }%s\n", (s < section_count - 1) ? "," : "");
        }
    }
    
    fprintf(fp, "}\n");
    
    for (size_t i = 0; i < section_count; i++) {
        free(sections[i]);
    }
    free(sections);
    
    return true;
}

static config_format_t detect_format(const char *filename) {
    const char *ext = strrchr(filename, '.');
    if (!ext) return CONFIG_FORMAT_INI;
    
    if (strcasecmp(ext, ".json") == 0) return CONFIG_FORMAT_JSON;
    if (strcasecmp(ext, ".yaml") == 0 || strcasecmp(ext, ".yml") == 0) return CONFIG_FORMAT_YAML;
    return CONFIG_FORMAT_INI;
}

config_t* config_load(const char *filename, config_format_t format, config_error_t *error) {
    if (!filename) {
        if (error) *error = CONFIG_ERROR_FILE_OPEN;
        return NULL;
    }

    FILE *fp = fopen(filename, "r");
    if (!fp) {
        if (error) *error = CONFIG_ERROR_FILE_OPEN;
        return NULL;
    }

    config_t *cfg = NULL;
    
    if (format == CONFIG_FORMAT_AUTO) {
        format = detect_format(filename);
    }
    
    switch (format) {
        case CONFIG_FORMAT_JSON:
            cfg = config_load_json(fp, error);
            break;
        case CONFIG_FORMAT_YAML:
            if (error) *error = CONFIG_ERROR_UNSUPPORTED_FORMAT;
            cfg = NULL;
            break;
        case CONFIG_FORMAT_INI:
        default:
            cfg = config_load_ini(fp, error);
            break;
    }
    
    fclose(fp);
    return cfg;
}

bool config_save(const config_t *cfg, const char *filename, config_format_t format, config_error_t *error) {
    if (!cfg || !filename) {
        if (error) *error = CONFIG_ERROR_INVALID_KEY;
        return false;
    }

    FILE *fp = fopen(filename, "w");
    if (!fp) {
        if (error) *error = CONFIG_ERROR_FILE_OPEN;
        return false;
    }

    if (format == CONFIG_FORMAT_AUTO) {
        format = detect_format(filename);
    }
    
    bool result = false;
    switch (format) {
        case CONFIG_FORMAT_JSON:
            result = config_save_json(cfg, fp);
            break;
        case CONFIG_FORMAT_YAML:
            if (error) *error = CONFIG_ERROR_UNSUPPORTED_FORMAT;
            result = false;
            break;
        case CONFIG_FORMAT_INI:
        default:
            result = config_save_ini(cfg, fp);
            break;
    }

    fclose(fp);
    if (error) *error = result ? CONFIG_OK : CONFIG_ERROR_FILE_WRITE;
    return result;
}

// 释放配置
void config_free(config_t *cfg) {
    if (!cfg) return;
    for (size_t i = 0; i < cfg->count; i++) {
        free(cfg->entries[i].section);
        free(cfg->entries[i].key);
        free(cfg->entries[i].value);
    }
    free(cfg->entries);
    free(cfg);
}

// 获取字符串配置值
const char* config_get_string(const config_t *cfg, const char *section, const char *key, const char *default_val) {
    if (!cfg || !key) return default_val;
    
    const char *sec = section ? section : "";
    
    for (size_t i = 0; i < cfg->count; i++) {
        if (strcmp(cfg->entries[i].section, sec) == 0 &&
            strcmp(cfg->entries[i].key, key) == 0) {
            return cfg->entries[i].value;
        }
    }
    return default_val;
}

// 获取整数配置值
int config_get_int(const config_t *cfg, const char *section, const char *key, int default_val) {
    const char *val = config_get_string(cfg, section, key, NULL);
    if (!val) return default_val;
    return atoi(val);
}

// 获取浮点数配置值
double config_get_double(const config_t *cfg, const char *section, const char *key, double default_val) {
    const char *val = config_get_string(cfg, section, key, NULL);
    if (!val) return default_val;
    return atof(val);
}

// 获取布尔值配置值
bool config_get_bool(const config_t *cfg, const char *section, const char *key, bool default_val) {
    const char *val = config_get_string(cfg, section, key, NULL);
    if (!val) return default_val;
    
    if (strcasecmp(val, "true") == 0 || strcmp(val, "1") == 0 ||
        strcasecmp(val, "yes") == 0 || strcasecmp(val, "on") == 0) {
        return true;
    }
    if (strcasecmp(val, "false") == 0 || strcmp(val, "0") == 0 ||
        strcasecmp(val, "no") == 0 || strcasecmp(val, "off") == 0) {
        return false;
    }
    return default_val;
}

// 设置字符串配置值
bool config_set_string(config_t *cfg, const char *section, const char *key, const char *value) {
    if (!cfg || !key || !value) return false;
    
    const char *sec = section ? section : "";
    
    // 查找是否已存在
    for (size_t i = 0; i < cfg->count; i++) {
        if (strcmp(cfg->entries[i].section, sec) == 0 &&
            strcmp(cfg->entries[i].key, key) == 0) {
            free(cfg->entries[i].value);
            cfg->entries[i].value = strdup(value);
            return true;
        }
    }
    
    // 添加新条目
    if (cfg->count >= cfg->capacity) {
        size_t new_capacity = cfg->capacity == 0 ? 16 : cfg->capacity * 2;
        config_entry_t *new_entries = realloc(cfg->entries, sizeof(config_entry_t) * new_capacity);
        if (!new_entries) return false;
        cfg->entries = new_entries;
        cfg->capacity = new_capacity;
    }
    
    cfg->entries[cfg->count].section = strdup(sec);
    cfg->entries[cfg->count].key = strdup(key);
    cfg->entries[cfg->count].value = strdup(value);
    cfg->count++;
    return true;
}

// 设置整数配置值
bool config_set_int(config_t *cfg, const char *section, const char *key, int value) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", value);
    return config_set_string(cfg, section, key, buf);
}

// 设置浮点数配置值
bool config_set_double(config_t *cfg, const char *section, const char *key, double value) {
    char buf[64];
    snprintf(buf, sizeof(buf), "%g", value);
    return config_set_string(cfg, section, key, buf);
}

// 设置布尔值配置值
bool config_set_bool(config_t *cfg, const char *section, const char *key, bool value) {
    return config_set_string(cfg, section, key, value ? "true" : "false");
}

// 删除配置项
bool config_remove(config_t *cfg, const char *section, const char *key) {
    if (!cfg || !key) return false;
    
    const char *sec = section ? section : "";
    
    for (size_t i = 0; i < cfg->count; i++) {
        if (strcmp(cfg->entries[i].section, sec) == 0 &&
            strcmp(cfg->entries[i].key, key) == 0) {
            free(cfg->entries[i].section);
            free(cfg->entries[i].key);
            free(cfg->entries[i].value);
            
            for (size_t j = i; j < cfg->count - 1; j++) {
                cfg->entries[j] = cfg->entries[j + 1];
            }
            cfg->count--;
            return true;
        }
    }
    return false;
}

bool config_delete(config_t *cfg, const char *section, const char *key) {
    return config_remove(cfg, section, key);
}

// 检查配置项是否存在
bool config_has_key(const config_t *cfg, const char *section, const char *key) {
    if (!cfg || !key) return false;
    
    const char *sec = section ? section : "";
    
    for (size_t i = 0; i < cfg->count; i++) {
        if (strcmp(cfg->entries[i].section, sec) == 0 &&
            strcmp(cfg->entries[i].key, key) == 0) {
            return true;
        }
    }
    return false;
}

bool config_exists(const config_t *cfg, const char *section, const char *key) {
    return config_has_key(cfg, section, key);
}

// 获取所有节名
char** config_get_sections(const config_t *cfg, size_t *count) {
    if (!cfg || !count) return NULL;
    
    // 使用简单数组去重
    char **sections = NULL;
    size_t section_count = 0;
    
    for (size_t i = 0; i < cfg->count; i++) {
        bool found = false;
        for (size_t j = 0; j < section_count; j++) {
            if (strcmp(sections[j], cfg->entries[i].section) == 0) {
                found = true;
                break;
            }
        }
        if (!found) {
            char **new_sections = realloc(sections, sizeof(char*) * (section_count + 1));
            if (!new_sections) {
                for (size_t k = 0; k < section_count; k++) free(sections[k]);
                free(sections);
                return NULL;
            }
            sections = new_sections;
            sections[section_count] = strdup(cfg->entries[i].section);
            section_count++;
        }
    }
    
    *count = section_count;
    return sections;
}

// 释放节名列表
void config_free_sections(char **sections, size_t count) {
    if (!sections) return;
    for (size_t i = 0; i < count; i++) {
        free(sections[i]);
    }
    free(sections);
}

// 获取指定节的所有键
char** config_get_keys(const config_t *cfg, const char *section, size_t *count) {
    if (!cfg || !count) return NULL;
    
    const char *sec = section ? section : "";
    char **keys = NULL;
    size_t key_count = 0;
    
    for (size_t i = 0; i < cfg->count; i++) {
        if (strcmp(cfg->entries[i].section, sec) == 0) {
            char **new_keys = realloc(keys, sizeof(char*) * (key_count + 1));
            if (!new_keys) {
                for (size_t k = 0; k < key_count; k++) free(keys[k]);
                free(keys);
                return NULL;
            }
            keys = new_keys;
            keys[key_count] = strdup(cfg->entries[i].key);
            key_count++;
        }
    }
    
    *count = key_count;
    return keys;
}

// 释放键列表
void config_free_keys(char **keys, size_t count) {
    if (!keys) return;
    for (size_t i = 0; i < count; i++) {
        free(keys[i]);
    }
    free(keys);
}

// 清除所有配置
void config_clear(config_t *cfg) {
    if (!cfg) return;
    for (size_t i = 0; i < cfg->count; i++) {
        free(cfg->entries[i].section);
        free(cfg->entries[i].key);
        free(cfg->entries[i].value);
    }
    free(cfg->entries);
    cfg->entries = NULL;
    cfg->count = 0;
    cfg->capacity = 0;
}

// 获取错误信息
const char* config_error_string(config_error_t error) {
    switch (error) {
        case CONFIG_OK: return "Success";
        case CONFIG_ERROR_FILE_OPEN: return "Failed to open file";
        case CONFIG_ERROR_FILE_READ: return "Failed to read file";
        case CONFIG_ERROR_FILE_WRITE: return "Failed to write file";
        case CONFIG_ERROR_PARSE: return "Parse error";
        case CONFIG_ERROR_INVALID_SECTION: return "Invalid section";
        case CONFIG_ERROR_INVALID_KEY: return "Invalid key";
        case CONFIG_ERROR_INVALID_VALUE: return "Invalid value";
        case CONFIG_ERROR_MEMORY_ALLOC: return "Memory allocation failed";
        case CONFIG_ERROR_UNSUPPORTED_FORMAT: return "Unsupported format";
        default: return "Unknown error";
    }
}

const char* config_strerror(config_error_t error) {
    return config_error_string(error);
}

size_t config_count(const config_t *cfg) {
    return cfg ? cfg->count : 0;
}
