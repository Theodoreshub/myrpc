#include "test.pb.h"
#include <iostream>
#include <string>
// 不建议这么写
using namespace fixbug;

int main(){
    // LoginResponse rsp;
    // ResultCode *rc= rsp.mutable_result();
    // rc->set_errcode(1);
    // rc->set_errmsg("登录失败");

    GetFriendListResponse rsp;
    // mutable 是拿里面的 message 字段的
    ResultCode *rc = rsp.mutable_result();
    rc->set_errcode(0);

    // add 是往 reapted 的字段新添加一个成员
    User *user1 = rsp.add_friend_list();
    user1->set_name("zhangsan");
    user1->set_age(20);
    user1->set_sex(User::MAN);
    
    std::cout << rsp.friend_list_size() << std::endl;
}

int main1(){
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