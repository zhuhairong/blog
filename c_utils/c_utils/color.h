#ifndef C_UTILS_COLOR_H
#define C_UTILS_COLOR_H

#include <stdint.h>
#include <stdbool.h>

// 颜色错误码
typedef enum {
    COLOR_OK = 0,
    COLOR_ERROR_INVALID_PARAM,
    COLOR_ERROR_OUT_OF_RANGE
} color_error_t;

// RGB 颜色 (0-255)
typedef struct { uint8_t r, g, b; } rgb_t;

// RGBA 颜色 (0-255)
typedef struct { uint8_t r, g, b, a; } rgba_t;

// HSL 颜色 (H:0-360, S:0-1, L:0-1)
typedef struct { float h, s, l; } hsl_t;

// HSV 颜色 (H:0-360, S:0-1, V:0-1)
typedef struct { float h, s, v; } hsv_t;

// CMYK 颜色 (0-1)
typedef struct { float c, m, y, k; } cmyk_t;

// RGB 到 HSL 转换
// rgb: RGB 颜色
// 返回: HSL 颜色
hsl_t rgb_to_hsl(rgb_t rgb);

// HSL 到 RGB 转换
// hsl: HSL 颜色
// 返回: RGB 颜色
rgb_t hsl_to_rgb(hsl_t hsl);

// RGB 到 HSV 转换
// rgb: RGB 颜色
// 返回: HSV 颜色
hsv_t rgb_to_hsv(rgb_t rgb);

// HSV 到 RGB 转换
// hsv: HSV 颜色
// 返回: RGB 颜色
rgb_t hsv_to_rgb(hsv_t hsv);

// RGB 到 CMYK 转换
// rgb: RGB 颜色
// 返回: CMYK 颜色
cmyk_t rgb_to_cmyk(rgb_t rgb);

// CMYK 到 RGB 转换
// cmyk: CMYK 颜色
// 返回: RGB 颜色
rgb_t cmyk_to_rgb(cmyk_t cmyk);

// RGBA 到 RGB 转换（带透明度混合）
// rgba: RGBA 颜色
// bg: 背景 RGB 颜色
// 返回: 混合后的 RGB 颜色
rgb_t rgba_to_rgb(rgba_t rgba, rgb_t bg);

// 颜色插值（线性）
// a: 起始颜色
// b: 结束颜色
// t: 插值参数 (0-1)
// 返回: 插值后的颜色
rgb_t rgb_lerp(rgb_t a, rgb_t b, float t);

// 颜色亮度调整
// rgb: 输入颜色
// factor: 亮度因子 (0-2，1 表示不变)
// 返回: 调整后的颜色
rgb_t rgb_brightness(rgb_t rgb, float factor);

// 颜色饱和度调整
// rgb: 输入颜色
// factor: 饱和度因子 (0-2，1 表示不变)
// 返回: 调整后的颜色
rgb_t rgb_saturation(rgb_t rgb, float factor);

// 颜色对比度调整
// rgb: 输入颜色
// factor: 对比度因子 (0-2，1 表示不变)
// 返回: 调整后的颜色
rgb_t rgb_contrast(rgb_t rgb, float factor);

// 获取颜色灰度值
// rgb: 输入颜色
// 返回: 灰度值 (0-255)
uint8_t rgb_to_gray(rgb_t rgb);

// 检查颜色是否有效
// rgb: RGB 颜色
// 返回: 有效返回 true，无效返回 false
bool rgb_is_valid(rgb_t rgb);

// 检查 HSL 颜色是否有效
// hsl: HSL 颜色
// 返回: 有效返回 true，无效返回 false
bool hsl_is_valid(hsl_t hsl);

// 检查 HSV 颜色是否有效
// hsv: HSV 颜色
// 返回: 有效返回 true，无效返回 false
bool hsv_is_valid(hsv_t hsv);

// 检查 CMYK 颜色是否有效
// cmyk: CMYK 颜色
// 返回: 有效返回 true，无效返回 false
bool cmyk_is_valid(cmyk_t cmyk);

// 获取错误信息
// error: 错误码
// 返回: 错误信息字符串
const char* color_strerror(color_error_t error);

// 预定义颜色
#define COLOR_RED     ((rgb_t){255, 0, 0})
#define COLOR_GREEN   ((rgb_t){0, 255, 0})
#define COLOR_BLUE    ((rgb_t){0, 0, 255})
#define COLOR_WHITE   ((rgb_t){255, 255, 255})
#define COLOR_BLACK   ((rgb_t){0, 0, 0})
#define COLOR_GRAY    ((rgb_t){128, 128, 128})
#define COLOR_YELLOW  ((rgb_t){255, 255, 0})
#define COLOR_CYAN    ((rgb_t){0, 255, 255})
#define COLOR_MAGENTA ((rgb_t){255, 0, 255})

#endif // C_UTILS_COLOR_H
