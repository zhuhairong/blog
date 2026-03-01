#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include "../c_utils/signal_utils.h"

// 自定义信号处理函数
void custom_signal_handler(int signum, siginfo_t *info, void *context) {
    printf("   收到信号: %s (%d)\n", signal_get_name(signum), signum);
    printf("   信号描述: %s\n", signal_get_description(signum));
}

int main() {
    printf("=== Signal Utils Demo ===\n\n");
    
    // 初始化信号处理
    printf("1. 初始化信号处理:\n");
    signal_config_t config;
    signal_state_t state;
    signal_config_init(&config);
    signal_state_init(&state);
    
    // 配置捕获的信号
    config.catch_sigint = true;
    config.catch_sigterm = true;
    config.catch_sigquit = true;
    
    signal_error_t error = signal_init_ex(&config, &state);
    if (error == SIGNAL_OK) {
        printf("   信号处理初始化成功\n");
    } else {
        printf("   信号处理初始化失败: %s\n", signal_strerror(&state));
        return 1;
    }
    
    // 注册自定义信号处理器
    printf("\n2. 注册自定义信号处理器:\n");
    error = signal_register_handler(SIGINT, custom_signal_handler, &state);
    if (error == SIGNAL_OK) {
        printf("   成功注册 SIGINT 信号处理器\n");
    } else {
        printf("   注册 SIGINT 信号处理器失败: %s\n", signal_strerror(&state));
    }
    
    // 测试信号名称和描述
    printf("\n3. 信号信息:\n");
    int test_signals[] = {SIGINT, SIGTERM, SIGQUIT, SIGPIPE};
    size_t num_signals = sizeof(test_signals) / sizeof(test_signals[0]);
    
    for (size_t i = 0; i < num_signals; i++) {
        int signum = test_signals[i];
        printf("   信号 %d: %s - %s\n", signum, signal_get_name(signum), signal_get_description(signum));
    }
    
    // 测试阻塞和解除阻塞信号
    printf("\n4. 测试信号阻塞:\n");
    error = signal_block(SIGINT, &state);
    if (error == SIGNAL_OK) {
        printf("   成功阻塞 SIGINT 信号\n");
    } else {
        printf("   阻塞 SIGINT 信号失败: %s\n", signal_strerror(&state));
    }
    
    // 解除阻塞
    error = signal_unblock(SIGINT, &state);
    if (error == SIGNAL_OK) {
        printf("   成功解除阻塞 SIGINT 信号\n");
    } else {
        printf("   解除阻塞 SIGINT 信号失败: %s\n", signal_strerror(&state));
    }
    
    // 测试忽略信号
    printf("\n5. 测试忽略信号:\n");
    error = signal_ignore(SIGPIPE, &state);
    if (error == SIGNAL_OK) {
        printf("   成功忽略 SIGPIPE 信号\n");
    } else {
        printf("   忽略 SIGPIPE 信号失败: %s\n", signal_strerror(&state));
    }
    
    // 恢复默认信号处理
    error = signal_restore_default(SIGPIPE, &state);
    if (error == SIGNAL_OK) {
        printf("   成功恢复 SIGPIPE 信号默认处理\n");
    } else {
        printf("   恢复 SIGPIPE 信号默认处理失败: %s\n", signal_strerror(&state));
    }
    
    // 测试发送信号
    printf("\n6. 测试发送信号:\n");
    pid_t pid = getpid();
    printf("   当前进程 ID: %d\n", pid);
    
    error = signal_send(pid, SIGUSR1, &state);
    if (error == SIGNAL_OK) {
        printf("   成功发送 SIGUSR1 信号到当前进程\n");
    } else {
        printf("   发送 SIGUSR1 信号失败: %s\n", signal_strerror(&state));
    }
    
    // 测试退出请求状态
    printf("\n7. 测试退出请求状态:\n");
    bool exit_requested;
    error = signal_get_exit_status(&exit_requested, &state);
    if (error == SIGNAL_OK) {
        printf("   退出请求状态: %s\n", exit_requested ? "已请求" : "未请求");
    } else {
        printf("   获取退出请求状态失败: %s\n", signal_strerror(&state));
    }
    
    // 测试基本退出处理器
    printf("\n8. 测试基本退出处理器:\n");
    signal_init_exit_handler();
    printf("   基本退出处理器初始化成功\n");
    
    // 测试信号计数
    printf("\n9. 信号计数:\n");
    printf("   信号计数: %u\n", state.signal_count);
    
    // 测试重置信号状态
    printf("\n10. 重置信号状态:\n");
    error = signal_reset(&state);
    if (error == SIGNAL_OK) {
        printf("   成功重置信号状态\n");
    } else {
        printf("   重置信号状态失败: %s\n", signal_strerror(&state));
    }
    
    // 测试禁用所有信号处理
    printf("\n11. 禁用所有信号处理:\n");
    error = signal_disable_all(&state);
    if (error == SIGNAL_OK) {
        printf("   成功禁用所有信号处理\n");
    } else {
        printf("   禁用所有信号处理失败: %s\n", signal_strerror(&state));
    }
    
    printf("\n=== Signal Utils Demo 完成 ===\n");
    printf("   注意: 此 demo 展示了信号处理的基本功能，\n");
    printf("   实际使用中可能需要更复杂的信号处理逻辑。\n");
    return 0;
}