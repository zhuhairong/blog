#include "kv_store.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

static kv_error_t parse_line(const char *line, char **key, char **value) {
    char *sep = strchr(line, '=');
    if (!sep) {
        return KV_PARSE_ERROR;
    }

    size_t key_len = sep - line;
    size_t value_len = strlen(sep + 1);

    *key = (char *)malloc(key_len + 1);
    if (!*key) {
        return KV_MEMORY_ERROR;
    }

    *value = (char *)malloc(value_len + 1);
    if (!*value) {
        free(*key);
        return KV_MEMORY_ERROR;
    }

    strncpy(*key, line, key_len);
    (*key)[key_len] = '\0';

    strncpy(*value, sep + 1, value_len);
    (*value)[value_len] = '\0';

    // 移除行尾的换行符
    (*value)[strcspn(*value, "\r\n")] = '\0';

    return KV_OK;
}

static kv_error_t read_all_entries(const char *filename, kv_entry_t **entries, size_t *count) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        // 文件不存在，返回空条目
        *entries = NULL;
        *count = 0;
        return KV_OK;
    }

    kv_entry_t *tmp_entries = NULL;
    size_t tmp_count = 0;
    size_t capacity = 16;

    tmp_entries = (kv_entry_t *)malloc(capacity * sizeof(kv_entry_t));
    if (!tmp_entries) {
        fclose(fp);
        return KV_MEMORY_ERROR;
    }

    char line[4096];
    while (fgets(line, sizeof(line), fp)) {
        if (tmp_count >= capacity) {
            capacity *= 2;
            kv_entry_t *new_entries = (kv_entry_t *)realloc(tmp_entries, capacity * sizeof(kv_entry_t));
            if (!new_entries) {
                for (size_t i = 0; i < tmp_count; i++) {
                    free(tmp_entries[i].key);
                    free(tmp_entries[i].value);
                }
                free(tmp_entries);
                fclose(fp);
                return KV_MEMORY_ERROR;
            }
            tmp_entries = new_entries;
        }

        kv_error_t error = parse_line(line, &tmp_entries[tmp_count].key, &tmp_entries[tmp_count].value);
        if (error == KV_OK) {
            tmp_count++;
        }
    }

    fclose(fp);

    *entries = tmp_entries;
    *count = tmp_count;
    return KV_OK;
}

static kv_error_t write_entries(const char *filename, const kv_entry_t *entries, size_t count) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        return KV_FILE_ERROR;
    }

    for (size_t i = 0; i < count; i++) {
        if (fprintf(fp, "%s=%s\n", entries[i].key, entries[i].value) < 0) {
            fclose(fp);
            return KV_WRITE_ERROR;
        }
    }

    fclose(fp);
    return KV_OK;
}

bool kv_save(const char *filename, const char *key, const char *value) {
    kv_error_t error = kv_save_ex(filename, key, value, strlen(value));
    return error == KV_OK;
}

char* kv_load(const char *filename, const char *key) {
    char buffer[4096];
    size_t value_len;
    kv_error_t error = kv_load_ex(filename, key, buffer, sizeof(buffer), &value_len);
    if (error != KV_OK) {
        return NULL;
    }
    return strdup(buffer);
}

kv_error_t kv_save_ex(const char *filename, const char *key, const char *value, size_t value_len) {
    if (!filename || !key || !value) {
        return KV_INVALID_INPUT;
    }

    kv_entry_t *entries = NULL;
    size_t count;
    kv_error_t error = read_all_entries(filename, &entries, &count);
    if (error != KV_OK) {
        return error;
    }

    // 检查是否已存在该键
    bool key_exists = false;
    for (size_t i = 0; i < count; i++) {
        if (strcmp(entries[i].key, key) == 0) {
            free(entries[i].value);
            entries[i].value = (char *)malloc(value_len + 1);
            if (!entries[i].value) {
                kv_free_entries(entries, count);
                return KV_MEMORY_ERROR;
            }
            strncpy(entries[i].value, value, value_len);
            entries[i].value[value_len] = '\0';
            key_exists = true;
            break;
        }
    }

    // 如果键不存在，添加新条目
    if (!key_exists) {
        kv_entry_t *new_entries = (kv_entry_t *)realloc(entries, (count + 1) * sizeof(kv_entry_t));
        if (!new_entries) {
            kv_free_entries(entries, count);
            return KV_MEMORY_ERROR;
        }
        entries = new_entries;

        entries[count].key = strdup(key);
        if (!entries[count].key) {
            kv_free_entries(entries, count);
            return KV_MEMORY_ERROR;
        }

        entries[count].value = (char *)malloc(value_len + 1);
        if (!entries[count].value) {
            free(entries[count].key);
            kv_free_entries(entries, count);
            return KV_MEMORY_ERROR;
        }

        strncpy(entries[count].value, value, value_len);
        entries[count].value[value_len] = '\0';
        count++;
    }

    error = write_entries(filename, entries, count);
    kv_free_entries(entries, count);
    return error;
}

