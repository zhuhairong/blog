#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/config.h"

static const char* test_ini_file = "/tmp/test_config.ini";
static const char* test_json_file = "/tmp/test_config.json";

static void create_test_ini_file(void) {
    FILE *fp = fopen(test_ini_file, "w");
    if (fp) {
        fprintf(fp, "# Test config file\n");
        fprintf(fp, "[database]\n");
        fprintf(fp, "host = localhost\n");
        fprintf(fp, "port = 3306\n");
        fprintf(fp, "name = testdb\n");
        fprintf(fp, "\n");
        fprintf(fp, "[server]\n");
        fprintf(fp, "host = 0.0.0.0\n");
        fprintf(fp, "port = 8080\n");
        fprintf(fp, "debug = true\n");
        fclose(fp);
    }
}

static void create_test_json_file(void) {
    FILE *fp = fopen(test_json_file, "w");
    if (fp) {
        fprintf(fp, "{\n");
        fprintf(fp, "  \"app\": {\n");
        fprintf(fp, "    \"name\": \"TestApp\",\n");
        fprintf(fp, "    \"version\": \"1.0.0\",\n");
        fprintf(fp, "    \"port\": 9000\n");
        fprintf(fp, "  },\n");
        fprintf(fp, "  \"debug\": true,\n");
        fprintf(fp, "  \"timeout\": 30.5\n");
        fprintf(fp, "}\n");
        fclose(fp);
    }
}

void test_config_create_free() {
    TEST(Config_CreateFree);
    config_t* cfg = config_create();
    EXPECT_TRUE(cfg != NULL);
    EXPECT_TRUE(config_count(cfg) == 0);
    config_free(cfg);
}

void test_config_null_params() {
    TEST(Config_NullParams);
    
    EXPECT_TRUE(config_get_string(NULL, "sec", "key", "default") != NULL);
    EXPECT_TRUE(strcmp(config_get_string(NULL, "sec", "key", "default"), "default") == 0);
    
    EXPECT_TRUE(config_get_int(NULL, "sec", "key", 42) == 42);
    EXPECT_TRUE(config_get_double(NULL, "sec", "key", 3.14) > 3.13);
    EXPECT_TRUE(config_get_bool(NULL, "sec", "key", true) == true);
    
    EXPECT_TRUE(config_set_string(NULL, "sec", "key", "val") == false);
    EXPECT_TRUE(config_delete(NULL, "sec", "key") == false);
    EXPECT_TRUE(config_exists(NULL, "sec", "key") == false);
    
    config_free(NULL);
    config_clear(NULL);
    EXPECT_TRUE(config_count(NULL) == 0);
}

void test_config_set_get_string() {
    TEST(Config_SetGetString);
    config_t* cfg = config_create();
    EXPECT_TRUE(cfg != NULL);
    
    EXPECT_TRUE(config_set_string(cfg, "section", "key1", "value1"));
    EXPECT_TRUE(strcmp(config_get_string(cfg, "section", "key1", ""), "value1") == 0);
    
    EXPECT_TRUE(config_set_string(cfg, NULL, "key2", "value2"));
    EXPECT_TRUE(strcmp(config_get_string(cfg, NULL, "key2", ""), "value2") == 0);
    
    EXPECT_TRUE(config_set_string(cfg, "", "key3", "value3"));
    EXPECT_TRUE(strcmp(config_get_string(cfg, "", "key3", ""), "value3") == 0);
    
    config_free(cfg);
}

void test_config_set_get_int() {
    TEST(Config_SetGetInt);
    config_t* cfg = config_create();
    EXPECT_TRUE(cfg != NULL);
    
    EXPECT_TRUE(config_set_int(cfg, "numbers", "positive", 42));
    EXPECT_TRUE(config_get_int(cfg, "numbers", "positive", 0) == 42);
    
    EXPECT_TRUE(config_set_int(cfg, "numbers", "negative", -100));
    EXPECT_TRUE(config_get_int(cfg, "numbers", "negative", 0) == -100);
    
    EXPECT_TRUE(config_set_int(cfg, "numbers", "zero", 0));
    EXPECT_TRUE(config_get_int(cfg, "numbers", "zero", -1) == 0);
    
    config_free(cfg);
}

void test_config_set_get_double() {
    TEST(Config_SetGetDouble);
    config_t* cfg = config_create();
    EXPECT_TRUE(cfg != NULL);
    
    EXPECT_TRUE(config_set_double(cfg, "floats", "pi", 3.14159));
    double pi = config_get_double(cfg, "floats", "pi", 0.0);
    EXPECT_TRUE(pi > 3.141 && pi < 3.142);
    
    EXPECT_TRUE(config_set_double(cfg, "floats", "negative", -2.5));
    double neg = config_get_double(cfg, "floats", "negative", 0.0);
    EXPECT_TRUE(neg < -2.4 && neg > -2.6);
    
    config_free(cfg);
}

