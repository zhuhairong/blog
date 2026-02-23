#include "suffix_array.h"
#include <string.h>
#include <stdlib.h>

typedef struct { int index; int rank[2]; } suffix_t;

static int compare(const void *a, const void *b) {
    suffix_t *s1 = (suffix_t*)a;
    suffix_t *s2 = (suffix_t*)b;
    if (s1->rank[0] != s2->rank[0]) return s1->rank[0] - s2->rank[0];
    return s1->rank[1] - s2->rank[1];
}

void suffix_array_build(const char *s, int *sa) {
    int n = (int)strlen(s);
    suffix_t *suffixes = malloc(n * sizeof(suffix_t));
    for (int i = 0; i < n; i++) {
        suffixes[i].index = i;
        suffixes[i].rank[0] = s[i] - 'a';
        suffixes[i].rank[1] = ((i + 1) < n) ? (s[i + 1] - 'a') : -1;
    }
    qsort(suffixes, n, sizeof(suffix_t), compare);
    int *ind = malloc(n * sizeof(int));
    for (int k = 4; k < 2 * n; k = k * 2) {
        int rank = 0;
        int prev_rank = suffixes[0].rank[0];
        suffixes[0].rank[0] = rank;
        ind[suffixes[0].index] = 0;
        for (int i = 1; i < n; i++) {
            if (suffixes[i].rank[0] == prev_rank && suffixes[i].rank[1] == suffixes[i - 1].rank[1]) {
                prev_rank = suffixes[i].rank[0];
                suffixes[i].rank[0] = rank;
            } else {
                prev_rank = suffixes[i].rank[0];
                suffixes[i].rank[0] = ++rank;
            }
            ind[suffixes[i].index] = i;
        }
        for (int i = 0; i < n; i++) {
            int nextindex = suffixes[i].index + k / 2;
            suffixes[i].rank[1] = (nextindex < n) ? suffixes[ind[nextindex]].rank[0] : -1;
        }
        qsort(suffixes, n, sizeof(suffix_t), compare);
    }
    for (int i = 0; i < n; i++) sa[i] = suffixes[i].index;
    free(suffixes); free(ind);
}
