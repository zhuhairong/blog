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
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "../c_utils/pipe.h"

static void demo_anonymous(void) {
    printf("\n=== 演示 1: 匿名管道 ===\n");

    pipe_t pipe;
    pipe_error_t error;

    printf("创建匿名管道...\n");

    if (pipe_create(&pipe)) {
        printf("管道创建成功!\n");
        printf("  读端: %d\n", (int)pipe.read_fd);
        printf("  写端: %d\n", (int)pipe.write_fd);

        const char *msg = "Hello from pipe!";
        printf("\n写入数据: \"%s\"\n", msg);

        size_t bytes_written;
        if (pipe_write(&pipe, msg, strlen(msg), &bytes_written, &error)) {
            printf("  写入 %zu 字节\n", bytes_written);
        }

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

static void demo_config(void) {
    printf("\n=== 演示 2: 管道配置 ===\n");

    pipe_config_t config = pipe_default_config();

    printf("默认配置:\n");
    printf("  类型: %s\n", config.type == PIPE_TYPE_ANONYMOUS ? "匿名" : "命名");
    printf("  名称: %s\n", config.name ? config.name : "(无)");
    printf("  非阻塞: %s\n", config.non_blocking ? "是" : "否");
    printf("  缓冲区大小: %zu\n", config.buffer_size);
    printf("  超时: %d ms\n", config.timeout_ms);

    printf("\n自定义配置:\n");
    config.non_blocking = true;
    config.buffer_size = 8192;
    config.timeout_ms = 5000;
    printf("  非阻塞: %s\n", config.non_blocking ? "是" : "否");
    printf("  缓冲区大小: %zu\n", config.buffer_size);
    printf("  超时: %d ms\n", config.timeout_ms);
}

static void demo_fork_ipc(void) {
    printf("\n=== 演示 3: 父子进程通信 (fork) ===\n");

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return;
    }

    printf("创建管道成功! pipefd[0]=%d, pipefd[1]=%d\n", pipefd[0], pipefd[1]);

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        close(pipefd[0]);
        close(pipefd[1]);
        return;
    }

    if (pid == 0) {
        close(pipefd[0]);

        const char *msg = "Hello from child process!";
        write(pipefd[1], msg, strlen(msg));
        printf("子进程: 发送数据 \"%s\"\n", msg);

        close(pipefd[1]);
        exit(0);
    } else {
        close(pipefd[1]);

        char buffer[256];
        ssize_t n = read(pipefd[0], buffer, sizeof(buffer) - 1);
        if (n > 0) {
            buffer[n] = '\0';
            printf("父进程: 收到数据 \"%s\"\n", buffer);
        }

        close(pipefd[0]);
        wait(NULL);
        printf("父子进程通信完成!\n");
    }
}

static void demo_child_to_parent(void) {
    printf("\n=== 演示 4: 子进程计算结果传回父进程 ===\n");

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        close(pipefd[0]);
        close(pipefd[1]);
        return;
    }

    if (pid == 0) {
        close(pipefd[0]);

        int result = 0;
        for (int i = 1; i <= 100; i++) {
            result += i;
        }

        char buffer[128];
        snprintf(buffer, sizeof(buffer), "1+2+3+...+100 = %d", result);
        write(pipefd[1], buffer, strlen(buffer));
        printf("子进程: 计算 1+2+3+...+100 = %d\n", result);

        close(pipefd[1]);
        exit(0);
    } else {
        close(pipefd[1]);

        char buffer[256];
        ssize_t n = read(pipefd[0], buffer, sizeof(buffer) - 1);
        if (n > 0) {
            buffer[n] = '\0';
            printf("父进程: 收到结果 \"%s\"\n", buffer);
        }

        close(pipefd[0]);
        wait(NULL);
    }
}

static void demo_two_way(void) {
    printf("\n=== 演示 5: 双向通信 (两个管道) ===\n");

    int pipe_parent_to_child[2];
    int pipe_child_to_parent[2];

    if (pipe(pipe_parent_to_child) == -1 || pipe(pipe_child_to_parent) == -1) {
        perror("pipe");
        return;
    }

    printf("创建两个管道实现双向通信\n");

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return;
    }

    if (pid == 0) {
        close(pipe_parent_to_child[1]);
        close(pipe_child_to_parent[0]);

        char buffer[256];
        ssize_t n = read(pipe_parent_to_child[0], buffer, sizeof(buffer) - 1);
        if (n > 0) {
            buffer[n] = '\0';
            printf("子进程: 收到 \"%s\"\n", buffer);
        }
        close(pipe_parent_to_child[0]);

        const char *reply = "Child received!";
        write(pipe_child_to_parent[1], reply, strlen(reply));
        printf("子进程: 回复 \"%s\"\n", reply);

        close(pipe_child_to_parent[1]);
        exit(0);
    } else {
        close(pipe_parent_to_child[0]);
        close(pipe_child_to_parent[1]);

        const char *msg = "Hello from parent!";
        write(pipe_parent_to_child[1], msg, strlen(msg));
        printf("父进程: 发送 \"%s\"\n", msg);
        close(pipe_parent_to_child[1]);

        char buffer[256];
        ssize_t n = read(pipe_child_to_parent[0], buffer, sizeof(buffer) - 1);
        if (n > 0) {
            buffer[n] = '\0';
            printf("父进程: 收到回复 \"%s\"\n", buffer);
        }
        close(pipe_child_to_parent[0]);

        wait(NULL);
        printf("双向通信完成!\n");
    }
}

