#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/utest.h"
#include "../c_utils/fsm.h"

static int enter_count = 0;
static int exit_count = 0;
static int event_count = 0;
static int guard_count = 0;
static int action_count = 0;

static void reset_counters(void) {
    enter_count = 0;
    exit_count = 0;
    event_count = 0;
    guard_count = 0;
    action_count = 0;
}

static void on_enter_cb(void *data, fsm_event_t event) {
    enter_count++;
}

static void on_exit_cb(void *data, fsm_event_t event) {
    exit_count++;
}

static void on_event_cb(void *data, fsm_event_t event) {
    event_count++;
}

static bool guard_always_true(void *data, fsm_event_t event) {
    guard_count++;
    return true;
}

static bool guard_always_false(void *data, fsm_event_t event) {
    guard_count++;
    return false;
}

static void transition_action_cb(void *data, fsm_event_t event) {
    action_count++;
}

void test_fsm_default_options() {
    TEST(Fsm_DefaultOptions);
    fsm_options_t options = fsm_default_options();
    
    EXPECT_TRUE(options.auto_start == false);
    EXPECT_TRUE(options.enable_guard == true);
    EXPECT_TRUE(options.enable_error_handling == true);
    EXPECT_TRUE(options.enable_nested_states == false);
}

void test_fsm_create_free() {
    TEST(Fsm_CreateFree);
    fsm_error_t error;
    fsm_t* fsm = fsm_create(NULL, NULL, &error);
    
    EXPECT_TRUE(fsm != NULL);
    EXPECT_EQ(error, FSM_OK);
    EXPECT_TRUE(fsm_is_running(fsm) == false);
    
    fsm_free(fsm);
}

void test_fsm_create_with_options() {
    TEST(Fsm_CreateWithOptions);
    fsm_error_t error;
    fsm_options_t opts = fsm_default_options();
    opts.auto_start = true;
    opts.enable_guard = false;
    
    fsm_t* fsm = fsm_create(NULL, &opts, &error);
    
    EXPECT_TRUE(fsm != NULL);
    EXPECT_EQ(error, FSM_OK);
    EXPECT_TRUE(fsm->options.auto_start == true);
    EXPECT_TRUE(fsm->options.enable_guard == false);
    
    fsm_free(fsm);
}

void test_fsm_add_state() {
    TEST(Fsm_AddState);
    fsm_error_t error;
    fsm_t* fsm = fsm_create(NULL, NULL, &error);
    EXPECT_TRUE(fsm != NULL);
    
    bool result = fsm_add_state(fsm, 0, NULL, NULL, NULL, &error);
    EXPECT_TRUE(result);
    EXPECT_EQ(error, FSM_OK);
    
    result = fsm_add_state(fsm, 1, NULL, NULL, NULL, &error);
    EXPECT_TRUE(result);
    
    fsm_free(fsm);
}

void test_fsm_add_duplicate_state() {
    TEST(Fsm_AddDuplicateState);
    fsm_error_t error;
    fsm_t* fsm = fsm_create(NULL, NULL, &error);
    EXPECT_TRUE(fsm != NULL);
    
    fsm_add_state(fsm, 0, NULL, NULL, NULL, &error);
    
    bool result = fsm_add_state(fsm, 0, NULL, NULL, NULL, &error);
    EXPECT_TRUE(result == false);
    EXPECT_EQ(error, FSM_ERROR_INVALID_STATE);
    
    fsm_free(fsm);
}

void test_fsm_add_transition() {
    TEST(Fsm_AddTransition);
    fsm_error_t error;
    fsm_t* fsm = fsm_create(NULL, NULL, &error);
    EXPECT_TRUE(fsm != NULL);
    
    fsm_add_state(fsm, 0, NULL, NULL, NULL, &error);
    fsm_add_state(fsm, 1, NULL, NULL, NULL, &error);
    
    bool result = fsm_add_transition(fsm, 0, 1, 1, NULL, NULL, &error);
    EXPECT_TRUE(result);
    EXPECT_EQ(error, FSM_OK);
    
    fsm_free(fsm);
}

