#pragma once
#include <netinet/in.h> //关键函数类型
#include <arpa/inet.h>
#include <sys/types.h>
#include <iostream>
#include <strings.h>
#include <string>
#include <cstring>
#define CONV(address) ((struct sockaddr *)address)

// 对客户端先描述
class InetAddr
{
public:
    InetAddr()
    {
    }
    InetAddr(const struct sockaddr_in &address)
        : _address(address), _len(sizeof(address))
    {
        //_ip = inet_ntoa(_address.sin_addr);
        char ipstr[32];
        inet_ntop(AF_INET, &(_address.sin_addr), ipstr, sizeof(ipstr));
        _ip = ipstr;
        _port = ntohs(_address.sin_port);
    }
    InetAddr(uint16_t port, const std::string &ip = "0.0.0.0")
        : _ip(ip), _port(port)
    {
        bzero(&_address, sizeof(_address));
        _address.sin_family = AF_INET;
        //_address.sin_addr.s_addr=inet_addr(ip.c_str());
        _address.sin_port = htons(_port);
        inet_pton(AF_INET, ip.c_str(), &(_address.sin_addr)); // 线程安全的进行转化
        _len = sizeof(_address);
    }
    // 重载比较符号
    std::string Tostring()
    {
        return _ip + " " + std::to_string(_port);
    }
    bool operator==(const InetAddr &addr)
    {
        return (this->_ip == addr._ip) && (this->_port == addr._port);
    }
    //对赋值语句进行重载
    void operator= (struct sockaddr_in &addr)
    {
        _address = addr;
        char ipstr[32];
        inet_ntop(AF_INET, &(_address.sin_addr), ipstr, sizeof(ipstr));
        _ip = ipstr;
        _port = ntohs(_address.sin_port);
    }
    struct sockaddr *NetAddress()
    {
        return CONV(&_address); // 直接获取网络地址
    }
    socklen_t len()
    {
        return _len;
    }
    ~InetAddr() {};

private:
    // net address
    struct sockaddr_in _address;
    socklen_t _len;
    // host address
    std::string _ip;
    uint16_t _port;
};