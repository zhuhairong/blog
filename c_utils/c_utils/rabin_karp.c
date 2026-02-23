#include "rabin_karp.h"
#include <string.h>

#define RK_D 256
#define RK_Q 101

int rabin_karp_search(const char *txt, const char *pat) {
    int M = (int)strlen(pat);
    int N = (int)strlen(txt);
    if (M > N) return -1;
    int i, j;
    int p = 0; // hash value for pattern
    int t = 0; // hash value for txt
    int h = 1;

    for (i = 0; i < M - 1; i++) h = (h * RK_D) % RK_Q;

    for (i = 0; i < M; i++) {
        p = (RK_D * p + pat[i]) % RK_Q;
        t = (RK_D * t + txt[i]) % RK_Q;
    }

    for (i = 0; i <= N - M; i++) {
        if (p == t) {
            for (j = 0; j < M; j++) { if (txt[i + j] != pat[j]) break; }
            if (j == M) return i;
        }
        if (i < N - M) {
            t = (RK_D * (t - txt[i] * h) + txt[i + M]) % RK_Q;
            if (t < 0) t = (t + RK_Q);
        }
    }
    return -1;
}
