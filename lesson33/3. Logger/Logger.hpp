#ifndef __LOGGER_HPP
#define __LOGGER_HPP
#include <iostream>
#include <string>
#include <cstdio>
#include <sys/time.h>
#include <ctime>
#include <unistd.h>
#include"Mutex.hpp"
namespace NS_LOG_MODULE
{
    enum class LogLevel
    {
        INFO,
        WARNING,
        ERROR,
        FATAL,
        DEBUG
    };
    // 1. 时间戳 2.日期+时间
    std::string GetCurrentTime()
    {
        struct timeval current_time;
        int n = gettimeofday(&current_time, nullptr);
        (void)n;
        struct tm struct_time;

        localtime_r(&(current_time.tv_sec), &struct_time); // r该函数是可重入函数
        char timestr[128];
        snprintf(timestr, sizeof(timestr), "%04d-%02d-%02d %02d:%02d:%02d.%ld", 
            struct_time.tm_year+1900, struct_time.tm_mon+1, struct_time.tm_mday,
            struct_time.tm_hour, struct_time.tm_min, struct_time.tm_sec,current_time.tv_usec);//格式输出
        return timestr;
    }
    //未来从输出的角度 -- 刷新策略的问题
    //1.显示器打印
    //2.向文件里写入
    //3.网络

    //日志的生成
    //1. 构建日志字符串
    //2. 根据不同的策略进行刷新

    //策略模式，设计一个策略接口
    class LogStrategy
    {
        //纯虚接口,顶层策略接口
    public:
        virtual ~LogStrategy() = default;
        virtual void SynLog(const std::string &message)=0;

    };
    //控制台日志刷新策略，日志将来要想显示器打印
    class ConsoleStrategy:public LogStrategy
    {
        void SynLog(const std::string &message) override
        {
            grouplock lock(_mutex);
            std::cerr<<message<<std::endl;
        }
        ~ConsoleStrategy()
        {

        }
    private:
    //避免信息相互干扰
        Mutex _mutex;
    };
    //文件日志
     class FileLogStrategy:public LogStrategy
     {
        virtual void SynLog(const std::string &message)=0;
     };
    class Logger
    {
        //日志设计
    };
};
#endif