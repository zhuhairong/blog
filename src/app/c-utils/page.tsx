import fs from 'fs';
import path from 'path';
import CodeViewer from './CodeViewer';

const C_UTILS_DIR = path.join(process.cwd(), 'c_utils', 'c_utils');
const TESTS_DIR = path.join(process.cwd(), 'c_utils', 'tests');
const DEMO_DIR = path.join(process.cwd(), 'c_utils', 'demo');

interface CodeFile {
  name: string;
  type: 'header' | 'source';
  category: string;
  description: string;
  headerContent: string;
  sourceContent: string | null;
  testContent: string | null;
  demoContent: string | null;
}

interface CodeFilesData {
  codeFiles: CodeFile[];
  categories: string[];
}

function getCodeFiles(): CodeFilesData {
  const files: CodeFile[] = [];
  
  if (!fs.existsSync(C_UTILS_DIR)) {
    return { codeFiles: [], categories: [] };
  }
  
  const allFiles = fs.existsSync(C_UTILS_DIR) ? fs.readdirSync(C_UTILS_DIR) : [];
  const testFiles = fs.existsSync(TESTS_DIR) ? fs.readdirSync(TESTS_DIR) : [];
  const demoFiles = fs.existsSync(DEMO_DIR) ? fs.readdirSync(DEMO_DIR) : [];
  const headers = allFiles.filter(f => f.endsWith('.h'));
  
  const categories: Record<string, string[]> = {
    '数据结构': ['dstr', 'vec', 'list', 'hashmap', 'rbtree', 'avl', 'skiplist', 'trie', 'ringbuf', 'ringbuffer', 'bitset', 'heap', 'bloom', 'disjoint_set', 'union_find', 'segment_tree', 'fenwick_tree', 'sparse_table', 'lru_cache', 'kv_store', 'bplus_tree'],
    '算法': ['sort', 'binary_search', 'kmp', 'rabin_karp', 'z_algorithm', 'manacher', 'levenshtein', 'suffix_array', 'glob_match', 'regex_tiny', 'astar', 'dijkstra', 'bellman_ford', 'floyd_warshall', 'prim', 'kruskal', 'topological_sort', 'tarjan_scc', 'convex_hull', 'line_intersection'],
    '编码': ['base16', 'base32', 'base58', 'base64', 'url_codec', 'html_codec', 'punycode', 'utf8', 'soundex', 'varint', 'zigzag', 'rle', 'lzw', 'huffman', 'delta_encoding'],
    '加密': ['md5', 'sha1', 'sha256', 'aes_tiny', 'chacha20_tiny', 'rsa_tiny', 'hmac', 'poly1305_tiny', 'crc32', 'adler32', 'otp', 'pbkdf2', 'crypto'],
    '系统': ['threadpool', 'process', 'memory_pool', 'page_allocator', 'arena', 'shm', 'mmap', 'sem', 'pipe', 'lockfile', 'cpu_affinity', 'cpu_usage', 'backtrace', 'dynamic_lib', 'env', 'signal_utils', 'thread_local_storage', 'timer', 'stopwatch'],
    '网络': ['http', 'http_parser_tiny', 'websocket_frame', 'mqtt_pkt', 'coap_pkt', 'net', 'dns_pkt', 'url', 'slip', 'cobs'],
    '格式': ['json', 'json_writer', 'ini', 'toml_parse', 'config', 'csv', 'csv_writer', 'xml_parse', 'wav_header', 'bmp_tiny'],
    '文件': ['fs_utils', 'path', 'glob', 'file_watcher', 'log_rotate'],
    '数学': ['bigint', 'matrix', 'vector3', 'quaternion', 'complex', 'fast_fourier_transform', 'kalman_scalar', 'pid_controller', 'stats', 'math_utils', 'random'],
    '工具': ['log', 'argparse', 'cron', 'uuid', 'version', 'version_compare', 'terminal', 'time_utils', 'units', 'color', 'endian_utils', 'bitstream', 'consistent_hash', 'rate_limiter_leaky', 'retry', 'fsm'],
  };
  
  for (const header of headers) {
    const baseName = header.replace('.h', '');
    const sourceFile = baseName + '.c';
    const testFile = 'test_' + baseName + '.c';
    
    const hasSource = allFiles.includes(sourceFile);
    const hasTest = testFiles.includes(testFile);
    const demoFile = 'demo_' + baseName + '.c';
    const hasDemo = demoFiles.includes(demoFile);
    
    let category = '其他';
    for (const [cat, prefixes] of Object.entries(categories)) {
      if (prefixes.some(p => baseName.startsWith(p))) {
        category = cat;
        break;
      }
    }
    
    const headerPath = path.join(C_UTILS_DIR, header);
    const sourcePath = path.join(C_UTILS_DIR, sourceFile);
    const testPath = path.join(TESTS_DIR, testFile);
    const demoPath = path.join(DEMO_DIR, demoFile);
    
    let headerContent = '';
    let sourceContent: string | null = null;
    let testContent: string | null = null;
    let demoContent: string | null = null;
    
    try {
      headerContent = fs.readFileSync(headerPath, 'utf-8');
    } catch (e) {
      headerContent = '// 无法读取文件';
    }
    
    if (hasSource) {
      try {
        sourceContent = fs.readFileSync(sourcePath, 'utf-8');
      } catch (e) {
        sourceContent = null;
      }
    }
    
    if (hasTest) {
      try {
        testContent = fs.readFileSync(testPath, 'utf-8');
      } catch (e) {
        testContent = null;
      }
    }
    
    if (hasDemo) {
      try {
        demoContent = fs.readFileSync(demoPath, 'utf-8');
      } catch (e) {
        demoContent = null;
      }
    }
    
    files.push({
      name: baseName,
      type: 'header',
      category,
      description: getModuleDescription(baseName),
      headerContent,
      sourceContent,
      testContent,
      demoContent,
    });
  }
  
  const sortedFiles = files.sort((a, b) => a.category.localeCompare(b.category) || a.name.localeCompare(b.name));
  const uniqueCategories = [...new Set(sortedFiles.map(f => f.category))];
  
  return { codeFiles: sortedFiles, categories: uniqueCategories };
}

