#pragma once
#include<iostream>
#include<memory>
#include<string>
#include<sys/socket.h>
#include"Mutex.hpp"
#include"UserManager.hpp"

//数据转发的逻辑
class Route
{
public:
    Route()
        :_uma(std::make_unique<UserManager>())
    {

    }
    void OfflineUser(const InetAddr &addr)
    {
        grouplock lockfuard(_lock);
        _uma->DelUser(addr);
    }
    void CheckUser(const InetAddr &addr)//检测用户是否存在
    {
        grouplock lockfuard(_lock);
        _uma->AddUser(addr);
    }
    void Broadcast(int sockfd,std::string message)
    {
        //广播给所有人
        grouplock lockfuard(_lock);
        //遍历
        auto &user=_uma->Users();
        for(auto &user:user)
        {
            sendto(sockfd,message.c_str(),message.size(),0,(struct sockaddr*)user.GetNetAddress(),user.len());//将数据发送给指定的用户
        }

    }
    //发送给某一个人
    // void Sendto(int sockfd,std::string message,InetAddr &who)
    // {

    // }

    ~Route()
    {

    }
private:
    std::unique_ptr<UserManager> _uma;
    Mutex _lock;
    //可以维护一个队列
    //std::queue<std::string> _q;
};