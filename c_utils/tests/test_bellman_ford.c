#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/bellman_ford.h"

void test_bellman_ford_basic() {
    TEST(BellmanFord_Basic);
    int n = 4;
    int m = 5;
    bf_edge_t edges[] = {
        {0, 1, 1},
        {0, 2, 4},
        {1, 2, 2},
        {1, 3, 6},
        {2, 3, 3}
    };
    int dist[4];
    
    bool has_negative_cycle = bellman_ford(n, m, edges, 0, dist);
    EXPECT_FALSE(has_negative_cycle);
    EXPECT_EQ(dist[0], 0);
    EXPECT_EQ(dist[1], 1);
    EXPECT_EQ(dist[2], 3);
    EXPECT_EQ(dist[3], 6);
}

void test_bellman_ford_negative_cycle() {
    TEST(BellmanFord_NegativeCycle);
    int n = 3;
    int m = 3;
    bf_edge_t edges[] = {
        {0, 1, 1},
        {1, 2, -3},
        {2, 0, 1}
    };
    int dist[3];
    
    bool has_negative_cycle = bellman_ford(n, m, edges, 0, dist);
    EXPECT_TRUE(has_negative_cycle);
}

void test_bellman_ford_validate_input() {
    TEST(BellmanFord_ValidateInput);
    int n = 4;
    int m = 5;
    bf_edge_t edges[] = {
        {0, 1, 1},
        {0, 2, 4},
        {1, 2, 2},
        {1, 3, 6},
        {2, 3, 3}
    };
    int dist[4];
    
    bool valid = bellman_ford_validate_input(n, m, edges, 0, dist);
    EXPECT_TRUE(valid);
}

void test_bellman_ford_with_path() {
    TEST(BellmanFord_WithPath);
    int n = 4;
    int m = 5;
    bf_edge_t edges[] = {
        {0, 1, 1},
        {0, 2, 4},
        {1, 2, 2},
        {1, 3, 6},
        {2, 3, 3}
    };
    int dist[4];
    int pred[4];
    
    bool has_negative_cycle = bellman_ford_with_path(n, m, edges, 0, dist, pred);
    EXPECT_FALSE(has_negative_cycle);
}

void test_bellman_ford_reconstruct_path() {
    TEST(BellmanFord_ReconstructPath);
    int pred[] = {-1, 0, 1, 2};
    int path[10];
    int path_len = 0;
    
    bool success = bellman_ford_reconstruct_path(0, 3, pred, path, &path_len, 10);
    EXPECT_TRUE(success);
    EXPECT_TRUE(path_len > 0);
}

int main() {
    test_bellman_ford_basic();
    test_bellman_ford_negative_cycle();
    test_bellman_ford_validate_input();
    test_bellman_ford_with_path();
    test_bellman_ford_reconstruct_path();

    return 0;
}
