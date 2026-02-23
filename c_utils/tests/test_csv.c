#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/csv.h"

void test_csv_default_config() {
    TEST(Csv_DefaultConfig);
    csv_config_t config = csv_default_config();
    
    EXPECT_EQ(config.delimiter, ',');
    EXPECT_EQ(config.quote, '"');
}

void test_csv_create_free() {
    TEST(Csv_CreateFree);
    csv_error_t error;
    csv_t* csv = csv_create(NULL, &error);
    
    EXPECT_TRUE(csv != NULL);
    EXPECT_EQ(error, CSV_OK);
    
    csv_free(csv);
}

void test_csv_add_row() {
    TEST(Csv_AddRow);
    csv_error_t error;
    csv_t* csv = csv_create(NULL, &error);
    EXPECT_TRUE(csv != NULL);
    
    const char* fields[] = {"a", "b", "c"};
    bool result = csv_add_row(csv, fields, 3, &error);
    
    EXPECT_TRUE(result);
    EXPECT_EQ(csv_get_rows(csv), (size_t)1);
    EXPECT_EQ(csv_get_cols(csv), (size_t)3);
    
    csv_free(csv);
}

void test_csv_get() {
    TEST(Csv_Get);
    csv_error_t error;
    csv_t* csv = csv_create(NULL, &error);
    EXPECT_TRUE(csv != NULL);
    
    const char* fields[] = {"a", "b", "c"};
    csv_add_row(csv, fields, 3, &error);
    
    const char* val = csv_get(csv, 0, 1, &error);
    EXPECT_TRUE(val != NULL);
    EXPECT_TRUE(strcmp(val, "b") == 0);
    
    csv_free(csv);
}

void test_csv_strerror() {
    TEST(Csv_Strerror);
    const char* msg = csv_strerror(CSV_OK);
    EXPECT_TRUE(msg != NULL);
    
    msg = csv_strerror(CSV_ERROR_FILE_OPEN);
    EXPECT_TRUE(msg != NULL);
}

int main() {
    test_csv_default_config();
    test_csv_create_free();
    test_csv_add_row();
    test_csv_get();
    test_csv_strerror();

    return 0;
}
