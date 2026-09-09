#pragma once
//自定义协议部分
#include<iostream>
#include <jsoncpp/json/json.h>
//进行协议定制
//请求报文--提供关于自身的序列化和反序列化方式
class Request
{
public:
    Request()
    {


    }
    Request(int x,int y,char oper)
        :_data_x(x)
        ,_data_y(y)
        ,_oper(oper)
    {

    }
    //如何序列和反序列化
    //1.自己手动搓一个--将结构化数据转化为字符串"_data_x _oper _date_y"--不建议
    //2.用别人的--1.json(可读性好) 2.protobuf 3.xml
    bool Serialize(std::string *out)//结构转字符串
    {
        //"_data_x _oper _date_y"
        Json::Value root;
        root["left"] =_data_x;
        root["right"] =_data_y;
        root["oper"] =_oper;
        Json::FastWriter writer;
        *out = writer.write(root);
        return true;
    }
    bool Deserialize(std::string &in)//字符串转结构
    {
        //"_data_x _oper _date_y"将数据设置到变量里
        Json::Value root;
        Json::Reader reader;
        bool parsesuccess = reader.parse(in,root);
        if(!parsesuccess)
            return false;
        _data_x=root["left"].asInt();
        _data_y=root["right"].asInt();
        _oper=root["oper"].asInt();

    }
    ~Request()
    {

    }
private:
//10 20 '-'->10-20=?
//_data_x _oper _date_y
    int _data_x;
    int _data_y;
    char _oper;//支持加减乘除
};
//协议--约定好的结构体类型
//应答报文--提供关于自身的序列化和反序列化方式
class Response
{
    Response()
    {

    }
    Response(int result,int code)
        :_result(result)
        ,_code(code)
    {

    }
    bool Serialize(std::string *out)
    {
        Json::Value root;
        root["result"] =_result;
        root["code"] =_code;
        Json::FastWriter writer;
        *out = writer.write(root);
        return true;
    }
    bool Deserialize(std::string &in)
    {
        Json::Value root;
        Json::Reader reader;
        bool parsesuccess = reader.parse(in,root);
        if(!parsesuccess)
            return false;
        _result = root["result"].asInt();  
        _code=root["code"].asInt();
    }   
    ~Response()
    {

    }
private:
//如果只有一个结果返回的话，比如说你返回的是-1，你知道他返回的这个东西是运算结果还是错误信号呢？
    int _result;
    //设计一个状态码
    int _code;//0:状态可信 1：状态不可信--约定：协议定制
};
const std::string gsep="\t\n";
class Protocol 
{
public:
    Protocol()
        :_version("1.0")
    {}
    //len\r\n{"left": 10,"right":20,"oper":'+'}\r\n
    std::string Packet(std::string &json_string)
    {
        return std::to_string(json_string.size())+gsep+json_string+gsep;
    }
    //传递进来的字符串可能是各种情况，一个甚至半个都有可能
    //ret > 0:no error,json_string!=NULL
    //ret == 0:no error,json_string==NULL
    //ret < 0:error
    int Unpack(std::string &packet,std::string *json_string)
    {
        //安全检查
        if(packet.empty()||json_string==nullptr)
            return 0;
        if(json_string==nullptr)
            return -1;
        //分析报文
        auto pos = packet.find(gsep);
        if(pos == std::string::npos)
            return 0;
        std::string lenstr=packet.substr(0,pos);//整个报文的长度字符串
        int len=std::stoi(lenstr);//推算完整报文的长度
        int total = lenstr.size()+len+2*gsep.size();//最终报文长度
        if(packet.size()<total)//报文不完整，不提取
            return 0;
        //提取报文
        *json_string = packet.substr(pos+gsep.size(),len);//截取有效数据的长度
        packet.erase(0,total);//提取后移除，便于下次的提取
        return 1;//代表正确解析提取报文

    }
    ~Protocol()
    {
        
    }
private:
    std::string _version;
};