function getModuleDescription(name: string): string {
  const descriptions: Record<string, string> = {
    'dstr': '动态字符串',
    'vec': '泛型动态数组',
    'list': '双向链表',
    'hashmap': '字符串键哈希表',
    'rbtree': '红黑树',
    'avl': 'AVL 平衡树',
    'skiplist': '跳表',
    'trie': '前缀树',
    'ringbuf': '字节环形缓冲区',
    'ringbuffer': '对象环形缓冲区',
    'bitset': '位图',
    'bitset_compressed': '压缩位图',
    'heap': '堆',
    'bloom': '布隆过滤器',
    'bloom_filter_counting': '计数布隆过滤器',
    'disjoint_set_forest': '并查集（森林）',
    'union_find': '并查集',
    'segment_tree': '线段树',
    'fenwick_tree': '树状数组',
    'sparse_table': '稀疏表',
    'lru_cache': 'LRU 缓存',
    'kv_store': '键值存储',
    'bplus_tree': 'B+ 树',
    'sort': '快速排序',
    'sort_heap': '堆排序',
    'sort_merge': '归并排序',
    'sort_utils': '排序工具',
    'binary_search': '二分查找',
    'kmp': 'KMP 字符串匹配',
    'rabin_karp': 'Rabin-Karp 算法',
    'z_algorithm': 'Z 算法',
    'manacher': 'Manacher 回文算法',
    'levenshtein': '编辑距离',
    'suffix_array': '后缀数组',
    'glob_match': 'Glob 模式匹配',
    'regex_tiny': '极简正则表达式',
    'astar': 'A* 寻路算法',
    'dijkstra': 'Dijkstra 最短路径',
    'bellman_ford': 'Bellman-Ford 算法',
    'floyd_warshall': 'Floyd-Warshall 算法',
    'prim': 'Prim 最小生成树',
    'kruskal': 'Kruskal 最小生成树',
    'topological_sort': '拓扑排序',
    'tarjan_scc': 'Tarjan 强连通分量',
    'convex_hull': '凸包算法',
    'line_intersection': '线段交点',
    'base16': 'Base16 (Hex) 编码',
    'base32': 'Base32 编码',
    'base32_hex': 'Base32 Hex 编码',
    'base58': 'Base58 编码',
    'base64': 'Base64 编码',
    'url_codec': 'URL 编解码',
    'html_codec': 'HTML 实体编解码',
    'punycode': 'Punycode 编码',
    'utf8': 'UTF-8 工具',
    'soundex': 'Soundex 算法',
    'varint': '变长整数编码',
    'zigzag': 'ZigZag 编码',
    'rle': '行程长度编码',
    'lzw': 'LZW 压缩',
    'huffman': '哈夫曼编码',
    'delta_encoding': '差分编码',
    'run_length_limited': 'RLL 编码',
    'md5': 'MD5 哈希',
    'sha1': 'SHA-1 哈希',
    'sha256_tiny': 'SHA-256 哈希',
    'aes_tiny': 'AES 加密',
    'chacha20_tiny': 'ChaCha20 流加密',
    'rsa_tiny': 'RSA 模幂运算',
    'hmac': 'HMAC 消息认证',
    'poly1305_tiny': 'Poly1305',
    'crc32': 'CRC32 校验',
    'adler32': 'Adler32 校验',
    'otp': 'OTP (TOTP/HOTP)',
    'pbkdf2': 'PBKDF2 密钥派生',
    'crypto': '通用加密接口',
    'threadpool': '线程池',
    'process': '进程操作',
    'memory_pool_fixed': '固定大小内存池',
    'page_allocator': '页分配器',
    'arena': '内存区域分配器',
    'shm': '共享内存',
    'mmap': '内存映射',
    'sem': '信号量',
    'pipe': '管道',
    'lockfile': '文件锁',
    'cpu_affinity': 'CPU 亲和性',
    'cpu_usage': 'CPU 使用率',
    'backtrace': '堆栈回溯',
    'dynamic_lib': '动态库加载',
    'env': '环境变量',
    'signal_utils': '信号处理',
    'thread_local_storage': '线程本地存储',
    'timer': '定时器',
    'stopwatch': '计时器',
    'http': 'HTTP 基础',
    'http_parser_tiny': '极简 HTTP 解析',
    'websocket_frame': 'WebSocket 帧',
    'mqtt_pkt': 'MQTT 数据包',
    'coap_pkt': 'CoAP 数据包',
    'net': '网络工具',
    'dns_pkt': 'DNS 数据包',
    'url': 'URL 解析',
    'slip': 'SLIP 编码',
    'cobs': 'COBS 编码',
    'json': 'JSON 解析器',
    'json_writer': 'JSON 写入器',
    'ini': 'INI 解析与写入',
    'toml_parse': 'TOML 解析',
    'config': '简单配置',
    'csv': 'CSV 解析',
    'csv_writer': 'CSV 写入',
    'xml_parse': 'XML 解析',
    'wav_header': 'WAV 文件头',
    'bmp_tiny': 'BMP 图像',
    'fs_utils': '文件系统工具',
    'path': '路径操作',
    'glob': '通配符匹配',
    'file_watcher': '文件监控',
    'log_rotate': '日志轮转',
    'bigint': '大整数运算',
    'matrix': '矩阵运算',
    'vector3': '3D 向量',
    'quaternion': '四元数',
    'complex': '复数运算',
    'fast_fourier_transform': 'FFT 快速傅里叶变换',
    'kalman_scalar': '卡尔曼滤波',
    'pid_controller': 'PID 控制器',
    'stats': '统计分析',
    'math_utils': '数学工具',
    'random': '随机数生成',
    'log': '日志系统',
    'argparse': '参数解析',
    'cron': 'Cron 表达式',
    'uuid': 'UUID v4',
    'uuid_v4_fast': '快速 UUID v4',
    'uuid_v7': 'UUID v7',
    'version': '版本解析',
    'version_compare': '版本比较',
    'terminal': '终端控制',
    'time_utils': '时间工具',
    'units': '单位转换',
    'color': '颜色转换',
    'endian_utils': '字节序转换',
    'bitstream': '位流读写',
    'consistent_hash': '一致性哈希',
    'rate_limiter_leaky': '漏桶限流',
    'retry': '重试机制',
    'fsm': '有限状态机',
  };
  
  return descriptions[name] || name;
}

export default function CUtilsPage() {
  const { codeFiles, categories } = getCodeFiles();
  
  return <CodeViewer codeFiles={codeFiles} categories={categories} />;
}
