cmake_minimum_required(VERSION 3.10)
project(c_utils VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ============================================================================
# 构建选项
# ============================================================================
option(ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)
option(BUILD_DEBUG "Enable debug build (-g -O0)" OFF)

# Debug 构建
if(BUILD_DEBUG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0")
    message(STATUS "Debug build enabled (-g -O0)")
endif()

# AddressSanitizer
if(ENABLE_ASAN)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fsanitize=address")
    message(STATUS "AddressSanitizer enabled")
endif()

# UndefinedBehaviorSanitizer
if(ENABLE_UBSAN)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=undefined -fno-omit-frame-pointer")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=undefined")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fsanitize=undefined")
    message(STATUS "UndefinedBehaviorSanitizer enabled")
endif()

# 启用复数支持
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_Complex_I=1.0fi -I/usr/include")

# ============================================================================
# 库源文件（显式列出，确保新增/删除文件后 CMake 能正确重新配置）
# ============================================================================

# c_utils 库源文件
set(C_UTILS_SOURCES
    c_utils/adler32.c
    c_utils/aes_tiny.c
    c_utils/arena.c
    c_utils/argparse.c
    c_utils/astar.c
    c_utils/avl.c
    c_utils/backtrace.c
    c_utils/base16.c
    c_utils/base32.c
    c_utils/base32_hex.c
    c_utils/base58.c
    c_utils/base64.c
    c_utils/bellman_ford.c
    c_utils/bigint.c
    c_utils/binary_search.c
    c_utils/bitset.c
    c_utils/bitset_compressed.c
    c_utils/bitstream.c
    c_utils/bloom.c
    c_utils/bloom_filter_counting.c
    c_utils/bmp_tiny.c
    c_utils/bplus_tree.c
    c_utils/c_utils.c
    c_utils/chacha20_tiny.c
    c_utils/cjson_wrapper.c
    c_utils/coap_pkt.c
    c_utils/cobs.c
    c_utils/color.c
    c_utils/complex.c
    c_utils/config.c
    c_utils/consistent_hash.c
    c_utils/convex_hull.c
    c_utils/cpu_affinity.c
    c_utils/cpu_usage.c
    c_utils/crc32.c
    c_utils/cron.c
    c_utils/crypto.c
    c_utils/csv.c
    c_utils/csv_writer.c
    c_utils/daemon.c
    c_utils/delta_encoding.c
    c_utils/dijkstra.c
    c_utils/disjoint_set_forest.c
    c_utils/dns_pkt.c
    c_utils/dstr.c
    c_utils/dynamic_lib.c
    c_utils/endian_utils.c
    c_utils/env.c
    c_utils/fast_fourier_transform.c
    c_utils/fenwick_tree.c
    c_utils/file_watcher.c
    c_utils/floyd_warshall.c
    c_utils/fs_utils.c
    c_utils/fsm.c
    c_utils/glob.c
    c_utils/glob_match.c
    c_utils/hashmap.c
    c_utils/heap.c
    c_utils/hmac.c
    c_utils/html_codec.c
    c_utils/http.c
    c_utils/http_parser_tiny.c
    c_utils/huffman.c
    c_utils/ini.c
    c_utils/json.c
    c_utils/json_writer.c
    c_utils/kalman_scalar.c
    c_utils/kmp.c
    c_utils/kruskal.c
    c_utils/kv_store.c
    c_utils/levenshtein.c
    c_utils/line_intersection.c
    c_utils/list.c
    c_utils/lockfile.c
    c_utils/log.c
    c_utils/log_rotate.c
    c_utils/lru_cache.c
    c_utils/lzw.c
    c_utils/manacher.c
    c_utils/math_utils.c
    c_utils/matrix.c
    c_utils/md5.c
    c_utils/memory_pool_fixed.c
    c_utils/mmap.c
    c_utils/mqtt_pkt.c
    c_utils/mysql_wrapper.c
    c_utils/nanopb_wrapper.c
    c_utils/net.c
    c_utils/otp.c
    c_utils/page_allocator.c
    c_utils/path.c
    c_utils/pbkdf2.c
    c_utils/pid_controller.c
    c_utils/pipe.c
    c_utils/poly1305_tiny.c
    c_utils/prim.c
    c_utils/process.c
    c_utils/punycode.c
    c_utils/quaternion.c
    c_utils/rabin_karp.c
    c_utils/random.c
    c_utils/rate_limiter_leaky.c
    c_utils/rbtree.c
    c_utils/regex_tiny.c
    c_utils/retry.c
    c_utils/ringbuf.c
    c_utils/ringbuffer.c
    c_utils/rle.c
    c_utils/rsa_tiny.c
    c_utils/run_length_limited.c
    c_utils/segment_tree.c
    c_utils/sem.c
    c_utils/sha1.c
    c_utils/sha256_tiny.c
    c_utils/shm.c
    c_utils/signal_utils.c
    c_utils/skiplist.c
    c_utils/slip.c
    c_utils/sort.c
    c_utils/sort_heap.c
    c_utils/sort_merge.c
    c_utils/sort_utils.c
    c_utils/soundex.c
    c_utils/sparse_table.c
    c_utils/stats.c
    c_utils/stopwatch.c
    c_utils/suffix_array.c
    c_utils/tarjan_scc.c
    c_utils/terminal.c
    c_utils/thread_local_storage.c
    c_utils/threadpool.c
    c_utils/time_utils.c
    c_utils/timer.c
    c_utils/toml_parse.c
    c_utils/topological_sort.c
    c_utils/trie.c
    c_utils/union_find.c
    c_utils/units.c
    c_utils/url.c
    c_utils/url_codec.c
    c_utils/utf8.c
    c_utils/uuid.c
    c_utils/uuid_v4_fast.c
    c_utils/uuid_v7.c
    c_utils/varint.c
    c_utils/vec.c
    c_utils/vector3.c
    c_utils/version.c
    c_utils/version_compare.c
    c_utils/wav_header.c
    c_utils/websocket_frame.c
    c_utils/xml_parse.c
    c_utils/z_algorithm.c
    c_utils/zigzag.c
    c_utils/zip_wrapper.c
)

# 第三方库源文件
set(THIRD_PARTY_SOURCES
    thirdparty/cjson/cJSON.c
    thirdparty/miniz/miniz.c
    thirdparty/miniz/miniz_tdef.c
    thirdparty/miniz/miniz_tinfl.c
    thirdparty/miniz/miniz_zip.c
    thirdparty/nanopb/pb_common.c
    thirdparty/nanopb/pb_decode.c
    thirdparty/nanopb/pb_encode.c
)
list(APPEND C_UTILS_SOURCES ${THIRD_PARTY_SOURCES})

# 创建静态库
add_library(c_utils STATIC ${C_UTILS_SOURCES})

# 设置头文件包含目录
target_include_directories(c_utils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/c_utils
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/cjson
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/nanopb
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/miniz
)

# 链接数学库
find_library(MATH_LIB m)
if(MATH_LIB)
    target_link_libraries(c_utils PUBLIC ${MATH_LIB})
endif()

# 链接 pthread 库
find_package(Threads REQUIRED)
target_link_libraries(c_utils PUBLIC Threads::Threads)

# 查找 dl 库
find_library(DL_LIB dl)
if(DL_LIB)
    target_link_libraries(c_utils PUBLIC ${DL_LIB})
endif()

# 查找 rt 库 (Linux 实时库)
find_library(RT_LIB rt)
if(RT_LIB)
    target_link_libraries(c_utils PUBLIC ${RT_LIB})
endif()

# 查找 MySQL 库
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(MYSQL mysqlclient QUIET)
endif()

if(NOT MYSQL_FOUND)
    find_library(MYSQL_LIB mysqlclient PATHS /usr/lib/mysql /usr/lib/x86_64-linux-gnu)
    find_path(MYSQL_INCLUDE_DIR mysql/mysql.h PATHS /usr/include/mysql /usr/include)
    if(MYSQL_LIB AND MYSQL_INCLUDE_DIR)
        set(MYSQL_FOUND TRUE)
        set(MYSQL_LIBRARIES ${MYSQL_LIB})
        set(MYSQL_INCLUDE_DIRS ${MYSQL_INCLUDE_DIR})
    endif()
endif()

if(MYSQL_FOUND)
    message(STATUS "Found MySQL client library: ${MYSQL_LIBRARIES}")
    target_include_directories(c_utils PUBLIC ${MYSQL_INCLUDE_DIRS})
    target_link_libraries(c_utils PUBLIC ${MYSQL_LIBRARIES})
    target_compile_definitions(c_utils PUBLIC C_UTILS_HAS_MYSQL=1)
else()
    message(STATUS "MySQL client library not found, mysql_wrapper will be disabled")
endif()

# 启用测试
option(BUILD_TESTS "Build tests" ON)

if(BUILD_TESTS)
    enable_testing()

    # 测试文件（显式列出）
    set(TEST_SOURCES
        tests/test_adler32.c
        tests/test_aes_tiny.c
        tests/test_arena.c
        tests/test_argparse.c
        tests/test_astar.c
        tests/test_avl.c
        tests/test_backtrace.c
        tests/test_base16.c
        tests/test_base32.c
        tests/test_base32_hex.c
        tests/test_base58.c
        tests/test_base64.c
        tests/test_bellman_ford.c
        tests/test_bigint.c
        tests/test_binary_search.c
        tests/test_bitset.c
        tests/test_bitset_compressed.c
        tests/test_bitstream.c
        tests/test_bloom.c
        tests/test_bloom_filter_counting.c
        tests/test_bmp_tiny.c
        tests/test_bplus_tree.c
        tests/test_chacha20_tiny.c
        tests/test_cjson.c
        tests/test_coap_pkt.c
        tests/test_cobs.c
        tests/test_color.c
        tests/test_complex.c
        tests/test_config.c
        tests/test_consistent_hash.c
        tests/test_convex_hull.c
        tests/test_cpu_affinity.c
        tests/test_cpu_usage.c
        tests/test_crc32.c
        tests/test_cron.c
        tests/test_crypto.c
        tests/test_csv.c
        tests/test_csv_writer.c
        tests/test_daemon.c
        tests/test_delta_encoding.c
        tests/test_dijkstra.c
        tests/test_disjoint_set_forest.c
        tests/test_dns_pkt.c
        tests/test_dstr.c
        tests/test_dynamic_lib.c
        tests/test_endian_utils.c
        tests/test_env.c
        tests/test_fast_fourier_transform.c
        tests/test_fenwick_tree.c
        tests/test_file_watcher.c
        tests/test_floyd_warshall.c
        tests/test_fs_utils.c
        tests/test_fsm.c
        tests/test_glob.c
        tests/test_glob_match.c
        tests/test_hashmap.c
        tests/test_heap.c
        tests/test_hmac.c
        tests/test_html_codec.c
        tests/test_http.c
        tests/test_http_parser_tiny.c
        tests/test_huffman.c
        tests/test_ini.c
        tests/test_json.c
        tests/test_json_writer.c
        tests/test_kalman_scalar.c
        tests/test_kmp.c
        tests/test_kruskal.c
        tests/test_kv_store.c
        tests/test_levenshtein.c
        tests/test_line_intersection.c
        tests/test_list.c
        tests/test_lockfile.c
        tests/test_log.c
        tests/test_log_rotate.c
        tests/test_lru_cache.c
        tests/test_lzw.c
        tests/test_manacher.c
        tests/test_math_utils.c
        tests/test_matrix.c
        tests/test_md5.c
        tests/test_memory_pool_fixed.c
        tests/test_mmap.c
        tests/test_mqtt_pkt.c
        tests/test_mysql.c
        tests/test_nanopb.c
        tests/test_net.c
        tests/test_otp.c
        tests/test_page_allocator.c
        tests/test_path.c
        tests/test_pbkdf2.c
        tests/test_pid_controller.c
        tests/test_pipe.c
        tests/test_poly1305_tiny.c
        tests/test_prim.c
        tests/test_process.c
        tests/test_punycode.c
        tests/test_quaternion.c
        tests/test_rabin_karp.c
        tests/test_random.c
        tests/test_rate_limiter_leaky.c
        tests/test_rbtree.c
        tests/test_regex_tiny.c
        tests/test_retry.c
        tests/test_ringbuf.c
        tests/test_ringbuffer.c
        tests/test_rle.c
        tests/test_rsa_tiny.c
        tests/test_run_length_limited.c
        tests/test_segment_tree.c
        tests/test_sem.c
        tests/test_sha1.c
        tests/test_sha256_tiny.c
        tests/test_shm.c
        tests/test_signal_utils.c
        tests/test_skiplist.c
        tests/test_slip.c
        tests/test_sort.c
        tests/test_sort_heap.c
        tests/test_sort_merge.c
        tests/test_sort_utils.c
        tests/test_soundex.c
        tests/test_sparse_table.c
        tests/test_stats.c
        tests/test_stopwatch.c
        tests/test_suffix_array.c
        tests/test_tarjan_scc.c
        tests/test_terminal.c
        tests/test_thread_local_storage.c
        tests/test_threadpool.c
        tests/test_time_utils.c
        tests/test_timer.c
        tests/test_toml_parse.c
        tests/test_topological_sort.c
        tests/test_trie.c
        tests/test_union_find.c
        tests/test_units.c
        tests/test_url.c
        tests/test_url_codec.c
        tests/test_utf8.c
        tests/test_uuid.c
        tests/test_uuid_v4_fast.c
        tests/test_uuid_v7.c
        tests/test_varint.c
        tests/test_vec.c
        tests/test_vector3.c
        tests/test_version.c
        tests/test_version_compare.c
        tests/test_wav_header.c
        tests/test_websocket_frame.c
        tests/test_xml_parse.c
        tests/test_z_algorithm.c
        tests/test_zigzag.c
        tests/test_zip.c
    )

    foreach(test_source ${TEST_SOURCES})
        # 获取测试名称 (去掉路径和扩展名)
        get_filename_component(test_name ${test_source} NAME_WE)

        # 创建测试可执行文件
        add_executable(${test_name} ${test_source})

        # 链接 c_utils 库
        target_link_libraries(${test_name} PRIVATE c_utils)

        # 添加测试
        add_test(NAME ${test_name} COMMAND ${test_name})
    endforeach()
endif()

# 构建演示程序
option(BUILD_DEMOS "Build demo programs" ON)

if(BUILD_DEMOS)
    # 演示文件（显式列出）
    set(DEMO_SOURCES
        demo/demo_adler32.c
        demo/demo_aes_tiny.c
        demo/demo_arena.c
        demo/demo_argparse.c
        demo/demo_astar.c
        demo/demo_avl.c
        demo/demo_backtrace.c
        demo/demo_base16.c
        demo/demo_base32.c
        demo/demo_base32_hex.c
        demo/demo_base58.c
        demo/demo_base64.c
        demo/demo_bellman_ford.c
        demo/demo_bigint.c
        demo/demo_binary_search.c
        demo/demo_bitset.c
        demo/demo_bitset_compressed.c
        demo/demo_bitstream.c
        demo/demo_bloom.c
        demo/demo_bloom_filter_counting.c
        demo/demo_bmp_tiny.c
        demo/demo_bplus_tree.c
        demo/demo_chacha20_tiny.c
        demo/demo_cjson_wrapper.c
        demo/demo_coap_pkt.c
        demo/demo_cobs.c
        demo/demo_color.c
        demo/demo_complex.c
        demo/demo_config.c
        demo/demo_consistent_hash.c
        demo/demo_convex_hull.c
        demo/demo_cpu_affinity.c
        demo/demo_cpu_usage.c
        demo/demo_crc32.c
        demo/demo_cron.c
        demo/demo_crypto.c
        demo/demo_csv.c
        demo/demo_csv_writer.c
        demo/demo_daemon.c
        demo/demo_delta_encoding.c
        demo/demo_dijkstra.c
        demo/demo_disjoint_set_forest.c
        demo/demo_dns_pkt.c
        demo/demo_dstr.c
        demo/demo_dynamic_lib.c
        demo/demo_endian_utils.c
        demo/demo_env.c
        demo/demo_fast_fourier_transform.c
        demo/demo_fenwick_tree.c
        demo/demo_file_watcher.c
        demo/demo_floyd_warshall.c
        demo/demo_fs_utils.c
        demo/demo_fsm.c
        demo/demo_glob.c
        demo/demo_glob_match.c
        demo/demo_hashmap.c
        demo/demo_heap.c
        demo/demo_hmac.c
        demo/demo_html_codec.c
        demo/demo_http.c
        demo/demo_http_parser_tiny.c
        demo/demo_huffman.c
        demo/demo_ini.c
        demo/demo_json.c
        demo/demo_json_writer.c
        demo/demo_kalman_scalar.c
        demo/demo_kmp.c
        demo/demo_kruskal.c
        demo/demo_kv_store.c
        demo/demo_levenshtein.c
        demo/demo_line_intersection.c
        demo/demo_list.c
        demo/demo_lockfile.c
        demo/demo_log.c
        demo/demo_log_rotate.c
        demo/demo_lru_cache.c
        demo/demo_lzw.c
        demo/demo_manacher.c
        demo/demo_math_utils.c
        demo/demo_matrix.c
        demo/demo_md5.c
        demo/demo_memory_pool_fixed.c
        demo/demo_mmap.c
        demo/demo_mqtt_pkt.c
        demo/demo_mysql_wrapper.c
        demo/demo_nanopb_wrapper.c
        demo/demo_net.c
        demo/demo_otp.c
        demo/demo_page_allocator.c
        demo/demo_path.c
        demo/demo_pbkdf2.c
        demo/demo_pid_controller.c
        demo/demo_pipe.c
        demo/demo_poly1305_tiny.c
        demo/demo_prim.c
        demo/demo_process.c
        demo/demo_punycode.c
        demo/demo_quaternion.c
        demo/demo_rabin_karp.c
        demo/demo_random.c
        demo/demo_rate_limiter_leaky.c
        demo/demo_rbtree.c
        demo/demo_regex_tiny.c
        demo/demo_retry.c
        demo/demo_ringbuf.c
        demo/demo_ringbuffer.c
        demo/demo_rle.c
        demo/demo_rsa_tiny.c
        demo/demo_run_length_limited.c
        demo/demo_segment_tree.c
        demo/demo_sem.c
        demo/demo_sha1.c
        demo/demo_sha256_tiny.c
        demo/demo_shm.c
        demo/demo_signal_utils.c
        demo/demo_skiplist.c
        demo/demo_slip.c
        demo/demo_sort.c
        demo/demo_sort_heap.c
        demo/demo_sort_merge.c
        demo/demo_sort_utils.c
        demo/demo_soundex.c
        demo/demo_sparse_table.c
        demo/demo_stats.c
        demo/demo_stopwatch.c
        demo/demo_suffix_array.c
        demo/demo_tarjan_scc.c
        demo/demo_terminal.c
        demo/demo_thread_local_storage.c
        demo/demo_threadpool.c
        demo/demo_time_utils.c
        demo/demo_timer.c
        demo/demo_toml_parse.c
        demo/demo_topological_sort.c
        demo/demo_trie.c
        demo/demo_union_find.c
        demo/demo_units.c
        demo/demo_url.c
        demo/demo_url_codec.c
        demo/demo_utf8.c
        demo/demo_uuid.c
        demo/demo_uuid_v4_fast.c
        demo/demo_uuid_v7.c
        demo/demo_varint.c
        demo/demo_vec.c
        demo/demo_vector3.c
        demo/demo_version.c
        demo/demo_version_compare.c
        demo/demo_wav_header.c
        demo/demo_websocket_frame.c
        demo/demo_xml_parse.c
        demo/demo_z_algorithm.c
        demo/demo_zigzag.c
        demo/demo_zip_wrapper.c
    )

    foreach(demo_source ${DEMO_SOURCES})
        # 获取演示名称 (去掉路径和扩展名)
        get_filename_component(demo_name ${demo_source} NAME_WE)

        # 创建演示可执行文件
        add_executable(${demo_name} ${demo_source})

        # 链接 c_utils 库
        target_link_libraries(${demo_name} PRIVATE c_utils)
    endforeach()
endif()

# 构建项目
option(BUILD_PROJECTS "Build project programs" ON)

if(BUILD_PROJECTS)
    # 日志分析工具
    add_executable(log_analyzer 
        ${CMAKE_CURRENT_SOURCE_DIR}/projects/log_analyzer/log_analyzer.c
    )
    target_link_libraries(log_analyzer PRIVATE c_utils)

    # 进程监控工具
    add_executable(process_monitor 
        ${CMAKE_CURRENT_SOURCE_DIR}/projects/process_monitor/process_monitor.c
    )
    target_link_libraries(process_monitor PRIVATE c_utils)

    # 任务管理器
    add_executable(task_manager 
        ${CMAKE_CURRENT_SOURCE_DIR}/projects/task_manager/task_manager.c
    )
    target_link_libraries(task_manager PRIVATE c_utils)

    # 文本差异比较工具
    add_executable(text_diff 
        ${CMAKE_CURRENT_SOURCE_DIR}/projects/text_diff/text_diff.c
    )
    target_link_libraries(text_diff PRIVATE c_utils)

    # 缓存服务器
    add_executable(cache_server 
        ${CMAKE_CURRENT_SOURCE_DIR}/projects/cache_server/cache_server.c
    )
    target_link_libraries(cache_server PRIVATE c_utils)

    # 消息队列系统
    add_executable(message_queue 
        ${CMAKE_CURRENT_SOURCE_DIR}/projects/message_queue/message_queue.c
    )
    target_link_libraries(message_queue PRIVATE c_utils)

    # 文件完整性校验工具
    add_executable(file_checksum 
        ${CMAKE_CURRENT_SOURCE_DIR}/projects/file_checksum/file_checksum.c
    )
    target_link_libraries(file_checksum PRIVATE c_utils)

    # 性能基准测试工具
    add_executable(benchmark 
        ${CMAKE_CURRENT_SOURCE_DIR}/projects/benchmark/benchmark.c
    )
    target_link_libraries(benchmark PRIVATE c_utils)

    # 图算法可视化工具
    add_executable(graph_visualizer 
        ${CMAKE_CURRENT_SOURCE_DIR}/projects/graph_visualizer/graph_visualizer.c
    )
    target_link_libraries(graph_visualizer PRIVATE c_utils)
endif()

# 安装目标
install(TARGETS c_utils
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/c_utils/
    DESTINATION include/c_utils
    FILES_MATCHING PATTERN "*.h"
)
