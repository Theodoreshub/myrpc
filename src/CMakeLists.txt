# 生成动态库写法
aux_source_directory(. SRC_LIST)
add_library(mprpc ${SRC_LIST})
# 使用了 muduo, 加依赖
target_link_libraries(mprpc protobuf muduo_net muduo_base pthread)