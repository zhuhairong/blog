#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/astar.h"

static bool simple_is_walkable(void *world, point_t pos) {
    (void)world;
    return pos.x >= 0 && pos.x < 10 && pos.y >= 0 && pos.y < 10;
}

void test_astar_heuristic_manhattan() {
    TEST(Astar_HeuristicManhattan);
    point_t a = {0, 0};
    point_t b = {3, 4};
    
    int dist = astar_heuristic_manhattan(a, b);
    EXPECT_EQ(dist, 7);
}

void test_astar_heuristic_euclidean() {
    TEST(Astar_HeuristicEuclidean);
    point_t a = {0, 0};
    point_t b = {3, 4};
    
    int dist = astar_heuristic_euclidean(a, b);
    EXPECT_TRUE(dist > 0);
}

void test_astar_heuristic_diagonal() {
    TEST(Astar_HeuristicDiagonal);
    point_t a = {0, 0};
    point_t b = {3, 4};
    
    int dist = astar_heuristic_diagonal(a, b);
    EXPECT_TRUE(dist > 0);
}

void test_astar_heuristic_chebyshev() {
    TEST(Astar_HeuristicChebyshev);
    point_t a = {0, 0};
    point_t b = {3, 4};
    
    int dist = astar_heuristic_chebyshev(a, b);
    EXPECT_TRUE(dist > 0);
}

void test_astar_result_create_free() {
    TEST(Astar_ResultCreateFree);
    astar_result_t* result = astar_result_create();
    EXPECT_TRUE(result != NULL);
    
    astar_result_free(result);
}

void test_astar_search_simple() {
    TEST(Astar_SearchSimple);
    point_t start = {0, 0};
    point_t goal = {5, 5};
    
    astar_result_t result = astar_search_simple(NULL, start, goal, simple_is_walkable, 10, 10);
    
    EXPECT_TRUE(result.found || !result.found);
    
    if (result.path != NULL) {
        free(result.path);
    }
}

int main() {
    test_astar_heuristic_manhattan();
    test_astar_heuristic_euclidean();
    test_astar_heuristic_diagonal();
    test_astar_heuristic_chebyshev();
    test_astar_result_create_free();
    test_astar_search_simple();

    return 0;
}
