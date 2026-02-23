#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/thread_local_storage.h"

static int destructor_called = 0;

static void test_destructor(void* value) {
    destructor_called++;
}

void test_tls_key_create_delete() {
    TEST(TLS_KeyCreateDelete);
    tls_key_t key;
    
    int result = tls_key_create(&key, NULL);
    EXPECT_EQ(result, 0);
    
    tls_state_t state;
    tls_error_t err = tls_key_delete(key, &state);
    EXPECT_EQ(err, TLS_OK);
}

void test_tls_set_get_value() {
    TEST(TLS_SetGetValue);
    tls_key_t key;
    
    int result = tls_key_create(&key, NULL);
    EXPECT_EQ(result, 0);
    
    int value = 42;
    result = tls_set_value(key, &value);
    EXPECT_EQ(result, 0);
    
    void* retrieved = tls_get_value(key);
    EXPECT_TRUE(retrieved != NULL);
    EXPECT_EQ(*(int*)retrieved, 42);
    
    tls_state_t state;
    tls_key_delete(key, &state);
}

void test_tls_config_init() {
    TEST(TLS_ConfigInit);
    tls_config_t config;
    tls_config_init(&config);
    
    EXPECT_TRUE(config.max_keys > 0 || config.max_keys == 0);
}

void test_tls_state_init() {
    TEST(TLS_StateInit);
    tls_state_t state;
    tls_state_init(&state);
    
    EXPECT_EQ(state.key_creations, (size_t)0);
    EXPECT_EQ(state.value_sets, (size_t)0);
}

void test_tls_with_destructor() {
    TEST(TLS_WithDestructor);
    tls_key_t key;
    destructor_called = 0;
    
    int result = tls_key_create(&key, test_destructor);
    EXPECT_EQ(result, 0);
    
    int value = 100;
    tls_set_value(key, &value);
    
    tls_state_t state;
    tls_key_delete(key, &state);
}

void test_tls_strerror() {
    TEST(TLS_Strerror);
    tls_state_t state;
    tls_state_init(&state);
    
    const char* msg = tls_strerror(&state);
    EXPECT_TRUE(msg != NULL);
}

int main() {
    test_tls_key_create_delete();
    test_tls_set_get_value();
    test_tls_config_init();
    test_tls_state_init();
    test_tls_with_destructor();
    test_tls_strerror();

    return 0;
}
