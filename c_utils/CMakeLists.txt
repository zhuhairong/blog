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

# 创建静态库
add_library(c_utils STATIC ${C_UTILS_SOURCES})

# 设置头文件包含目录
target_include_directories(c_utils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/c_utils
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

# 安装目标
install(TARGETS c_utils
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/c_utils/
    DESTINATION include/c_utils
    FILES_MATCHING PATTERN "*.h"
)
