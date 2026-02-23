#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/line_intersection.h"

void test_line_intersection_basic() {
    TEST(LineIntersection_Basic);
    point_t p1 = {0, 0};
    point_t p2 = {2, 2};
    point_t p3 = {0, 2};
    point_t p4 = {2, 0};
    point_t result;
    
    bool intersects = line_intersection(p1, p2, p3, p4, &result);
    EXPECT_TRUE(intersects);
    EXPECT_TRUE(result.x > 0 && result.x < 2);
    EXPECT_TRUE(result.y > 0 && result.y < 2);
}

void test_line_intersection_parallel() {
    TEST(LineIntersection_Parallel);
    point_t p1 = {0, 0};
    point_t p2 = {1, 0};
    point_t p3 = {0, 1};
    point_t p4 = {1, 1};
    point_t result;
    
    bool intersects = line_intersection(p1, p2, p3, p4, &result);
    EXPECT_FALSE(intersects);
}

void test_line_segment_length() {
    TEST(LineSegment_Length);
    line_segment_t seg;
    seg.p1.x = 0;
    seg.p1.y = 0;
    seg.p2.x = 3;
    seg.p2.y = 4;
    
    double length = line_segment_length(seg);
    EXPECT_TRUE(length == 5.0);
}

void test_lines_are_parallel() {
    TEST(LinesAreParallel);
    point_t p1 = {0, 0};
    point_t p2 = {1, 1};
    point_t p3 = {0, 1};
    point_t p4 = {1, 2};
    
    bool parallel = lines_are_parallel(p1, p2, p3, p4, 0.0001);
    EXPECT_TRUE(parallel);
}

void test_line_intersection_types() {
    TEST(LineIntersection_Types);
    line_intersection_config_t config;
    line_intersection_get_default_config(&config);
    EXPECT_TRUE(sizeof(config) > 0);
    
    point_t p;
    EXPECT_TRUE(sizeof(p) > 0);
    
    line_segment_t seg;
    EXPECT_TRUE(sizeof(seg) > 0);
}

int main() {
    test_line_intersection_basic();
    test_line_intersection_parallel();
    test_line_segment_length();
    test_lines_are_parallel();
    test_line_intersection_types();

    return 0;
}
