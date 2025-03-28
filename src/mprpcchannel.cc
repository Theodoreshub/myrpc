#include "mprpcchannel.h"
#include <string>
#include "rpcheader.pb.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <error.h>
#include "mprpcapplication.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include "mprpccontroller.h"

void MprpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
    google::protobuf::RpcController* controller, const google::protobuf::Message* request,
    google::protobuf::Message* response, google::protobuf::Closure* done){

        const google::protobuf::ServiceDescriptor *sd = method->service();
        std::string service_name = sd->name();
        std::string method_name = method->name();

        uint32_t args_size = 0;
        std::string args_str;
        if (request->SerializePartialToString(&args_str)){
            args_size = args_str.size();
        }else{
            // std::cout << "serialize request error! " << std::endl;
            controller->SetFailed("serialize request error!");
            return;
        }

        mprpc::RpcHeader rpcHeader;
        rpcHeader.set_service_name(service_name);
        rpcHeader.set_method_name(method_name);
        rpcHeader.set_args_size(args_size);

        uint32_t header_size = 0;
        std::string rpc_header_str;
        if(rpcHeader.SerializeToString(&rpc_header_str)){
            header_size = rpc_header_str.size();
        }else{
            // std::cout << "serialize request error! " << std::endl;
            controller->SetFailed("serialize header error!");
            return;
        }

        std::string send_rpc_str;
        send_rpc_str.insert(0, std::string((char*)&header_size, 4));

        send_rpc_str+=rpc_header_str;
        send_rpc_str+=args_str;

        // 打印
        std::cout << "=======================" << std::endl;
        
        std::cout << "header_size:  " << header_size << std::endl;
        std::cout << "rpc_header_str: "<< rpc_header_str << std::endl;
        std::cout << "service_name: " << service_name <<  std::endl;
        std::cout << "method_name: " << method_name << std::endl;
        std::cout << "args_str: " << args_str << std::endl;
        
        std::cout << "=======================" << std::endl;

        // 网络编程
        int clientfd = socket(AF_INET, SOCK_STREAM, 0);
        if (clientfd == -1){
            // std::cout << "create socket error! errno: " << errno << std::endl;
            char errtext[512] = {0};
            sprintf(errtext, "create socket error! errno:%d", errno);
            controller->SetFailed(errtext);
            return;
            // exit(EXIT_FAILURE);
        }

        std::string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
        uint16_t port = atoi( MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());

        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

        if(-1 == connect(clientfd, (struct sockaddr*)&server_addr, sizeof(server_addr))){
            // std::cout << "connect error! errno: " << errno <<  std::endl;
            close(clientfd);
            char errtext[512] = {0};
            sprintf(errtext, "connect socket error! errno:%d", errno);
            controller->SetFailed(errtext);
            // exit(EXIT_FAILURE);
            return;
        }

        if  (-1 == send(clientfd, send_rpc_str.c_str(),  send_rpc_str.size(), 0)){
            char errtext[512] = {0};
            sprintf(errtext, "send socket error! errno:%d", errno);
            controller->SetFailed(errtext);
            close(clientfd);
            return;
        }

        char recv_buf[1024] = {0};
        int recv_size = 0;
        if (-1 == (recv_size = recv(clientfd, recv_buf, 1024, 0))){
            char errtext[512] = {0};
            sprintf(errtext, "recv socket error! errno:%d", errno);
            controller->SetFailed(errtext);
            close(clientfd);
            return;
        }

        // std::string response_str(recv_buf, 0, recv_size);    // 整个方法遇到 /0 就结束了，不用
        // if (!response->ParseFromString(response_str)){
        if (!response->ParseFromArray(recv_buf, recv_size)){
            // std::cout << "parse error! response_str:" << recv_buf << std::endl;
            char errtext[512] = {0};
            sprintf(errtext, "parse error! response_str:%s", recv_buf);
            controller->SetFailed(errtext);
            close(clientfd);
            return;
        }
        // 至此 response 已经获得了反序列化的结果。
        close(clientfd);

    }