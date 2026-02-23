#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/prim.h"

void test_prim_types() {
    TEST(Prim_Types);
    prim_config_t config;
    memset(&config, 0, sizeof(config));
    EXPECT_TRUE(sizeof(config) > 0);
    
    prim_edge_t edge;
    memset(&edge, 0, sizeof(edge));
    EXPECT_TRUE(sizeof(edge) > 0);
    
    prim_result_t result;
    memset(&result, 0, sizeof(result));
    EXPECT_TRUE(sizeof(result) > 0);
}

void test_prim_error_values() {
    TEST(Prim_ErrorValues);
    EXPECT_TRUE(PRIM_OK == 0);
    EXPECT_TRUE(PRIM_ERROR_NULL_PTR != 0);
    EXPECT_TRUE(PRIM_ERROR_INVALID_GRAPH != 0);
}

void test_prim_config_fields() {
    TEST(Prim_ConfigFields);
    prim_config_t config;
    memset(&config, 0, sizeof(config));
    
    config.max_nodes = 100;
    config.infinity = 999999;
    config.directed = false;
    
    EXPECT_EQ(config.max_nodes, 100);
    EXPECT_EQ(config.infinity, 999999);
    EXPECT_FALSE(config.directed);
}

void test_prim_edge_fields() {
    TEST(Prim_EdgeFields);
    prim_edge_t edge;
    edge.src = 0;
    edge.dest = 1;
    edge.weight = 10;
    
    EXPECT_EQ(edge.src, 0);
    EXPECT_EQ(edge.dest, 1);
    EXPECT_EQ(edge.weight, 10);
}

void test_prim_result_fields() {
    TEST(Prim_ResultFields);
    prim_result_t result;
    memset(&result, 0, sizeof(result));
    
    result.total_weight = 100;
    result.connected = true;
    result.edge_count = 5;
    
    EXPECT_EQ(result.total_weight, 100);
    EXPECT_TRUE(result.connected);
    EXPECT_EQ(result.edge_count, 5);
}

int main() {
    test_prim_types();
    test_prim_error_values();
    test_prim_config_fields();
    test_prim_edge_fields();
    test_prim_result_fields();

    return 0;
}
