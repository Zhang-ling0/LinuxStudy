#ifndef __LOGGER_HPP
#define __LOGGER_HPP

#include <iostream>
#include <string>
#include <cstdio>
#include <sys/time.h>
#include <ctime>
#include <unistd.h>
#include <filesystem> //C++17
#include <fstream>
#include <sys/stat.h>
#include <memory>
#include <sstream>
#include "Mutex.hpp"

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
    std::string LogLevelMessage(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::WARNING:
            return "WARNING";
        case LogLevel::ERROR:
            return "ERROR";
        case LogLevel::FATAL:
            return "FATAL";
        case LogLevel::DEBUG:
            return "DEBUG";
        default:
            return "UNKNOWN";
        }
    }
    // 1. 时间戳 2.日期+时间
    std::string GetCurrentTime()
    {
        struct timeval current_time;
        int n = gettimeofday(&current_time, nullptr);
        (void)n;
        struct tm struct_time;

        localtime_r(&(current_time.tv_sec), &struct_time); // r该函数是可重入函数
        char timestr[128];
        snprintf(timestr, sizeof(timestr), "%04d-%02d-%02d %02d:%02d:%02d.%06ld",
                 struct_time.tm_year + 1900, struct_time.tm_mon + 1, struct_time.tm_mday,
                 struct_time.tm_hour, struct_time.tm_min, struct_time.tm_sec, current_time.tv_usec); // 格式输出
        return timestr;
    }

    // 未来从输出的角度 -- 刷新策略的问题
    // 1.显示器打印
    // 2.向文件里写入
    // 3.网络

    // 策略模式，设计一个策略接口
    class LogStrategy
    {
        // 纯虚接口,顶层策略接口
    public:
        virtual ~LogStrategy() = default;
        virtual void SynLog(const std::string &message) = 0; // 刷新字符串的策略
    };

    // 控制台日志刷新策略，日志将来要想显示器打印
    class ConsoleStrategy : public LogStrategy
    {
    public:
        void SynLog(const std::string &message) override
        {
            grouplock lock(_mutex);
            std::cerr << message << std::endl;
        }
        ~ConsoleStrategy() {}

    private:
        // 避免信息相互干扰
        Mutex _mutex;
    };

    const std::string defaultpath = "./log";
    const std::string defaultfilename = "log.txt";

    // 文件日志
    class FileLogStrategy : public LogStrategy
    {
    public:
        FileLogStrategy(const std::string &path = defaultpath, const std::string &name = defaultfilename)
            : _logpath(path), _logfilename(name)
        {
            // 判断一个文件是否存在
            // struct stat;系统级不太好
            if (std::filesystem::exists(_logpath))
                return; // 判断路径是否存在，这是一个目录
            try
            {
                // 查看文件属性是否存在
                std::filesystem::create_directories(_logpath); // 创建目录或者文件
            }
            catch (const std::filesystem::filesystem_error &e)
            {
                std::cerr << e.what() << "\n";
                exit(0);//创建LOG目录
            }
            
        }
        // 为什么要将目录和文件写在一起
        void SynLog(const std::string &message) override
        {
            grouplock lock(_mutex);
            if (!_logpath.empty() && _logpath.back() != '/')//刷新时动态检测
            {
                _logpath += "/";
            } // 确保路径以 / 结尾
            std::string targetlog = _logpath + _logfilename; // 拼接路径"./log/log.txt"
            std::ofstream out(targetlog, std::ios::app);     // 必须以追加方式写入
            if (!out.is_open())
            {
                std::cerr << "open " << targetlog << " failed" << std::endl;
                return;
            }
            // out.write();
            out << message << "\n";
            out.close();
        }
        ~FileLogStrategy() {}

    private:
        std::string _logpath;
        std::string _logfilename;
        Mutex _mutex;
    };
    // const std::string defaultfilename = "log.info";
    // const std::string defaultfilename = "log.warning";
    // const std::string defaultfilename = "log.fetal";
    // const std::string defaultfilename = "log.debug";
    //文件策略&&分日志等级来进行保存
    class FileLogLevelStrategy:public LogStrategy
    {
        //刷新的时候把等级传递进去

    };

    // 日志类
    // 1. 日志生成
    // 2. 根据不同的策略，进行属性
    class Logger
    {
        // 日志的生成
        // 1. 构建日志字符串
        // 2. 根据不同的策略进行刷新
    public:
        Logger()
        {
            UseConsoleStrategy();
        }
        // 日志是一条一条的字符串
        void UseConsoleStrategy()
        {
            _strategy = std::make_unique<ConsoleStrategy>(); // 基类指针指向子类对象
        }

        void UseFileStrategy()
        {
            _strategy = std::make_unique<FileLogStrategy>();
        }

        // void Debug(const std::string &message)
        // {
        //     if (_strategy != nullptr) // 策略已经选择好
        //     {
        //         _strategy->SynLog(message); // 使用不同的策略打印字符串
        //     }
        // }

        // void Info(const std::string &message) { Debug(message); }
        // void Warning(const std::string &message) { Debug(message); }
        // void Error(const std::string &message) { Debug(message); }
        // void Fatal(const std::string &message) { Debug(message); }
        // 内部类，表示一条完整的日志信息
        // 一条完整的日志信息=左部分固定部分+右部分不固定部分
        class LogMessage
        {
        public:
            LogMessage(LogLevel level, std::string &filename, int line, Logger &logger)
                : _level(level), _curr_time(GetCurrentTime()), _pid(getpid()), _filename(filename), _line(line), _logger(logger)
            {
                // 拼接日志的左半部分stringstring流式格式化
                std::stringstream ss;
                // 向ss里面写入数据
                ss << "[" << _curr_time << "]"
                   << "[" << LogLevelMessage(_level) << "]"
                   << "[" << _pid << "]"
                   << "[" << _filename << "]"
                   << "[" << _line << "]"
                   << "-";
                _loginfo = ss.str();
            }
            // 处理右半部分
            // 可变且类型不唯一
            template <typename T>
            LogMessage &operator<<(const T &info)
            {
                std::stringstream ss;
                ss << info;           // 转化为字符串
                _loginfo += ss.str(); // 将特定类型转化为字符串
                return *this;         // 返回当前LogMessage对象，方便下次继续进行<<
                // 将传递进来的参数进行拼接，返回this会持续获取字符串并不断拼接
                //<<就相当于是一个函数调用
            }
            // 拥有了一条完整的日志信息，后面应该如何刷新

            ~LogMessage()
            {
                // 析构时进行刷新
                if (_logger._strategy)
                {
                    _logger._strategy->SynLog(_loginfo);
                }
                // 怎样让其自动自购
            }

        private:
            LogLevel _level;
            std::string _curr_time;
            pid_t _pid;
            std::string _filename;
            int _line;            // 日志所对应的行号
            std::string _loginfo; // 一条完整的日志信息

            // 一个引用，引用外部的Logger对象
            Logger &_logger; // 方便我们后续进行策略式刷新
        };
        // 这里不是内部类
        // 故意采用拷贝LogMessage，临时对象
        LogMessage operator()(LogLevel level, std::string filename, int line)
        {
            return LogMessage(level, filename, line, *this); // 将外部类对象引用进来
        }
        ~Logger() {}

    private:
        std::unique_ptr<LogStrategy> _strategy; // 智能指针刷新策略
        //  日志设计
        //  std::string _logname;
        //  std::string _logpath;
    };

    // 日志对象：全局使用
    Logger logger;

#define ENABLE_CONSOLE_LOG_STRATEGY() logger.UseConsoleStrategy()
#define ENABLE_FILE_LOG_STRATEGY() logger.UseFileStrategy() // 将函数以宏的方式进行使用
#define LOG(level) logger(level,__FILE__,__LINE__)
} // namespace NS_LOG_MODULE

#endif