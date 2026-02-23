#include "bmp_tiny.h"
#include <stdio.h>

int bmp_tiny_write(const char *path, int w, int h, const uint8_t *rgb_data) {
    FILE *f = fopen(path, "wb");
    if (!f) return -1;
    uint32_t row_size = (w * 3 + 3) & ~3;
    uint32_t data_size = row_size * h;
    uint8_t header[54] = {
        'B', 'M', 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0, 40, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    *(uint32_t*)(header + 2) = 54 + data_size;
    *(int32_t*)(header + 18) = w;
    *(int32_t*)(header + 22) = h;
    fwrite(header, 1, 54, f);
    for (int i = h - 1; i >= 0; i--) {
        for (int j = 0; j < w; j++) {
            const uint8_t *p = &rgb_data[(i * w + j) * 3];
            uint8_t bgr[3] = {p[2], p[1], p[0]};
            fwrite(bgr, 1, 3, f);
        }
        uint8_t pad[3] = {0};
        fwrite(pad, 1, row_size - w * 3, f);
    }
    fclose(f);
    return 0;
}
