#include "hashmap.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct {
    char *key;
    void *value;
    bool in_use;
    bool deleted;   /* tombstone marker: entry was removed, probe chain continues past it */
} hash_entry;

struct hashmap_s {
    hash_entry *entries;
    size_t capacity;
    size_t size;
};

/* Simple FNV-1a hash */
static uint32_t hash_key(const char *key) {
    uint32_t hash = 2166136261u;
    while (*key) {
        hash ^= (uint8_t)*key++;
        hash *= 16777619u;
    }
    return hash;
}

hashmap_t* hashmap_create(void) {
    hashmap_t *m = malloc(sizeof(hashmap_t));
    if (!m) return NULL;
    m->capacity = 16;
    m->size = 0;
    m->entries = calloc(m->capacity, sizeof(hash_entry));
    if (!m->entries) {
        free(m);
        return NULL;
    }
    return m;
}

void hashmap_free(hashmap_t *m) {
    if (!m) return;
    for (size_t i = 0; i < m->capacity; i++) {
        /* Only free keys of live entries; tombstone keys were freed in remove */
        if (m->entries[i].in_use && !m->entries[i].deleted) {
            free(m->entries[i].key);
        }
    }
    free(m->entries);
    free(m);
}

static bool hashmap_rehash(hashmap_t *m) {
    size_t old_cap = m->capacity;
    hash_entry *old_entries = m->entries;

    m->capacity *= 2;
    m->entries = calloc(m->capacity, sizeof(hash_entry));
    if (!m->entries) {
        m->capacity = old_cap;
        m->entries = old_entries;
        return false;
    }
    m->size = 0;

    for (size_t i = 0; i < old_cap; i++) {
        /* Only re-insert live entries (skip never-used and tombstones) */
        if (old_entries[i].in_use && !old_entries[i].deleted) {
            hashmap_set(m, old_entries[i].key, old_entries[i].value);
            free(old_entries[i].key);
        }
    }
    free(old_entries);
    return true;
}

bool hashmap_set(hashmap_t *m, const char *key, void *value) {
    if (!m || !key) return false;
    if (m->size >= m->capacity * 0.75) {
        if (!hashmap_rehash(m)) return false;
    }

    uint32_t h = hash_key(key);
    size_t idx = h % m->capacity;
    size_t first_tombstone = m->capacity; /* sentinel: no tombstone found yet */

    while (m->entries[idx].in_use || m->entries[idx].deleted) {
        if (m->entries[idx].in_use && !m->entries[idx].deleted) {
            /* Live entry: check for key match */
            if (strcmp(m->entries[idx].key, key) == 0) {
                m->entries[idx].value = value;
                return true;
            }
        } else if (m->entries[idx].deleted) {
            /* Tombstone: remember first one for reuse */
            if (first_tombstone == m->capacity) {
                first_tombstone = idx;
            }
        }
        idx = (idx + 1) % m->capacity;
    }

    /* Prefer to reuse a tombstone slot */
    if (first_tombstone != m->capacity) {
        idx = first_tombstone;
    }

    char *new_key = strdup(key);
    if (!new_key) return false;
    m->entries[idx].key = new_key;
    m->entries[idx].value = value;
    m->entries[idx].in_use = true;
    m->entries[idx].deleted = false;
    m->size++;
    return true;
}

void* hashmap_get(const hashmap_t *m, const char *key) {
    if (!m || !key) return NULL;
    uint32_t h = hash_key(key);
    size_t idx = h % m->capacity;

    while (m->entries[idx].in_use || m->entries[idx].deleted) {
        if (m->entries[idx].in_use && !m->entries[idx].deleted) {
            if (strcmp(m->entries[idx].key, key) == 0) {
                return m->entries[idx].value;
            }
        }
        /* Tombstone: keep probing */
        idx = (idx + 1) % m->capacity;
    }
    return NULL;
}

size_t hashmap_size(const hashmap_t *m) {
    return m ? m->size : 0;
}

bool hashmap_remove(hashmap_t *m, const char *key) {
    if (!m || !key) return false;
    uint32_t h = hash_key(key);
    size_t idx = h % m->capacity;

    while (m->entries[idx].in_use || m->entries[idx].deleted) {
        if (m->entries[idx].in_use && !m->entries[idx].deleted) {
            if (strcmp(m->entries[idx].key, key) == 0) {
                free(m->entries[idx].key);
                m->entries[idx].key = NULL;
                m->entries[idx].value = NULL;
                m->entries[idx].in_use = false;
                m->entries[idx].deleted = true;   /* mark as tombstone */
                m->size--;
                return true;
            }
        }
        idx = (idx + 1) % m->capacity;
    }
    return false;
}

void hashmap_clear(hashmap_t *m) {
    if (!m) return;
    for (size_t i = 0; i < m->capacity; i++) {
        if (m->entries[i].in_use && !m->entries[i].deleted) {
            free(m->entries[i].key);
        }
        m->entries[i].key = NULL;
        m->entries[i].value = NULL;
        m->entries[i].in_use = false;
        m->entries[i].deleted = false;
    }
    m->size = 0;
}

hashmap_iter_t hashmap_iter_begin(const hashmap_t *m) {
    hashmap_iter_t iter = {m, 0, NULL};
    if (!m) return iter;

    for (size_t i = 0; i < m->capacity; i++) {
        if (m->entries[i].in_use && !m->entries[i].deleted) {
            iter.bucket = i;
            iter.entry = &m->entries[i];
            return iter;
        }
    }
    iter.bucket = m->capacity;
    return iter;
}

bool hashmap_iter_valid(const hashmap_iter_t *iter) {
    return iter && iter->m && iter->bucket < iter->m->capacity;
}

void hashmap_iter_next(hashmap_iter_t *iter) {
    if (!iter || !iter->m) return;

    for (size_t i = iter->bucket + 1; i < iter->m->capacity; i++) {
        if (iter->m->entries[i].in_use && !iter->m->entries[i].deleted) {
            iter->bucket = i;
            iter->entry = &iter->m->entries[i];
            return;
        }
    }
    iter->bucket = iter->m->capacity;
}
