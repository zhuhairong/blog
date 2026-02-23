#include "skiplist.h"
#include <stdlib.h>
#include <string.h>

static skiplist_node_t* create_node(int level, void *key, void *value) {
    skiplist_node_t *n = malloc(sizeof(skiplist_node_t) + level * sizeof(skiplist_node_t*));
    n->key = key;
    n->value = value;
    return n;
}

static int random_level() {
    int level = 1;
    while ((rand() & 0xFFFF) < (0.5 * 0xFFFF) && level < SKIPLIST_MAX_LEVEL) level++;
    return level;
}

skiplist_t* skiplist_create(int (*compar)(const void *, const void *)) {
    skiplist_t *sl = malloc(sizeof(skiplist_t));
    sl->level = 1;
    sl->size = 0;
    sl->compar = compar;
    sl->header = create_node(SKIPLIST_MAX_LEVEL, NULL, NULL);
    for (int i = 0; i < SKIPLIST_MAX_LEVEL; i++) sl->header->forward[i] = NULL;
    return sl;
}

void skiplist_insert(skiplist_t *sl, void *key, void *value) {
    skiplist_node_t *update[SKIPLIST_MAX_LEVEL];
    skiplist_node_t *p = sl->header;
    for (int i = sl->level - 1; i >= 0; i--) {
        while (p->forward[i] && sl->compar(p->forward[i]->key, key) < 0) p = p->forward[i];
        update[i] = p;
    }
    p = p->forward[0];
    if (p && sl->compar(p->key, key) == 0) {
        p->value = value;
        return;
    }
    int level = random_level();
    if (level > sl->level) {
        for (int i = sl->level; i < level; i++) update[i] = sl->header;
        sl->level = level;
    }
    p = create_node(level, key, value);
    for (int i = 0; i < level; i++) {
        p->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = p;
    }
    sl->size++;
}

void* skiplist_get(skiplist_t *sl, const void *key) {
    skiplist_node_t *p = sl->header;
    for (int i = sl->level - 1; i >= 0; i--) {
        while (p->forward[i] && sl->compar(p->forward[i]->key, key) < 0) p = p->forward[i];
    }
    p = p->forward[0];
    if (p && sl->compar(p->key, key) == 0) return p->value;
    return NULL;
}

void skiplist_delete(skiplist_t *sl, const void *key) {
    skiplist_node_t *update[SKIPLIST_MAX_LEVEL];
    skiplist_node_t *p = sl->header;
    for (int i = sl->level - 1; i >= 0; i--) {
        while (p->forward[i] && sl->compar(p->forward[i]->key, key) < 0) p = p->forward[i];
        update[i] = p;
    }
    p = p->forward[0];
    if (p && sl->compar(p->key, key) == 0) {
        for (int i = 0; i < sl->level; i++) {
            if (update[i]->forward[i] != p) break;
            update[i]->forward[i] = p->forward[i];
        }
        free(p);
        while (sl->level > 1 && sl->header->forward[sl->level - 1] == NULL) sl->level--;
        sl->size--;
    }
}

void skiplist_free(skiplist_t *sl) {
    if (!sl) return;
    skiplist_node_t *p = sl->header->forward[0];
    while (p) {
        skiplist_node_t *next = p->forward[0];
        free(p);
        p = next;
    }
    free(sl->header);
    free(sl);
}
