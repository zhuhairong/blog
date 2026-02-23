#ifndef C_UTILS_LINE_INTERSECTION_H
#define C_UTILS_LINE_INTERSECTION_H

#include <stdbool.h>
#include <stddef.h>

// 线交点错误码
typedef enum {
    LINE_INTERSECTION_OK = 0,
    LINE_INTERSECTION_INVALID_INPUT = -1,
    LINE_INTERSECTION_PARALLEL = -2,
    LINE_INTERSECTION_COLLINEAR = -3,
    LINE_INTERSECTION_NO_INTERSECTION = -4,
    LINE_INTERSECTION_DIVISION_BY_ZERO = -5,
    LINE_INTERSECTION_OUT_OF_BOUNDS = -6
} line_intersection_error_t;

// 线交点配置
typedef struct {
    bool include_endpoints;
    bool use_precision_check;
    double epsilon;
    bool calculate_collinear;
    bool return_infinite;
} line_intersection_config_t;

typedef struct { double x, y; } point_t;

typedef struct {
    point_t p1;
    point_t p2;
} line_segment_t;

// 计算两条线段 (p1,p2) 和 (p3,p4) 的交点
bool line_intersection(point_t p1, point_t p2, point_t p3, point_t p4, point_t *res);

// 带错误处理的线段交点计算
line_intersection_error_t line_intersection_ex(line_segment_t seg1, line_segment_t seg2, 
                                              point_t *intersection, const line_intersection_config_t *config);

// 计算两条直线的交点（无限延伸）
line_intersection_error_t line_intersection_infinite(point_t p1, point_t p2, point_t p3, point_t p4, 
                                                    point_t *intersection);

// 检查点是否在线段上
bool point_on_segment(point_t p, line_segment_t seg, double epsilon);

// 计算点到直线的距离
double point_to_line_distance(point_t p, point_t line_p1, point_t line_p2);

// 计算线段长度
double line_segment_length(line_segment_t seg);

// 检查两条线段是否平行
bool lines_are_parallel(point_t p1, point_t p2, point_t p3, point_t p4, double epsilon);

// 检查两条线段是否共线
bool lines_are_collinear(point_t p1, point_t p2, point_t p3, point_t p4, double epsilon);

// 获取默认配置
void line_intersection_get_default_config(line_intersection_config_t *config);

#endif // C_UTILS_LINE_INTERSECTION_H
