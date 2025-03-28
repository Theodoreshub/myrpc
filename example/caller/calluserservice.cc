#include <iostream>
#include "mprpcapplication.h"
#include "user.pb.h"
#include "mprpcchannel.h"

int main(int argc, char **argv){
    MprpcApplication::Init(argc, argv);

    fixbug::UserServiceRpc_Stub stub(new MprpcChannel());
    // stub.Login(); // RpcChannel -> callMethod 集中做序列化和网络发送

    fixbug::LoginRequest request;
    request.set_name("zhang san");
    request.set_pwd("123456");
    fixbug::LoginResponse response;

    // 底层是 callmethod 的调用
    stub.Login(nullptr, &request, &response, nullptr);

    // 结果
    if(response.result().errcode() == 0){
        std::cout << "rpc login response success: " << response.success() << std::endl;
    }else {
        std::cout << "rpc login response error: " << response.result().errmsg() << std::endl;
    }

    fixbug::RegisterRequest req;
    req.set_id(2000);
    req.set_name("mprpc");
    req.set_pwd("666");

    fixbug::RegisterResponse rsp;
    stub.Register(nullptr, &req, &rsp, nullptr);

    if(rsp.mutable_result()->errcode() == 0){
        std::cout << "rpc register response success: " << rsp.success() << std::endl;
    }else {
        std::cout << "rpc register response error: " << rsp.result().errmsg() << std::endl;
    }

    return 0;
}