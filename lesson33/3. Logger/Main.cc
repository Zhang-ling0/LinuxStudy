#include "Logger.hpp"
#include"Mutex.hpp"
using namespace NS_LOG_MODULE;

int main(){
    //日志部分，未来在使用的时候，我们想仿照C++输出模式来进行使用
    //std::cout<<" "
    //我们想这样输出LOG(LogLevel:DEBUG) << xxxx <<yyyyy;
    //ENABLE_CONSOLE_LOG_STRATEGY();
    // logger(LogLevel::DEBUG,__FILE__,__LINE__)<<"hello"<<"1323"<<3456<<"hahaha";//俩个预处理符号，自动获取文件名和行号
    // logger(LogLevel::ERROR,__FILE__,__LINE__)<<"hello"<<"1323"<<3456<<"hahaha";//俩个预处理符号，自动获取文件名和行号
    // logger(LogLevel::FATAL,__FILE__,__LINE__)<<"hello"<<"1323"<<3456<<"hahaha";//俩个预处理符号，自动获取文件名和行号
    // logger(LogLevel::WARNING,__FILE__,__LINE__)<<"hello"<<"1323"<<3456<<"hahaha";//俩个预处理符号，自动获取文件名和行号
    ENABLE_FILE_LOG_STRATEGY();
    LOG(LogLevel::DEBUG) <<" hello"<<" 1323"<<3456<<" hahaha";
    LOG(LogLevel::ERROR) <<" hello"<<" 1323"<<3456<<" hahaha";
    LOG(LogLevel::FATAL) <<" hello"<<" 1323"<<3456<<" hahaha";
    LOG(LogLevel::WARNING) <<" hello"<<" 1323"<<3456<<" hahaha";

    // ENABLE_CONSOLE_LOG_STRATEGY();
    // logger.Debug("console strategy!\n");
    // logger.Debug("console strategy!\n");
    // logger.Debug("console strategy!\n");
    // logger.Debug("console strategy!\n");
    // logger.Debug("console strategy!\n");

    // ENABLE_FILE_LOG_STRATEGY();
    // logger.Debug("file strategy!");
    // logger.Debug("file strategy!");
    // logger.Debug("file strategy!");
    // logger.Debug("file strategy!");
    // logger.Debug("file strategy!");

    // ENABLE_CONSOLE_LOG_STRATEGY();
    // logger.Debug("console strategy1!\n");
    // logger.Debug("console strategy1!\n");
    // logger.Debug("console strategy1!\n");
    // logger.Debug("console strategy1!\n");
    // logger.Debug("console strategy1!\n");

    // std::cout <<GetCurrentTime()<<std::endl;
    // sleep(1);
    // std::cout <<GetCurrentTime()<<std::endl;
    // sleep(1);
    // std::cout <<GetCurrentTime()<<std::endl;
    // sleep(1);
    // std::cout <<GetCurrentTime()<<std::endl;
    // sleep(1);
    return 0;
}