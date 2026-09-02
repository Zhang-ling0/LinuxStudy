#pragma once
#include<netinet/in.h>//关键函数类型
#include<arpa/inet.h>
#include<sys/types.h>
#include<iostream>
#include<strings.h>
#include<string>

//对客户端先描述
class InetAddr
{
public:
    InetAddr(const struct sockaddr_in &address)
        :_address(address)
        ,_len(sizeof(address))
        {
            _ip = inet_ntoa(_address.sin_addr);
            _port = ntohs(_address.sin_port);
        }
    InetAddr(uint16_t port,const std::string &ip="0.0.0.0")
        :_ip(ip)
        ,_port(port)
    {
        bzero(&_address,sizeof(_address));
        _address.sin_family = AF_INET;
        _address.sin_addr.s_addr=INADDR_ANY;
        _address.sin_port = htons(_port);
        _len=sizeof(_address);
    }
    //重载比较符号
    InetAddr()
    {

    }
    std::string Tostring()
    {
        return _ip+" "+std::to_string(_port);
    } 
    bool operator ==(const InetAddr &addr)
    {
        return (this->_ip==addr._ip)&&(this->_ip==addr._port);
    }
    struct sockaddr_in *GetNetAddress()
    {
        return &_address;//直接获取网络地址
    }
    socklen_t len()
    {
        return _len;
    }
    ~InetAddr(){};
private:
    //net address
    struct sockaddr_in _address;
    socklen_t _len;
    //host address
    std::string _ip;
    uint16_t _port;
}