void test_fsm_add_transition_invalid_state() {
    TEST(Fsm_AddTransitionInvalidState);
    fsm_error_t error;
    fsm_t* fsm = fsm_create(NULL, NULL, &error);
    EXPECT_TRUE(fsm != NULL);
    
    fsm_add_state(fsm, 0, NULL, NULL, NULL, &error);
    
    bool result = fsm_add_transition(fsm, 0, 1, 99, NULL, NULL, &error);
    EXPECT_TRUE(result == false);
    EXPECT_EQ(error, FSM_ERROR_STATE_NOT_FOUND);
    
    fsm_free(fsm);
}

void test_fsm_set_initial_state() {
    TEST(Fsm_SetInitialState);
    fsm_error_t error;
    fsm_t* fsm = fsm_create(NULL, NULL, &error);
    EXPECT_TRUE(fsm != NULL);
    
    fsm_add_state(fsm, 0, NULL, NULL, NULL, &error);
    fsm_add_state(fsm, 1, NULL, NULL, NULL, &error);
    
    bool result = fsm_set_initial_state(fsm, 0, &error);
    EXPECT_TRUE(result);
    EXPECT_EQ(error, FSM_OK);
    
    fsm_free(fsm);
}

void test_fsm_set_initial_state_invalid() {
    TEST(Fsm_SetInitialStateInvalid);
    fsm_error_t error;
    fsm_t* fsm = fsm_create(NULL, NULL, &error);
    EXPECT_TRUE(fsm != NULL);
    
    fsm_add_state(fsm, 0, NULL, NULL, NULL, &error);
    
    bool result = fsm_set_initial_state(fsm, 99, &error);
    EXPECT_TRUE(result == false);
    EXPECT_EQ(error, FSM_ERROR_STATE_NOT_FOUND);
    
    fsm_free(fsm);
}

void test_fsm_start_stop() {
    TEST(Fsm_StartStop);
    fsm_error_t error;
    fsm_t* fsm = fsm_create(NULL, NULL, &error);
    EXPECT_TRUE(fsm != NULL);
    
    fsm_add_state(fsm, 0, NULL, NULL, NULL, &error);
    fsm_set_initial_state(fsm, 0, &error);
    
    EXPECT_TRUE(fsm_is_running(fsm) == false);
    
    bool result = fsm_start(fsm, &error);
    EXPECT_TRUE(result);
    EXPECT_TRUE(fsm_is_running(fsm) == true);
    EXPECT_EQ(fsm_current_state(fsm), 0);
    
    result = fsm_stop(fsm, &error);
    EXPECT_TRUE(result);
    EXPECT_TRUE(fsm_is_running(fsm) == false);
    EXPECT_EQ(fsm_current_state(fsm), -1);
    
    fsm_free(fsm);
}

void test_fsm_start_without_initial_state() {
    TEST(Fsm_StartWithoutInitialState);
    fsm_error_t error;
    fsm_t* fsm = fsm_create(NULL, NULL, &error);
    EXPECT_TRUE(fsm != NULL);
    
    fsm_add_state(fsm, 0, NULL, NULL, NULL, &error);
    
    bool result = fsm_start(fsm, &error);
    EXPECT_TRUE(result == false);
    EXPECT_EQ(error, FSM_ERROR_INVALID_STATE);
    
    fsm_free(fsm);
}

void test_fsm_transition() {
    TEST(Fsm_Transition);
    reset_counters();
    
    fsm_error_t error;
    fsm_t* fsm = fsm_create(NULL, NULL, &error);
    EXPECT_TRUE(fsm != NULL);
    
    fsm_add_state(fsm, 0, on_enter_cb, on_exit_cb, on_event_cb, &error);
    fsm_add_state(fsm, 1, on_enter_cb, on_exit_cb, on_event_cb, &error);
    
    fsm_add_transition(fsm, 0, 1, 1, NULL, NULL, &error);
    
    fsm_set_initial_state(fsm, 0, &error);
    fsm_start(fsm, &error);
    
    EXPECT_EQ(fsm_current_state(fsm), 0);
    EXPECT_EQ(enter_count, 1);
    
    bool result = fsm_handle_event(fsm, 1, &error);
    EXPECT_TRUE(result);
    EXPECT_EQ(fsm_current_state(fsm), 1);
    EXPECT_EQ(exit_count, 1);
    EXPECT_EQ(enter_count, 2);
    
    fsm_free(fsm);
}

