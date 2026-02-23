#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/topological_sort.h"

void test_topological_sort_simple() {
    TEST(TopologicalSort_Simple);
    int adj[10][10] = {0};
    int result[10] = {0};
    
    adj[0][1] = 1;
    adj[0][2] = 1;
    adj[1][3] = 1;
    adj[2][3] = 1;
    
    bool success = topological_sort(4, adj, result);
    EXPECT_TRUE(success);
}

void test_topological_sort_single() {
    TEST(TopologicalSort_Single);
    int adj[10][10] = {0};
    int result[10] = {0};
    
    bool success = topological_sort(1, adj, result);
    EXPECT_TRUE(success);
    EXPECT_EQ(result[0], 0);
}

void test_topological_sort_linear() {
    TEST(TopologicalSort_Linear);
    int adj[10][10] = {0};
    int result[10] = {0};
    
    adj[0][1] = 1;
    adj[1][2] = 1;
    adj[2][3] = 1;
    
    bool success = topological_sort(4, adj, result);
    EXPECT_TRUE(success);
}

void test_topological_sort_cycle() {
    TEST(TopologicalSort_Cycle);
    int adj[10][10] = {0};
    int result[10] = {0};
    
    adj[0][1] = 1;
    adj[1][2] = 1;
    adj[2][0] = 1;
    
    bool success = topological_sort(3, adj, result);
    EXPECT_FALSE(success);
}

void test_topological_sort_disconnected() {
    TEST(TopologicalSort_Disconnected);
    int adj[10][10] = {0};
    int result[10] = {0};
    
    bool success = topological_sort(3, adj, result);
    EXPECT_TRUE(success);
}

int main() {
    test_topological_sort_simple();
    test_topological_sort_single();
    test_topological_sort_linear();
    test_topological_sort_cycle();
    test_topological_sort_disconnected();

    return 0;
}
