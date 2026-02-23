#include "line_intersection.h"
#include <math.h>
#include <stdbool.h>

static double cross_product(point_t a, point_t b) {
    return a.x * b.y - a.y * b.x;
}

static point_t subtract_points(point_t a, point_t b) {
    point_t result = { a.x - b.x, a.y - b.y };
    return result;
}

static bool is_between(double value, double min, double max, double epsilon) {
    return value >= (min - epsilon) && value <= (max + epsilon);
}

static bool is_point_on_segment(point_t p, point_t p1, point_t p2, double epsilon) {
    if (!is_between(p.x, fmin(p1.x, p2.x), fmax(p1.x, p2.x), epsilon)) {
        return false;
    }
    if (!is_between(p.y, fmin(p1.y, p2.y), fmax(p1.y, p2.y), epsilon)) {
        return false;
    }

    // 检查点是否在直线上
    point_t v1 = subtract_points(p2, p1);
    point_t v2 = subtract_points(p, p1);
    double cross = fabs(cross_product(v1, v2));
    return cross < epsilon;
}

static double calculate_denominator(point_t p1, point_t p2, point_t p3, point_t p4) {
    return (p1.x - p2.x) * (p3.y - p4.y) - (p1.y - p2.y) * (p3.x - p4.x);
}

static line_intersection_error_t calculate_intersection_point(point_t p1, point_t p2, point_t p3, point_t p4, 
                                                            point_t *intersection, double epsilon) {
    double den = calculate_denominator(p1, p2, p3, p4);
    if (fabs(den) < epsilon) {
        return LINE_INTERSECTION_PARALLEL;
    }

    double num_x = ((p1.x * p2.y - p1.y * p2.x) * (p3.x - p4.x) - 
                    (p1.x - p2.x) * (p3.x * p4.y - p3.y * p4.x));
    double num_y = ((p1.x * p2.y - p1.y * p2.x) * (p3.y - p4.y) - 
                    (p1.y - p2.y) * (p3.x * p4.y - p3.y * p4.x));

    intersection->x = num_x / den;
    intersection->y = num_y / den;

    return LINE_INTERSECTION_OK;
}

bool line_intersection(point_t p1, point_t p2, point_t p3, point_t p4, point_t *res) {
    if (!res) {
        return false;
    }

    line_segment_t seg1 = { p1, p2 };
    line_segment_t seg2 = { p3, p4 };
    line_intersection_config_t config;
    line_intersection_get_default_config(&config);

    line_intersection_error_t error = line_intersection_ex(seg1, seg2, res, &config);
    return error == LINE_INTERSECTION_OK;
}

line_intersection_error_t line_intersection_ex(line_segment_t seg1, line_segment_t seg2, 
                                              point_t *intersection, const line_intersection_config_t *config) {
    if (!intersection || !config) {
        return LINE_INTERSECTION_INVALID_INPUT;
    }

    double epsilon = config->epsilon;
    point_t p1 = seg1.p1, p2 = seg1.p2, p3 = seg2.p1, p4 = seg2.p2;

    // 计算直线交点
    line_intersection_error_t error = calculate_intersection_point(p1, p2, p3, p4, intersection, epsilon);
    if (error != LINE_INTERSECTION_OK) {
        if (error == LINE_INTERSECTION_PARALLEL) {
            // 检查是否共线
            point_t v1 = subtract_points(p2, p1);
            point_t v2 = subtract_points(p3, p1);
            double cross = fabs(cross_product(v1, v2));
            if (cross < epsilon) {
                return LINE_INTERSECTION_COLLINEAR;
            }
        }
        return error;
    }

    // 检查交点是否在线段上
    bool on_seg1 = is_point_on_segment(*intersection, p1, p2, epsilon);
    bool on_seg2 = is_point_on_segment(*intersection, p3, p4, epsilon);

    if (!on_seg1 || !on_seg2) {
        return LINE_INTERSECTION_NO_INTERSECTION;
    }

    return LINE_INTERSECTION_OK;
}

line_intersection_error_t line_intersection_infinite(point_t p1, point_t p2, point_t p3, point_t p4, 
                                                    point_t *intersection) {
    if (!intersection) {
        return LINE_INTERSECTION_INVALID_INPUT;
    }

    line_intersection_config_t config;
    line_intersection_get_default_config(&config);
    config.return_infinite = true;

    line_segment_t seg1 = { p1, p2 };
    line_segment_t seg2 = { p3, p4 };

    return calculate_intersection_point(p1, p2, p3, p4, intersection, config.epsilon);
}

bool point_on_segment(point_t p, line_segment_t seg, double epsilon) {
    return is_point_on_segment(p, seg.p1, seg.p2, epsilon);
}

double point_to_line_distance(point_t p, point_t line_p1, point_t line_p2) {
    double A = line_p2.y - line_p1.y;
    double B = line_p1.x - line_p2.x;
    double C = line_p2.x * line_p1.y - line_p1.x * line_p2.y;

    double denominator = sqrt(A*A + B*B);
    if (denominator == 0) {
        // 线段退化为点
        point_t diff = subtract_points(p, line_p1);
        return sqrt(diff.x*diff.x + diff.y*diff.y);
    }

    return fabs(A * p.x + B * p.y + C) / denominator;
}

double line_segment_length(line_segment_t seg) {
    double dx = seg.p2.x - seg.p1.x;
    double dy = seg.p2.y - seg.p1.y;
    return sqrt(dx*dx + dy*dy);
}

bool lines_are_parallel(point_t p1, point_t p2, point_t p3, point_t p4, double epsilon) {
    double den = calculate_denominator(p1, p2, p3, p4);
    return fabs(den) < epsilon;
}

bool lines_are_collinear(point_t p1, point_t p2, point_t p3, point_t p4, double epsilon) {
    if (!lines_are_parallel(p1, p2, p3, p4, epsilon)) {
        return false;
    }

    point_t v1 = subtract_points(p2, p1);
    point_t v2 = subtract_points(p3, p1);
    double cross = fabs(cross_product(v1, v2));
    return cross < epsilon;
}

void line_intersection_get_default_config(line_intersection_config_t *config) {
    if (config) {
        config->include_endpoints = true;
        config->use_precision_check = true;
        config->epsilon = 1e-9;
        config->calculate_collinear = false;
        config->return_infinite = false;
    }
}
