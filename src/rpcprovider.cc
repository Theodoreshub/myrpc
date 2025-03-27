#include "rpcprovider.h"
#include "rpcheader.pb.h"

// 接收任意的service
void RpcProvider::NotifyService(google::protobuf::Service *service){
    ServiceInfo service_info;

    const google::protobuf::ServiceDescriptor *pserviceDesc = service->GetDescriptor();
    std::string service_name = pserviceDesc->name();
    int methodCnt = pserviceDesc->method_count();

    for (int i = 0; i < methodCnt; i++){
        // 获取描述
        const google::protobuf::MethodDescriptor * pmethodDesc = pserviceDesc->method(i);
        std::string method_name = pmethodDesc->name();
        service_info.m_methodMap.insert({method_name, pmethodDesc});
    }
    service_info.m_service = service;
    m_serviceInfoMap.insert({service_name, service_info});
}

// 实例方法，需要绑定一个当前对象
void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr& conn){
    if (!conn->connected()){
        conn->shutdown();
    }
}

/*
框架内部商量好数据类型，要有
service method args，比如方法名有16个字符，还要记录参数长度，防止粘包问题
16UserServiceLoginzhang san123456
header_size（4字节） + header_str + args_str
*/
// 响应远程的RPC请求，把字符流解析出来
void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buf, muduo::Timestamp timestamp){
    std::string recv_buf = buf->retrieveAllAsString();

    // 读前4字节
    uint32_t header_size = 0;
    recv_buf.copy((char*)&header_size, 4, 0);

    std::string rpc_header_str = recv_buf.substr(4, header_size);
    mprpc::RpcHeader rpcHeader;
    std::string service_name;
    std::string method_name;
    uint32_t args_size;
    if (rpcHeader.ParseFromString(rpc_header_str)){
        // 反序列化成功
        service_name = rpcHeader.service_name();
        method_name = rpcHeader.method_name();
        args_size = rpcHeader.args_size();

    }else{
        std::cout << "rpc_header_str:" << rpc_header_str << " parse error!" << std::endl;
        return;
    }

    // 拿到参数
    std::string args_str = recv_buf.substr(4+header_size, args_size);
    // 打印
    std::cout << "=======================" << std::endl;
    
    std::cout << "header_size:  " << header_size << std::endl;
    std::cout << "rpc_header_str: "<< rpc_header_str << std::endl;
    std::cout << "service_name: " << service_name <<  std::endl;
    std::cout << "method_name: " << method_name << std::endl;
    std::cout << "args_str: " << args_str << std::endl;
    
    std::cout << "=======================" << std::endl;

    // 获取 service 和 method
    auto it = m_serviceInfoMap.find(service_name);
    if (it == m_serviceInfoMap.end()){
        std::cout << service_name << " is not exist!" << std::endl;
        return;
    }

    auto mit = it->second.m_methodMap.find(method_name);
    if (mit == it->second.m_methodMap.end()){
        std::cout << method_name << " is not exist!" << std::endl;
        return;
    }

    google::protobuf::Service* service = it->second.m_service;
    const google::protobuf::MethodDescriptor*method = mit->second;


    // request 和 response
    // 获得具体方法的请求request原型。
    google::protobuf::Message* request = service->GetRequestPrototype(method).New();
    if (!request->ParseFromString(args_str)){
        std::cout << " request parse error, content:" << args_str << std::endl;
        return;
    }
    google::protobuf::Message* response = service->GetResponsePrototype(method).New();

    // 绑定一个回调
    google::protobuf::Closure* done = google::protobuf::NewCallback
    <RpcProvider, const muduo::net::TcpConnectionPtr&, google::protobuf::Message*>
    (this, &RpcProvider::SendRpcResponse, conn, response);

    // 传给业务层
    service->CallMethod(method, nullptr, request, response, done);
}

// 序列化
void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* response){
    std::string response_str;
    if(response->SerializeToString(&response_str)){
        conn->send(response_str);
    }else{
        std::cout << "serialize response_str error! ";
    }
    conn->shutdown();

}


// 启动服务节点
void RpcProvider::Run(){
    std::string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port = atoi( MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
    muduo::net::InetAddress address(ip, port);
    // 创建 TcpServer
    muduo::net::TcpServer server(&m_eventLoop, address, "rpcProvider");

    // 写回调
    server.setConnectionCallback(std::bind(&RpcProvider::OnConnection, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&RpcProvider::OnMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    // 设置线程数量
    server.setThreadNum(4);

    server.start();

    // pthread_t thread_id = pthread_self();
    // std::cout << "RpcProvider start service at " << thread_id << std::endl;
    std::cout << "RpcProvider start service at ip: " << ip << " port: " << port << std::endl; 

    m_eventLoop.loop();
}