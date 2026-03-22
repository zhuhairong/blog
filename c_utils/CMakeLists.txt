cmake_minimum_required(VERSION 3.10)
project(c_utils VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 启用复数支持
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_Complex_I=1.0fi -I/usr/include")

# 自动发现所有源文件（排除test_main.c）
file(GLOB C_UTILS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/c_utils/*.c")
# 排除test_main.c，因为它是测试入口
list(FILTER C_UTILS_SOURCES EXCLUDE REGEX "test_main\\.c$")

# 添加第三方库源文件
file(GLOB THIRD_PARTY_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/*/*.c")
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
    
    # 自动发现所有测试文件
    file(GLOB TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_*.c")
    
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
    # 自动发现所有演示文件
    file(GLOB DEMO_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/demo/demo_*.c")

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
