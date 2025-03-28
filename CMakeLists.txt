# 设置版本
cmake_minimum_required(VERSION 3.0)
project(mprpc)

# 生成 Debug 版本，进行调试
set(CMAKE_BUILD_TYPE "Debug")

# 可执行文件位置
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
# 库文件输出位置
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# 头文件
include_directories(${PROJECT_SOURCE_DIR}/src/include)
include_directories(${PROJECT_SOURCE_DIR}/example)
# 库文件搜索
link_directories(${PROJECT_SOURCE_DIR}/lib)

# src 为框架代码 example 为框架使用示例
add_subdirectory(src)
add_subdirectory(example)