void test_config_set_get_bool() {
    TEST(Config_SetGetBool);
    config_t* cfg = config_create();
    EXPECT_TRUE(cfg != NULL);
    
    EXPECT_TRUE(config_set_bool(cfg, "flags", "enabled", true));
    EXPECT_TRUE(config_get_bool(cfg, "flags", "enabled", false) == true);
    
    EXPECT_TRUE(config_set_bool(cfg, "flags", "disabled", false));
    EXPECT_TRUE(config_get_bool(cfg, "flags", "disabled", true) == false);
    
    config_set_string(cfg, "flags", "yes_val", "yes");
    EXPECT_TRUE(config_get_bool(cfg, "flags", "yes_val", false) == true);
    
    config_set_string(cfg, "flags", "no_val", "no");
    EXPECT_TRUE(config_get_bool(cfg, "flags", "no_val", true) == false);
    
    config_set_string(cfg, "flags", "on_val", "ON");
    EXPECT_TRUE(config_get_bool(cfg, "flags", "on_val", false) == true);
    
    config_set_string(cfg, "flags", "off_val", "OFF");
    EXPECT_TRUE(config_get_bool(cfg, "flags", "off_val", true) == false);
    
    config_free(cfg);
}

void test_config_exists_delete() {
    TEST(Config_ExistsDelete);
    config_t* cfg = config_create();
    EXPECT_TRUE(cfg != NULL);
    
    EXPECT_TRUE(config_exists(cfg, "sec", "key") == false);
    EXPECT_TRUE(config_has_key(cfg, "sec", "key") == false);
    
    config_set_string(cfg, "sec", "key", "value");
    EXPECT_TRUE(config_exists(cfg, "sec", "key") == true);
    EXPECT_TRUE(config_has_key(cfg, "sec", "key") == true);
    
    EXPECT_TRUE(config_delete(cfg, "sec", "key") == true);
    EXPECT_TRUE(config_exists(cfg, "sec", "key") == false);
    
    EXPECT_TRUE(config_delete(cfg, "sec", "key") == false);
    
    config_free(cfg);
}

void test_config_load_ini() {
    TEST(Config_LoadIni);
    create_test_ini_file();
    
    config_error_t error;
    config_t* cfg = config_load(test_ini_file, CONFIG_FORMAT_INI, &error);
    EXPECT_TRUE(cfg != NULL);
    EXPECT_TRUE(error == CONFIG_OK);
    
    EXPECT_TRUE(strcmp(config_get_string(cfg, "database", "host", ""), "localhost") == 0);
    EXPECT_TRUE(config_get_int(cfg, "database", "port", 0) == 3306);
    EXPECT_TRUE(strcmp(config_get_string(cfg, "database", "name", ""), "testdb") == 0);
    
    EXPECT_TRUE(strcmp(config_get_string(cfg, "server", "host", ""), "0.0.0.0") == 0);
    EXPECT_TRUE(config_get_int(cfg, "server", "port", 0) == 8080);
    EXPECT_TRUE(config_get_bool(cfg, "server", "debug", false) == true);
    
    config_free(cfg);
}

void test_config_save_ini() {
    TEST(Config_SaveIni);
    config_t* cfg = config_create();
    EXPECT_TRUE(cfg != NULL);
    
    config_set_string(cfg, "test", "key1", "value1");
    config_set_int(cfg, "test", "key2", 123);
    config_set_bool(cfg, "test", "key3", true);
    
    config_error_t error;
    EXPECT_TRUE(config_save(cfg, test_ini_file, CONFIG_FORMAT_INI, &error) == true);
    EXPECT_TRUE(error == CONFIG_OK);
    
    config_free(cfg);
    
    cfg = config_load(test_ini_file, CONFIG_FORMAT_INI, &error);
    EXPECT_TRUE(cfg != NULL);
    EXPECT_TRUE(strcmp(config_get_string(cfg, "test", "key1", ""), "value1") == 0);
    EXPECT_TRUE(config_get_int(cfg, "test", "key2", 0) == 123);
    EXPECT_TRUE(config_get_bool(cfg, "test", "key3", false) == true);
    
    config_free(cfg);
}

void test_config_load_json() {
    TEST(Config_LoadJson);
    create_test_json_file();
    
    config_error_t error;
    config_t* cfg = config_load(test_json_file, CONFIG_FORMAT_JSON, &error);
    EXPECT_TRUE(cfg != NULL);
    EXPECT_TRUE(error == CONFIG_OK);
    
    EXPECT_TRUE(strcmp(config_get_string(cfg, "app", "name", ""), "TestApp") == 0);
    EXPECT_TRUE(strcmp(config_get_string(cfg, "app", "version", ""), "1.0.0") == 0);
    EXPECT_TRUE(config_get_int(cfg, "app", "port", 0) == 9000);
    
    EXPECT_TRUE(config_get_bool(cfg, "", "debug", false) == true);
    
    config_free(cfg);
}

