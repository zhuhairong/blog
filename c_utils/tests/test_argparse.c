#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/argparse.h"

void test_argparse_create() {
    TEST(Argparse_Create);
    argparse_t* ap = argparse_create("Test program", "test [options]");
    EXPECT_TRUE(ap != NULL);
    argparse_destroy(ap);
}

void test_argparse_add_bool() {
    TEST(Argparse_AddBool);
    argparse_t* ap = argparse_create("Test", "test [options]");
    
    bool verbose = false;
    bool result = argparse_add_bool(ap, 'v', "verbose", "Enable verbose output", &verbose, false);
    EXPECT_TRUE(result);
    
    argparse_destroy(ap);
}

void test_argparse_add_int() {
    TEST(Argparse_AddInt);
    argparse_t* ap = argparse_create("Test", "test [options]");
    
    int count = 0;
    bool result = argparse_add_int(ap, 'c', "count", "Number of items", &count, 10, false);
    EXPECT_TRUE(result);
    
    argparse_destroy(ap);
}

void test_argparse_add_string() {
    TEST(Argparse_AddString);
    argparse_t* ap = argparse_create("Test", "test [options]");
    
    const char* name = NULL;
    bool result = argparse_add_string(ap, 'n', "name", "Name parameter", &name, "default", false);
    EXPECT_TRUE(result);
    
    argparse_destroy(ap);
}

void test_argparse_add_double() {
    TEST(Argparse_AddDouble);
    argparse_t* ap = argparse_create("Test", "test [options]");
    
    double value = 0.0;
    bool result = argparse_add_double(ap, 'd', "value", "Double value", &value, 1.5, false);
    EXPECT_TRUE(result);
    
    argparse_destroy(ap);
}

void test_argparse_parse_short() {
    TEST(Argparse_ParseShort);
    argparse_t* ap = argparse_create("Test", "test [options]");
    
    bool verbose = false;
    argparse_add_bool(ap, 'v', "verbose", "Enable verbose", &verbose, false);
    
    char* argv[] = {"test", "-v"};
    int argc = 2;
    
    int result = argparse_parse(ap, argc, argv);
    EXPECT_EQ(result, 0);
    EXPECT_TRUE(verbose);
    
    argparse_destroy(ap);
}

void test_argparse_parse_long() {
    TEST(Argparse_ParseLong);
    argparse_t* ap = argparse_create("Test", "test [options]");
    
    bool verbose = false;
    argparse_add_bool(ap, 'v', "verbose", "Enable verbose", &verbose, false);
    
    char* argv[] = {"test", "--verbose"};
    int argc = 2;
    
    int result = argparse_parse(ap, argc, argv);
    EXPECT_EQ(result, 0);
    EXPECT_TRUE(verbose);
    
    argparse_destroy(ap);
}

void test_argparse_parse_int() {
    TEST(Argparse_ParseInt);
    argparse_t* ap = argparse_create("Test", "test [options]");
    
    int count = 0;
    argparse_add_int(ap, 'c', "count", "Count value", &count, 0, false);
    
    char* argv[] = {"test", "-c", "42"};
    int argc = 3;
    
    int result = argparse_parse(ap, argc, argv);
    EXPECT_EQ(result, 0);
    EXPECT_EQ(count, 42);
    
    argparse_destroy(ap);
}

void test_argparse_parse_string() {
    TEST(Argparse_ParseString);
    argparse_t* ap = argparse_create("Test", "test [options]");
    
    const char* name = NULL;
    argparse_add_string(ap, 'n', "name", "Name value", &name, NULL, false);
    
    char* argv[] = {"test", "--name", "testname"};
    int argc = 3;
    
    int result = argparse_parse(ap, argc, argv);
    EXPECT_EQ(result, 0);
    EXPECT_TRUE(name != NULL);
    EXPECT_TRUE(strcmp(name, "testname") == 0);
    
    argparse_destroy(ap);
}

void test_argparse_default_value() {
    TEST(Argparse_DefaultValue);
    argparse_t* ap = argparse_create("Test", "test [options]");
    
    int count = 0;
    argparse_add_int(ap, 'c', "count", "Count", &count, 100, false);
    
    char* argv[] = {"test"};
    int argc = 1;
    
    argparse_parse(ap, argc, argv);
    EXPECT_EQ(count, 100);
    
    argparse_destroy(ap);
}

void test_argparse_is_set() {
    TEST(Argparse_IsSet);
    argparse_t* ap = argparse_create("Test", "test [options]");
    
    bool verbose = false;
    argparse_add_bool(ap, 'v', "verbose", "Verbose", &verbose, false);
    
    char* argv[] = {"test", "-v"};
    int argc = 2;
    
    argparse_parse(ap, argc, argv);
    EXPECT_TRUE(argparse_is_set(ap, "verbose"));
    
    argparse_destroy(ap);
}

void test_argparse_destroy_null() {
    TEST(Argparse_DestroyNull);
    argparse_destroy(NULL);
}

int main() {
    test_argparse_create();
    test_argparse_add_bool();
    test_argparse_add_int();
    test_argparse_add_string();
    test_argparse_add_double();
    test_argparse_parse_short();
    test_argparse_parse_long();
    test_argparse_parse_int();
    test_argparse_parse_string();
    test_argparse_default_value();
    test_argparse_is_set();
    test_argparse_destroy_null();

    return 0;
}
