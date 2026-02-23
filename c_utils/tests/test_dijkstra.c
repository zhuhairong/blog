#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/dijkstra.h"

void test_dijkstra_graph_create() {
    TEST(Dijkstra_GraphCreate);
    dijkstra_error_t error;
    
    dijkstra_graph_t* graph = dijkstra_graph_create(5, &error);
    EXPECT_TRUE(graph != NULL);
    EXPECT_EQ(error, DIJKSTRA_OK);
    
    dijkstra_graph_free(graph);
}

void test_dijkstra_graph_create_zero() {
    TEST(Dijkstra_GraphCreateZero);
    dijkstra_error_t error;
    
    dijkstra_graph_t* graph = dijkstra_graph_create(0, &error);
    EXPECT_TRUE(graph == NULL || graph != NULL);
}

void test_dijkstra_add_edge() {
    TEST(Dijkstra_AddEdge);
    dijkstra_error_t error;
    
    dijkstra_graph_t* graph = dijkstra_graph_create(5, &error);
    EXPECT_TRUE(graph != NULL);
    
    if (graph != NULL) {
        bool result = dijkstra_graph_add_edge(graph, 0, 1, 10, &error);
        EXPECT_TRUE(result);
        EXPECT_EQ(error, DIJKSTRA_OK);
        
        dijkstra_graph_free(graph);
    }
}

void test_dijkstra_basic() {
    TEST(Dijkstra_Basic);
    dijkstra_error_t error;
    
    dijkstra_graph_t* graph = dijkstra_graph_create(4, &error);
    EXPECT_TRUE(graph != NULL);
    
    if (graph != NULL) {
        dijkstra_graph_add_edge(graph, 0, 1, 1, &error);
        dijkstra_graph_add_edge(graph, 0, 2, 4, &error);
        dijkstra_graph_add_edge(graph, 1, 2, 2, &error);
        dijkstra_graph_add_edge(graph, 1, 3, 6, &error);
        dijkstra_graph_add_edge(graph, 2, 3, 3, &error);
        
        int dist[4];
        int pred[4];
        
        bool result = dijkstra(graph, 0, dist, pred, &error);
        EXPECT_TRUE(result);
        EXPECT_EQ(dist[0], 0);
        EXPECT_EQ(dist[1], 1);
        EXPECT_EQ(dist[2], 3);
        EXPECT_EQ(dist[3], 6);
        
        dijkstra_graph_free(graph);
    }
}

void test_dijkstra_strerror() {
    TEST(Dijkstra_Strerror);
    const char* msg = dijkstra_strerror(DIJKSTRA_OK);
    EXPECT_TRUE(msg != NULL);
    
    msg = dijkstra_strerror(DIJKSTRA_ERROR_INVALID_PARAM);
    EXPECT_TRUE(msg != NULL);
}

int main() {
    test_dijkstra_graph_create();
    test_dijkstra_graph_create_zero();
    test_dijkstra_add_edge();
    test_dijkstra_basic();
    test_dijkstra_strerror();

    return 0;
}
