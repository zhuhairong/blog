#include "pbkdf2.h"
#include "hmac.h"
#include <string.h>

void pbkdf2_sha1(const char *pass, const uint8_t *salt, size_t salt_len, int iterations, uint8_t *out, size_t out_len) {
    uint8_t u[20], t[20], s[128];
    int blocks = (int)((out_len + 19) / 20);
    size_t pass_len = strlen(pass);
    
    for (int i = 1; i <= blocks; i++) {
        memcpy(s, salt, salt_len);
        s[salt_len] = (uint8_t)(i >> 24); s[salt_len+1] = (uint8_t)(i >> 16);
        s[salt_len+2] = (uint8_t)(i >> 8); s[salt_len+3] = (uint8_t)i;
        
        hmac_sha1((uint8_t*)pass, pass_len, s, salt_len + 4, t, NULL);
        memcpy(u, t, 20);
        
        for (int j = 1; j < iterations; j++) {
            hmac_sha1((uint8_t*)pass, pass_len, u, 20, u, NULL);
            for (int k = 0; k < 20; k++) t[k] ^= u[k];
        }
        
        size_t cpy = (out_len < 20) ? out_len : 20;
        memcpy(out, t, cpy);
        out += cpy; out_len -= cpy;
    }
}
