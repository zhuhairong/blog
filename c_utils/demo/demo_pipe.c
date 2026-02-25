/**
 * 管道演示程序
 *
 * 功能：
 * - 匿名管道
 * - 命名管道
 * - 进程间通信
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../c_utils/pipe.h"

// 演示 1: 匿名管道
static void demo_anonymous(void) {
    printf("\n=== 演示 1: 匿名管道 ===\n");

    pipe_t pipe;

    printf("创建匿名管道...\n");

    if (pipe_create(&pipe)) {
        printf("管道创建成功!\n");
        printf("  读端: %d\n", (int)pipe.read_fd);
        printf("  写端: %d\n", (int)pipe.write_fd);

        // 写入数据
        const char *msg = "Hello from pipe!";
        printf("\n写入数据: \"%s\"\n", msg);

        pipe_error_t error;
        size_t bytes_written;
        if (pipe_write(&pipe, msg, strlen(msg), &bytes_written, &error)) {
            printf("  写入 %zu 字节\n", bytes_written);
        }

        // 读取数据
        char buffer[256];
        size_t bytes_read;
        if (pipe_read(&pipe, buffer, sizeof(buffer) - 1, &bytes_read, &error)) {
            buffer[bytes_read] = '\0';
            printf("  读取数据: \"%s\"\n", buffer);
        }

        pipe_close(&pipe);
        printf("\n管道已关闭\n");
    } else {
        printf("管道创建失败\n");
    }
}

// 演示 2: 管道配置
static void demo_config(void) {
    printf("\n=== 演示 2: 管道配置 ===\n");

    pipe_config_t config = pipe_default_config();

    printf("默认配置:\n");
    printf("  类型: %s\n", config.type == PIPE_TYPE_ANONYMOUS ? "匿名" : "命名");
    printf("  名称: %s\n", config.name ? config.name : "(无)");
    printf("  非阻塞: %s\n", config.non_blocking ? "是" : "否");
    printf("  缓冲区大小: %zu\n", config.buffer_size);
    printf("  超时: %d ms\n", config.timeout_ms);

    printf("\n配置说明:\n");
    printf("  - 匿名管道: 用于父子进程通信\n");
    printf("  - 命名管道: 用于无关进程通信\n");
    printf("  - 非阻塞: 读写立即返回\n");
    printf("  - 超时: 阻塞模式下的最大等待时间\n");
}

// 演示 3: 进程间通信示例
static void demo_ipc(void) {
    printf("\n=== 演示 3: 进程间通信示例 ===\n");

    printf("父进程与子进程通信:\n\n");

    printf("父进程流程:\n");
    printf("  1. 创建管道\n");
    printf("  2. fork() 创建子进程\n");
    printf("  3. 关闭不用的端\n");
    printf("  4. 读写数据\n");
    printf("  5. 等待子进程结束\n\n");

    printf("子进程流程:\n");
    printf("  1. 继承管道\n");
    printf("  2. 关闭不用的端\n");
    printf("  3. 读写数据\n");
    printf("  4. 退出\n\n");

    printf("注意事项:\n");
    printf("  - 及时关闭不用的端\n");
    printf("  - 处理 EOF 情况\n");
    printf("  - 注意缓冲区大小\n");
}

// 演示 4: 命名管道
static void demo_named(void) {
    printf("\n=== 演示 4: 命名管道 (FIFO) ===\n");

    const char *pipename = "/tmp/demo_pipe";

    printf("命名管道特点:\n");
    printf("  - 有文件系统路径\n");
    printf("  - 无关进程可以通信\n");
    printf("  - 持久存在直到删除\n\n");

    printf("创建命名管道: %s\n", pipename);

    pipe_t pipe;
    pipe_error_t error;

    if (pipe_create_named(&pipe, pipename, &error)) {
        printf("命名管道创建成功!\n");

        // 写入数据
        const char *msg = "Hello from named pipe!";
        size_t bytes_written;
        pipe_write(&pipe, msg, strlen(msg), &bytes_written, &error);

        printf("数据已写入\n");

        pipe_close(&pipe);

        // 清理
        unlink(pipename);
    } else {
        printf("创建失败: %d\n", error);
    }
}

// 演示 5: 错误处理
static void demo_errors(void) {
    printf("\n=== 演示 5: 错误处理 ===\n");

    printf("可能的错误码:\n");
    printf("  PIPE_OK - 成功\n");
    printf("  PIPE_ERROR_NULL_PTR - 空指针\n");
    printf("  PIPE_ERROR_CREATE_FAILED - 创建失败\n");
    printf("  PIPE_ERROR_READ_FAILED - 读取失败\n");
    printf("  PIPE_ERROR_WRITE_FAILED - 写入失败\n");
    printf("  PIPE_ERROR_CLOSE_FAILED - 关闭失败\n");
    printf("  PIPE_ERROR_INVALID_HANDLE - 无效句柄\n");
    printf("  PIPE_ERROR_TIMEOUT - 超时\n");

    printf("\n常见错误场景:\n");
    printf("  - 读取端关闭后写入 -> SIGPIPE\n");
    printf("  - 管道满时写入 -> 阻塞或失败\n");
    printf("  - 空管道读取 -> 阻塞或返回 0\n");
}

// 演示 6: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 6: 应用场景 ===\n");

    printf("1. Shell 管道\n");
    printf("   - cmd1 | cmd2 | cmd3\n");
    printf("   - 进程间数据传递\n\n");

    printf("2. 父子进程通信\n");
    printf("   - 向子进程发送数据\n");
    printf("   - 从子进程接收结果\n\n");

    printf("3. 生产者-消费者\n");
    printf("   - 多生产者单消费者\n");
    printf("   - 数据流处理\n\n");

    printf("4. 日志系统\n");
    printf("   - 日志收集\n");
    printf("   - 进程间日志传输\n\n");

    printf("5. 进程池\n");
    printf("   - 任务分发\n");
    printf("   - 结果收集\n");
}

// 演示 7: 与消息队列对比
static void demo_comparison(void) {
    printf("\n=== 演示 7: 管道 vs 其他 IPC ===\n");

    printf("管道特点:\n");
    printf("  - 单向数据流\n");
    printf("  - 字节流，无消息边界\n");
    printf("  - 内核缓冲区\n");
    printf("  - 简单高效\n\n");

    printf("消息队列:\n");
    printf("  - 保留消息边界\n");
    printf("  - 可以双向\n");
    printf("  - 更复杂的 API\n\n");

    printf("共享内存:\n");
    printf("  - 最快的 IPC\n");
    printf("  - 需要同步机制\n");
    printf("  - 更复杂\n\n");

    printf("Socket:\n");
    printf("  - 网络透明\n");
    printf("  - 双向通信\n");
    printf("  - 开销较大\n");
}

int main(void) {
    printf("========================================\n");
    printf("    管道演示\n");
    printf("========================================\n");

    demo_anonymous();
    demo_config();
    demo_ipc();
    demo_named();
    demo_errors();
    demo_applications();
    demo_comparison();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
