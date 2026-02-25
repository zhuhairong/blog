/**
 * 红黑树演示程序
 *
 * 功能：
 * - 自平衡二叉搜索树
 * - 插入、删除、查找
 * - 遍历操作
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/rbtree.h"

// 演示 1: 基本概念
static void demo_concept(void) {
    printf("\n=== 演示 1: 红黑树基本概念 ===\n");

    printf("红黑树:\n\n");

    printf("性质:\n");
    printf("  1. 节点是红色或黑色\n");
    printf("  2. 根节点是黑色\n");
    printf("  3. 所有叶子（NIL）是黑色\n");
    printf("  4. 红色节点的子节点是黑色\n");
    printf("  5. 从任一节点到叶子的路径包含相同数目的黑色节点\n\n");

    printf("时间复杂度:\n");
    printf("  查找: O(log n)\n");
    printf("  插入: O(log n)\n");
    printf("  删除: O(log n)\n\n");

    printf("优点:\n");
    printf("  - 自平衡，保证性能\n");
    printf("  - 最坏情况也是 O(log n)\n");
    printf("  - 实际应用广泛\n");
}

// 演示 2: 基本操作
static void demo_basic(void) {
    printf("\n=== 演示 2: 基本操作 ===\n");

    printf("rbtree_create 函数:\n");
    printf("  功能: 创建红黑树\n");
    printf("  参数: compare - 键比较函数\n");
    printf("  返回: 红黑树指针\n\n");

    printf("rbtree_insert 函数:\n");
    printf("  功能: 插入键值对\n");
    printf("  参数: tree - 红黑树\n");
    printf("        key - 键\n");
    printf("        value - 值\n\n");

    printf("rbtree_find 函数:\n");
    printf("  功能: 查找键对应的值\n");
    printf("  参数: tree - 红黑树\n");
    printf("        key - 键\n");
    printf("  返回: 值指针\n\n");

    printf("rbtree_delete 函数:\n");
    printf("  功能: 删除键值对\n");
    printf("  参数: tree - 红黑树\n");
    printf("        key - 键\n");
}

// 演示 3: 遍历操作
static void demo_traversal(void) {
    printf("\n=== 演示 3: 遍历操作 ===\n");

    printf("遍历方式:\n\n");

    printf("rbtree_inorder:\n");
    printf("  中序遍历（左-根-右）\n");
    printf("  结果: 按键排序顺序\n\n");

    printf("rbtree_preorder:\n");
    printf("  前序遍历（根-左-右）\n");
    printf("  结果: 树结构展示\n\n");

    printf("rbtree_postorder:\n");
    printf("  后序遍历（左-右-根）\n");
    printf("  结果: 用于释放资源\n\n");

    printf("rbtree_levelorder:\n");
    printf("  层序遍历（按层）\n");
    printf("  结果: 广度优先\n");
}

// 演示 4: 统计信息
static void demo_stats(void) {
    printf("\n=== 演示 4: 统计信息 ===\n");

    printf("rbtree_size 函数:\n");
    printf("  功能: 获取节点数量\n");
    printf("  返回: 节点数\n\n");

    printf("rbtree_height 函数:\n");
    printf("  功能: 获取树高度\n");
    printf("  返回: 高度\n\n");

    printf("rbtree_is_empty 函数:\n");
    printf("  功能: 检查是否为空\n");
    printf("  返回: 是否为空\n\n");

    printf("红黑树高度:\n");
    printf("  最大高度: 2 * log2(n+1)\n");
    printf("  保证平衡\n");
}

// 演示 5: 配置选项
static void demo_config(void) {
    printf("\n=== 演示 5: 配置选项 ===\n");

    printf("rbtree_config_t 结构:\n");
    printf("  allow_duplicates: 是否允许重复键\n");
    printf("  free_keys: 释放时是否释放键\n");
    printf("  free_values: 释放时是否释放值\n");
    printf("  key_size: 键大小（可选）\n");
    printf("  value_size: 值大小（可选）\n\n");

    printf("rbtree_iterator_t 结构:\n");
    printf("  用于遍历红黑树\n");
    printf("  支持正向和反向遍历\n");
}

// 演示 6: 错误处理
static void demo_errors(void) {
    printf("\n=== 演示 6: 错误处理 ===\n");

    printf("可能的错误码:\n");
    printf("  RBTREE_OK - 成功\n");
    printf("  RBTREE_ERROR_NULL_PTR - 空指针\n");
    printf("  RBTREE_ERROR_INVALID_ARGS - 无效参数\n");
    printf("  RBTREE_ERROR_KEY_NOT_FOUND - 键不存在\n");
    printf("  RBTREE_ERROR_KEY_EXISTS - 键已存在\n");
    printf("  RBTREE_ERROR_OUT_OF_MEMORY - 内存不足\n");

    printf("\n注意事项:\n");
    printf("  - 比较函数必须定义全序\n");
    printf("  - 键的生命周期管理\n");
    printf("  - 注意内存泄漏\n");
}

// 演示 7: 应用场景
static void demo_applications(void) {
    printf("\n=== 演示 7: 应用场景 ===\n");

    printf("1. 关联数组\n");
    printf("   - 键值对存储\n");
    printf("   - 快速查找\n");
    printf("   - 有序遍历\n\n");

    printf("2. 内存管理\n");
    printf("   - 内存分配器\n");
    printf("   - 空闲块管理\n");
    printf("   - 地址映射\n\n");

    printf("3. 数据库索引\n");
    printf("   - 主键索引\n");
    printf("   - 范围查询\n");
    printf("   - 排序输出\n\n");

    printf("4. 编译器\n");
    printf("   - 符号表\n");
    printf("   - 作用域管理\n");
    printf("   - 类型检查\n\n");

    printf("5. 网络协议\n");
    printf("   - 连接管理\n");
    printf("   - 超时处理\n");
    printf("   - 会话跟踪\n");
}

int main(void) {
    printf("========================================\n");
    printf("    红黑树演示\n");
    printf("========================================\n");

    demo_concept();
    demo_basic();
    demo_traversal();
    demo_stats();
    demo_config();
    demo_errors();
    demo_applications();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
