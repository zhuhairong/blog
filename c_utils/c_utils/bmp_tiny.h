#ifndef C_UTILS_BMP_TINY_H
#define C_UTILS_BMP_TINY_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// BMP 错误码
typedef enum {
    BMP_TINY_OK = 0,
    BMP_TINY_ERROR_FILE_OPEN,
    BMP_TINY_ERROR_FILE_READ,
    BMP_TINY_ERROR_FILE_WRITE,
    BMP_TINY_ERROR_INVALID_FORMAT,
    BMP_TINY_ERROR_UNSUPPORTED_BITS,
    BMP_TINY_ERROR_MEMORY_ALLOC,
    BMP_TINY_ERROR_INVALID_PARAM
} bmp_tiny_error_t;

// BMP 文件信息
typedef struct {
    int width;
    int height;
    int bits_per_pixel;
    int stride;
    size_t data_size;
    uint8_t *data;
} bmp_tiny_info_t;

// 写入 24-bit RGB BMP 文件
// path: 文件路径
// w: 宽度
// h: 高度
// rgb_data: RGB 数据（按行存储，每个像素 3 字节）
// 返回: 成功返回 BMP_TINY_OK，失败返回错误码
bmp_tiny_error_t bmp_tiny_write_rgb(const char *path, int w, int h, const uint8_t *rgb_data);

// 写入 32-bit RGBA BMP 文件
// path: 文件路径
// w: 宽度
// h: 高度
// rgba_data: RGBA 数据（按行存储，每个像素 4 字节）
// 返回: 成功返回 BMP_TINY_OK，失败返回错误码
bmp_tiny_error_t bmp_tiny_write_rgba(const char *path, int w, int h, const uint8_t *rgba_data);

// 写入 8-bit 灰度 BMP 文件
// path: 文件路径
// w: 宽度
// h: 高度
// gray_data: 灰度数据（按行存储，每个像素 1 字节）
// 返回: 成功返回 BMP_TINY_OK，失败返回错误码
bmp_tiny_error_t bmp_tiny_write_gray(const char *path, int w, int h, const uint8_t *gray_data);

// 读取 BMP 文件
// path: 文件路径
// info: BMP 信息输出
// 返回: 成功返回 BMP_TINY_OK，失败返回错误码
bmp_tiny_error_t bmp_tiny_read(const char *path, bmp_tiny_info_t *info);

// 释放 BMP 信息
// info: BMP 信息
void bmp_tiny_free(bmp_tiny_info_t *info);

// 转换 BMP 数据为 RGB 格式
// info: BMP 信息
// rgb_data: 输出 RGB 数据缓冲区（需要足够大）
// 返回: 成功返回 true，失败返回 false
bool bmp_tiny_to_rgb(const bmp_tiny_info_t *info, uint8_t *rgb_data);

// 转换 BMP 数据为 RGBA 格式
// info: BMP 信息
// rgba_data: 输出 RGBA 数据缓冲区（需要足够大）
// 返回: 成功返回 true，失败返回 false
bool bmp_tiny_to_rgba(const bmp_tiny_info_t *info, uint8_t *rgba_data);

// 转换 BMP 数据为灰度格式
// info: BMP 信息
// gray_data: 输出灰度数据缓冲区（需要足够大）
// 返回: 成功返回 true，失败返回 false
bool bmp_tiny_to_gray(const bmp_tiny_info_t *info, uint8_t *gray_data);

// 获取错误信息
// error: 错误码
// 返回: 错误信息字符串
const char* bmp_tiny_strerror(bmp_tiny_error_t error);

#endif // C_UTILS_BMP_TINY_H
