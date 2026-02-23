# C-Utils: C 语言实用工具库集合

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)]()
[![Tests](https://img.shields.io/badge/tests-149%2F149%20passing-brightgreen)]()
[![License](https://img.shields.io/badge/license-MIT-blue)]()
[![Modules](https://img.shields.io/badge/modules-149-orange)]()

本项目包含 **149 个**轻量级、模块化的 C 语言工具库，涵盖数据结构、算法、系统编程、网络协议、加密编码等领域。每个库都设计为单头文件或头文件+源文件形式，易于集成到项目中。

## 分类总览

| 分类 | 模块数量 | 描述 |
|------|----------|------|
| [数据结构](#数据结构) | 23 | 链表、树、哈希表、堆、环形缓冲区等 |
| [算法](#算法) | 24 | 排序、搜索、图算法、计算几何等 |
| [字符串与编码](#字符串与编码) | 17 | Base编码、URL/HTML编码、压缩算法等 |
| [加密与哈希](#加密与哈希) | 12 | MD5、SHA、AES、ChaCha20、RSA等 |
| [系统与并发](#系统与并发) | 20 | 线程池、进程管理、内存管理、IPC等 |
| [网络与协议](#网络与协议) | 12 | HTTP、WebSocket、MQTT、CoAP、DNS等 |
| [文件与格式](#文件与格式) | 14 | JSON、XML、CSV、INI、TOML等 |
| [数学与统计](#数学与统计) | 12 | 大整数、矩阵、向量、FFT、统计等 |
| [实用工具](#实用工具) | 14 | 日志、计时器、UUID、终端控制等 |

## 特性

- **模块化设计** - 每个组件独立，可按需引入
- **跨平台** - 支持 Linux/Unix 系统
- **零依赖** - 纯 C 标准库实现
- **完整测试** - 79+ 个单元测试，全部通过
- **CMake 构建** - 现代化构建系统
- **完善接口** - 所有数据结构支持遍历、迭代器、极值查找等完整操作

## 快速开始

### 构建项目

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### 运行测试

```bash
ctest --output-on-failure
```

### 使用库

```c
#include "c_utils/dstr.h"
#include "c_utils/list.h"
#include "c_utils/avl.h"
#include "c_utils/json.h"
#include "c_utils/base64.h"

int compare_ints(const void *a, const void *b) {
    int ia = *(const int*)a, ib = *(const int*)b;
    return (ia > ib) - (ia < ib);
}

int main() {
    // 动态字符串
    dstr_t str;
    dstr_init(&str);
    dstr_append(&str, "Hello");
    dstr_free(&str);
    
    // 双向链表 - 完整操作
    list_t *list = list_create();
    int vals[] = {3, 1, 4, 1, 5};
    for (int i = 0; i < 5; i++) {
        list_push_back(list, &vals[i]);
    }
    list_sort(list, compare_ints);  // 排序
    list_foreach(list, node) {
        printf("%d ", *(int*)node->data);
    }
    list_free(list);
    
    // AVL 树 - 遍历和迭代器
    avl_t *avl = avl_create(compare_ints);
    for (int i = 0; i < 5; i++) {
        avl_insert(avl, &vals[i], &vals[i]);
    }
    // 中序遍历
    avl_inorder(avl, print_visitor, NULL);
    // 迭代器遍历
    avl_iter_t iter = avl_iter_begin(avl);
    while (avl_iter_valid(&iter)) {
        printf("%d ", *(int*)avl_iter_key(&iter));
        avl_iter_next(&iter);
    }
    avl_iter_free(&iter);
    avl_free(avl);
    
    // JSON 解析
    json_value_t *json = json_parse("{\"name\":\"test\"}");
    json_free(json);
    
    // Base64 编码
    char encoded[64];
    base64_encode((unsigned char*)"data", 4, encoded);
    
    return 0;
}
```

## 目录

1. [数据结构](#数据结构)
2. [算法](#算法)
3. [字符串与编码](#字符串与编码)
4. [加密与哈希](#加密与哈希)
5. [系统与并发](#系统与并发)
6. [网络与协议](#网络与协议)
7. [文件与格式](#文件与格式)
8. [数学与统计](#数学与统计)
9. [实用工具](#实用工具)

---

## 数据结构

| 头文件 | 描述 | 关键 API |
|--------|------|----------|
| `dstr.h` / `dstr.c` | 动态字符串 | `dstr_init`, `dstr_append`, `dstr_printf` |
| `vec.h` / `vec.c` | 泛型动态数组 | `vec_create`, `vec_push`, `vec_at`, `vec_insert`, `vec_sort` |
| `list.h` / `list.c` | 双向链表 | `list_create`, `list_push_back`, `list_insert`, `list_remove`, `list_find`, `list_sort`, `list_foreach` |
| `hashmap.h` / `hashmap.c` | 字符串键哈希表 | `hashmap_create`, `hashmap_put`, `hashmap_get`, `hashmap_remove`, `hashmap_iter` |
| `rbtree.h` / `rbtree.c` | 红黑树 | `rbtree_insert`, `rbtree_get`, `rbtree_delete`, `rbtree_min`, `rbtree_max`, `rbtree_inorder`, `rbtree_iter_begin` |
| `avl.h` / `avl.c` | AVL 树 | `avl_insert`, `avl_get`, `avl_delete`, `avl_min`, `avl_max`, `avl_inorder`, `avl_iter_begin` |
| `skiplist.h` / `skiplist.c` | 跳表 | `skiplist_insert`, `skiplist_search` |
| `trie.h` / `trie.c` | 前缀树 | `trie_insert`, `trie_search` |
| `ringbuf.h` / `ringbuf.c` | 字节环形缓冲区 | `ringbuf_write`, `ringbuf_read` |
| `ringbuffer.h` / `ringbuffer.c` | 对象环形缓冲区 | `ringbuffer_push`, `ringbuffer_pop` |
| `bitset.h` / `bitset.c` | 位图 | `bitset_set`, `bitset_get` |
| `bitset_compressed.h` / `bitset_compressed.c` | 压缩位图 (RLE) | `bitset_comp_add_run` |
| `heap.h` / `heap.c` | 堆 | `heap_push`, `heap_pop` |
| `bloom.h` / `bloom.c` | 布隆过滤器 | `bloom_add`, `bloom_check` |
| `bloom_filter_counting.h` / `bloom_filter_counting.c` | 计数布隆过滤器 | `bloom_counting_add`, `bloom_counting_remove` |
| `disjoint_set_forest.h` / `disjoint_set_forest.c` | 并查集 | `dsf_union`, `dsf_find` |
| `union_find.h` / `union_find.c` | 简单并查集 | `uf_union`, `uf_find` |
| `segment_tree.h` / `segment_tree.c` | 线段树 | `segment_tree_query`, `segment_tree_update` |
| `fenwick_tree.h` / `fenwick_tree.c` | 树状数组 | `fenwick_update`, `fenwick_query` |
| `sparse_table.h` / `sparse_table.c` | 稀疏表 (RMQ) | `sparse_table_query` |
| `lru_cache.h` / `lru_cache.c` | LRU 缓存 | `lru_put`, `lru_get` |
| `kv_store.h` / `kv_store.c` | 简单键值存储 | `kv_set`, `kv_get` |
| `bplus_tree.h` / `bplus_tree.c` | B+ 树 | `bpt_insert`, `bpt_search` |

---

## 算法

### 排序与搜索

| 头文件 | 描述 | 关键 API |
|--------|------|----------|
| `sort.h` / `sort.c` | 快速排序 | `quick_sort` |
| `sort_heap.h` / `sort_heap.c` | 堆排序 | `heap_sort` |
| `sort_merge.h` / `sort_merge.c` | 归并排序 | `merge_sort` |
| `sort_utils.h` / `sort_utils.c` | 通用排序工具 | `sort_utils_quicksort`, `sort_binary_search` |
| `binary_search.h` | 二分查找 | `binary_search` |

### 字符串算法

| 头文件 | 描述 | 关键 API |
|--------|------|----------|
| `kmp.h` / `kmp.c` | KMP 字符串匹配 | `kmp_search` |
| `rabin_karp.h` / `rabin_karp.c` | Rabin-Karp 算法 | `rabin_karp_search` |
| `z_algorithm.h` / `z_algorithm.c` | Z 算法 | `z_algorithm_compute` |
| `manacher.h` / `manacher.c` | Manacher 回文算法 | `manacher_longest_palindrome` |
| `levenshtein.h` / `levenshtein.c` | 编辑距离 | `levenshtein_distance` |
| `suffix_array.h` / `suffix_array.c` | 后缀数组 | `suffix_array_construct` |
| `glob_match.h` / `glob_match.c` | Glob 模式匹配 | `glob_match` |
| `regex_tiny.h` / `regex_tiny.c` | 极简正则表达式 | `regex_match` |

### 图算法

| 头文件 | 描述 | 关键 API |
|--------|------|----------|
| `astar.h` / `astar.c` | A* 寻路算法 | `astar_search` |
| `dijkstra.h` / `dijkstra.c` | Dijkstra 最短路径 | `dijkstra` |
| `bellman_ford.h` / `bellman_ford.c` | Bellman-Ford 算法 | `bellman_ford` |
| `floyd_warshall.h` / `floyd_warshall.c` | Floyd-Warshall 算法 | `floyd_warshall` |
| `prim.h` / `prim.c` | Prim 最小生成树 | `prim_mst` |
| `kruskal.h` / `kruskal.c` | Kruskal 最小生成树 | `kruskal_mst` |
| `topological_sort.h` / `topological_sort.c` | 拓扑排序 | `topological_sort` |
| `tarjan_scc.h` / `tarjan_scc.c` | Tarjan 强连通分量 | `tarjan_scc` |

### 计算几何

| 头文件 | 描述 | 关键 API |
|--------|------|----------|
| `convex_hull.h` / `convex_hull.c` | 凸包算法 | `convex_hull_compute` |
| `line_intersection.h` / `line_intersection.c` | 线段交点 | `line_intersection` |

---

## 字符串与编码

### Base 编码

| 头文件 | 描述 | 关键 API |
|--------|------|----------|
| `base16.h` / `base16.c` | Base16 (Hex) 编码 | `base16_encode`, `base16_decode` |
| `base32.h` / `base32.c` | Base32 编码 | `base32_encode`, `base32_decode` |
| `base32_hex.h` / `base32_hex.c` | Base32 Hex 编码 | `base32_hex_encode`, `base32_hex_decode` |
| `base58.h` / `base58.c` | Base58 编码 | `base58_encode`, `base58_decode` |
| `base64.h` / `base64.c` | Base64 编码 | `base64_encode`, `base64_decode` |

### URL/HTML 编码

| 头文件 | 描述 | 关键 API |
|--------|------|----------|
| `url_codec.h` / `url_codec.c` | URL 编解码 | `url_encode`, `url_decode` |
| `html_codec.h` / `html_codec.c` | HTML 实体编解码 | `html_encode`, `html_decode` |
| `punycode.h` / `punycode.c` | Punycode 编码 | `punycode_encode`, `punycode_decode` |

### 字符串处理

| 头文件 | 描述 | 关键 API |
|--------|------|----------|
| `utf8.h` / `utf8.c` | UTF-8 工具 | `utf8_validate`, `utf8_strlen` |
| `soundex.h` / `soundex.c` | Soundex 算法 | `soundex_encode` |

### 整数编码

| 头文件 | 描述 | 关键 API |
|--------|------|----------|
| `varint.h` / `varint.c` | 变长整数编码 | `varint_encode`, `varint_decode` |
| `zigzag.h` / `zigzag.c` | ZigZag 编码 | `zigzag_encode`, `zigzag_decode` |

### 压缩算法

| 头文件 | 描述 | 关键 API |
|--------|------|----------|
| `rle.h` / `rle.c` | 行程长度编码 | `rle_encode`, `rle_decode` |
| `lzw.h` / `lzw.c` | LZW 压缩 | `lzw_encode`, `lzw_decode` |
| `huffman.h` / `huffman.c` | 哈夫曼编码 | `huffman_stats` |
| `delta_encoding.h` / `delta_encoding.c` | 差分编码 | `delta_encode`, `delta_decode` |
| `run_length_limited.h` / `run_length_limited.c` | RLL 编码 | `rll_encode` |

---

## 加密与哈希

### 哈希算法

| 头文件 | 描述 | 关键 API |
|--------|------|----------|
| `md5.h` / `md5.c` | MD5 哈希 | `md5_init`, `md5_update`, `md5_final` |
| `sha1.h` / `sha1.c` | SHA-1 哈希 | `sha1_init`, `sha1_update`, `sha1_final` |
| `sha256_tiny.h` / `sha256_tiny.c` | SHA-256 哈希 | `sha256_tiny` |
| `crypto.h` / `crypto.c` | 通用加密接口 | `sha256_init`, `sha256_update`, `sha256_final` |

### 消息认证码

| 头文件 | 描述 | 关键 API |
|--------|------|----------|
| `hmac.h` / `hmac.c` | HMAC | `hmac_sha1`, `hmac_sha256` |
| `poly1305_tiny.h` / `poly1305_tiny.c` | Poly1305 | `poly1305_tiny` |

### 对称加密

| 头文件 | 描述 | 关键 API |
|--------|------|----------|
| `aes_tiny.h` / `aes_tiny.c` | AES 加密 | `aes_tiny_encrypt`, `aes_tiny_decrypt` |
| `chacha20_tiny.h` / `chacha20_tiny.c` | ChaCha20 流加密 | `chacha20_tiny` |

### 非对称加密

| 头文件 | 描述 | 关键 API |
|--------|------|----------|
| `rsa_tiny.h` / `rsa_tiny.c` | RSA 模幂运算 | `rsa_tiny_modpow` |

### 校验和

| 头文件 | 描述 | 关键 API |
|--------|------|----------|
| `crc32.h` / `crc32.c` | CRC32 校验 | `crc32` |
| `adler32.h` / `adler32.c` | Adler32 校验 | `adler32` |

### 密码学工具

| 头文件 | 描述 | 关键 API |
|--------|------|----------|
| `otp.h` / `otp.c` | OTP (TOTP/HOTP) | `otp_generate_totp`, `otp_generate_hotp` |
| `pbkdf2.h` / `pbkdf2.c` | PBKDF2 密钥派生 | `pbkdf2_sha256` |

---

## 系统与并发

### 线程与并发

| 头文件 | 描述 | 关键 API |
|--------|------|----------|
| `threadpool.h` / `threadpool.c` | 线程池 | `threadpool_create`, `threadpool_add_task`, `threadpool_add_task_with_priority`, `threadpool_wait_all`, `threadpool_resize`, `threadpool_pause`, `threadpool_resume` |
| `thread_local_storage.h` / `thread_local_storage.c` | 线程本地存储 | `tls_key_create`, `tls_get`, `tls_set` |

### 进程管理

| 头文件 | 描述 | 关键 API |
|--------|------|----------|
| `process.h` / `process.c` | 进程操作 | `process_spawn`, `process_wait` |
| `signal_utils.h` / `signal_utils.c` | 信号处理 | `signal_register` |

### 内存管理

| 头文件 | 描述 | 关键 API |
|--------|------|----------|
| `memory_pool_fixed.h` / `memory_pool_fixed.c` | 固定大小内存池 | `mem_pool_fixed_alloc` |
| `page_allocator.h` / `page_allocator.c` | 页分配器 | `page_alloc` |
| `arena.h` / `arena.c` | 内存区域分配器 | `arena_alloc`, `arena_reset` |
| `shm.h` / `shm.c` | 共享内存 | `shm_create`, `shm_attach` |
| `mmap.h` / `mmap.c` | 内存映射 | `mmap_file` |

### 系统信息

| 头文件 | 描述 | 关键 API |
|--------|------|----------|
| `cpu_affinity.h` / `cpu_affinity.c` | CPU 亲和性 | `cpu_affinity_set` |
| `cpu_usage.h` / `cpu_usage.c` | CPU 使用率 | `cpu_usage_get` |
| `backtrace.h` / `backtrace.c` | 堆栈回溯 | `backtrace_print` |
| `dynamic_lib.h` / `dynamic_lib.c` | 动态库加载 | `dynamic_lib_open` |

### IPC 与同步

| 头文件 | 描述 | 关键 API |
|--------|------|----------|
| `sem.h` / `sem.c` | 信号量 | `sem_wait`, `sem_post` |
| `pipe.h` / `pipe.c` | 管道 | `pipe_create`, `pipe_read`, `pipe_write` |
| `lockfile.h` / `lockfile.c` | 文件锁 | `lockfile_lock` |

### 环境变量

| 头文件 | 描述 | 关键 API |
|--------|------|----------|
| `env.h` / `env.c` | 环境变量 | `env_get`, `env_set` |

---

## 网络与协议

### HTTP

| 头文件 | 描述 | 关键 API |
|--------|------|----------|
| `http.h` / `http.c` | HTTP 基础 | `http_parse_request_line` |
| `http_parser_tiny.h` / `http_parser_tiny.c` | 极简 HTTP 解析 | `http_parser_execute` |

### WebSocket

| 头文件 | 描述 | 关键 API |
|--------|------|----------|
| `websocket_frame.h` / `websocket_frame.c` | WebSocket 帧 | `ws_frame_encode_hdr`, `ws_frame_decode_hdr` |

### IoT 协议

| 头文件 | 描述 | 关键 API |
|--------|------|----------|
| `mqtt_pkt.h` / `mqtt_pkt.c` | MQTT 数据包 | `mqtt_encode_connect`, `mqtt_decode_publish` |
| `coap_pkt.h` / `coap_pkt.c` | CoAP 数据包 | `coap_pkt_encode_hdr`, `coap_pkt_decode_hdr` |

### 网络基础

| 头文件 | 描述 | 关键 API |
|--------|------|----------|
| `net.h` / `net.c` | 网络工具 | `net_tcp_connect`, `net_udp_socket` |
| `dns_pkt.h` / `dns_pkt.c` | DNS 数据包 | `dns_encode_query`, `dns_decode_response` |
| `url.h` / `url.c` | URL 解析 | `url_parse` |

### 串口通信

| 头文件 | 描述 | 关键 API |
|--------|------|----------|
| `slip.h` / `slip.c` | SLIP 编码 | `slip_encode`, `slip_decode` |
| `cobs.h` / `cobs.c` | COBS 编码 | `cobs_encode`, `cobs_decode` |

---

## 文件与格式

### JSON

| 头文件 | 描述 | 关键 API |
|--------|------|----------|
| `json.h` / `json.c` | JSON 解析器 | `json_parse`, `json_get_string`, `json_free` |
| `json_writer.h` / `json_writer.c` | JSON 写入器 | `json_writer_key`, `json_writer_string` |

### 配置文件

| 头文件 | 描述 | 关键 API |
|--------|------|----------|
| `ini.h` / `ini.c` | INI 解析与写入 | `ini_load`, `ini_get`, `ini_set`, `ini_save`, `ini_delete`, `ini_foreach` |
| `toml_parse.h` / `toml_parse.c` | TOML 解析 | `toml_parse` |
| `config.h` / `config.c` | 简单配置 | `config_load` |

### 数据格式

| 头文件 | 描述 | 关键 API |
|--------|------|----------|
| `csv.h` / `csv.c` | CSV 解析 | `csv_parse_line`, `csv_load`, `csv_get` |
| `csv_writer.h` / `csv_writer.c` | CSV 写入 | `csv_writer_init`, `csv_writer_row` |
| `xml_parse.h` / `xml_parse.c` | XML 解析 | `xml_parse` |

### 多媒体格式

| 头文件 | 描述 | 关键 API |
|--------|------|----------|
| `wav_header.h` / `wav_header.c` | WAV 文件头 | `wav_hdr_init`, `wav_hdr_write` |
| `bmp_tiny.h` / `bmp_tiny.c` | BMP 图像 | `bmp_tiny_write` |

### 文件系统

| 头文件 | 描述 | 关键 API |
|--------|------|----------|
| `fs_utils.h` / `fs_utils.c` | 文件系统工具 | `fs_exists`, `fs_mkdir` |
| `path.h` / `path.c` | 路径操作 | `path_join`, `path_dirname` |
| `glob.h` / `glob.c` | 通配符匹配 | `glob_expand` |
| `file_watcher.h` / `file_watcher.c` | 文件监控 | `file_watcher_check` |
| `log_rotate.h` / `log_rotate.c` | 日志轮转 | `log_rotate` |

---

## 数学与统计

### 数值计算

| 头文件 | 描述 | 关键 API |
|--------|------|----------|
| `bigint.h` / `bigint.c` | 大整数运算 | `bigint_add`, `bigint_mul` |
| `matrix.h` / `matrix.c` | 矩阵运算 | `matrix_mul`, `matrix_transpose` |
| `vector3.h` / `vector3.c` | 3D 向量 | `vec3_add`, `vec3_dot`, `vec3_cross` |
| `quaternion.h` / `quaternion.c` | 四元数 | `quat_mul`, `quat_rotate` |
| `complex.h` / `complex.c` | 复数运算 | `complex_add`, `complex_mul` |

### 信号处理

| 头文件 | 描述 | 关键 API |
|--------|------|----------|
| `fast_fourier_transform.h` / `fast_fourier_transform.c` | FFT | `fft_compute`, `ifft_compute` |
| `kalman_scalar.h` / `kalman_scalar.c` | 卡尔曼滤波 | `kalman_scalar_init`, `kalman_scalar_update` |
| `pid_controller.h` / `pid_controller.c` | PID 控制器 | `pid_init`, `pid_update` |

### 统计

| 头文件 | 描述 | 关键 API |
|--------|------|----------|
| `stats.h` / `stats.c` | 统计分析 | `stats_mean`, `stats_stddev` |
| `math_utils.h` / `math_utils.c` | 数学工具 | `clamp`, `lerp`, `map_range` |

### 随机数

| 头文件 | 描述 | 关键 API |
|--------|------|----------|
| `random.h` / `random.c` | 随机数生成 | `random_int`, `random_float` |

---

## 实用工具

### 日志与调试

| 头文件 | 描述 | 关键 API |
|--------|------|----------|
| `log.h` / `log.c` | 日志系统 | `LOGI`, `LOGW`, `LOGE` |
| `timer.h` / `timer.c` | 定时器 | `timer_set`, `timer_cancel` |
| `stopwatch.h` / `stopwatch.c` | 计时器 | `stopwatch_start`, `stopwatch_elapsed` |

### 命令行与配置

| 头文件 | 描述 | 关键 API |
|--------|------|----------|
| `argparse.h` / `argparse.c` | 参数解析 | `argparse_parse` |
| `cron.h` / `cron.c` | Cron 表达式 | `cron_match` |

### 标识与版本

| 头文件 | 描述 | 关键 API |
|--------|------|----------|
| `uuid.h` / `uuid.c` | UUID v4 | `uuid_generate` |
| `uuid_v4_fast.h` / `uuid_v4_fast.c` | 快速 UUID v4 | `uuid_v4_fast_generate` |
| `uuid_v7.h` / `uuid_v7.c` | UUID v7 | `uuid_v7_generate` |
| `version.h` / `version.c` | 版本解析 | `version_parse`, `version_compare` |
| `version_compare.h` / `version_compare.c` | 版本比较 | `version_compare_str` |

### 系统工具

| 头文件 | 描述 | 关键 API |
|--------|------|----------|
| `terminal.h` / `terminal.c` | 终端控制 | `term_clear`, `term_printf` |
| `time_utils.h` / `time_utils.c` | 时间工具 | `time_now_ms`, `time_format` |
| `units.h` / `units.c` | 单位转换 | `units_bytes_to_human` |
| `color.h` / `color.c` | 颜色转换 | `color_hex_to_rgb` |
| `endian_utils.h` / `endian_utils.c` | 字节序转换 | `endian_swap16`, `endian_swap32` |

### 数据结构与算法工具

| 头文件 | 描述 | 关键 API |
|--------|------|----------|
| `bitstream.h` / `bitstream.c` | 位流读写 | `bitstream_read`, `bitstream_write` |
| `consistent_hash.h` / `consistent_hash.c` | 一致性哈希 | `ch_get_node` |
| `rate_limiter_leaky.h` / `rate_limiter_leaky.c` | 漏桶限流 | `leaky_consume` |
| `retry.h` / `retry.c` | 重试机制 | `retry_run` |
| `fsm.h` / `fsm.c` | 有限状态机 | `fsm_handle_event` |

### 测试工具

| 头文件 | 描述 | 关键 API |
|--------|------|----------|
| `utest.h` | 单元测试框架 | `TEST`, `EXPECT_EQ`, `EXPECT_TRUE` |

---

## 项目结构

```
c_utils/
├── *.h          # 头文件
├── *.c          # 源文件
tests/
├── test_*.c     # 单元测试
├── utest.h      # 测试框架
build/
├── CMakeFiles/  # CMake 构建文件
├── lib/         # 生成的库
└── test_*       # 测试可执行文件
```

## 构建选项

```bash
# 调试构建
cmake -DCMAKE_BUILD_TYPE=Debug ..

# 发布构建
cmake -DCMAKE_BUILD_TYPE=Release ..

# 禁用测试
cmake -DBUILD_TESTS=OFF ..

# 安装
make install
```

## 依赖

- C11 编译器
- CMake 3.10+
- pthread (线程相关功能)
- math library (数学相关功能)

## 许可证

MIT License - 详见 LICENSE 文件

## 贡献

欢迎提交 Issue 和 Pull Request！

## 最近更新

### v2.0 - 接口完善版

- **list** - 新增 `insert`, `remove`, `find`, `sort`, `reverse`, `foreach` 等完整操作
- **avl** - 新增 `delete`, `min`, `max`, `inorder/preorder/postorder` 遍历、迭代器支持
- **rbtree** - 新增 `contains`, `min`, `max`, `inorder/preorder/postorder` 遍历、迭代器支持
- **ini** - 新增 `set`, `save`, `delete`, `foreach` 读写支持
- **threadpool** - 新增优先级任务、等待完成、动态调整线程数、暂停/恢复功能

## 测试状态

```
149/149 tests passed (100%)
```

所有单元测试均通过，确保代码质量和稳定性。
