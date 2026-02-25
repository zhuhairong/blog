/**
 * 线段树演示程序
 *
 * 功能：
 * - 区间查询
 * - 单点更新
 * - 延迟传播
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/segment_tree.h"

// 演示 1: 基本概念
static void demo_concept(void) {
    printf("\n=== 演示 1: 线段树基本概念 ===\n");

    printf("线段树:\n\n");

    printf("定义:\n");
    printf("  - 二叉树结构\n");
    printf("  - 每个节点表示一个区间\n");
    printf("  - 支持区间查询和更新\n\n");

    printf("时间复杂度:\n");
    printf("  建树: O(n)\n");
    printf("  查询: O(log n)\n");
    printf("  更新: O(log n)\n\n");

    printf("应用:\n");
    printf("  - 区间最值\n");
    printf("  - 区间求和\n");
    printf("  - 区间修改\n");
}

// 演示 2: 初始化
static void demo_init(void) {
    printf("\n=== 演示 2: 初始化 ===\n");

    printf("segment_tree_init 函数:\n");
    printf("  功能: 初始化线段树\n");
    printf("  参数: st - 线段树指针\n");
    printf("        arr - 输入数组\n");
    printf("        n - 数组大小\n\n");

    printf("segment_tree_init_ex 函数:\n");
    printf("  功能: 使用配置初始化\n");
    printf("  参数: config - 配置选项\n");
    printf("        error - 错误码\n");
}

// 演示 3: 查询操作
static void demo_query(void) {
    printf("\n=== 演示 3: 查询操作 ===\n");

    printf("segment_tree_query 函数:\n");
    printf("  功能: 区间查询\n");
    printf("  参数: st - 线段树\n");
    printf("        left - 左边界\n");
    printf("        right - 右边界\n");
    printf("  返回: 查询结果\n\n");

    printf("支持的查询:\n");
    printf("  - 最小值\n");
    printf("  - 最大值\n");
    printf("  - 区间和\n");
}

// 演示 4: 更新操作
static void demo_update(void) {
    printf("\n=== 演示 4: 更新操作 ===\n");

    printf("segment_tree_update 函数:\n");
    printf("  功能: 单点更新\n");
    printf("  参数: st - 线段树\n");
    printf("        idx - 索引\n");
    printf("        val - 新值\n\n");

    printf("segment_tree_range_update 函数:\n");
    printf("  功能: 区间更新\n");
    printf("  参数: st - 线段树\n");
    printf("        left - 左边界\n");
    printf("        right - 右边界\n");
    printf("        val - 更新值\n");
}

// 演示 5: 延迟传播
static void demo_lazy(void) {
    printf("\n=== 演示 5: 延迟传播 ===\n");

    printf("延迟传播:\n\n");

    printf("原理:\n");
    printf("  - 延迟执行更新操作\n");
    printf("  - 需要时才下推\n");
    printf("  - 提高效率\n\n");

    printf("优点:\n");
    printf("  - 区间更新: O(log n)\n");
    printf("  - 避免重复更新\n");
    printf("  - 减少操作次数\n\n");

    printf("配置:\n");
    printf("  use_lazy_propagation: 启用延迟传播\n");
}

// 演示 6: 配置选项
static void demo_config(void) {
    printf("\n=== 演示 6: 配置选项 ===\n");

    printf("segment_tree_config_t 结构:\n");
    printf("  operation: 操作类型\n");
    printf("    - SEGMENT_TREE_OP_MIN\n");
    printf("    - SEGMENT_TREE_OP_MAX_VAL\n");
    printf("    - SEGMENT_TREE_OP_SUM\n");
    printf("    - SEGMENT_TREE_OP_RANGE_UPDATE\n");
    printf("  use_optimized: 是否优化\n");
    printf("  check_bounds: 是否检查边界\n");
    printf("  use_lazy_propagation: 是否延迟传播\n");
    printf("  max_size: 最大大小\n");
    printf("  default_value: 默认值\n\n");

    printf("segment_tree_state_t 结构:\n");
    printf("  size: 原始数组大小\n");
    printf("  tree_size: 线段树大小\n");
    printf("  operation: 当前操作\n");
    printf("  initialized: 是否初始化\n");
}

// 演示 7: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 7: 应用场景 ===\n");

    printf("1. 区间最值查询 (RMQ)\n");
    printf("   - 最小值查询\n");
    printf("   - 最大值查询\n");
    printf("   - 区间统计\n\n");

    printf("2. 区间求和\n");
    printf("   - 前缀和\n");
    printf("   - 区间和\n");
    printf("   - 累积统计\n\n");

    printf("3. 动态规划\n");
    printf("   - 区间 DP\n");
    printf("   - 状态转移\n");
    printf("   - 优化计算\n\n");

    printf("4. 在线算法\n");
    printf("   - 实时查询\n");
    printf("   - 动态更新\n");
    printf("   - 流式处理\n\n");

    printf("5. 游戏开发\n");
    printf("   - 范围伤害\n");
    printf("   - 区域统计\n");
    printf("   - 地图查询\n");
}

int main(void) {
    printf("========================================\n");
    printf("    线段树演示\n");
    printf("========================================\n");

    demo_concept();
    demo_init();
    demo_query();
    demo_update();
    demo_lazy();
    demo_config();
    demo_applications();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
