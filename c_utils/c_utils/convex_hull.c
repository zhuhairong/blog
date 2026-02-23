#include "convex_hull.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>

static int compare(const void *a, const void *b) {
    const point_2d_t *p1 = (const point_2d_t*)a;
    const point_2d_t *p2 = (const point_2d_t*)b;
    if (p1->x != p2->x) return (p1->x < p2->x) ? -1 : 1;
    return (p1->y < p2->y) ? -1 : 1;
}

static double cross_product(point_2d_t a, point_2d_t b, point_2d_t c) {
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

// 创建凸包
convex_hull_t* convex_hull_create(void) {
    convex_hull_t *hull = calloc(1, sizeof(convex_hull_t));
    return hull;
}

// 计算凸包
convex_hull_t* convex_hull_compute(const point_2d_t *pts, size_t n, convex_hull_algorithm_t algorithm, convex_hull_error_t *error) {
    if (!pts || n < 3) {
        if (error) *error = CONVEX_HULL_ERROR_INSUFFICIENT_POINTS;
        return NULL;
    }

    convex_hull_t *hull = convex_hull_create();
    if (!hull) {
        if (error) *error = CONVEX_HULL_ERROR_MEMORY_ALLOC;
        return NULL;
    }

    // 复制点集以便排序
    point_2d_t *sorted_pts = malloc(n * sizeof(point_2d_t));
    if (!sorted_pts) {
        convex_hull_free(hull);
        if (error) *error = CONVEX_HULL_ERROR_MEMORY_ALLOC;
        return NULL;
    }
    memcpy(sorted_pts, pts, n * sizeof(point_2d_t));

    qsort(sorted_pts, n, sizeof(point_2d_t), compare);

    // 使用单调链算法计算凸包
    hull->points = malloc(n * sizeof(point_2d_t));
    if (!hull->points) {
        free(sorted_pts);
        convex_hull_free(hull);
        if (error) *error = CONVEX_HULL_ERROR_MEMORY_ALLOC;
        return NULL;
    }

    size_t k = 0;
    // 构建下半部分
    for (size_t i = 0; i < n; i++) {
        while (k >= 2 && cross_product(hull->points[k-2], hull->points[k-1], sorted_pts[i]) <= 0) k--;
        hull->points[k++] = sorted_pts[i];
    }

    // 构建上半部分
    for (size_t i = n - 1, t = k + 1; i > 0; i--) {
        while (k >= t && cross_product(hull->points[k-2], hull->points[k-1], sorted_pts[i-1]) <= 0) k--;
        hull->points[k++] = sorted_pts[i-1];
    }

    free(sorted_pts);

    hull->count = k - 1;
    hull->algorithm = algorithm;

    if (error) *error = CONVEX_HULL_OK;
    return hull;
}

// 释放凸包
void convex_hull_free(convex_hull_t *hull) {
    if (!hull) return;
    if (hull->points) {
        free(hull->points);
    }
    free(hull);
}

// 计算凸包面积
double convex_hull_area(const convex_hull_t *hull) {
    if (!hull || hull->count < 3) return 0.0;

    double area = 0.0;
    for (size_t i = 0; i < hull->count; i++) {
        size_t j = (i + 1) % hull->count;
        area += hull->points[i].x * hull->points[j].y;
        area -= hull->points[j].x * hull->points[i].y;
    }
    return fabs(area) / 2.0;
}

// 计算凸包周长
double convex_hull_perimeter(const convex_hull_t *hull) {
    if (!hull || hull->count < 2) return 0.0;

    double perimeter = 0.0;
    for (size_t i = 0; i < hull->count; i++) {
        size_t j = (i + 1) % hull->count;
        double dx = hull->points[j].x - hull->points[i].x;
        double dy = hull->points[j].y - hull->points[i].y;
        perimeter += sqrt(dx * dx + dy * dy);
    }
    return perimeter;
}

// 检查点是否在凸包内
static bool point_in_polygon(const point_2d_t *polygon, size_t n, const point_2d_t *point) {
    bool inside = false;
    for (size_t i = 0, j = n - 1; i < n; j = i++) {
        if (((polygon[i].y > point->y) != (polygon[j].y > point->y)) &&
            (point->x < (polygon[j].x - polygon[i].x) * (point->y - polygon[i].y) / 
             (polygon[j].y - polygon[i].y) + polygon[i].x)) {
            inside = !inside;
        }
    }
    return inside;
}

bool convex_hull_contains_point(const convex_hull_t *hull, const point_2d_t *point) {
    if (!hull || !point || hull->count < 3) return false;
    return point_in_polygon(hull->points, hull->count, point);
}

// 检查凸包是否有效
bool convex_hull_is_valid(const convex_hull_t *hull) {
    if (!hull || hull->count < 3) return false;
    return hull->points != NULL;
}

// 获取凸包点数
size_t convex_hull_get_point_count(const convex_hull_t *hull) {
    if (!hull) return 0;
    return hull->count;
}

// 获取凸包点
const point_2d_t* convex_hull_get_points(const convex_hull_t *hull) {
    if (!hull) return NULL;
    return hull->points;
}

// 获取错误信息
const char* convex_hull_error_string(convex_hull_error_t error) {
    switch (error) {
        case CONVEX_HULL_OK: return "Success";
        case CONVEX_HULL_ERROR_INVALID_POINTS: return "Invalid points";
        case CONVEX_HULL_ERROR_INSUFFICIENT_POINTS: return "Insufficient points";
        case CONVEX_HULL_ERROR_INVALID_HULL: return "Invalid hull";
        case CONVEX_HULL_ERROR_MEMORY_ALLOC: return "Memory allocation failed";
        default: return "Unknown error";
    }
}
