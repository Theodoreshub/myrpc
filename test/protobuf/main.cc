#include "test.pb.h"
#include <iostream>
#include <string>
// 不建议这么写
using namespace fixbug;

int main(){
    LoginRequest req;
    req.set_name("zhangsan");
    req.set_pwd("123456");

    std::string send_str;
    if (req.SerializeToString(&send_str)){
        std::cout << send_str.c_str() << std::endl;
    }

    LoginRequest req1;
    if (req1.ParseFromString(send_str)){
        std::cout << req1.name() << std::endl;
        std::cout << req1.pwd() << std::endl;
    }

    return 0;
}