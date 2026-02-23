#ifndef C_UTILS_CONVEX_HULL_H
#define C_UTILS_CONVEX_HULL_H

#include <stddef.h>
#include <stdbool.h>

// 凸包错误码
typedef enum {
    CONVEX_HULL_OK = 0,
    CONVEX_HULL_ERROR_INVALID_POINTS,
    CONVEX_HULL_ERROR_INSUFFICIENT_POINTS,
    CONVEX_HULL_ERROR_INVALID_HULL,
    CONVEX_HULL_ERROR_MEMORY_ALLOC
} convex_hull_error_t;

// 凸包算法
typedef enum {
    CONVEX_HULL_ALGORITHM_MONOTONE_CHAIN,
    CONVEX_HULL_ALGORITHM_GRAHAM_SCAN,
    CONVEX_HULL_ALGORITHM_AUTO
} convex_hull_algorithm_t;

// 二维点
typedef struct {
    double x;
    double y;
} point_2d_t;

// 凸包结构
typedef struct {
    point_2d_t *points;
    size_t count;
    convex_hull_algorithm_t algorithm;
} convex_hull_t;

// 创建凸包
// 返回: 成功返回凸包指针，失败返回 NULL
convex_hull_t* convex_hull_create(void);

// 计算凸包
// pts: 点集
// n: 点集大小
// algorithm: 凸包算法
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回凸包指针，失败返回 NULL
convex_hull_t* convex_hull_compute(const point_2d_t *pts, size_t n, convex_hull_algorithm_t algorithm, convex_hull_error_t *error);

// 释放凸包
// hull: 凸包
void convex_hull_free(convex_hull_t *hull);

// 计算凸包面积
// hull: 凸包
// 返回: 凸包面积
// 注意: 如果凸包无效，返回 0.0
double convex_hull_area(const convex_hull_t *hull);

// 计算凸包周长
// hull: 凸包
// 返回: 凸包周长
// 注意: 如果凸包无效，返回 0.0
double convex_hull_perimeter(const convex_hull_t *hull);

// 检查点是否在凸包内
// hull: 凸包
// point: 点
// 返回: 在凸包内（包括边界）返回 true，否则返回 false
bool convex_hull_contains_point(const convex_hull_t *hull, const point_2d_t *point);

// 检查凸包是否有效
// hull: 凸包
// 返回: 有效返回 true，无效返回 false
bool convex_hull_is_valid(const convex_hull_t *hull);

// 获取凸包点数
// hull: 凸包
// 返回: 凸包点数
size_t convex_hull_get_point_count(const convex_hull_t *hull);

// 获取凸包点
// hull: 凸包
// index: 点索引
// point: 点（输出参数）
// 返回: 成功返回 true，失败返回 false
bool convex_hull_get_point(const convex_hull_t *hull, size_t index, point_2d_t *point);

// 复制凸包
// hull: 凸包
// 返回: 成功返回凸包副本，失败返回 NULL
convex_hull_t* convex_hull_clone(const convex_hull_t *hull);

// 获取错误信息
// error: 错误码
// 返回: 错误信息字符串
const char* convex_hull_strerror(convex_hull_error_t error);

#endif // C_UTILS_CONVEX_HULL_H
