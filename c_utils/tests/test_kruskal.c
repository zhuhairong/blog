#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/kruskal.h"

void test_kruskal_types() {
    TEST(Kruskal_Types);
    kruskal_config_t config;
    memset(&config, 0, sizeof(config));
    /* kruskal_config_t type compiles and is instantiable */

    kruskal_edge_t edge;
    memset(&edge, 0, sizeof(edge));
    /* kruskal_edge_t type compiles and is instantiable */
}

void test_kruskal_error_values() {
    TEST(Kruskal_ErrorValues);
    EXPECT_TRUE(KRUSKAL_OK == 0);
    EXPECT_TRUE(KRUSKAL_INVALID_INPUT != 0);
    EXPECT_TRUE(KRUSKAL_MEMORY_ERROR != 0);
}

void test_kruskal_default_config() {
    TEST(Kruskal_DefaultConfig);
    kruskal_config_t config;
    kruskal_get_default_config(&config);
    EXPECT_TRUE(config.max_edges > 0);
}

void test_kruskal_edge_fields() {
    TEST(Kruskal_EdgeFields);
    kruskal_edge_t edge;
    edge.u = 0;
    edge.v = 1;
    edge.w = 10;
    
    EXPECT_EQ(edge.u, 0);
    EXPECT_EQ(edge.v, 1);
    EXPECT_EQ(edge.w, 10);
}

void test_kruskal_result() {
    TEST(Kruskal_Result);
    kruskal_result_t result;
    memset(&result, 0, sizeof(result));
    /* kruskal_result_t type compiles and is instantiable */
}

int main() {
    UTEST_BEGIN();
    test_kruskal_types();
    test_kruskal_error_values();
    test_kruskal_default_config();
    test_kruskal_edge_fields();
    test_kruskal_result();

    UTEST_END();
}
