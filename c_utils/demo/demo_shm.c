/**
 * 共享内存演示程序
 *
 * 功能：
 * - 进程间共享内存
 * - 快速数据交换
 * - 同步机制
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/shm.h"

// 演示 1: 基本概念
static void demo_concept(void) {
    printf("\n=== 演示 1: 共享内存基本概念 ===\n");

    printf("共享内存:\n\n");

    printf("原理:\n");
    printf("  - 多个进程访问同一块物理内存\n");
    printf("  - 最快的 IPC 方式\n");
    printf("  - 需要同步机制\n\n");

    printf("优点:\n");
    printf("  - 速度最快\n");
    printf("  - 无需数据拷贝\n");
    printf("  - 大容量传输\n\n");

    printf("注意:\n");
    printf("  - 需要同步（信号量/互斥锁）\n");
    printf("  - 生命周期管理\n");
    printf("  - 权限控制\n");
}

// 演示 2: 创建和打开
static void demo_open(void) {
    printf("\n=== 演示 2: 创建和打开 ===\n");

    printf("shm_open_map 函数:\n");
    printf("  功能: 创建或打开共享内存\n");
    printf("  参数: name - 共享内存名称\n");
    printf("        size - 大小\n");
    printf("  返回: 内存指针\n\n");

    printf("shm_open_map_ex 函数:\n");
    printf("  功能: 增强版创建/打开\n");
    printf("  参数: config - 配置选项\n");
    printf("        state - 状态输出\n\n");

    printf("shm_create 函数:\n");
    printf("  功能: 创建新的共享内存\n");
}

// 演示 3: 关闭和删除
static void demo_close(void) {
    printf("\n=== 演示 3: 关闭和删除 ===\n");

    printf("shm_close_unmap 函数:\n");
    printf("  功能: 解除映射并关闭\n");
    printf("  参数: ptr - 内存指针\n");
    printf("        name - 共享内存名称\n");
    printf("        size - 大小\n\n");

    printf("shm_close_unmap_ex 函数:\n");
    printf("  功能: 增强版关闭\n");
    printf("  返回: 错误码\n\n");

    printf("shm_unlink 函数:\n");
    printf("  功能: 删除共享内存对象\n");
}

// 演示 4: 配置选项
static void demo_config(void) {
    printf("\n=== 演示 4: 配置选项 ===\n");

    printf("shm_config_t 结构:\n");
    printf("  create_if_not_exists: 不存在则创建\n");
    printf("  exclusive: 独占模式\n");
    printf("  unlink_on_close: 关闭时删除\n");
    printf("  read_only: 只读模式\n");
    printf("  permissions: 权限模式\n");
    printf("  min_size: 最小大小\n");
    printf("  max_size: 最大大小\n\n");

    printf("权限模式:\n");
    printf("  0644: 所有者可读写，其他只读\n");
    printf("  0666: 所有用户可读写\n");
    printf("  0600: 仅所有者可访问\n");
}

// 演示 5: 状态查询
static void demo_state(void) {
    printf("\n=== 演示 5: 状态查询 ===\n");

    printf("shm_state_t 结构:\n");
    printf("  last_error: 最后错误\n");
    printf("  error_code: 系统错误码\n");
    printf("  actual_size: 实际大小\n");
    printf("  is_mapped: 是否已映射\n");
    printf("  is_read_only: 是否只读\n");
    printf("  is_created: 是否新创建\n\n");

    printf("shm_strerror 函数:\n");
    printf("  功能: 获取错误描述\n");
}

// 演示 6: 错误处理
static void demo_errors(void) {
    printf("\n=== 演示 6: 错误处理 ===\n");

    printf("可能的错误码:\n");
    printf("  SHM_OK - 成功\n");
    printf("  SHM_ERROR_INVALID_PARAMS - 无效参数\n");
    printf("  SHM_ERROR_NAME_NULL - 名称为空\n");
    printf("  SHM_ERROR_SIZE_ZERO - 大小为零\n");
    printf("  SHM_ERROR_OPEN - 打开失败\n");
    printf("  SHM_ERROR_FTRUNCATE - 截断失败\n");
    printf("  SHM_ERROR_MAP - 映射失败\n");
    printf("  SHM_ERROR_PERMISSION - 权限错误\n");
    printf("  SHM_ERROR_EXISTS - 已存在\n");
    printf("  SHM_ERROR_NOT_FOUND - 未找到\n");

    printf("\n调试建议:\n");
    printf("  - 检查 /dev/shm 目录\n");
    printf("  - 验证权限设置\n");
    printf("  - 查看系统限制\n");
}

// 演示 7: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 7: 应用场景 ===\n");

    printf("1. 高速数据交换\n");
    printf("   - 大容量数据传输\n");
    printf("   - 实时数据共享\n");
    printf("   - 零拷贝通信\n\n");

    printf("2. 进程池\n");
    printf("   - 工作队列\n");
    printf("   - 结果收集\n");
    printf("   - 状态共享\n\n");

    printf("3. 数据库缓存\n");
    printf("   - 查询缓存\n");
    printf("   - 连接池\n");
    printf("   - 会话存储\n\n");

    printf("4. 多媒体处理\n");
    printf("   - 视频帧共享\n");
    printf("   - 音频缓冲\n");
    printf("   - 图像处理\n\n");

    printf("5. 系统监控\n");
    printf("   - 性能统计\n");
    printf("   - 日志共享\n");
    printf("   - 配置同步\n");
}

int main(void) {
    printf("========================================\n");
    printf("    共享内存演示\n");
    printf("========================================\n");

    demo_concept();
    demo_open();
    demo_close();
    demo_config();
    demo_state();
    demo_errors();
    demo_applications();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
