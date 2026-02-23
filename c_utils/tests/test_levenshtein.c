#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/levenshtein.h"

void test_levenshtein_distance_same() {
    TEST(Levenshtein_DistanceSame);
    size_t dist = levenshtein_distance("hello", "hello");
    EXPECT_EQ(dist, (size_t)0);
}

void test_levenshtein_distance_different() {
    TEST(Levenshtein_DistanceDifferent);
    size_t dist = levenshtein_distance("kitten", "sitting");
    EXPECT_TRUE(dist > 0);
}

void test_levenshtein_distance_empty() {
    TEST(Levenshtein_DistanceEmpty);
    size_t dist = levenshtein_distance("", "hello");
    EXPECT_EQ(dist, (size_t)5);
    
    dist = levenshtein_distance("hello", "");
    EXPECT_EQ(dist, (size_t)5);
}

void test_levenshtein_similarity() {
    TEST(Levenshtein_Similarity);
    double sim = levenshtein_similarity("hello", "hello");
    EXPECT_TRUE(sim > 0.99);
    
    sim = levenshtein_similarity("", "");
    EXPECT_TRUE(sim > 0.99);
}

void test_levenshtein_get_default_config() {
    TEST(Levenshtein_GetDefaultConfig);
    levenshtein_config_t config;
    levenshtein_get_default_config(&config);
    
    EXPECT_TRUE(config.case_sensitive || !config.case_sensitive);
}

int main() {
    test_levenshtein_distance_same();
    test_levenshtein_distance_different();
    test_levenshtein_distance_empty();
    test_levenshtein_similarity();
    test_levenshtein_get_default_config();

    return 0;
}
