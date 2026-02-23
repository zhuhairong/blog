#ifndef C_UTILS_BPLUS_TREE_H
#define C_UTILS_BPLUS_TREE_H

#include <stddef.h>
#include <stdbool.h>

// 可以通过宏定义自定义 B+ 树的阶数
#ifndef BPLUS_ORDER
#define BPLUS_ORDER 4
#endif

typedef struct bplus_tree_s bplus_tree_t;
typedef struct bplus_iterator_s bplus_iterator_t;

// 创建 B+ 树
// compar: 键比较函数
// 返回: 成功返回树指针，失败返回 NULL
bplus_tree_t* bplus_tree_create(int (*compar)(const void *, const void *));

// 释放 B+ 树
// tree: B+ 树
void bplus_tree_free(bplus_tree_t *tree);

// 插入键值对
// tree: B+ 树
// key: 键指针
// value: 值指针
// 返回: 成功返回 true，失败返回 false
bool bplus_tree_insert(bplus_tree_t *tree, const void *key, void *value);

// 获取值
// tree: B+ 树
// key: 键指针
// 返回: 找到返回值指针，未找到返回 NULL
void* bplus_tree_get(const bplus_tree_t *tree, const void *key);

// 删除键值对
// tree: B+ 树
// key: 键指针
// 返回: 成功返回 true，未找到返回 false
bool bplus_tree_delete(bplus_tree_t *tree, const void *key);

// 检查 B+ 树是否为空
// tree: B+ 树
// 返回: 为空返回 true，否则返回 false
bool bplus_tree_is_empty(const bplus_tree_t *tree);

// 获取 B+ 树大小
// tree: B+ 树
// 返回: 键值对数量
size_t bplus_tree_size(const bplus_tree_t *tree);

// 创建 B+ 树迭代器
// tree: B+ 树
// 返回: 成功返回迭代器指针，失败返回 NULL
bplus_iterator_t* bplus_iterator_create(const bplus_tree_t *tree);

// 释放迭代器
// iter: 迭代器
void bplus_iterator_free(bplus_iterator_t *iter);

// 迭代到下一个元素
// iter: 迭代器
// key: 输出键指针
// value: 输出值指针
// 返回: 有下一个元素返回 true，否则返回 false
bool bplus_iterator_next(bplus_iterator_t *iter, const void **key, void **value);

// 重置迭代器到起始位置
// iter: 迭代器
void bplus_iterator_reset(bplus_iterator_t *iter);

// 范围查询
// tree: B+ 树
// start_key: 起始键（包含）
// end_key: 结束键（包含）
// callback: 回调函数，参数为 (key, value, user_data)
// user_data: 用户数据
// 返回: 找到的元素数量
size_t bplus_tree_range_query(const bplus_tree_t *tree, const void *start_key, const void *end_key,
                              bool (*callback)(const void *key, void *value, void *user_data),
                              void *user_data);

// 获取最小键
// tree: B+ 树
// key: 输出键指针
// value: 输出值指针
// 返回: 成功返回 true，失败返回 false
bool bplus_tree_min(const bplus_tree_t *tree, const void **key, void **value);

// 获取最大键
// tree: B+ 树
// key: 输出键指针
// value: 输出值指针
// 返回: 成功返回 true，失败返回 false
bool bplus_tree_max(const bplus_tree_t *tree, const void **key, void **value);

// 验证 B+ 树结构
// tree: B+ 树
// 返回: 结构有效返回 true，无效返回 false
bool bplus_tree_validate(const bplus_tree_t *tree);

#endif // C_UTILS_BPLUS_TREE_H
