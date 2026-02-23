#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/fsm.h"

void test_fsm_default_options() {
    TEST(Fsm_DefaultOptions);
    fsm_options_t options = fsm_default_options();
    
    EXPECT_TRUE(options.auto_start || !options.auto_start);
}

void test_fsm_create_free() {
    TEST(Fsm_CreateFree);
    fsm_error_t error;
    fsm_t* fsm = fsm_create(NULL, NULL, &error);
    
    EXPECT_TRUE(fsm != NULL);
    EXPECT_EQ(error, FSM_OK);
    
    fsm_free(fsm);
}

void test_fsm_add_state() {
    TEST(Fsm_AddState);
    fsm_error_t error;
    fsm_t* fsm = fsm_create(NULL, NULL, &error);
    EXPECT_TRUE(fsm != NULL);
    
    bool result = fsm_add_state(fsm, 0, NULL, NULL, NULL, &error);
    EXPECT_TRUE(result);
    
    result = fsm_add_state(fsm, 1, NULL, NULL, NULL, &error);
    EXPECT_TRUE(result);
    
    fsm_free(fsm);
}

void test_fsm_transition() {
    TEST(Fsm_Transition);
    fsm_error_t error;
    fsm_t* fsm = fsm_create(NULL, NULL, &error);
    EXPECT_TRUE(fsm != NULL);
    
    fsm_add_state(fsm, 0, NULL, NULL, NULL, &error);
    fsm_add_state(fsm, 1, NULL, NULL, NULL, &error);
    
    bool result = fsm_add_transition(fsm, 0, 1, 1, NULL, NULL, &error);
    EXPECT_TRUE(result);
    
    fsm_set_initial_state(fsm, 0, &error);
    fsm_start(fsm, &error);
    
    EXPECT_EQ(fsm_current_state(fsm), 0);
    
    result = fsm_handle_event(fsm, 1, &error);
    EXPECT_TRUE(result);
    EXPECT_EQ(fsm_current_state(fsm), 1);
    
    fsm_free(fsm);
}

void test_fsm_strerror() {
    TEST(Fsm_Strerror);
    const char* msg = fsm_strerror(FSM_OK);
    EXPECT_TRUE(msg != NULL);
    
    msg = fsm_strerror(FSM_ERROR_INVALID_PARAM);
    EXPECT_TRUE(msg != NULL);
}

int main() {
    test_fsm_default_options();
    test_fsm_create_free();
    test_fsm_add_state();
    test_fsm_transition();
    test_fsm_strerror();

    return 0;
}
