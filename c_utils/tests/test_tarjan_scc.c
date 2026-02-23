#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/tarjan_scc.h"

void test_tarjan_types() {
    TEST(Tarjan_Types);
    tarjan_config_t config;
    memset(&config, 0, sizeof(config));
    EXPECT_TRUE(sizeof(config) > 0);
    
    tarjan_state_t state;
    memset(&state, 0, sizeof(state));
    EXPECT_TRUE(sizeof(state) > 0);
}

void test_tarjan_error_values() {
    TEST(Tarjan_ErrorValues);
    EXPECT_TRUE(TARJAN_OK == 0);
    EXPECT_TRUE(TARJAN_ERROR_INVALID_PARAMS != 0);
}

void test_tarjan_graph_size() {
    TEST(Tarjan_GraphSize);
    tarjan_graph_t g;
    memset(&g, 0, sizeof(g));
    EXPECT_TRUE(sizeof(g) > 0);
}

void test_tarjan_config_fields() {
    TEST(Tarjan_ConfigFields);
    tarjan_config_t config;
    memset(&config, 0, sizeof(config));
    
    config.use_dynamic_memory = true;
    config.enable_statistics = true;
    config.max_nodes = 100;
    
    EXPECT_TRUE(config.use_dynamic_memory);
    EXPECT_TRUE(config.enable_statistics);
    EXPECT_TRUE(config.max_nodes == 100);
}

void test_tarjan_state_fields() {
    TEST(Tarjan_StateFields);
    tarjan_state_t state;
    memset(&state, 0, sizeof(state));
    
    state.scc_computations = 5;
    state.nodes_processed = 100;
    state.is_initialized = true;
    
    EXPECT_TRUE(state.scc_computations == 5);
    EXPECT_TRUE(state.nodes_processed == 100);
    EXPECT_TRUE(state.is_initialized);
}

int main() {
    test_tarjan_types();
    test_tarjan_error_values();
    test_tarjan_graph_size();
    test_tarjan_config_fields();
    test_tarjan_state_fields();

    return 0;
}
