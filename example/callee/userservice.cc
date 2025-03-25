#include <iostream>
#include <string>
#include "user.pb.h"

/*
提供两个进程内的本地方法
*/
class UserService : fixbug::UserServiceRpc  // 服务提供者
{
public:
    bool Login(std::string name, std::string pwd){
        std::cout << "doing local service : Login" << std::endl;
    }

    // 这个方法写来给框架直接调用的
    // 从 request 拿出数据，处理，放回response，最后回调
/*
    caller ==> Login(LoginRequest) ==> muduo ==> callee
    callee ==> Login(LoginRequest) ==> 下面这个函数
*/
    void Login(::google::protobuf::RpcController* controller,
        const ::fixbug::LoginRequest* request,
        ::fixbug::LoginResponse* response,
        ::google::protobuf::Closure* done)
        {
            // 去参数给本地业务
            std::string name = request->name();
            std::string pwd = request->pwd();

            // 业务
            bool login_rusult = Login(name, pwd);

            // 写响应
            fixbug::ResultCode* code  = response->mutable_result();
            code->set_errcode(0);
            code->set_errmsg("");
            response->set_success(login_rusult);

            // 做回调，closure 是一个抽象类,可以给虚函数Run写一些动作，就是序列化和网络发送。
            done->Run();
        }
};

int main(){
    return 0;
}