kv_error_t kv_load_ex(const char *filename, const char *key, char *buffer, size_t buffer_size, size_t *value_len) {
    if (!filename || !key || !buffer || !value_len) {
        return KV_INVALID_INPUT;
    }

    kv_entry_t *entries = NULL;
    size_t count;
    kv_error_t error = read_all_entries(filename, &entries, &count);
    if (error != KV_OK) {
        return error;
    }

    bool found = false;
    for (size_t i = 0; i < count; i++) {
        if (strcmp(entries[i].key, key) == 0) {
            size_t len = strlen(entries[i].value);
            if (len >= buffer_size) {
                kv_free_entries(entries, count);
                return KV_BUFFER_TOO_SMALL;
            }
            strcpy(buffer, entries[i].value);
            *value_len = len;
            found = true;
            break;
        }
    }

    kv_free_entries(entries, count);
    return found ? KV_OK : KV_KEY_NOT_FOUND;
}

kv_error_t kv_delete(const char *filename, const char *key) {
    if (!filename || !key) {
        return KV_INVALID_INPUT;
    }

    kv_entry_t *entries = NULL;
    size_t count;
    kv_error_t error = read_all_entries(filename, &entries, &count);
    if (error != KV_OK) {
        return error;
    }

    // 过滤掉要删除的键
    kv_entry_t *new_entries = NULL;
    size_t new_count = 0;
    size_t capacity = count;

    new_entries = (kv_entry_t *)malloc(capacity * sizeof(kv_entry_t));
    if (!new_entries) {
        kv_free_entries(entries, count);
        return KV_MEMORY_ERROR;
    }

    for (size_t i = 0; i < count; i++) {
        if (strcmp(entries[i].key, key) != 0) {
            if (new_count >= capacity) {
                capacity *= 2;
                kv_entry_t *tmp = (kv_entry_t *)realloc(new_entries, capacity * sizeof(kv_entry_t));
                if (!tmp) {
                    kv_free_entries(entries, count);
                    kv_free_entries(new_entries, new_count);
                    return KV_MEMORY_ERROR;
                }
                new_entries = tmp;
            }
            new_entries[new_count].key = entries[i].key;
            new_entries[new_count].value = entries[i].value;
            new_count++;
        } else {
            // 释放要删除的条目
            free(entries[i].key);
            free(entries[i].value);
        }
    }

    // 释放原始条目数组（已转移的条目不再释放）
    free(entries);

    error = write_entries(filename, new_entries, new_count);
    kv_free_entries(new_entries, new_count);
    return error;
}

bool kv_exists(const char *filename, const char *key) {
    char buffer[1];
    size_t value_len;
    kv_error_t error = kv_load_ex(filename, key, buffer, sizeof(buffer), &value_len);
    return error == KV_OK;
}

size_t kv_get_all(const char *filename, kv_entry_t **entries, kv_error_t *error) {
    if (!filename || !entries) {
        if (error) *error = KV_INVALID_INPUT;
        return 0;
    }

    size_t count;
    kv_error_t err = read_all_entries(filename, entries, &count);
    if (error) *error = err;
    return count;
}

kv_error_t kv_save_batch(const char *filename, const kv_entry_t *entries, size_t count) {
    if (!filename || !entries || count == 0) {
        return KV_INVALID_INPUT;
    }

    for (size_t i = 0; i < count; i++) {
        kv_error_t error = kv_save_ex(filename, entries[i].key, entries[i].value, strlen(entries[i].value));
        if (error != KV_OK) {
            return error;
        }
    }

    return KV_OK;
}

kv_error_t kv_clear(const char *filename) {
    if (!filename) {
        return KV_INVALID_INPUT;
    }

    FILE *fp = fopen(filename, "w");
    if (!fp) {
        return KV_FILE_ERROR;
    }

    fclose(fp);
    return KV_OK;
}

kv_error_t kv_get_stats(const char *filename, size_t *entry_count, size_t *file_size) {
    if (!filename || !entry_count || !file_size) {
        return KV_INVALID_INPUT;
    }

    kv_entry_t *entries = NULL;
    size_t count;
    kv_error_t error = read_all_entries(filename, &entries, &count);
    if (error != KV_OK) {
        return error;
    }

    *entry_count = count;

    // 获取文件大小
    FILE *fp = fopen(filename, "r");
    if (fp) {
        fseek(fp, 0, SEEK_END);
        *file_size = ftell(fp);
        fclose(fp);
    } else {
        *file_size = 0;
    }

    kv_free_entries(entries, count);
    return KV_OK;
}

kv_error_t kv_init_with_config(const char *filename, const kv_config_t *config) {
    if (!filename || !config) {
        return KV_INVALID_INPUT;
    }

    // 简单实现：创建空文件
    FILE *fp = fopen(filename, "a");
    if (!fp) {
        return KV_FILE_ERROR;
    }

    fclose(fp);
    return KV_OK;
}

void kv_get_default_config(kv_config_t *config) {
    if (config) {
        config->enable_compression = false;
        config->enable_encryption = false;
        config->enable_backup = false;
        config->max_key_length = 256;
        config->max_value_length = 4096;
        config->max_entries = 1000;
    }
}

void kv_free_entries(kv_entry_t *entries, size_t count) {
    if (entries) {
        for (size_t i = 0; i < count; i++) {
            if (entries[i].key) free(entries[i].key);
            if (entries[i].value) free(entries[i].value);
        }
        free(entries);
    }
}
