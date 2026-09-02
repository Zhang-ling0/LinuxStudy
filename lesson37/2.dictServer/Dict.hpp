#pragma once
#include<iostream>
#include<string>
#include"Logger.hpp"
using namespace NS_LOG_MODULE;
static const std::string default_dict = "./dict.txt";
static const std::string sep=": ";

class Dict
{
public:
    Dict(const std::string &dict_path = default_dict)
        :_dict_path(dict_path)

    {
        //初始化字典
        Load();
    }
    ~Dict()
    {
        //释放字典
    }
    void Load()
    {
        //加载字典
        std::ifstream in(_dict_path);
        if(!in.is_open())
        {
            LOG(LogLevel::DEBUG)<<"open"<<_dict_path<<"error";
            exit(1);
        }
        std::string line;
        while(std::getline(in,line))//从指定文件流里获取指定一行
        {
            LOG(LogLevel::DEBUG)<<"line:"<<line<<"success";
            //将一行数据分割成俩个部分插入到字典里，前闭后开截取到单词
            auto pos = line.find(sep);//字符串
            if(pos==std::string::npos)
            {
                //没有找到
                LOG(LogLevel::WARNING)<<"Format:"<<line<<"error";
                continue;
            }
            std::string k=line.substr(0,pos);//[)
            std::string v=line.substr(pos+sep.size());
            _dict.insert(std::make_pair(k,v));
        }
        in.close();
        LOG(LogLevel::INFO)<<"load....";
    }
    std::string Translate(const std::string &word)
    {
        //将网络问题转化为了系统问题

        auto iter= _dict.find(word);
        if(iter != _dict.end())
        {
            //return _dict[word];
            return iter->second;
        }
        else{
            return "None";
        }
        
    }
private:
    //字典数据结构
    std::string _dict_path;
    std::unordered_map<std::string,std::string> _dict;

};