void test_fsm_transition_before_start() {
    TEST(Fsm_TransitionBeforeStart);
    fsm_error_t error;
    fsm_t* fsm = fsm_create(NULL, NULL, &error);
    EXPECT_TRUE(fsm != NULL);
    
    fsm_add_state(fsm, 0, NULL, NULL, NULL, &error);
    fsm_add_state(fsm, 1, NULL, NULL, NULL, &error);
    
    fsm_set_initial_state(fsm, 0, &error);
    
    bool result = fsm_transition(fsm, 1, &error);
    EXPECT_TRUE(result == false);
    EXPECT_EQ(error, FSM_ERROR_INVALID_STATE);
    
    fsm_free(fsm);
}

void test_fsm_guard_condition() {
    TEST(Fsm_GuardCondition);
    reset_counters();
    
    fsm_error_t error;
    fsm_options_t opts = fsm_default_options();
    opts.enable_guard = true;
    
    fsm_t* fsm = fsm_create(NULL, &opts, &error);
    EXPECT_TRUE(fsm != NULL);
    
    fsm_add_state(fsm, 0, NULL, NULL, NULL, &error);
    fsm_add_state(fsm, 1, NULL, NULL, NULL, &error);
    
    fsm_add_transition(fsm, 0, 1, 1, guard_always_false, NULL, &error);
    
    fsm_set_initial_state(fsm, 0, &error);
    fsm_start(fsm, &error);
    
    bool result = fsm_handle_event(fsm, 1, &error);
    EXPECT_TRUE(result == false);
    EXPECT_EQ(guard_count, 1);
    EXPECT_EQ(fsm_current_state(fsm), 0);
    
    fsm_free(fsm);
}

void test_fsm_guard_disabled() {
    TEST(Fsm_GuardDisabled);
    reset_counters();
    
    fsm_error_t error;
    fsm_options_t opts = fsm_default_options();
    opts.enable_guard = false;
    
    fsm_t* fsm = fsm_create(NULL, &opts, &error);
    EXPECT_TRUE(fsm != NULL);
    
    fsm_add_state(fsm, 0, NULL, NULL, NULL, &error);
    fsm_add_state(fsm, 1, NULL, NULL, NULL, &error);
    
    fsm_add_transition(fsm, 0, 1, 1, guard_always_false, NULL, &error);
    
    fsm_set_initial_state(fsm, 0, &error);
    fsm_start(fsm, &error);
    
    bool result = fsm_handle_event(fsm, 1, &error);
    EXPECT_TRUE(result);
    EXPECT_EQ(guard_count, 0);
    EXPECT_EQ(fsm_current_state(fsm), 1);
    
    fsm_free(fsm);
}

void test_fsm_transition_action() {
    TEST(Fsm_TransitionAction);
    reset_counters();
    
    fsm_error_t error;
    fsm_t* fsm = fsm_create(NULL, NULL, &error);
    EXPECT_TRUE(fsm != NULL);
    
    fsm_add_state(fsm, 0, NULL, NULL, NULL, &error);
    fsm_add_state(fsm, 1, NULL, NULL, NULL, &error);
    
    fsm_add_transition(fsm, 0, 1, 1, NULL, transition_action_cb, &error);
    
    fsm_set_initial_state(fsm, 0, &error);
    fsm_start(fsm, &error);
    
    fsm_handle_event(fsm, 1, &error);
    EXPECT_EQ(action_count, 1);
    
    fsm_free(fsm);
}

void test_fsm_event_not_handled() {
    TEST(Fsm_EventNotHandled);
    fsm_error_t error;
    fsm_t* fsm = fsm_create(NULL, NULL, &error);
    EXPECT_TRUE(fsm != NULL);
    
    fsm_add_state(fsm, 0, NULL, NULL, NULL, &error);
    
    fsm_set_initial_state(fsm, 0, &error);
    fsm_start(fsm, &error);
    
    bool result = fsm_handle_event(fsm, 99, &error);
    EXPECT_TRUE(result == false);
    EXPECT_EQ(error, FSM_ERROR_EVENT_NOT_HANDLED);
    
    fsm_free(fsm);
}