static void demo_named_pipe(void) {
    printf("\n=== 演示 6: 命名管道 (FIFO) ===\n");

    const char *fifo_path = "/tmp/demo_pipe_test";

    unlink(fifo_path);

    if (mkfifo(fifo_path, 0666) == -1) {
        perror("mkfifo");
        return;
    }
    printf("创建命名管道: %s\n", fifo_path);

    pid_t pid = fork();
    if (pid == 0) {
        int fd = open(fifo_path, O_WRONLY);
        if (fd >= 0) {
            const char *msg = "Message via named pipe!";
            write(fd, msg, strlen(msg));
            printf("子进程: 通过命名管道发送 \"%s\"\n", msg);
            close(fd);
        }
        exit(0);
    } else {
        int fd = open(fifo_path, O_RDONLY);
        if (fd >= 0) {
            char buffer[256];
            ssize_t n = read(fd, buffer, sizeof(buffer) - 1);
            if (n > 0) {
                buffer[n] = '\0';
                printf("父进程: 通过命名管道收到 \"%s\"\n", buffer);
            }
            close(fd);
        }
        wait(NULL);
    }

    unlink(fifo_path);
    printf("命名管道通信完成，已清理\n");
}

static void demo_error_handling(void) {
    printf("\n=== 演示 7: 错误处理 ===\n");

    pipe_t pipe;
    pipe_error_t error;

    printf("测试 1: 空指针错误\n");
    if (pipe_create(NULL)) {
        printf("  意外成功\n");
    } else {
        printf("  正确捕获空指针错误\n");
    }

    printf("\n测试 2: 读取关闭的管道\n");
    char buffer[64];
    size_t bytes_read;

    pipe_create(&pipe);
    close(pipe.read_fd);

    if (pipe_read(&pipe, buffer, sizeof(buffer), &bytes_read, &error)) {
        printf("  意外成功\n");
    } else {
        printf("  正确捕获读取错误 (error=%d)\n", error);
    }

    pipe_close(&pipe);

    printf("\n测试 3: 写入已关闭的写端\n");
    pipe_create(&pipe);
    close(pipe.write_fd);

    if (pipe_write(&pipe, "test", 4, NULL, &error)) {
        printf("  意外成功\n");
    } else {
        printf("  正确捕获写入错误 (error=%d)\n", error);
    }

    pipe_close(&pipe);

    printf("\n错误处理演示完成!\n");
}

static void demo_pipe_buffer(void) {
    printf("\n=== 演示 8: 管道缓冲区测试 ===\n");

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return;
    }

    int flags = fcntl(pipefd[1], F_GETFL, 0);
    fcntl(pipefd[1], F_SETFL, flags | O_NONBLOCK);

    printf("测试管道缓冲区大小 (非阻塞写入)...\n");

    char buffer[4096];
    memset(buffer, 'A', sizeof(buffer));

    size_t total_written = 0;
    ssize_t n;

    while ((n = write(pipefd[1], buffer, sizeof(buffer))) > 0) {
        total_written += n;
    }

    printf("管道已满，总写入: %zu 字节\n", total_written);

    close(pipefd[0]);
    close(pipefd[1]);
    printf("缓冲区测试完成!\n");
}

static void demo_non_blocking(void) {
    printf("\n=== 演示 9: 非阻塞模式 ===\n");

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return;
    }

    int flags = fcntl(pipefd[0], F_GETFL, 0);
    fcntl(pipefd[0], F_SETFL, flags | O_NONBLOCK);
    flags = fcntl(pipefd[1], F_GETFL, 0);
    fcntl(pipefd[1], F_SETFL, flags | O_NONBLOCK);

    printf("设置管道为非阻塞模式\n");

    char buffer[64];
    ssize_t n = read(pipefd[0], buffer, sizeof(buffer));
    if (n == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            printf("读取空管道返回 EAGAIN (无数据)\n");
        }
    } else {
        printf("读取: %zd\n", n);
    }

    const char *msg = "test";
    n = write(pipefd[1], msg, strlen(msg));
    printf("写入数据: %zd 字节\n", n);

    n = read(pipefd[0], buffer, sizeof(buffer));
    if (n > 0) {
        buffer[n] = '\0';
        printf("读取数据: \"%s\"\n", buffer);
    }

    close(pipefd[0]);
    close(pipefd[1]);
    printf("非阻塞模式测试完成!\n");
}

int main(void) {
    printf("========================================\n");
    printf("    管道演示\n");
    printf("========================================\n");

    demo_anonymous();
    demo_config();
    demo_fork_ipc();
    demo_child_to_parent();
    demo_two_way();
    demo_named_pipe();
    demo_error_handling();
    demo_pipe_buffer();
    demo_non_blocking();

    printf("\n========================================\n");
    printf("演示完成!\n");
    printf("========================================\n");

    return 0;
}
