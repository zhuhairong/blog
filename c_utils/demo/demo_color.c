/**
 * @file demo_color.c
 * @brief 颜色空间转换演示
 *
 * 本演示展示了 RGB 与 HSL 颜色空间之间的转换
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../c_utils/color.h"

// 打印 RGB 颜色
static void print_rgb(const char* label, rgb_t rgb) {
    printf("%s: RGB(%d, %d, %d)\n", label, rgb.r, rgb.g, rgb.b);
}

// 打印 HSL 颜色
static void print_hsl(const char* label, hsl_t hsl) {
    printf("%s: HSL(%.1f, %.1f%%, %.1f%%)\n",
           label, hsl.h, hsl.s * 100, hsl.l * 100);
}

// 演示 1: RGB 与 HSL 转换
static void demo_rgb_hsl(void) {
    printf("\n=== 演示 1: RGB 与 HSL 转换 ===\n");

    rgb_t test_colors[] = {
        {255, 0, 0},      // 红
        {0, 255, 0},      // 绿
        {0, 0, 255},      // 蓝
        {255, 255, 0},    // 黄
        {255, 0, 255},    // 品红
        {0, 255, 255},    // 青
        {128, 128, 128},  // 灰
        {255, 255, 255},  // 白
        {0, 0, 0}         // 黑
    };
    int num_colors = sizeof(test_colors) / sizeof(test_colors[0]);

    printf("RGB -> HSL -> RGB 转换:\n\n");

    for (int i = 0; i < num_colors; i++) {
        rgb_t rgb = test_colors[i];
        hsl_t hsl = rgb_to_hsl(rgb);
        rgb_t rgb_back = hsl_to_rgb(hsl);

        print_rgb("  原始 RGB", rgb);
        print_hsl("  -> HSL  ", hsl);
        print_rgb("  -> RGB  ", rgb_back);

        // 验证
        int dr = abs((int)rgb.r - (int)rgb_back.r);
        int dg = abs((int)rgb.g - (int)rgb_back.g);
        int db = abs((int)rgb.b - (int)rgb_back.b);
        printf("  误差: %d\n\n", dr + dg + db);
    }
}

// 演示 2: 颜色插值
static void demo_color_interpolation(void) {
    printf("\n=== 演示 2: 颜色插值 ===\n");

    rgb_t color1 = {255, 0, 0};    // 红
    rgb_t color2 = {0, 0, 255};    // 蓝

    printf("从红色到蓝色的渐变:\n\n");

    for (int i = 0; i <= 10; i++) {
        float t = i / 10.0f;

        // RGB 空间插值
        rgb_t rgb_interp = {
            (uint8_t)(color1.r * (1 - t) + color2.r * t),
            (uint8_t)(color1.g * (1 - t) + color2.g * t),
            (uint8_t)(color1.b * (1 - t) + color2.b * t)
        };

        // HSL 空间插值
        hsl_t hsl1 = rgb_to_hsl(color1);
        hsl_t hsl2 = rgb_to_hsl(color2);
        hsl_t hsl_interp = {
            hsl1.h * (1 - t) + hsl2.h * t,
            hsl1.s * (1 - t) + hsl2.s * t,
            hsl1.l * (1 - t) + hsl2.l * t
        };
        rgb_t hsl_rgb = hsl_to_rgb(hsl_interp);

        printf("  %.0f%%: RGB插值(%3d,%3d,%3d) HSL插值(%3d,%3d,%3d)\n",
               t * 100,
               rgb_interp.r, rgb_interp.g, rgb_interp.b,
               hsl_rgb.r, hsl_rgb.g, hsl_rgb.b);
    }
}

// 演示 3: 色相环
static void demo_color_wheel(void) {
    printf("\n=== 演示 3: 色相环 ===\n");

    printf("色相环颜色 (HSL, S=100%%, L=50%%):\n\n");

    for (int h = 0; h < 360; h += 30) {
        hsl_t hsl = {h, 1.0f, 0.5f};
        rgb_t rgb = hsl_to_rgb(hsl);

        const char* color_name;
        switch (h) {
            case 0:   color_name = "红"; break;
            case 30:  color_name = "橙"; break;
            case 60:  color_name = "黄"; break;
            case 90:  color_name = "黄绿"; break;
            case 120: color_name = "绿"; break;
            case 150: color_name = "青绿"; break;
            case 180: color_name = "青"; break;
            case 210: color_name = "天蓝"; break;
            case 240: color_name = "蓝"; break;
            case 270: color_name = "紫"; break;
            case 300: color_name = "品红"; break;
            case 330: color_name = "玫红"; break;
            default:  color_name = ""; break;
        }

        printf("  H=%3d° (%s): RGB(%3d, %3d, %3d)\n",
               h, color_name, rgb.r, rgb.g, rgb.b);
    }
}

// 演示 4: 颜色调整
static void demo_color_adjustments(void) {
    printf("\n=== 演示 4: 颜色调整 ===\n");

    rgb_t base = {128, 64, 192};  // 基础颜色

    printf("基础颜色: ");
    print_rgb("", base);
    printf("\n");

    // 变亮
    hsl_t hsl = rgb_to_hsl(base);
    hsl.l = fminf(hsl.l + 0.3f, 1.0f);
    rgb_t lighter = hsl_to_rgb(hsl);
    print_rgb("  变亮 30%", lighter);

    // 变暗
    hsl = rgb_to_hsl(base);
    hsl.l = fmaxf(hsl.l - 0.3f, 0.0f);
    rgb_t darker = hsl_to_rgb(hsl);
    print_rgb("  变暗 30%", darker);

    // 增加饱和度
    hsl = rgb_to_hsl(base);
    hsl.s = fminf(hsl.s + 0.5f, 1.0f);
    rgb_t more_saturated = hsl_to_rgb(hsl);
    print_rgb("  增加饱和度", more_saturated);

    // 降低饱和度
    hsl = rgb_to_hsl(base);
    hsl.s = fmaxf(hsl.s - 0.5f, 0.0f);
    rgb_t less_saturated = hsl_to_rgb(hsl);
    print_rgb("  降低饱和度", less_saturated);

    // 互补色
    hsl = rgb_to_hsl(base);
    hsl.h = fmodf(hsl.h + 180.0f, 360.0f);
    rgb_t complement = hsl_to_rgb(hsl);
    print_rgb("  互补色", complement);
}

// 演示 5: 实际应用 - 主题生成
static void demo_theme_generation(void) {
    printf("\n=== 演示 5: 实际应用 - 主题生成 ===\n");

    // 基础色
    rgb_t primary = {52, 152, 219};  // 蓝色

    printf("基于主色生成配色方案:\n\n");
    print_rgb("主色", primary);
    printf("\n");

    // 生成配色方案
    hsl_t primary_hsl = rgb_to_hsl(primary);

    // 类似色
    printf("类似色方案:\n");
    for (int i = -2; i <= 2; i++) {
        hsl_t analog = primary_hsl;
        analog.h = fmodf(analog.h + i * 30 + 360, 360);
        rgb_t analog_rgb = hsl_to_rgb(analog);
        printf("  H %+d°: ", i * 30);
        print_rgb("", analog_rgb);
    }

    // 互补色
    printf("\n互补色方案:\n");
    hsl_t comp_hsl = primary_hsl;
    comp_hsl.h = fmodf(comp_hsl.h + 180, 360);
    rgb_t comp_rgb = hsl_to_rgb(comp_hsl);
    print_rgb("  互补色", comp_rgb);

    // 分裂互补
    printf("\n分裂互补方案:\n");
    for (int i = -1; i <= 1; i += 2) {
        hsl_t split = primary_hsl;
        split.h = fmodf(split.h + 180 + i * 30 + 360, 360);
        rgb_t split_rgb = hsl_to_rgb(split);
        printf("  %+d°: ", i * 30);
        print_rgb("", split_rgb);
    }
}

// 演示 6: 颜色模型介绍
static void demo_color_models(void) {
    printf("\n=== 演示 6: 颜色模型介绍 ===\n");

    printf("RGB 颜色模型:\n");
    printf("  - Red (红): 0-255\n");
    printf("  - Green (绿): 0-255\n");
    printf("  - Blue (蓝): 0-255\n");
    printf("  - 用于显示器、相机等发光设备\n\n");

    printf("HSL 颜色模型:\n");
    printf("  - Hue (色相): 0-360 度\n");
    printf("  - Saturation (饱和度): 0-100%%\n");
    printf("  - Lightness (亮度): 0-100%%\n");
    printf("  - 更符合人类直觉\n\n");

    printf("常用颜色:\n");
    printf("  黑色:   RGB(0, 0, 0)       HSL(0, 0%%, 0%%)\n");
    printf("  白色:   RGB(255, 255, 255) HSL(0, 0%%, 100%%)\n");
    printf("  红色:   RGB(255, 0, 0)     HSL(0, 100%%, 50%%)\n");
    printf("  绿色:   RGB(0, 255, 0)     HSL(120, 100%%, 50%%)\n");
    printf("  蓝色:   RGB(0, 0, 255)     HSL(240, 100%%, 50%%)\n");
    printf("  黄色:   RGB(255, 255, 0)   HSL(60, 100%%, 50%%)\n");
}

// 演示 7: 灰度转换
static void demo_grayscale(void) {
    printf("\n=== 演示 7: 灰度转换 ===\n");

    rgb_t test_colors[] = {
        {255, 0, 0},
        {0, 255, 0},
        {0, 0, 255},
        {255, 255, 0},
        {128, 128, 128},
        {64, 64, 64}
    };
    int num_colors = sizeof(test_colors) / sizeof(test_colors[0]);

    printf("颜色转灰度:\n\n");

    for (int i = 0; i < num_colors; i++) {
        rgb_t rgb = test_colors[i];
        uint8_t gray = (uint8_t)(0.299 * rgb.r + 0.587 * rgb.g + 0.114 * rgb.b);

        print_rgb("  原始", rgb);
        printf("  -> 灰度: %d\n\n", gray);
    }
}

// 演示 8: 颜色混合
static void demo_color_mixing(void) {
    printf("\n=== 演示 8: 颜色混合 ===\n");

    rgb_t red = {255, 0, 0};
    rgb_t green = {0, 255, 0};
    rgb_t blue = {0, 0, 255};

    printf("加色混合 (RGB):\n\n");

    rgb_t yellow = {(uint8_t)fmin(red.r + green.r, 255), 
                    (uint8_t)fmin(red.g + green.g, 255), 
                    (uint8_t)fmin(red.b + green.b, 255)};
    print_rgb("  红 + 绿 = 黄", yellow);

    rgb_t magenta = {(uint8_t)fmin(red.r + blue.r, 255), 
                     (uint8_t)fmin(red.g + blue.g, 255), 
                     (uint8_t)fmin(red.b + blue.b, 255)};
    print_rgb("  红 + 蓝 = 品红", magenta);

    rgb_t cyan = {(uint8_t)fmin(green.r + blue.r, 255), 
                  (uint8_t)fmin(green.g + blue.g, 255), 
                  (uint8_t)fmin(green.b + blue.b, 255)};
    print_rgb("  绿 + 蓝 = 青", cyan);
}

// 演示 9: 颜色距离
static void demo_color_distance(void) {
    printf("\n=== 演示 9: 颜色距离 ===\n");

    rgb_t base = {128, 128, 128};
    rgb_t colors[] = {
        {128, 128, 128},  // 相同
        {130, 130, 130},  // 接近
        {255, 0, 0},      // 远
        {0, 255, 0},      // 远
        {0, 0, 255}       // 远
    };
    int num_colors = sizeof(colors) / sizeof(colors[0]);

    printf("与灰色的距离:\n\n");
    print_rgb("基准", base);
    printf("\n");

    for (int i = 0; i < num_colors; i++) {
        double distance = sqrt(
            pow(colors[i].r - base.r, 2) +
            pow(colors[i].g - base.g, 2) +
            pow(colors[i].b - base.b, 2)
        );
        print_rgb("  对比", colors[i]);
        printf("  距离: %.2f\n\n", distance);
    }
}

// 演示 10: 应用场景
static void demo_use_cases(void) {
    printf("\n=== 演示 10: 应用场景 ===\n");

    printf("1. 图像处理:\n");
    printf("   - 亮度/对比度调整\n");
    printf("   - 饱和度调整\n");
    printf("   - 颜色滤镜\n\n");

    printf("2. 数据可视化:\n");
    printf("   - 热力图颜色映射\n");
    printf("   - 图表配色\n");
    printf("   - 渐变生成\n\n");

    printf("3. UI 设计:\n");
    printf("   - 主题生成\n");
    printf("   - 配色方案\n");
    printf("   - 高亮/禁用状态\n\n");

    printf("4. 游戏开发:\n");
    printf("   - 粒子效果\n");
    printf("   - 光照计算\n");
    printf("   - 材质着色\n");
}

int main(void) {
    printf("========================================\n");
    printf("    颜色空间转换演示程序\n");
    printf("========================================\n");

    demo_rgb_hsl();
    demo_color_interpolation();
    demo_color_wheel();
    demo_color_adjustments();
    demo_theme_generation();
    demo_color_models();
    demo_grayscale();
    demo_color_mixing();
    demo_color_distance();
    demo_use_cases();

    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("========================================\n");

    return 0;
}
