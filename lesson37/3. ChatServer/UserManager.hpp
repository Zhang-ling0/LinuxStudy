#pragma once
#include <iostream>
#include "InetAddr.hpp"
#include "Logger.hpp"
#include<string>
#include<vector>
//#include<unordered_map>

// class User
// {
//     std::string username;
//     std::string userstatus;  
// }
using namespace NS_LOG_MODULE;
class UserManager
{
public:
    UserManager()
    {

    }
    //增加
    void AddUser(const InetAddr &addr)
    {
        if(SearchUser(addr))
            return;
        _users.push_back(addr);
    }
    //查找
    bool SearchUser(const InetAddr &addr)
    {
        for(auto &user:_users)
        {
            if(user==addr)
            {
                return true;
            }
        }

    }
    //删除
    void DelUser(const InetAddr &addr)
    {
        if(!SearchUser(addr))
            return;
        for(auto iter=_users.begin();iter!=_users.end();iter++)
        {
            if(*iter ==addr)
            {
                _users.erase(iter);
                break;
            }
        }
        
    }
    //修改
    bool ModUser(const InetAddr &addr)
    {
        DelUser(addr);
        AddUser(addr);
    }
    ~UserManager(){

    }
    std::vector<InetAddr> &Users()
    {
        return _users;
    }


private:
    //ip:port->InetAddr
    //std::unordered_map<std::string,InetAddr> _users;
    std::vector<InetAddr> _users;
}