# 基于 c\_utils 库的项目建议 Spec

## Why

c\_utils库提供了丰富的功能模块，用户希望了解可以基于这些模块构建哪些实际项目，以便选择合适的方向进行开发学习。

## c\_utils 库功能分类

### 1. 数据结构与算法

* **数据结构**: list, hashmap, rbtree, avl, skiplist, trie, heap, bplus\_tree, lru\_cache, ringbuf, bitset

* **图算法**: dijkstra, astar, bellman\_ford, floyd\_warshall, kruskal, prim, tarjan\_scc, topological\_sort

* **字符串算法**: kmp, rabin\_karp, manacher, suffix\_array, z\_algorithm, levenshtein

* **排序搜索**: sort, sort\_heap, sort\_merge, sort\_utils, binary\_search

### 2. 编码与解析

* **编码**: base16, base32, base58, base64, url\_codec, html\_codec, punycode

* **格式解析**: json, json\_writer, xml\_parse, toml\_parse, csv, csv\_writer, ini, config

* **压缩**: huffman, lzw, rle, delta\_encoding

### 3. 加密与安全

* **加密**: aes\_tiny, chacha20\_tiny, rsa\_tiny, poly1305\_tiny

* **哈希**: md5, sha1, sha256\_tiny, adler32, crc32, hmac, pbkdf2

* **其他**: uuid, uuid\_v4\_fast, uuid\_v7, otp, bigint

### 4. 网络与协议

* **协议**: http, http\_parser\_tiny, websocket\_frame, mqtt\_pkt, coap\_pkt, dns\_pkt

* **网络工具**: net, url

### 5. 系统与并发

* **并发**: threadpool, thread\_local\_storage, sem, shm, process, pipe

* **系统**: timer, terminal, file\_watcher, cpu\_usage, cpu\_affinity, env, dynamic\_lib

* **日志**: log, log\_rotate, backtrace

### 6. 数学与科学

* **数学**: matrix, complex, quaternion, vector3, math\_utils, fast\_fourier\_transform

* **统计**: stats, kalman\_scalar, pid\_controller

* **单位**: units

### 7. 工具与实用程序

* **时间**: time\_utils, timer, stopwatch, cron

* **文件**: fs\_utils, path, mmap, page\_allocator, wav\_header, bmp\_tiny

* **内存**: arena, memory\_pool\_fixed

* **其他**: argparse, random, retry, rate\_limiter\_leaky, consistent\_hash

***

## 推荐项目列表

### 项目零：目录管理工具 🆕

**难度**: ⭐⭐\
**使用模块**: fs\_utils, path, argparse, terminal, log, hashmap, json

**功能描述**:

* 目录浏览和导航

* 目录创建、删除、重命名

* 目录大小统计

* 目录树显示

* 目录比较和同步

* 收藏夹功能（保存常用目录）

* 搜索文件/目录

**学习价值**: 文件系统操作、路径处理、命令行界面设计

**实现步骤**:

1. 实现基本的目录浏览功能
2. 添加目录操作（创建、删除、重命名）
3. 实现目录大小统计
4. 添加目录树显示功能
5. 实现收藏夹功能
6. 添加搜索功能

***

### 项目一：命令行任务管理器

**难度**: ⭐⭐\
**使用模块**: argparse, json, hashmap, list, time\_utils, log

**功能描述**:

* 添加、删除、修改任务

* 任务优先级和截止日期

* 任务分类和标签

* 数据持久化（JSON格式）

* 命令行交互界面

**学习价值**: 掌握数据结构、文件操作、命令行参数解析

***

### 项目二：简单的HTTP静态文件服务器

**难度**: ⭐⭐⭐\
**使用模块**: http, http\_parser\_tiny, fs\_utils, path, threadpool

**功能描述**:

* 监听指定端口

* 处理GET请求返回静态文件

* 支持多线程并发

* 基本的MIME类型识别

* 日志记录

**学习价值**: 网络编程、HTTP协议、多线程

***

### 项目三：日志分析工具

**难度**: ⭐⭐\
**使用模块**: regex\_tiny, csv, csv\_writer, stats, time\_utils, sort, hashmap

**功能描述**:

* 解析各种格式的日志文件

* 统计错误类型和频率

* 时间范围过滤

* 生成统计报告（CSV格式）

* 关键词搜索

**学习价值**: 文本处理、正则表达式、数据分析

***

### 项目四：配置文件管理器

**难度**: ⭐⭐\
**使用模块**: ini, json, toml\_parse, config, argparse, fs\_utils

**功能描述**:

* 支持多种配置格式（INI/JSON/TOML）

* 配置文件格式转换

* 配置项查询和修改

* 配置验证

* 批量配置管理

