#include "fsm.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// 默认 FSM 选项
fsm_options_t fsm_default_options(void) {
    fsm_options_t options = {
        .auto_start = false,
        .enable_guard = true,
        .enable_error_handling = true,
        .enable_nested_states = false
    };
    return options;
}

// 创建 FSM
fsm_t* fsm_create(void *data, const fsm_options_t *options, fsm_error_t *error) {
    fsm_t *fsm = calloc(1, sizeof(fsm_t));
    if (!fsm) {
        if (error) *error = FSM_ERROR_MEMORY_ALLOC;
        return NULL;
    }
    
    fsm->data = data;
    fsm->current_state = -1;
    fsm->initial_state = -1;
    fsm->states = NULL;
    fsm->transitions = NULL;
    fsm->state_count = 0;
    fsm->transition_count = 0;
    fsm->running = false;
    fsm->has_error = false;
    fsm->error = FSM_OK;
    memset(fsm->error_msg, 0, sizeof(fsm->error_msg));
    fsm->parent = NULL;
    
    if (error) *error = FSM_OK;
    return fsm;
}

// 释放 FSM
void fsm_free(fsm_t *fsm) {
    if (!fsm) return;
    
    // 释放状态
    fsm_state_s *state = fsm->states;
    while (state) {
        fsm_state_s *next = state->next;
        free(state);
        state = next;
    }
    
    // 释放转换
    fsm_transition_s *trans = fsm->transitions;
    while (trans) {
        fsm_transition_s *next = trans->next;
        free(trans);
        trans = next;
    }
    
    free(fsm);
}

// 添加状态
bool fsm_add_state(fsm_t *fsm, int state_id, fsm_action_t on_enter, fsm_action_t on_exit, 
                   fsm_action_t on_event, fsm_error_t *error) {
    if (!fsm) {
        if (error) *error = FSM_ERROR_INVALID_PARAM;
        return false;
    }
    
    // 检查状态是否已存在
    fsm_state_s *existing = fsm->states;
    while (existing) {
        if (existing->state_id == state_id) {
            if (error) *error = FSM_ERROR_INVALID_STATE;
            return false;
        }
        existing = existing->next;
    }
    
    fsm_state_s *state = malloc(sizeof(fsm_state_s));
    if (!state) {
        if (error) *error = FSM_ERROR_MEMORY_ALLOC;
        return false;
    }
    
    state->state_id = state_id;
    state->on_enter = on_enter;
    state->on_exit = on_exit;
    state->on_event = on_event;
    state->next = fsm->states;
    fsm->states = state;
    fsm->state_count++;
    
    if (error) *error = FSM_OK;
    return true;
}

// 查找状态
static fsm_state_s* fsm_find_state(fsm_t *fsm, int state_id) {
    fsm_state_s *state = fsm->states;
    while (state) {
        if (state->state_id == state_id) return state;
        state = state->next;
    }
    return NULL;
}

// 添加状态转换
bool fsm_add_transition(fsm_t *fsm, int from_state, fsm_event_t event, int to_state, 
                        fsm_guard_t guard, fsm_action_t action, fsm_error_t *error) {
    if (!fsm) {
        if (error) *error = FSM_ERROR_INVALID_PARAM;
        return false;
    }
    
    // 检查状态是否存在
    if (!fsm_find_state(fsm, from_state) || !fsm_find_state(fsm, to_state)) {
        if (error) *error = FSM_ERROR_STATE_NOT_FOUND;
        return false;
    }
    
    fsm_transition_s *trans = malloc(sizeof(fsm_transition_s));
    if (!trans) {
        if (error) *error = FSM_ERROR_MEMORY_ALLOC;
        return false;
    }
    
    trans->from_state = from_state;
    trans->event = event;
    trans->to_state = to_state;
    trans->guard = guard;
    trans->action = action;
    trans->next = fsm->transitions;
    fsm->transitions = trans;
    fsm->transition_count++;
    
    if (error) *error = FSM_OK;
    return true;
}

// 触发事件
bool fsm_handle_event(fsm_t *fsm, fsm_event_t event, fsm_error_t *error) {
    if (!fsm) {
        if (error) *error = FSM_ERROR_INVALID_PARAM;
        return false;
    }
    
    if (!fsm->running) {
        if (error) *error = FSM_ERROR_INVALID_STATE;
        return false;
    }
    
    // 查找匹配的转换
    fsm_transition_s *trans = fsm->transitions;
    while (trans) {
        if (trans->from_state == fsm->current_state && trans->event == event) {
            // 检查守卫条件
            if (trans->guard && !trans->guard(fsm->data, event)) {
                trans = trans->next;
                continue;
            }
            
            // 执行转换动作
            if (trans->action) {
                trans->action(fsm->data, event);
            }
            
            // 执行状态转换
            return fsm_transition(fsm, trans->to_state, error);
        }
        trans = trans->next;
    }
    
    // 没有找到匹配的转换，调用当前状态的事件回调
    fsm_state_s *state = fsm_find_state(fsm, fsm->current_state);
    if (state && state->on_event) {
        state->on_event(fsm->data, event);
        if (error) *error = FSM_OK;
        return true;
    }
    
    if (error) *error = FSM_ERROR_EVENT_NOT_HANDLED;
    return false;
}

