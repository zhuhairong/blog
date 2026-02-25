/**
 * @file demo_bmp_tiny.c
 * @brief BMP 图像处理演示
 *
 * 本演示展示了 BMP 图像的创建和处理，包括：
 * - RGB 图像写入
 * - 图像生成和保存
 * - 渐变和图案生成
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/bmp_tiny.h"

// 演示 1: 创建 RGB 渐变图像
static void demo_rgb_gradient(void) {
    printf("\n=== 演示 1: 创建 RGB 渐变图像 ===\n");

    int w = 256, h = 256;
    uint8_t *rgb_data = malloc(w * h * 3);
    if (!rgb_data) {
        fprintf(stderr, "内存分配失败\n");
        return;
    }

    // 生成渐变色图像
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int idx = (y * w + x) * 3;
            rgb_data[idx] = x;      // R: 水平渐变
            rgb_data[idx + 1] = y;  // G: 垂直渐变
            rgb_data[idx + 2] = 128; // B: 固定值
        }
    }

    const char *filename = "/tmp/demo_gradient.bmp";
    int err = bmp_tiny_write(filename, w, h, rgb_data);

    if (err == 0) {
        printf("RGB 渐变图像已保存: %s\n", filename);
        printf("  尺寸: %dx%d\n", w, h);
        printf("  格式: 24-bit RGB\n");
        printf("  描述: 红色从左到右渐变，绿色从上到下渐变\n");
    } else {
        printf("保存失败，错误码: %d\n", err);
    }

    free(rgb_data);
}

// 演示 2: 创建测试图案
static void demo_test_pattern(void) {
    printf("\n=== 演示 2: 创建测试图案 ===\n");

    int w = 400, h = 300;
    uint8_t *rgb_data = malloc(w * h * 3);
    if (!rgb_data) return;

    // 生成彩色条测试图案
    int bar_width = w / 7;
    uint8_t colors[7][3] = {
        {255, 0, 0},     // 红
        {255, 127, 0},   // 橙
        {255, 255, 0},   // 黄
        {0, 255, 0},     // 绿
        {0, 0, 255},     // 蓝
        {75, 0, 130},    // 靛
        {148, 0, 211}    // 紫
    };

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int color_idx = x / bar_width;
            if (color_idx > 6) color_idx = 6;

            int idx = (y * w + x) * 3;
            rgb_data[idx] = colors[color_idx][0];
            rgb_data[idx + 1] = colors[color_idx][1];
            rgb_data[idx + 2] = colors[color_idx][2];
        }
    }

    const char *filename = "/tmp/demo_test_pattern.bmp";
    int err = bmp_tiny_write(filename, w, h, rgb_data);

    if (err == 0) {
        printf("测试图案已保存: %s\n", filename);
        printf("  尺寸: %dx%d\n", w, h);
        printf("  图案: 彩虹色条\n");
    }

    free(rgb_data);
}

// 演示 3: 创建棋盘图案
static void demo_checkerboard(void) {
    printf("\n=== 演示 3: 创建棋盘图案 ===\n");

    int w = 320, h = 320;
    int square_size = 40;
    uint8_t *rgb_data = malloc(w * h * 3);
    if (!rgb_data) return;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int idx = (y * w + x) * 3;
            int square_x = x / square_size;
            int square_y = y / square_size;

            if ((square_x + square_y) % 2 == 0) {
                // 白色方块
                rgb_data[idx] = 255;
                rgb_data[idx + 1] = 255;
                rgb_data[idx + 2] = 255;
            } else {
                // 黑色方块
                rgb_data[idx] = 0;
                rgb_data[idx + 1] = 0;
                rgb_data[idx + 2] = 0;
            }
        }
    }

    const char *filename = "/tmp/demo_checkerboard.bmp";
    int err = bmp_tiny_write(filename, w, h, rgb_data);

    if (err == 0) {
        printf("棋盘图案已保存: %s\n", filename);
        printf("  尺寸: %dx%d\n", w, h);
        printf("  方块大小: %dx%d\n", square_size, square_size);
    }

    free(rgb_data);
}

// 演示 4: 创建几何图形
static void demo_geometric_shapes(void) {
    printf("\n=== 演示 4: 创建几何图形 ===\n");

    int w = 400, h = 400;
    uint8_t *rgb_data = malloc(w * h * 3);
    if (!rgb_data) return;

    // 白色背景
    memset(rgb_data, 255, w * h * 3);

    // 绘制红色矩形
    for (int y = 50; y < 150; y++) {
        for (int x = 50; x < 200; x++) {
            int idx = (y * w + x) * 3;
            rgb_data[idx] = 255;     // R
            rgb_data[idx + 1] = 0;   // G
            rgb_data[idx + 2] = 0;   // B
        }
    }

    // 绘制绿色圆形
    int cx = 280, cy = 100, radius = 60;
    for (int y = cy - radius; y <= cy + radius; y++) {
        for (int x = cx - radius; x <= cx + radius; x++) {
            int dx = x - cx;
            int dy = y - cy;
            if (dx * dx + dy * dy <= radius * radius && x >= 0 && x < w && y >= 0 && y < h) {
                int idx = (y * w + x) * 3;
                rgb_data[idx] = 0;
                rgb_data[idx + 1] = 255;
                rgb_data[idx + 2] = 0;
            }
        }
    }

    // 绘制蓝色三角形
    for (int y = 200; y < 350; y++) {
        int left = 100 + (y - 200) / 3;
        int right = 300 - (y - 200) / 3;
        for (int x = left; x <= right && x < w; x++) {
            int idx = (y * w + x) * 3;
            rgb_data[idx] = 0;
            rgb_data[idx + 1] = 0;
            rgb_data[idx + 2] = 255;
        }
    }

    const char *filename = "/tmp/demo_shapes.bmp";
    int err = bmp_tiny_write(filename, w, h, rgb_data);

    if (err == 0) {
        printf("几何图形已保存: %s\n", filename);
        printf("  尺寸: %dx%d\n", w, h);
        printf("  图形: 红色矩形、绿色圆形、蓝色三角形\n");
    }

    free(rgb_data);
}

// 演示 5: 创建曼德勃罗特集预览
static void demo_mandelbrot(void) {
    printf("\n=== 演示 5: 创建曼德勃罗特集预览 ===\n");

    int w = 400, h = 300;
    uint8_t *rgb_data = malloc(w * h * 3);
    if (!rgb_data) return;

    double xmin = -2.5, xmax = 1.0;
    double ymin = -1.25, ymax = 1.25;
    int max_iter = 50;

    for (int py = 0; py < h; py++) {
        for (int px = 0; px < w; px++) {
            double x0 = xmin + (xmax - xmin) * px / w;
            double y0 = ymin + (ymax - ymin) * py / h;

            double x = 0, y = 0;
            int iter = 0;

            while (x * x + y * y <= 4 && iter < max_iter) {
                double xtemp = x * x - y * y + x0;
                y = 2 * x * y + y0;
                x = xtemp;
                iter++;
            }

            int idx = (py * w + px) * 3;
            if (iter == max_iter) {
                // 集合内部 - 黑色
                rgb_data[idx] = 0;
                rgb_data[idx + 1] = 0;
                rgb_data[idx + 2] = 0;
            } else {
                // 集合外部 - 根据迭代次数着色
                rgb_data[idx] = (iter * 5) % 256;
                rgb_data[idx + 1] = (iter * 10) % 256;
                rgb_data[idx + 2] = (iter * 15) % 256;
            }
        }
    }

    const char *filename = "/tmp/demo_mandelbrot.bmp";
    int err = bmp_tiny_write(filename, w, h, rgb_data);

    if (err == 0) {
        printf("曼德勃罗特集已保存: %s\n", filename);
        printf("  尺寸: %dx%d\n", w, h);
        printf("  迭代次数: %d\n", max_iter);
    }

    free(rgb_data);
}

// 演示 6: 批量生成缩略图
static void demo_batch_thumbnails(void) {
    printf("\n=== 演示 6: 批量生成缩略图 ===\n");

    int sizes[][2] = {
        {32, 32},
        {64, 64},
        {128, 128},
        {256, 256}
    };
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    printf("生成 %d 种尺寸的缩略图:\n", num_sizes);

    for (int s = 0; s < num_sizes; s++) {
        int w = sizes[s][0];
        int h = sizes[s][1];

        uint8_t *rgb_data = malloc(w * h * 3);
        if (!rgb_data) continue;

        // 生成简单的缩略图内容
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                int idx = (y * w + x) * 3;
                rgb_data[idx] = (x * 255) / w;      // R 渐变
                rgb_data[idx + 1] = (y * 255) / h;  // G 渐变
                rgb_data[idx + 2] = 128;             // B 固定
            }
        }

        char filename[256];
        snprintf(filename, sizeof(filename), "/tmp/demo_thumb_%dx%d.bmp", w, h);

        int err = bmp_tiny_write(filename, w, h, rgb_data);
        if (err == 0) {
            printf("  %s: %dx%d\n", filename, w, h);
        }

        free(rgb_data);
    }
}

// 演示 7: BMP 文件信息
static void demo_bmp_info(void) {
    printf("\n=== 演示 7: BMP 文件信息 ===\n");

    printf("BMP 文件格式:\n");
    printf("  文件头 (14 bytes):\n");
    printf("    - 文件类型标识 'BM'\n");
    printf("    - 文件大小\n");
    printf("    - 保留字段\n");
    printf("    - 数据偏移量\n\n");

    printf("  信息头 (40 bytes - BITMAPINFOHEADER):\n");
    printf("    - 头大小\n");
    printf("    - 图像宽度\n");
    printf("    - 图像高度\n");
    printf("    - 颜色平面数\n");
    printf("    - 每像素位数 (24-bit RGB)\n");
    printf("    - 压缩方式\n");
    printf("    - 图像数据大小\n\n");

    printf("  像素数据:\n");
    printf("    - 每行按 4 字节对齐\n");
    printf("    - 从下到上存储\n");
    printf("    - BGR 格式 (蓝绿红)\n");
}

// 演示 8: 颜色模型
static void demo_color_models(void) {
    printf("\n=== 演示 8: 颜色模型 ===\n");

    printf("RGB 颜色模型:\n");
    printf("  - Red (红): 0-255\n");
    printf("  - Green (绿): 0-255\n");
    printf("  - Blue (蓝): 0-255\n\n");

    printf("常用颜色:\n");
    printf("  黑色:   (0, 0, 0)\n");
    printf("  白色:   (255, 255, 255)\n");
    printf("  红色:   (255, 0, 0)\n");
    printf("  绿色:   (0, 255, 0)\n");
    printf("  蓝色:   (0, 0, 255)\n");
    printf("  黄色:   (255, 255, 0)\n");
    printf("  青色:   (0, 255, 255)\n");
    printf("  品红:   (255, 0, 255)\n\n");

    printf("灰度:\n");
    printf("  R = G = B (0-255)\n");
    printf("  0 = 黑色, 255 = 白色\n");
}

// 演示 9: 图像处理基础
static void demo_image_processing(void) {
    printf("\n=== 演示 9: 图像处理基础 ===\n");

    printf("基本图像操作:\n\n");

    printf("1. 缩放:\n");
    printf("   - 最近邻插值\n");
    printf("   - 双线性插值\n");
    printf("   - 双三次插值\n\n");

    printf("2. 旋转:\n");
    printf("   - 围绕中心点旋转\n");
    printf("   - 需要重新采样\n\n");

    printf("3. 颜色调整:\n");
    printf("   - 亮度调整\n");
    printf("   - 对比度调整\n");
    printf("   - 饱和度调整\n\n");

    printf("4. 滤波:\n");
    printf("   - 模糊 (均值、高斯)\n");
    printf("   - 锐化\n");
    printf("   - 边缘检测\n");
}

// 演示 10: 应用场景
static void demo_use_cases(void) {
    printf("\n=== 演示 10: 应用场景 ===\n");

    printf("1. 图像生成:\n");
    printf("   - 验证码生成\n");
    printf("   - 图表和图形渲染\n");
    printf("   - 测试图像生成\n\n");

    printf("2. 数据可视化:\n");
    printf("   - 热力图\n");
    printf("   - 散点图\n");
    printf("   - 直方图\n\n");

    printf("3. 嵌入式系统:\n");
    printf("   - 简单图像保存\n");
    printf("   - 屏幕截图\n");
    printf("   - 传感器数据可视化\n\n");

    printf("4. 教育和研究:\n");
    printf("   - 分形图形\n");
    printf("   - 数学可视化\n");
    printf("   - 算法演示\n");
}

int main(void) {
    printf("========================================\n");
    printf("    BMP 图像处理演示程序\n");
    printf("========================================\n");

    demo_rgb_gradient();
    demo_test_pattern();
    demo_checkerboard();
    demo_geometric_shapes();
    demo_mandelbrot();
    demo_batch_thumbnails();
    demo_bmp_info();
    demo_color_models();
    demo_image_processing();
    demo_use_cases();

    printf("\n========================================\n");
    printf("    演示程序结束\n");
    printf("    生成的图像保存在 /tmp/ 目录\n");
    printf("========================================\n");

    return 0;
}
