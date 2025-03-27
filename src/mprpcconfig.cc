#include "mprpcconfig.h"
#include <iostream>

void MprpcConfig::Trim(std::string &src_buf){

    int idx = src_buf.find_first_not_of(' ');
    if (idx != -1){
        // 有空格
        src_buf = src_buf.substr(idx);
    }
    // 去掉后面多余空格
    idx = src_buf.find_last_not_of(' ');
    if (idx != -1){
        src_buf = src_buf.substr(0, idx+1);
    }
    idx = src_buf.find_last_not_of('\n');
    if (idx != -1){
        src_buf = src_buf.substr(0, idx+1);
    }
}


void MprpcConfig::LoadConfigFile(const char *config_file){
    FILE *file = fopen(config_file, "r");
    if(file == nullptr){
        std::cout << config_file << "is not exit" << std::endl;
    }

    // 注释 去掉开头多余空格 正确配置
    while(!feof(file)){
        char buf[512] = {0};
        fgets(buf, 512, file); 

        // 去掉空格
        std::string src_buf(buf);
        Trim(src_buf);
        // 注释
        if (src_buf[0] == '#' || src_buf.empty()){
            continue;
        }

        // 解析
        int idx = src_buf.find('=');
        if(idx == -1){
            continue;
        }

        std::string key;
        std::string value;
        key = src_buf.substr(0, idx);
        Trim(key);
        value = src_buf.substr(idx + 1);
        Trim(value);
        // std::cout << "key" << key << "  value:" << value << std::endl;

        m_configMap[key] = value;
    } 
}


std::string MprpcConfig::Load(const std::string &key){
    auto it = m_configMap.find(key);
    if (it == m_configMap.end()){
        return "";
    }else return it->second;
}