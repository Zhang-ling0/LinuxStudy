#ifndef _ECHOSERVER_HPP
#define _ECHOSERVER_HPP

#include <iostream>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>
#include "Logger.hpp"
#include "Mutex.hpp"
#include <netinet/in.h>
#include <strings.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <cstring>
#include "InetAddr.hpp"
#include <functional>
#include <errno.h>   // 添加错误码支持

using namespace NS_LOG_MODULE;
using handler_addr_t = std::function<void(const InetAddr &)>;
using handler_msg_t = std::function<void(int sockfd, const std::string msg)>;  // 修正拼写

const static int default_fd = -1;
const static int default_port = 8888;

enum {
    SUCCESS = 0,
    SOCKET_ERR,
    USAGE_ERR,
    BIND_ERR
};

class UdpServer
{
public:
    UdpServer(uint16_t port = default_port)
        : _sockfd(default_fd)
        , _port(port)
        , _handler_addr([](const InetAddr&){})  // 默认空回调，避免崩溃
        , _handler_msg([](int, const std::string&){})
    {}
    
    ~UdpServer() 
    {
        if(_sockfd >= 0) {  // 修正判断条件
            close(_sockfd);
            _sockfd = -1;
        }
    }

    void Init()
    {
        _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (_sockfd < 0) {
            LOG(LogLevel::FATAL) << "create socket error: " << strerror(errno);
            exit(SOCKET_ERR);
        }
        LOG(LogLevel::INFO) << "create socket success sockfd: " << _sockfd;
        
        InetAddr local(_port);
        
        // 修正：使用正确的大小
        int n = bind(_sockfd, (struct sockaddr*)(local.GetNetAddress()), local.len());
        if(n < 0) {
            LOG(LogLevel::FATAL) << "bind socket error: " << strerror(errno);
            exit(BIND_ERR);
        }
        LOG(LogLevel::INFO) << "bind socket success, port: " << _port;  // 完善日志
    }
    
    // 修正：参数名不要加下划线前缀
    void RegisterService(handler_addr_t handler_addr, handler_msg_t handler_msg)
    {
        if (!handler_addr || !handler_msg) {
            LOG(LogLevel::WARNING) << "RegisterService: null handler provided";
            return;
        }
        //注册服务接口
        _handler_addr = handler_addr;
        _handler_msg = handler_msg;
    }
    
    void Start() {
        // 检查回调是否注册
        if (!_handler_addr || !_handler_msg) {
            LOG(LogLevel::FATAL) << "Service handlers not registered! Call RegisterService() first.";
            exit(1);
        }
        
        char inbuffer[65536];  // UDP最大包大小
        while(true) {
            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);
            
            ssize_t n = recvfrom(_sockfd, inbuffer, sizeof(inbuffer)-1, 0, 
                                 (struct sockaddr*)&peer, &len);
            if(n > 0) {
                inbuffer[n] = 0;
                InetAddr clientaddress(peer);
                _handler_addr(clientaddress);
                _handler_msg(_sockfd, inbuffer);
            } else if(n < 0) {
                // 修正：打印具体错误，并继续循环
                LOG(LogLevel::ERROR) << "recvfrom error: " << strerror(errno);
                continue;  // UDP可以继续，不退出
            }
        }
    }

private:
    int _sockfd;
    uint16_t _port;
    handler_addr_t _handler_addr;
    handler_msg_t _handler_msg;
};

#endif