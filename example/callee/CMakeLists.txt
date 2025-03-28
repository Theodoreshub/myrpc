# 生成exe
set(SRC_LIST userservice.cc ../user.pb.cc)
add_executable(provider ${SRC_LIST})

# 需要用到动态库的代码
target_link_libraries(provider mprpc protobuf)