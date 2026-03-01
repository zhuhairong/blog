/**
 * 有限状态机 (FSM) 演示程序
 * 
 * 功能：
 * - 状态机创建和配置
 * - 状态转换
 * - 事件处理
 * - 实际应用场景
 */

#include <stdio.h>
#include <string.h>
#include "../c_utils/fsm.h"

// 状态定义
enum {
    STATE_IDLE = 0,
    STATE_RUNNING,
    STATE_PAUSED,
    STATE_STOPPED,
    STATE_ERROR
};

// 事件定义
enum {
    EVENT_START = 0,
    EVENT_PAUSE,
    EVENT_RESUME,
    EVENT_STOP,
    EVENT_RESET,
    EVENT_ERROR
};

// 状态名称
static const char* state_name(int state) {
    switch (state) {
        case STATE_IDLE: return "IDLE";
        case STATE_RUNNING: return "RUNNING";
        case STATE_PAUSED: return "PAUSED";
        case STATE_STOPPED: return "STOPPED";
        case STATE_ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

// 事件名称
static const char* event_name(int event) {
    switch (event) {
        case EVENT_START: return "START";
        case EVENT_PAUSE: return "PAUSE";
        case EVENT_RESUME: return "RESUME";
        case EVENT_STOP: return "STOP";
        case EVENT_RESET: return "RESET";
        case EVENT_ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

// 进入状态回调
static void on_enter(void *data, fsm_event_t event) {
    fsm_t *fsm = (fsm_t*)data;
    printf("  -> 进入状态: %s\n", state_name(fsm->current_state));
}

// 退出状态回调
static void on_exit(void *data, fsm_event_t event) {
    fsm_t *fsm = (fsm_t*)data;
    printf("  <- 退出状态: %s\n", state_name(fsm->current_state));
}

// 事件处理回调
static void on_event(void *data, fsm_event_t event) {
    fsm_t *fsm = (fsm_t*)data;
    printf("  处理事件: %s\n", event_name(event));
}

// 演示 1: 基本状态机创建和配置
static void demo_basic(void) {
    printf("\n=== 演示 1: 基本状态机 ===\n");
    
    fsm_error_t error;
    fsm_options_t opts = fsm_default_options();
    
    printf("创建状态机...\n");
    fsm_t *fsm = fsm_create(NULL, &opts, &error);
    if (!fsm) {
        printf("创建失败: %d\n", error);
        return;
    }
    
    printf("添加状态...\n");
    fsm_add_state(fsm, STATE_IDLE, on_enter, on_exit, on_event, &error);
    fsm_add_state(fsm, STATE_RUNNING, on_enter, on_exit, on_event, &error);
    fsm_add_state(fsm, STATE_PAUSED, on_enter, on_exit, on_event, &error);
    fsm_add_state(fsm, STATE_STOPPED, on_enter, on_exit, on_event, &error);
    
    printf("添加状态转换...\n");
    fsm_add_transition(fsm, STATE_IDLE, EVENT_START, STATE_RUNNING, NULL, NULL, &error);
    fsm_add_transition(fsm, STATE_RUNNING, EVENT_PAUSE, STATE_PAUSED, NULL, NULL, &error);
    fsm_add_transition(fsm, STATE_PAUSED, EVENT_RESUME, STATE_RUNNING, NULL, NULL, &error);
    fsm_add_transition(fsm, STATE_RUNNING, EVENT_STOP, STATE_STOPPED, NULL, NULL, &error);
    fsm_add_transition(fsm, STATE_PAUSED, EVENT_STOP, STATE_STOPPED, NULL, NULL, &error);
    fsm_add_transition(fsm, STATE_STOPPED, EVENT_RESET, STATE_IDLE, NULL, NULL, &error);
    
    printf("状态机结构:\n");
    printf("  IDLE --START--> RUNNING\n");
    printf("  RUNNING --PAUSE--> PAUSED\n");
    printf("  PAUSED --RESUME--> RUNNING\n");
    printf("  RUNNING --STOP--> STOPPED\n");
    printf("  PAUSED --STOP--> STOPPED\n");
    printf("  STOPPED --RESET--> IDLE\n");
    
    fsm_free(fsm);
}

// 演示 2: 状态转换流程
static void demo_transitions(void) {
    printf("\n=== 演示 2: 状态转换流程 ===\n");
    
    fsm_error_t error;
    fsm_options_t opts = fsm_default_options();
    
    fsm_t *fsm = fsm_create(NULL, &opts, &error);
    if (!fsm) {
        printf("创建失败: %d\n", error);
        return;
    }
    
    // 添加状态
    fsm_add_state(fsm, STATE_IDLE, on_enter, on_exit, on_event, &error);
    fsm_add_state(fsm, STATE_RUNNING, on_enter, on_exit, on_event, &error);
    fsm_add_state(fsm, STATE_PAUSED, on_enter, on_exit, on_event, &error);
    fsm_add_state(fsm, STATE_STOPPED, on_enter, on_exit, on_event, &error);
    
    // 添加转换
    fsm_add_transition(fsm, STATE_IDLE, EVENT_START, STATE_RUNNING, NULL, NULL, &error);
    fsm_add_transition(fsm, STATE_RUNNING, EVENT_PAUSE, STATE_PAUSED, NULL, NULL, &error);
    fsm_add_transition(fsm, STATE_PAUSED, EVENT_RESUME, STATE_RUNNING, NULL, NULL, &error);
    fsm_add_transition(fsm, STATE_RUNNING, EVENT_STOP, STATE_STOPPED, NULL, NULL, &error);
    fsm_add_transition(fsm, STATE_PAUSED, EVENT_STOP, STATE_STOPPED, NULL, NULL, &error);
    fsm_add_transition(fsm, STATE_STOPPED, EVENT_RESET, STATE_IDLE, NULL, NULL, &error);
    
    printf("模拟播放器状态机:\n\n");
    
    // 设置初始状态
    fsm_set_initial_state(fsm, STATE_IDLE, &error);
    
    // 启动状态机
    fsm_start(fsm, &error);
    printf("初始状态: %s\n\n", state_name(fsm->current_state));
    
    // 模拟状态转换
    printf("事件: START\n");
    fsm_handle_event(fsm, EVENT_START, &error);
    printf("当前状态: %s\n\n", state_name(fsm->current_state));
    
    printf("事件: PAUSE\n");
    fsm_handle_event(fsm, EVENT_PAUSE, &error);
    printf("当前状态: %s\n\n", state_name(fsm->current_state));
    
    printf("事件: RESUME\n");
    fsm_handle_event(fsm, EVENT_RESUME, &error);
    printf("当前状态: %s\n\n", state_name(fsm->current_state));
    
    printf("事件: STOP\n");
    fsm_handle_event(fsm, EVENT_STOP, &error);
    printf("当前状态: %s\n\n", state_name(fsm->current_state));
    
    printf("事件: RESET\n");
    fsm_handle_event(fsm, EVENT_RESET, &error);
    printf("当前状态: %s\n\n", state_name(fsm->current_state));
    
    fsm_free(fsm);
}

// 演示 3: 错误处理
static void demo_error_handling(void) {
    printf("\n=== 演示 3: 错误处理 ===\n");
    
    fsm_error_t error;
    fsm_options_t opts = fsm_default_options();
    
    fsm_t *fsm = fsm_create(NULL, &opts, &error);
    if (!fsm) {
        printf("创建失败: %d\n", error);
        return;
    }
    
    // 只添加一个状态
    fsm_add_state(fsm, STATE_IDLE, on_enter, on_exit, on_event, &error);
    
    // 设置初始状态
    fsm_set_initial_state(fsm, STATE_IDLE, &error);
    
    // 启动状态机
    fsm_start(fsm, &error);
    
    // 尝试发送未处理的事件
    printf("尝试发送未处理的事件...\n");
    error = FSM_OK;
    fsm_handle_event(fsm, EVENT_START, &error);
    printf("错误码: %d (应该是 %d - 事件未处理)\n", error, FSM_ERROR_EVENT_NOT_HANDLED);
    
    // 尝试添加无效状态
    printf("尝试添加无效状态...\n");
    error = FSM_OK;
    bool ok = fsm_add_state(NULL, STATE_RUNNING, on_enter, on_exit, on_event, &error);
    printf("结果: %s, 错误码: %d (应该是 %d - 无效参数)\n", ok ? "成功" : "失败", error, FSM_ERROR_INVALID_PARAM);
    
    fsm_free(fsm);
}

// 演示 4: 带条件的状态转换
static bool guard_condition(void *data, fsm_event_t event) {
    // 模拟条件检查
    static int count = 0;
    count++;
    bool allowed = (count % 2 == 0);
    printf("  条件检查: %s (count=%d)\n", allowed ? "允许" : "拒绝", count);
    return allowed;
}

static void transition_action(void *data, fsm_event_t event) {
    printf("  执行转换动作\n");
}

static void demo_guards_and_actions(void) {
    printf("\n=== 演示 4: 带条件的状态转换 ===\n");
    
    fsm_error_t error;
    fsm_options_t opts = fsm_default_options();
    opts.enable_guard = true;
    
    fsm_t *fsm = fsm_create(NULL, &opts, &error);
    if (!fsm) {
        printf("创建失败: %d\n", error);
        return;
    }
    
    fsm_add_state(fsm, STATE_IDLE, on_enter, on_exit, on_event, &error);
    fsm_add_state(fsm, STATE_RUNNING, on_enter, on_exit, on_event, &error);
    
    // 添加带条件的转换
    fsm_add_transition(fsm, STATE_IDLE, EVENT_START, STATE_RUNNING, guard_condition, transition_action, &error);
    
    // 设置初始状态
    fsm_set_initial_state(fsm, STATE_IDLE, &error);
    
    // 启动状态机
    fsm_start(fsm, &error);
    
    // 多次尝试转换
    for (int i = 1; i <= 5; i++) {
        printf("\n尝试 %d:\n", i);
        error = FSM_OK;
        fsm_handle_event(fsm, EVENT_START, &error);
        printf("结果: %s, 错误码: %d\n", 
               (fsm->current_state == STATE_RUNNING) ? "成功" : "失败", error);
        
        if (fsm->current_state == STATE_RUNNING) {
            // 重置状态
            fsm_stop(fsm, &error);
            fsm_set_initial_state(fsm, STATE_IDLE, &error);
            fsm_start(fsm, &error);
        }
    }
    
    fsm_free(fsm);
}

// 演示 5: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 5: 状态机应用场景 ===\n");
    
    printf("1. 网络连接管理\n");
    printf("   状态: DISCONNECTED -> CONNECTING -> CONNECTED -> DISCONNECTING\n");
    printf("   事件: connect, connected, disconnect, disconnected\n\n");
    
    printf("2. 订单处理系统\n");
    printf("   状态: CREATED -> PAID -> SHIPPED -> DELIVERED -> COMPLETED\n");
    printf("   事件: pay, ship, deliver, complete, cancel\n\n");
    
    printf("3. 游戏角色状态\n");
    printf("   状态: IDLE -> WALKING -> RUNNING -> JUMPING -> ATTACKING\n");
    printf("   事件: walk, run, jump, attack, stop\n\n");
    
    printf("4. 线程生命周期\n");
    printf("   状态: NEW -> RUNNABLE -> RUNNING -> BLOCKED -> TERMINATED\n");
    printf("   事件: start, run, block, unblock, terminate\n");
}

int main(void) {
    printf("========================================\n");
    printf("    有限状态机 (FSM) 演示\n");
    printf("========================================\n");
    
    demo_basic();
    demo_transitions();
    demo_error_handling();
    demo_guards_and_actions();
    demo_applications();
    
    printf("\n========================================\n");
    printf("演示完成!\n");
    
    return 0;
}