void test_fsm_has_error() {
    TEST(Fsm_HasError);
    fsm_error_t error;
    fsm_t* fsm = fsm_create(NULL, NULL, &error);
    EXPECT_TRUE(fsm != NULL);
    
    fsm_error_t err_out;
    const char *msg_out;
    bool has_err = fsm_has_error(fsm, &err_out, &msg_out);
    
    EXPECT_TRUE(has_err == false);
    EXPECT_EQ(err_out, FSM_OK);
    EXPECT_TRUE(msg_out == NULL);
    
    fsm_free(fsm);
}

void test_fsm_strerror() {
    TEST(Fsm_Strerror);
    EXPECT_TRUE(strcmp(fsm_strerror(FSM_OK), "Success") == 0);
    EXPECT_TRUE(strcmp(fsm_strerror(FSM_ERROR_INVALID_PARAM), "Invalid parameter") == 0);
    EXPECT_TRUE(strcmp(fsm_strerror(FSM_ERROR_INVALID_STATE), "Invalid state") == 0);
    EXPECT_TRUE(strcmp(fsm_strerror(FSM_ERROR_STATE_NOT_FOUND), "State not found") == 0);
    EXPECT_TRUE(strcmp(fsm_strerror(FSM_ERROR_EVENT_NOT_HANDLED), "Event not handled") == 0);
    EXPECT_TRUE(strcmp(fsm_strerror(FSM_ERROR_MEMORY_ALLOC), "Memory allocation failed") == 0);
    EXPECT_TRUE(strcmp(fsm_strerror(FSM_ERROR_TRANSITION_FAILED), "Transition failed") == 0);
}

void test_fsm_null_params() {
    TEST(Fsm_NullParams);
    fsm_error_t error;
    
    EXPECT_TRUE(fsm_add_state(NULL, 0, NULL, NULL, NULL, &error) == false);
    EXPECT_EQ(error, FSM_ERROR_INVALID_PARAM);
    
    EXPECT_TRUE(fsm_add_transition(NULL, 0, 1, 1, NULL, NULL, &error) == false);
    EXPECT_EQ(error, FSM_ERROR_INVALID_PARAM);
    
    EXPECT_TRUE(fsm_handle_event(NULL, 1, &error) == false);
    EXPECT_EQ(error, FSM_ERROR_INVALID_PARAM);
    
    EXPECT_TRUE(fsm_transition(NULL, 1, &error) == false);
    EXPECT_EQ(error, FSM_ERROR_INVALID_PARAM);
    
    EXPECT_TRUE(fsm_set_initial_state(NULL, 0, &error) == false);
    EXPECT_EQ(error, FSM_ERROR_INVALID_PARAM);
    
    EXPECT_TRUE(fsm_start(NULL, &error) == false);
    EXPECT_EQ(error, FSM_ERROR_INVALID_PARAM);
    
    EXPECT_TRUE(fsm_stop(NULL, &error) == false);
    EXPECT_EQ(error, FSM_ERROR_INVALID_PARAM);
    
    EXPECT_TRUE(fsm_is_running(NULL) == false);
    EXPECT_EQ(fsm_current_state(NULL), -1);
    
    fsm_free(NULL);
}

void test_fsm_on_event_callback() {
    TEST(Fsm_OnEventCallback);
    reset_counters();
    
    fsm_error_t error;
    fsm_t* fsm = fsm_create(NULL, NULL, &error);
    EXPECT_TRUE(fsm != NULL);
    
    fsm_add_state(fsm, 0, NULL, NULL, on_event_cb, &error);
    
    fsm_set_initial_state(fsm, 0, &error);
    fsm_start(fsm, &error);
    
    fsm_handle_event(fsm, 99, &error);
    EXPECT_EQ(event_count, 1);
    
    fsm_free(fsm);
}

int main() {
    test_fsm_default_options();
    test_fsm_create_free();
    test_fsm_create_with_options();
    test_fsm_add_state();
    test_fsm_add_duplicate_state();
    test_fsm_add_transition();
    test_fsm_add_transition_invalid_state();
    test_fsm_set_initial_state();
    test_fsm_set_initial_state_invalid();
    test_fsm_start_stop();
    test_fsm_start_without_initial_state();
    test_fsm_transition();
    test_fsm_transition_before_start();
    test_fsm_guard_condition();
    test_fsm_guard_disabled();
    test_fsm_transition_action();
    test_fsm_event_not_handled();
    test_fsm_has_error();
    test_fsm_strerror();
    test_fsm_null_params();
    test_fsm_on_event_callback();

    return 0;
}