**学习价值**: 配置管理、文件格式处理

***

### 项目五：简单的键值存储数据库

**难度**: ⭐⭐⭐\
**使用模块**: hashmap, rbtree, lru\_cache, json, fs\_utils, threadpool, sem

**功能描述**:

* 内存键值存储

* 持久化到磁盘

* 支持过期时间

* 简单的命令协议

* 并发访问控制

**学习价值**: 数据库原理、并发控制、持久化

***

### 项目六：文件加密工具

**难度**: ⭐⭐\
**使用模块**: aes\_tiny, sha256\_tiny, hmac, pbkdf2, fs\_utils, argparse

**功能描述**:

* 文件加密/解密

* 密码派生

* 文件完整性校验

* 批量加密

* 命令行界面

**学习价值**: 加密算法、安全编程

***

### 项目七：网络爬虫框架

**难度**: ⭐⭐⭐⭐\
**使用模块**: http, http\_parser\_tiny, url\_codec, html\_codec, regex\_tiny, json, threadpool, hashmap

**功能描述**:

* URL队列管理

* 并发下载

* HTML解析

* 数据提取

* 结果存储

**学习价值**: 网络编程、HTML解析、并发设计

***

### 项目八：实时聊天服务器

**难度**: ⭐⭐⭐⭐\
**使用模块**: websocket\_frame, threadpool, hashmap, json, sha256\_tiny, uuid

**功能描述**:

* WebSocket协议支持

* 多房间/频道

* 用户认证

* 消息广播

* 聊天记录

**学习价值**: WebSocket协议、实时通信、用户管理

***

### 项目九：数据压缩工具

**难度**: ⭐⭐⭐\
**使用模块**: huffman, lzw, rle, delta\_encoding, bitstream, fs\_utils

**功能描述**:

* 多种压缩算法支持

* 压缩率比较

* 文件/目录压缩

* 解压缩功能

* 命令行界面

**学习价值**: 压缩算法、位操作、文件处理

***

### 项目十：进程监控工具

**难度**: ⭐⭐⭐\
**使用模块**: process, cpu\_usage, timer, log, log\_rotate, terminal, argparse

**功能描述**:

* 进程状态监控

* CPU/内存使用统计

* 进程启停控制

* 告警通知

* 日志记录

**学习价值**: 系统编程、进程管理、监控

***

### 项目十一：简单的MQTT客户端

**难度**: ⭐⭐⭐\
**使用模块**: mqtt\_pkt, net, threadpool, log, timer

**功能描述**:

* MQTT协议实现

* 发布/订阅功能

* QoS支持

* 心跳保活

* 命令行界面

**学习价值**: MQTT协议、物联网通信

***

### 项目十二：缓存服务器

**难度**: ⭐⭐⭐\
**使用模块**: lru\_cache, hashmap, threadpool, json, timer, stats

**功能描述**:

* 内存缓存

* LRU淘汰策略

* 过期时间支持

* 统计信息

* 简单协议

**学习价值**: 缓存设计、性能优化

***

### 项目十三：文本差异比较工具

**难度**: ⭐⭐\
**使用模块**: levenshtein, terminal, argparse, fs\_utils

**功能描述**:

* 文件差异比较

* 行级/字符级比较

* 差异高亮显示

* 合并功能

* 目录比较

**学习价值**: 字符串算法、终端输出

***

### 项目十四：定时任务调度器

**难度**: ⭐⭐⭐\
**使用模块**: cron, timer, threadpool, process, log, argparse

**功能描述**:

* Cron表达式解析

* 定时执行命令

* 任务历史记录

* 错误重试

* 任务依赖

**学习价值**: 时间调度、任务管理

***

### 项目十五：简单的搜索引擎

**难度**: ⭐⭐⭐⭐\
**使用模块**: trie, hashmap, sort, kmp, rabin\_karp, json, fs\_utils, threadpool

**功能描述**:

* 文档索引

* 关键词搜索

* 模糊匹配

* 搜索结果排序

* 命令行界面

**学习价值**: 信息检索、索引结构

***

## 项目选择建议

### 初学者推荐

1. **目录管理工具** 🆕 - 文件系统操作入门
2. 命令行任务管理器
3. 配置文件管理器
4. 文件加密工具

### 进阶开发者推荐

1. HTTP静态文件服务器
2. 键值存储数据库
3. 数据压缩工具
4. 进程监控工具

### 高级开发者推荐

1. 网络爬虫框架
2. 实时聊天服务器
3. 简单的搜索引擎
4. MQTT客户端

***

## 下一步行动

用户选择感兴趣的项目后，可以：

1. 创建详细的项目spec文档
2. 分解任务列表
3. 逐步实现功能模块

