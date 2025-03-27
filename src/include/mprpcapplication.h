#pragma once

#include "mprpcconfig.h"

// 基础类，通过该类获得一些共有的基础信息
// 用单例模式
class MprpcApplication
{
public:
    static bool isInit;
    static void Init(int argc, char **argv);
    static MprpcApplication& GetInstance();

    static MprpcConfig& GetConfig();
private:
    // Init 是 静态方法，下面这个也要静态才能使用，因为静态可以在非实例化的情况使用。
    static MprpcConfig m_config;

    MprpcApplication(){};
    // 拷贝和移动都不要
    MprpcApplication(const MprpcApplication&) = delete;
    MprpcApplication(MprpcApplication&&) = delete;
};