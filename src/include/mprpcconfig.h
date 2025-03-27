#pragma once

#include <unordered_map>
#include <string>

// rpcserver_ip=  rpcserver_port=  zookeeper_ip=  zookeeper_port=
// 框架读取配置文件类
class MprpcConfig{
public:
    void LoadConfigFile(const char *config_file);
    std::string Load(const std::string &key);
private:
    std::unordered_map<std::string, std::string> m_configMap;
    void Trim(std::string &src_buf);

};