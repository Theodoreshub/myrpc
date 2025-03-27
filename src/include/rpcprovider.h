#pragma once
#include "google/protobuf/service.h"
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>
#include <muduo/net/InetAddress.h>
#include <string>
#include "mprpcapplication.h"
#include <functional>
#include <google/protobuf/descriptor.h>
#include <unordered_map>

// 用于发布rpc服务
class RpcProvider
{
public:
    // 接收任意的service
    void NotifyService(google::protobuf::Service *service);

    // 启动服务节点
    void Run();
private:
    // 因为很多地方会访问到 eventloop 所以做成员
    muduo::net::EventLoop m_eventLoop;
    // 回调
    void OnConnection(const muduo::net::TcpConnectionPtr& conn);
    void OnMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buf, muduo::Timestamp timestamp);
    // Closure的回调操作，用于序列化rpc响应和网络发送
    void SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message*); 

    // 服务类型信息
    struct ServiceInfo{
        google::protobuf::Service *m_service;
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor*> m_methodMap;
    };
    std::unordered_map<std::string, ServiceInfo> m_serviceInfoMap;
};