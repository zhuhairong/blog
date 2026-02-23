#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/convex_hull.h"

void test_convex_hull_create_free() {
    TEST(ConvexHull_CreateFree);
    convex_hull_t* hull = convex_hull_create();
    EXPECT_TRUE(hull != NULL);
    
    convex_hull_free(hull);
}

void test_convex_hull_insufficient_points() {
    TEST(ConvexHull_InsufficientPoints);
    point_2d_t pts[] = {{0.0, 0.0}, {1.0, 1.0}};
    
    convex_hull_error_t err;
    convex_hull_t* hull = convex_hull_compute(pts, 2, CONVEX_HULL_ALGORITHM_MONOTONE_CHAIN, &err);
    
    EXPECT_TRUE(hull == NULL);
    EXPECT_EQ(err, CONVEX_HULL_ERROR_INSUFFICIENT_POINTS);
}

void test_convex_hull_null_points() {
    TEST(ConvexHull_NullPoints);
    convex_hull_error_t err;
    convex_hull_t* hull = convex_hull_compute(NULL, 0, CONVEX_HULL_ALGORITHM_MONOTONE_CHAIN, &err);
    
    EXPECT_TRUE(hull == NULL);
}

void test_convex_hull_get_point_count_null() {
    TEST(ConvexHull_GetPointCountNull);
    size_t count = convex_hull_get_point_count(NULL);
    EXPECT_EQ(count, (size_t)0);
}

void test_convex_hull_get_points_null() {
    TEST(ConvexHull_GetPointsNull);
    const point_2d_t* pts = convex_hull_get_points(NULL);
    EXPECT_TRUE(pts == NULL);
}

int main() {
    test_convex_hull_create_free();
    test_convex_hull_insufficient_points();
    test_convex_hull_null_points();
    test_convex_hull_get_point_count_null();
    test_convex_hull_get_points_null();

    return 0;
}
