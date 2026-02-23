#include "color.h"
#include <math.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

static float hue_to_rgb(float p, float q, float t) {
    if (t < 0.0f) t += 1.0f;
    if (t > 1.0f) t -= 1.0f;
    if (t < 1.0f/6.0f) return p + (q - p) * 6.0f * t;
    if (t < 1.0f/2.0f) return q;
    if (t < 2.0f/3.0f) return p + (q - p) * (2.0f/3.0f - t) * 6.0f;
    return p;
}

hsl_t rgb_to_hsl(rgb_t rgb) {
    float r = rgb.r / 255.0f, g = rgb.g / 255.0f, b = rgb.b / 255.0f;
    float max = MAX(r, MAX(g, b)), min = MIN(r, MIN(g, b));
    float h, s, l = (max + min) / 2.0f;
    if (max == min) h = s = 0;
    else {
        float d = max - min;
        s = l > 0.5f ? d / (2.0f - max - min) : d / (max + min);
        if (max == r) h = (g - b) / d + (g < b ? 6 : 0);
        else if (max == g) h = (b - r) / d + 2;
        else h = (r - g) / d + 4;
        h /= 6.0f;
    }
    return (hsl_t){h * 360.0f, s, l};
}

rgb_t hsl_to_rgb(hsl_t hsl) {
    float h = hsl.h / 360.0f;
    float s = hsl.s;
    float l = hsl.l;
    
    float r, g, b;
    
    if (s == 0.0f) {
        r = g = b = l;
    } else {
        float q = l < 0.5f ? l * (1.0f + s) : l + s - l * s;
        float p = 2.0f * l - q;
        
        r = hue_to_rgb(p, q, h + 1.0f/3.0f);
        g = hue_to_rgb(p, q, h);
        b = hue_to_rgb(p, q, h - 1.0f/3.0f);
    }
    
    return (rgb_t){(uint8_t)(r * 255), (uint8_t)(g * 255), (uint8_t)(b * 255)};
}
