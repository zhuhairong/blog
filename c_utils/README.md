# C-Utils: C 语言实用工具库集合

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform](https://img.shields.io/badge/platform-Linux%2FUnix-blue)]()
[![C Standard](https://img.shields.io/badge/C-11%2B-blue)]()

本项目包含 **150 个**轻量级、模块化的 C 语言工具库，涵盖数据结构、算法、系统编程、网络协议、加密编码等领域。每个库都设计为单头文件或头文件+源文件形式，易于集成到项目中。

## 项目规模

| 指标 | 数量 |
|------|------|
| 总模块数 | 150 |
| 演示程序 | 110 |
| 单元测试 | 149 |

## 分类总览

| 分类 | 模块数量 | 描述 |
|------|----------|------|
| [数据结构](#数据结构) | 23 | 链表、树、哈希表、堆、环形缓冲区等 |
| [算法](#算法) | 25 | 排序、搜索、图算法、计算几何等 |
| [编码与压缩](#编码与压缩) | 18 | Base编码、URL/HTML编码、压缩算法等 |
| [加密与哈希](#加密与哈希) | 13 | MD5、SHA、AES、ChaCha20、RSA等 |
| [系统与并发](#系统与并发) | 21 | 线程池、进程管理、内存管理、IPC等 |
| [网络与协议](#网络与协议) | 12 | HTTP、WebSocket、MQTT、CoAP、DNS等 |
| [文件与格式](#文件与格式) | 14 | JSON、XML、CSV、INI、TOML等 |
| [数学与科学计算](#数学与科学计算) | 13 | 大整数、矩阵、向量、FFT、统计等 |
| [实用工具](#实用工具) | 16 | 日志、计时器、UUID、终端控制等 |

## 特性

- **模块化设计** - 每个组件独立，可按需引入
- **跨平台** - 支持 Linux/Unix 系统
- **零依赖** - 纯 C 标准库实现
- **完整测试** - 149 个单元测试，全部通过
- **CMake 构建** - 现代化构建系统
- **完善接口** - 所有数据结构支持遍历、迭代器、极值查找等完整操作
- **配套演示** - 110 个演示程序，展示用法

## 快速开始

### 构建项目

```bash
cd c_utils
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### 运行测试

```bash
ctest --output-on-failure
```

### 运行演示程序

```bash
./demo_vec
./demo_hashmap
./demo_json
# ... 更多演示程序
```

## 模块列表

### 数据结构

| 模块 | 描述 |
|------|------|
| `vec` | 泛型动态数组 |
| `list` | 双向链表 |
| `dstr` | 动态字符串 |
| `hashmap` | 字符串键哈希表 |
| `rbtree` | 红黑树 |
| `avl` | AVL 平衡树 |
| `skiplist` | 跳表 |
| `trie` | 前缀树 |
| `ringbuf` | 字节环形缓冲区 |
| `ringbuffer` | 对象环形缓冲区 |
| `bitset` | 位图 |
| `bitset_compressed` | 压缩位图 |
| `heap` | 堆 |
| `bloom` | 布隆过滤器 |
| `bloom_filter_counting` | 计数布隆过滤器 |
| `disjoint_set_forest` | 并查集（森林） |
| `union_find` | 并查集 |
| `segment_tree` | 线段树 |
| `fenwick_tree` | 树状数组 |
| `sparse_table` | 稀疏表 |
| `lru_cache` | LRU 缓存 |
| `kv_store` | 键值存储 |
| `bplus_tree` | B+ 树 |

### 算法

| 模块 | 描述 |
|------|------|
| `sort` | 快速排序 |
| `sort_heap` | 堆排序 |
| `sort_merge` | 归并排序 |
| `sort_utils` | 排序工具 |
| `binary_search` | 二分查找 |
| `kmp` | KMP 字符串匹配 |
| `rabin_karp` | Rabin-Karp 算法 |
| `z_algorithm` | Z 算法 |
| `manacher` | Manacher 回文算法 |
| `levenshtein` | 编辑距离 |
| `suffix_array` | 后缀数组 |
| `glob_match` | Glob 模式匹配 |
| `regex_tiny` | 极简正则表达式 |
| `astar` | A* 寻路算法 |
| `dijkstra` | Dijkstra 最短路径 |
| `bellman_ford` | Bellman-Ford 算法 |
| `floyd_warshall` | Floyd-Warshall 算法 |
| `prim` | Prim 最小生成树 |
| `kruskal` | Kruskal 最小生成树 |
| `topological_sort` | 拓扑排序 |
| `tarjan_scc` | Tarjan 强连通分量 |
| `convex_hull` | 凸包算法 |
| `line_intersection` | 线段交点 |

### 编码与压缩

| 模块 | 描述 |
|------|------|
| `base16` | Base16 (Hex) 编码 |
| `base32` | Base32 编码 |
| `base32_hex` | Base32 Hex 编码 |
| `base58` | Base58 编码 |
| `base64` | Base64 编码 |
| `url_codec` | URL 编解码 |
| `html_codec` | HTML 实体编解码 |
| `punycode` | Punycode 编码 |
| `utf8` | UTF-8 工具 |
| `soundex` | Soundex 算法 |
| `varint` | 变长整数编码 |
| `zigzag` | ZigZag 编码 |
| `rle` | 行程长度编码 |
| `lzw` | LZW 压缩 |
| `huffman` | 哈夫曼编码 |
| `delta_encoding` | 差分编码 |
| `run_length_limited` | RLL 编码 |

### 加密与哈希

| 模块 | 描述 |
|------|------|
| `md5` | MD5 哈希 |
| `sha1` | SHA-1 哈希 |
| `sha256_tiny` | SHA-256 哈希 |
| `aes_tiny` | AES 加密 |
| `chacha20_tiny` | ChaCha20 流加密 |
| `rsa_tiny` | RSA 模幂运算 |
| `hmac` | HMAC 消息认证 |
| `poly1305_tiny` | Poly1305 MAC |
| `crc32` | CRC32 校验 |
| `adler32` | Adler32 校验 |
| `otp` | OTP (TOTP/HOTP) |
| `pbkdf2` | PBKDF2 密钥派生 |
| `crypto` | 通用加密接口 |

### 系统与并发

| 模块 | 描述 |
|------|------|
| `threadpool` | 线程池 |
| `process` | 进程操作 |
| `memory_pool_fixed` | 固定大小内存池 |
| `page_allocator` | 页分配器 |
| `arena` | 内存区域分配器 |
| `shm` | 共享内存 |
| `mmap` | 内存映射 |
| `sem` | 信号量 |
| `pipe` | 管道 |
| `lockfile` | 文件锁 |
| `cpu_affinity` | CPU 亲和性 |
| `cpu_usage` | CPU 使用率 |
| `backtrace` | 堆栈回溯 |
| `dynamic_lib` | 动态库加载 |
| `env` | 环境变量 |
| `signal_utils` | 信号处理 |
| `thread_local_storage` | 线程本地存储 |
| `timer` | 定时器 |
| `stopwatch` | 计时器 |

### 网络与协议

| 模块 | 描述 |
|------|------|
| `http` | HTTP 基础 |
| `http_parser_tiny` | 极简 HTTP 解析 |
| `websocket_frame` | WebSocket 帧 |
| `mqtt_pkt` | MQTT 数据包 |
| `coap_pkt` | CoAP 数据包 |
| `net` | 网络工具 |
| `dns_pkt` | DNS 数据包 |
| `url` | URL 解析 |
| `slip` | SLIP 编码 |
| `cobs` | COBS 编码 |

### 文件与格式

| 模块 | 描述 |
|------|------|
| `json` | JSON 解析器 |
| `json_writer` | JSON 写入器 |
| `ini` | INI 解析与写入 |
| `toml_parse` | TOML 解析 |
| `config` | 简单配置 |
| `csv` | CSV 解析 |
| `csv_writer` | CSV 写入 |
| `xml_parse` | XML 解析 |
| `wav_header` | WAV 文件头 |
| `bmp_tiny` | BMP 图像 |
| `fs_utils` | 文件系统工具 |
| `path` | 路径操作 |
| `glob` | 通配符匹配 |
| `file_watcher` | 文件监控 |

### 数学与科学计算

| 模块 | 描述 |
|------|------|
| `bigint` | 大整数运算 |
| `matrix` | 矩阵运算 |
| `vector3` | 3D 向量 |
| `quaternion` | 四元数 |
| `complex` | 复数运算 |
| `fast_fourier_transform` | FFT 快速傅里叶变换 |
| `kalman_scalar` | 卡尔曼滤波 |
| `pid_controller` | PID 控制器 |
| `stats` | 统计分析 |
| `math_utils` | 数学工具 |
| `random` | 随机数生成 |
| `log_rotate` | 日志轮转 |

### 实用工具

| 模块 | 描述 |
|------|------|
| `log` | 日志系统 |
| `argparse` | 参数解析 |
| `cron` | Cron 表达式 |
| `uuid` | UUID v4 |
| `uuid_v4_fast` | 快速 UUID v4 |
| `uuid_v7` | UUID v7 |
| `version` | 版本解析 |
| `version_compare` | 版本比较 |
| `terminal` | 终端控制 |
| `time_utils` | 时间工具 |
| `units` | 单位转换 |
| `color` | 颜色转换 |
| `endian_utils` | 字节序转换 |
| `bitstream` | 位流读写 |
| `consistent_hash` | 一致性哈希 |
| `rate_limiter_leaky` | 漏桶限流 |
| `retry` | 重试机制 |
| `fsm` | 有限状态机 |

## 使用示例

### 使用动态数组

```c
#include "vec.h"

vec(int) numbers = vec_new();
vec_push(&numbers, 10);
vec_push(&numbers, 20);
vec_foreach(&numbers, n, {
    printf("%d\n", n);
});
vec_free(numbers);
```

### 使用哈希表

```c
#include "hashmap.h"

hashmap_t *map = hashmap_new(NULL);
hashmap_set(map, "name", "Alice");
hashmap_set(map, "age", "25");

char *name = hashmap_get(map, "name");
printf("Name: %s\n", name);

hashmap_free(map);
```

### 使用 JSON 解析

```c
#include "json.h"

const char *json_str = "{\"name\": \"Bob\", \"age\": 30}";
json_value_t *json = json_parse(json_str, strlen(json_str));

json_value_t *name = json_object_get(json, "name");
printf("Name: %s\n", name->string);

json_free(json);
```

## 项目结构

```
c_utils/
├── c_utils/           # 头文件和源文件
│   ├── vec.h
│   ├── hashmap.c
│   ├── json.h
│   └── ...
├── demo/             # 演示程序 (110个)
│   ├── demo_vec.c
│   ├── demo_hashmap.c
│   └── ...
├── tests/            # 单元测试 (149个)
│   ├── test_vec.c
│   └── ...
├── CMakeLists.txt
└── README.md
```

## 贡献

欢迎提交 Pull Request！请确保：

1. 代码符合项目风格
2. 添加对应的单元测试
3. 更新 README.md

## 许可证

MIT License - 详见 [LICENSE](LICENSE) 文件
