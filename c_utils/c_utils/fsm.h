#ifndef C_UTILS_FSM_H
#define C_UTILS_FSM_H

#include <stddef.h>
#include <stdbool.h>

// FSM 错误码
typedef enum {
    FSM_OK = 0,
    FSM_ERROR_INVALID_PARAM,
    FSM_ERROR_INVALID_STATE,
    FSM_ERROR_INVALID_EVENT,
    FSM_ERROR_STATE_NOT_FOUND,
    FSM_ERROR_EVENT_NOT_HANDLED,
    FSM_ERROR_MEMORY_ALLOC,
    FSM_ERROR_TRANSITION_FAILED
} fsm_error_t;

// FSM 事件类型
typedef int fsm_event_t;

// FSM 动作回调
typedef void (*fsm_action_t)(void *data, fsm_event_t event);

// FSM 转换条件回调
typedef bool (*fsm_guard_t)(void *data, fsm_event_t event);

// FSM 状态转换
typedef struct {
    int from_state;
    fsm_event_t event;
    int to_state;
    fsm_guard_t guard;
    fsm_action_t action;
    struct fsm_transition_s *next;
} fsm_transition_s;

// FSM 状态
typedef struct {
    int state_id;
    fsm_action_t on_enter;
    fsm_action_t on_exit;
    fsm_action_t on_event;
    struct fsm_state_s *next;
} fsm_state_s;

// FSM
typedef struct fsm_s {
    void *data;
    int current_state;
    int initial_state;
    fsm_state_s *states;
    fsm_transition_s *transitions;
    size_t state_count;
    size_t transition_count;
    bool running;
    bool has_error;
    fsm_error_t error;
    char error_msg[256];
    struct fsm_s *parent;
} fsm_t;

// FSM 选项
typedef struct {
    bool auto_start;
    bool enable_guard;
    bool enable_error_handling;
    bool enable_nested_states;
} fsm_options_t;

// 默认 FSM 选项
// 返回: 默认选项
fsm_options_t fsm_default_options(void);

// 创建 FSM
// data: 用户数据
// options: 选项（可为 NULL 使用默认选项）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 FSM 指针，失败返回 NULL
fsm_t* fsm_create(void *data, const fsm_options_t *options, fsm_error_t *error);

// 释放 FSM
// fsm: FSM
void fsm_free(fsm_t *fsm);

// 添加状态
// fsm: FSM
// state_id: 状态 ID
// on_enter: 进入状态回调
// on_exit: 退出状态回调
// on_event: 事件回调
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool fsm_add_state(fsm_t *fsm, int state_id, fsm_action_t on_enter, fsm_action_t on_exit, fsm_action_t on_event, fsm_error_t *error);

// 添加状态转换
// fsm: FSM
// from_state: 源状态
// event: 事件
// to_state: 目标状态
// guard: 转换条件（可为 NULL）
// action: 转换动作（可为 NULL）
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool fsm_add_transition(fsm_t *fsm, int from_state, fsm_event_t event, int to_state, fsm_guard_t guard, fsm_action_t action, fsm_error_t *error);

// 触发事件
// fsm: FSM
// event: 事件
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool fsm_handle_event(fsm_t *fsm, fsm_event_t event, fsm_error_t *error);

// 手动转换状态
// fsm: FSM
// next_state: 目标状态
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool fsm_transition(fsm_t *fsm, int next_state, fsm_error_t *error);

// 获取当前状态
// fsm: FSM
// 返回: 当前状态 ID
int fsm_current_state(const fsm_t *fsm);

// 设置初始状态
// fsm: FSM
// state_id: 初始状态 ID
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool fsm_set_initial_state(fsm_t *fsm, int state_id, fsm_error_t *error);

// 启动 FSM
// fsm: FSM
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool fsm_start(fsm_t *fsm, fsm_error_t *error);

// 停止 FSM
// fsm: FSM
// error: 错误码（输出参数，可为 NULL）
// 返回: 成功返回 true，失败返回 false
bool fsm_stop(fsm_t *fsm, fsm_error_t *error);

// 检查 FSM 是否运行
// fsm: FSM
// 返回: 运行中返回 true，否则返回 false
bool fsm_is_running(const fsm_t *fsm);

// 检查 FSM 是否有错误
// fsm: FSM
// error: 错误码（输出参数，可为 NULL）
// error_msg: 错误信息（输出参数，可为 NULL）
// 返回: 有错误返回 true，无错误返回 false
bool fsm_has_error(const fsm_t *fsm, fsm_error_t *error, const char **error_msg);

// 获取错误信息
// error: 错误码
// 返回: 错误信息字符串
const char* fsm_strerror(fsm_error_t error);

#endif // C_UTILS_FSM_H