void test_config_save_json() {
    TEST(Config_SaveJson);
    config_t* cfg = config_create();
    EXPECT_TRUE(cfg != NULL);
    
    config_set_string(cfg, "app", "name", "MyApp");
    config_set_int(cfg, "app", "port", 8080);
    config_set_bool(cfg, "", "enabled", true);
    config_set_double(cfg, "", "ratio", 0.5);
    
    config_error_t error;
    EXPECT_TRUE(config_save(cfg, test_json_file, CONFIG_FORMAT_JSON, &error) == true);
    EXPECT_TRUE(error == CONFIG_OK);
    
    config_free(cfg);
    
    cfg = config_load(test_json_file, CONFIG_FORMAT_JSON, &error);
    EXPECT_TRUE(cfg != NULL);
    EXPECT_TRUE(strcmp(config_get_string(cfg, "app", "name", ""), "MyApp") == 0);
    EXPECT_TRUE(config_get_int(cfg, "app", "port", 0) == 8080);
    EXPECT_TRUE(config_get_bool(cfg, "", "enabled", false) == true);
    
    config_free(cfg);
}

void test_config_auto_format() {
    TEST(Config_AutoFormat);
    create_test_ini_file();
    create_test_json_file();
    
    config_error_t error;
    
    config_t* cfg_ini = config_load(test_ini_file, CONFIG_FORMAT_AUTO, &error);
    EXPECT_TRUE(cfg_ini != NULL);
    EXPECT_TRUE(strcmp(config_get_string(cfg_ini, "database", "host", ""), "localhost") == 0);
    config_free(cfg_ini);
    
    config_t* cfg_json = config_load(test_json_file, CONFIG_FORMAT_AUTO, &error);
    EXPECT_TRUE(cfg_json != NULL);
    EXPECT_TRUE(strcmp(config_get_string(cfg_json, "app", "name", ""), "TestApp") == 0);
    config_free(cfg_json);
}

void test_config_get_sections() {
    TEST(Config_GetSections);
    config_t* cfg = config_create();
    EXPECT_TRUE(cfg != NULL);
    
    config_set_string(cfg, "sec1", "key", "val");
    config_set_string(cfg, "sec2", "key", "val");
    config_set_string(cfg, "sec3", "key", "val");
    config_set_string(cfg, NULL, "key", "val");
    
    size_t count;
    char** sections = config_get_sections(cfg, &count);
    EXPECT_TRUE(sections != NULL);
    EXPECT_TRUE(count == 4);
    
    config_free_sections(sections, count);
    config_free(cfg);
}

void test_config_get_keys() {
    TEST(Config_GetKeys);
    config_t* cfg = config_create();
    EXPECT_TRUE(cfg != NULL);
    
    config_set_string(cfg, "section", "key1", "val1");
    config_set_string(cfg, "section", "key2", "val2");
    config_set_string(cfg, "section", "key3", "val3");
    config_set_string(cfg, "other", "key4", "val4");
    
    size_t count;
    char** keys = config_get_keys(cfg, "section", &count);
    EXPECT_TRUE(keys != NULL);
    EXPECT_TRUE(count == 3);
    
    config_free_keys(keys, count);
    config_free(cfg);
}

void test_config_clear() {
    TEST(Config_Clear);
    config_t* cfg = config_create();
    EXPECT_TRUE(cfg != NULL);
    
    config_set_string(cfg, "sec", "key1", "val1");
    config_set_string(cfg, "sec", "key2", "val2");
    EXPECT_TRUE(config_count(cfg) == 2);
    
    config_clear(cfg);
    EXPECT_TRUE(config_count(cfg) == 0);
    EXPECT_TRUE(config_exists(cfg, "sec", "key1") == false);
    
    config_free(cfg);
}

void test_config_update_value() {
    TEST(Config_UpdateValue);
    config_t* cfg = config_create();
    EXPECT_TRUE(cfg != NULL);
    
    config_set_string(cfg, "sec", "key", "value1");
    EXPECT_TRUE(strcmp(config_get_string(cfg, "sec", "key", ""), "value1") == 0);
    
    config_set_string(cfg, "sec", "key", "value2");
    EXPECT_TRUE(strcmp(config_get_string(cfg, "sec", "key", ""), "value2") == 0);
    EXPECT_TRUE(config_count(cfg) == 1);
    
    config_free(cfg);
}