// 手动转换状态
bool fsm_transition(fsm_t *fsm, int next_state, fsm_error_t *error) {
    if (!fsm) {
        if (error) *error = FSM_ERROR_INVALID_PARAM;
        return false;
    }
    
    fsm_state_s *curr_state = fsm_find_state(fsm, fsm->current_state);
    fsm_state_s *next_st = fsm_find_state(fsm, next_state);
    
    if (!next_st) {
        if (error) *error = FSM_ERROR_STATE_NOT_FOUND;
        return false;
    }
    
    // 执行退出动作
    if (curr_state && curr_state->on_exit) {
        curr_state->on_exit(fsm->data, 0);
    }
    
    fsm->current_state = next_state;
    
    // 执行进入动作
    if (next_st->on_enter) {
        next_st->on_enter(fsm->data, 0);
    }
    
    if (error) *error = FSM_OK;
    return true;
}

// 获取当前状态
int fsm_current_state(const fsm_t *fsm) {
    return fsm ? fsm->current_state : -1;
}

// 设置初始状态
bool fsm_set_initial_state(fsm_t *fsm, int state_id, fsm_error_t *error) {
    if (!fsm) {
        if (error) *error = FSM_ERROR_INVALID_PARAM;
        return false;
    }
    
    if (!fsm_find_state(fsm, state_id)) {
        if (error) *error = FSM_ERROR_STATE_NOT_FOUND;
        return false;
    }
    
    fsm->initial_state = state_id;
    
    if (error) *error = FSM_OK;
    return true;
}

// 启动 FSM
bool fsm_start(fsm_t *fsm, fsm_error_t *error) {
    if (!fsm) {
        if (error) *error = FSM_ERROR_INVALID_PARAM;
        return false;
    }
    
    if (fsm->initial_state == -1) {
        if (error) *error = FSM_ERROR_INVALID_STATE;
        return false;
    }
    
    fsm->running = true;
    fsm->current_state = fsm->initial_state;
    
    // 执行初始状态的进入动作
    fsm_state_s *state = fsm_find_state(fsm, fsm->current_state);
    if (state && state->on_enter) {
        state->on_enter(fsm->data, 0);
    }
    
    if (error) *error = FSM_OK;
    return true;
}

// 停止 FSM
bool fsm_stop(fsm_t *fsm, fsm_error_t *error) {
    if (!fsm) {
        if (error) *error = FSM_ERROR_INVALID_PARAM;
        return false;
    }
    
    // 执行当前状态的退出动作
    fsm_state_s *state = fsm_find_state(fsm, fsm->current_state);
    if (state && state->on_exit) {
        state->on_exit(fsm->data, 0);
    }
    
    fsm->running = false;
    fsm->current_state = -1;
    
    if (error) *error = FSM_OK;
    return true;
}

// 检查 FSM 是否运行
bool fsm_is_running(const fsm_t *fsm) {
    return fsm ? fsm->running : false;
}

// 检查 FSM 是否有错误
bool fsm_has_error(const fsm_t *fsm, fsm_error_t *error, const char **error_msg) {
    if (!fsm) {
        if (error) *error = FSM_ERROR_INVALID_PARAM;
        return true;
    }
    
    if (fsm->has_error) {
        if (error) *error = fsm->error;
        if (error_msg) *error_msg = fsm->error_msg;
        return true;
    }
    
    if (error) *error = FSM_OK;
    if (error_msg) *error_msg = NULL;
    return false;
}

// 获取错误信息
const char* fsm_strerror(fsm_error_t error) {
    switch (error) {
        case FSM_OK: return "Success";
        case FSM_ERROR_INVALID_PARAM: return "Invalid parameter";
        case FSM_ERROR_INVALID_STATE: return "Invalid state";
        case FSM_ERROR_INVALID_EVENT: return "Invalid event";
        case FSM_ERROR_STATE_NOT_FOUND: return "State not found";
        case FSM_ERROR_EVENT_NOT_HANDLED: return "Event not handled";
        case FSM_ERROR_MEMORY_ALLOC: return "Memory allocation failed";
        case FSM_ERROR_TRANSITION_FAILED: return "Transition failed";
        default: return "Unknown error";
    }
}
