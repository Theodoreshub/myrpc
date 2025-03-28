# 生成exe
set(SRC_LIST calluserservice.cc ../user.pb.cc)
add_executable(consumer ${SRC_LIST})

# 需要用到动态库的代码
target_link_libraries(consumer mprpc protobuf)