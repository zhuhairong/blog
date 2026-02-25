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

// 演示 1: 基本状态机
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
    fsm_add_state(fsm, STATE_IDLE, on_enter, on_exit, NULL, &error);
    fsm_add_state(fsm, STATE_RUNNING, on_enter, on_exit, NULL, &error);
    fsm_add_state(fsm, STATE_PAUSED, on_enter, on_exit, NULL, &error);
    fsm_add_state(fsm, STATE_STOPPED, on_enter, on_exit, NULL, &error);
    
    printf("添加状态转换...\n");
    // IDLE -> RUNNING (START)
    // RUNNING -> PAUSED (PAUSE)
    // PAUSED -> RUNNING (RESUME)
    // RUNNING -> STOPPED (STOP)
    // PAUSED -> STOPPED (STOP)
    // STOPPED -> IDLE (RESET)
    
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
    
    printf("模拟播放器状态机:\n\n");
    
    const char *scenario[] = {
        "初始状态: IDLE",
        "事件: START -> 状态: RUNNING (开始播放)",
        "事件: PAUSE -> 状态: PAUSED (暂停播放)",
        "事件: RESUME -> 状态: RUNNING (恢复播放)",
        "事件: STOP -> 状态: STOPPED (停止播放)",
        "事件: RESET -> 状态: IDLE (重置)",
        NULL
    };
    
    for (int i = 0; scenario[i]; i++) {
        printf("  %s\n", scenario[i]);
    }
}

// 演示 3: 状态机应用场景
static void demo_applications(void) {
    printf("\n=== 演示 3: 状态机应用场景 ===\n");
    
    printf("1. 网络连接管理\n");
    printf("   状态: DISCONNECTED -> CONNECTING -> CONNECTED -> DISCONNECTING\n");
    printf("   事件: connect, connected, disconnect, disconnected\n");
    printf("\n");
    
    printf("2. 订单处理系统\n");
    printf("   状态: CREATED -> PAID -> SHIPPED -> DELIVERED -> COMPLETED\n");
    printf("   事件: pay, ship, deliver, complete, cancel\n");
    printf("\n");
    
    printf("3. 游戏角色状态\n");
    printf("   状态: IDLE -> WALKING -> RUNNING -> JUMPING -> ATTACKING\n");
    printf("   事件: walk, run, jump, attack, stop\n");
    printf("\n");
    
    printf("4. 线程生命周期\n");
    printf("   状态: NEW -> RUNNABLE -> RUNNING -> BLOCKED -> TERMINATED\n");
    printf("   事件: start, run, block, unblock, terminate\n");
}

// 演示 4: FSM 优势
static void demo_advantages(void) {
    printf("\n=== 演示 4: FSM 优势 ===\n");
    
    printf("使用 FSM 的好处:\n\n");
    
    printf("1. 代码结构清晰\n");
    printf("   - 状态和行为分离\n");
    printf("   - 易于理解和维护\n");
    printf("\n");
    
    printf("2. 避免复杂条件判断\n");
    printf("   - 消除大量 if-else/switch\n");
    printf("   - 状态转换集中管理\n");
    printf("\n");
    
    printf("3. 易于扩展\n");
    printf("   - 添加新状态简单\n");
    printf("   - 添加新转换简单\n");
    printf("\n");
    
    printf("4. 可测试性强\n");
    printf("   - 每个状态独立测试\n");
    printf("   - 转换路径清晰\n");
    printf("\n");
    
    printf("5. 可视化支持\n");
    printf("   - 可生成状态图\n");
    printf("   - 便于文档化\n");
}

// 演示 5: 实现模式
static void demo_patterns(void) {
    printf("\n=== 演示 5: FSM 实现模式 ===\n");
    
    printf("1.  switch/case 模式 (简单)\n");
    printf("   switch (state) {\n");
    printf("     case STATE_A: ...\n");
    printf("     case STATE_B: ...\n");
    printf("   }\n");
    printf("\n");
    
    printf("2.  状态表模式 (中等)\n");
    printf("   transition_table[state][event] = next_state;\n");
    printf("\n");
    
    printf("3.  面向对象模式 (复杂)\n");
    printf("   class State { virtual void handle() = 0; };\n");
    printf("   class StateA : public State { ... };\n");
    printf("\n");
    
    printf("4.  本库模式 (推荐)\n");
    printf("   - 声明式配置\n");
    printf("   - 回调函数支持\n");
    printf("   - 守卫条件支持\n");
    printf("   - 嵌套状态支持\n");
}

int main(void) {
    printf("========================================\n");
    printf("    有限状态机 (FSM) 演示\n");
    printf("========================================\n");
    
    demo_basic();
    demo_transitions();
    demo_applications();
    demo_advantages();
    demo_patterns();
    
    printf("\n========================================\n");
    printf("演示完成!\n");
    
    return 0;
}
