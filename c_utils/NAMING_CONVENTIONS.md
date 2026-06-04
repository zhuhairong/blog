# c_utils 命名规范

## 模块命名

### 文件和目录
- **库源文件**: `c_utils/<module_name>.c`, `c_utils/<module_name>.h`
- **测试文件**: `tests/test_<module_name>.c`
- **Demo 文件**: `demo/demo_<module_name>.c`
- 使用小写字母和下划线 (`snake_case`)
- 模块名称应简洁且具有描述性

### 函数命名
统一的命名模式：`{module}_{action}_{qualifier}`

| 类别 | 模式 | 示例 |
|------|------|------|
| 创建/初始化 | `{module}_create` | `hashmap_create` |
| 带配置的创建 | `{module}_create_ex` | `hashmap_create_ex` |
| 销毁/清理 | `{module}_free` / `{module}_destroy` | `hashmap_free` |
| 基本操作 | `{module}_{verb}` | `hashmap_insert`, `hashmap_get`, `hashmap_remove` |
| 带错误处理的操作 | `{module}_{verb}_ex` | `ringbuffer_write_ex` |
| 属性查询 | `{module}_{property}` | `hashmap_size`, `hashmap_is_empty` |
| 字符串表示 | `{module}_to_string` | 转换为字符串形式 |
| 错误信息 | `{module}_strerror` | `url_strerror` |
| 默认配置 | `{module}_default_config` | `ringbuffer_default_config` |

### 类型命名

| 类别 | 模式 | 示例 |
|------|------|------|
| 结构体/类型 | `{module}_t` | `hashmap_t`, `rbtree_t` |
| 配置结构体 | `{module}_config_t` | `ringbuf_config_t` |
| 状态结构体 | `{module}_state_t` | `ringbuf_state_t` |
| 错误码枚举 | `{module}_error_t` | `ringbuf_error_t` |
| 迭代器 | `{module}_iter_t` | `rbtree_iter_t` |
| 回调函数 | `{module}_{action}_fn` | `rbtree_visit_fn` |

### 常量和宏
- **宏常量**: 全大写，下划线分隔: `{MODULE}_{NAME}`
- **错误码常量**: `{MODULE}_OK`, `{MODULE}_ERROR_{REASON}`
- **版本信息**: `C_UTILS_VERSION_MAJOR`, `C_UTILS_VERSION_MINOR`, `C_UTILS_VERSION_PATCH`

### 枚举值
- 错误码使用 `{MODULE}_{STATUS}` 前缀: `QUAT_OK`, `QUAT_ERROR_NULL_PTR`
- 状态值使用描述性名称: `RBTREE_RED`, `RBTREE_BLACK`

### 头文件保护宏
```c
#ifndef C_UTILS_MODULE_NAME_H
#define C_UTILS_MODULE_NAME_H
// ...
#endif // C_UTILS_MODULE_NAME_H
```

## Include 路径规范

### Demo 文件
Demo 文件必须使用相对路径引用库头文件：
```c
#include "../c_utils/module_name.h"
```

Demo 文件引用第三方库：
```c
#include "../thirdparty/library_name.h"
```

### 测试文件
测试文件使用相对路径：
```c
#include "../c_utils/module_name.h"
#include "../c_utils/utest.h"
```

### 库内部引用
库源文件 (.c) 引用同目录头文件：
```c
#include "module_name.h"
```

跨模块引用：
```c
#include "other_module.h"
```

## 文件组织

```
c_utils/
├── c_utils/           # 库源文件和头文件
│   ├── module_name.h
│   └── module_name.c
├── tests/             # 单元测试
│   └── test_module_name.c
├── demo/              # 演示程序
│   └── demo_module_name.c
├── thirdparty/        # 第三方依赖
├── CMakeLists.txt     # 构建配置
└── NAMING_CONVENTIONS.md
```

## 废弃 API 处理

- 使用 `CU_DEPRECATED("message")` 宏标记废弃的函数
- 在文件头部添加迁移指南
- 废弃的模块保留以维持向后兼容
- 示例: `ringbuf.h` 所有函数标记 `CU_DEPRECATED("Use ringbuffer_* functions instead")`

## 注释规范

### 文件头部
Demo 文件必须包含描述性头部注释：
```c
/**
 * 模块名演示程序
 *
 * 功能：
 * - 功能点 1
 * - 功能点 2
 */
```

### 函数注释
公开 API 函数使用 Doxygen 风格注释：
```c
/**
 * @brief 简要描述
 * @param param_name 参数说明
 * @return 返回值说明
 */
```

## 错误处理约定

- 函数返回错误码时使用 `{module}_error_t` 枚举
- 成功返回 `{MODULE}_OK` (值为 0)
- 每个模块提供 `{module}_strerror()` 函数将错误码转换为字符串
- `_ex` 后缀的函数通过输出参数返回详细错误信息