void test_config_error_handling() {
    TEST(Config_ErrorHandling);
    config_error_t error;
    
    config_t* cfg = config_load("/nonexistent/path/config.ini", CONFIG_FORMAT_INI, &error);
    EXPECT_TRUE(cfg == NULL);
    EXPECT_TRUE(error == CONFIG_ERROR_FILE_OPEN);
    
    EXPECT_TRUE(strcmp(config_strerror(CONFIG_OK), "Success") == 0);
    EXPECT_TRUE(strcmp(config_strerror(CONFIG_ERROR_FILE_OPEN), "Failed to open file") == 0);
    EXPECT_TRUE(strcmp(config_strerror(CONFIG_ERROR_MEMORY_ALLOC), "Memory allocation failed") == 0);
    EXPECT_TRUE(strcmp(config_strerror(CONFIG_ERROR_UNSUPPORTED_FORMAT), "Unsupported format") == 0);
    
    FILE* fp = fopen("/tmp/test_yaml.yaml", "w");
    if (fp) {
        fprintf(fp, "key: value\n");
        fclose(fp);
    }
    config_t* cfg2 = config_load("/tmp/test_yaml.yaml", CONFIG_FORMAT_YAML, &error);
    EXPECT_TRUE(cfg2 == NULL);
    EXPECT_TRUE(error == CONFIG_ERROR_UNSUPPORTED_FORMAT);
}

void test_config_count() {
    TEST(Config_Count);
    config_t* cfg = config_create();
    EXPECT_TRUE(cfg != NULL);
    EXPECT_TRUE(config_count(cfg) == 0);
    
    for (int i = 0; i < 100; i++) {
        char key[16];
        snprintf(key, sizeof(key), "key%d", i);
        config_set_int(cfg, "section", key, i);
    }
    EXPECT_TRUE(config_count(cfg) == 100);
    
    config_free(cfg);
}

void test_config_strerror() {
    TEST(Config_Strerror);
    EXPECT_TRUE(strcmp(config_strerror(CONFIG_OK), "Success") == 0);
    EXPECT_TRUE(strcmp(config_strerror(CONFIG_ERROR_FILE_OPEN), "Failed to open file") == 0);
    EXPECT_TRUE(strcmp(config_strerror(CONFIG_ERROR_FILE_READ), "Failed to read file") == 0);
    EXPECT_TRUE(strcmp(config_strerror(CONFIG_ERROR_FILE_WRITE), "Failed to write file") == 0);
    EXPECT_TRUE(strcmp(config_strerror(CONFIG_ERROR_PARSE), "Parse error") == 0);
    EXPECT_TRUE(strcmp(config_strerror(CONFIG_ERROR_INVALID_SECTION), "Invalid section") == 0);
    EXPECT_TRUE(strcmp(config_strerror(CONFIG_ERROR_INVALID_KEY), "Invalid key") == 0);
    EXPECT_TRUE(strcmp(config_strerror(CONFIG_ERROR_INVALID_VALUE), "Invalid value") == 0);
    EXPECT_TRUE(strcmp(config_strerror(CONFIG_ERROR_MEMORY_ALLOC), "Memory allocation failed") == 0);
    EXPECT_TRUE(strcmp(config_strerror(CONFIG_ERROR_UNSUPPORTED_FORMAT), "Unsupported format") == 0);
}

void test_config_empty_values() {
    TEST(Config_EmptyValues);
    config_t* cfg = config_create();
    EXPECT_TRUE(cfg != NULL);
    
    config_set_string(cfg, "sec", "empty", "");
    EXPECT_TRUE(strcmp(config_get_string(cfg, "sec", "empty", "default"), "") == 0);
    
    config_free(cfg);
}

void test_config_special_chars() {
    TEST(Config_SpecialChars);
    config_t* cfg = config_create();
    EXPECT_TRUE(cfg != NULL);
    
    config_set_string(cfg, "sec", "key", "value with spaces");
    EXPECT_TRUE(strcmp(config_get_string(cfg, "sec", "key", ""), "value with spaces") == 0);
    
    config_set_string(cfg, "sec", "path", "/path/to/file.txt");
    EXPECT_TRUE(strcmp(config_get_string(cfg, "sec", "path", ""), "/path/to/file.txt") == 0);
    
    config_free(cfg);
}

int main() {
    test_config_create_free();
    test_config_null_params();
    test_config_set_get_string();
    test_config_set_get_int();
    test_config_set_get_double();
    test_config_set_get_bool();
    test_config_exists_delete();
    test_config_load_ini();
    test_config_save_ini();
    test_config_load_json();
    test_config_save_json();
    test_config_auto_format();
    test_config_get_sections();
    test_config_get_keys();
    test_config_clear();
    test_config_update_value();
    test_config_error_handling();
    test_config_count();
    test_config_strerror();
    test_config_empty_values();
    test_config_special_chars();

    return 0;
}
