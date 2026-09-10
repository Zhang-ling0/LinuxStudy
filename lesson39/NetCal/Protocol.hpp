#pragma once
// 自定义协议部分
#include <iostream>
#include <jsoncpp/json/json.h>
#include <functional>
#include "Logger.hpp"
// 进行协议定制
// 请求报文--提供关于自身的序列化和反序列化方式
using namespace NS_LOG_MODULE;
class Request
{
public:
    Request()
        : _data_x(0), _data_y(0), _oper(0)
    {
    }
    Request(int x, int y, char oper)
        : _data_x(x), _data_y(y), _oper(oper)
    {
    }
    // 如何序列和反序列化
    // 1.自己手动搓一个--将结构化数据转化为字符串"_data_x _oper _date_y"--不建议
    // 2.用别人的--1.json(可读性好) 2.protobuf 3.xml
    bool Serialize(std::string *out) // 结构转字符串
    {
        //"_data_x _oper _date_y"
        Json::Value root;
        root["left"] = _data_x;
        root["right"] = _data_y;
        root["oper"] = _oper;
        Json::FastWriter writer;
        *out = writer.write(root);
        return true;
    }
    bool Deserialize(std::string &in) // 字符串转结构
    {
        //"_data_x _oper _date_y"将数据设置到变量里
        Json::Value root;
        Json::Reader reader;
        bool parsesuccess = reader.parse(in, root);
        if (!parsesuccess)
            return false;
        _data_x = root["left"].asInt();
        _data_y = root["right"].asInt();
        _oper = root["oper"].asInt();
        return true;
    }
    ~Request()
    {
    }

public:
    // 10 20 '-'->10-20=?
    //_data_x _oper _date_y
    int _data_x;
    int _data_y;
    char _oper; // 支持加减乘除
};
// 协议--约定好的结构体类型
// 应答报文--提供关于自身的序列化和反序列化方式

class Response
{
public:
    Response()
        : _result(0), _code(0)
    {
    }
    Response(int result, int code)
        : _result(result), _code(code)
    {
    }
    bool Serialize(std::string *out)
    {
        Json::Value root;
        root["result"] = _result;
        root["code"] = _code;
        Json::FastWriter writer;
        *out = writer.write(root);
        return true;
    }
    bool Deserialize(std::string &in)
    {
        Json::Value root;
        Json::Reader reader;
        bool parsesuccess = reader.parse(in, root);
        if (!parsesuccess)
            return false;
        _result = root["result"].asInt();
        _code = root["code"].asInt();
        return true;
    }
    ~Response()
    {
    }

public:
    // 如果只有一个结果返回的话，比如说你返回的是-1，你知道他返回的这个东西是运算结果还是错误信号呢？
    int _result;
    // 设计一个状态码
    int _code; // 0:状态可信 1：状态不可信--约定：协议定制
};
const std::string gsep = "\t\n";
using HandlerRequest_t = std::function<Response(Request &)>;
using HandlerResponse_t = std::function<void(Response &)>;
class Protocol
{
public:
    Protocol(HandlerRequest_t handler_request)
        : _version("1.0"), _handler_request(handler_request)
    {
    }
    Protocol(HandlerResponse_t Handler_response)
        : _version("1.0"), _handler_response(Handler_response)
    {
    }
    Protocol()
    {
    }
    // len\r\n{"left": 10,"right":20,"oper":'+'}\r\n
    std::string Packet(std::string &json_string)
    {
        return std::to_string(json_string.size()) + gsep + json_string + gsep;
    }
    // 传递进来的字符串可能是各种情况，一个甚至半个都有可能
    // ret > 0:no error,json_string!=NULL
    // ret == 0:no error,json_string==NULL
    // ret < 0:error
    int Unpack(std::string &packet, std::string *json_string)
    {
        // 安全检查
        if (packet.empty() || json_string == nullptr)
            return 0;
        if (json_string == nullptr)
            return -1;
        // 分析报文
        auto pos = packet.find(gsep);
        if (pos == std::string::npos)
            return 0;
        std::string lenstr = packet.substr(0, pos);        // 整个报文的长度字符串
        int len = std::stoi(lenstr);                       // 推算完整报文的长度
        int total = lenstr.size() + len + 2 * gsep.size(); // 最终报文长度
        if (packet.size() < total)                         // 报文不完整，不提取
            return 0;
        // 提取报文
        *json_string = packet.substr(pos + gsep.size(), len); // 截取有效数据的长度
        packet.erase(0, total);                               // 提取后移除，便于下次的提取
        return 1;                                             // 代表正确解析提取报文
    }
    // 如果读到半个报文，什么都不做
    // 读到多个报文，循环处理，所有合法的报文都统一处理
    std::string ParseRequest(std::string &inbuffer) // 解析结果需要返回
    {                                               // v1:一次解析一个报文，解决发送多个报文的情况
        std::string result;
        // 将协议交到报文解析部分
        while (true)
        {
            std::string json_string;
            // 1.解包
            int n = Unpack(inbuffer, &json_string);
            if (n < 0)
            {
                // 解析出错
                LOG(LogLevel::DEBUG) << "no way!";
                return std::string();
            }
            if (n == 0)
            {
                LOG(LogLevel::INFO) << inbuffer << "parse done";
                return result;
            }
            LOG(LogLevel::DEBUG) << "json_string:\n " << json_string;
            LOG(LogLevel::DEBUG) << "unpack done,inbuffer:\n " << json_string; // 你的buffer还剩什么？

            // 2.反序列化
            // 这里解析出一个完整的报文
            //&可以直接修改，简单维护的一个生产消费模型
            Request req;                       // 临时对象
            if (!req.Deserialize(json_string)) // 反序列化失败
                return std::string();
            // 协议不需要处理业务功能
            // 3.业务计算
            Response resp;
            if (_handler_request)
                resp = _handler_request(req);
            // 4. 将回答再进行序列化
            std::string resp_json_string;
            resp.Serialize(&resp_json_string);

            // 5.返回：有效载荷长的+\r\n+数据。。。添加报头
            result += Packet(resp_json_string);
        }
    }
    std::string ParseResponse(std::string &inbuffer)
    {
        std::string result;
        while(true){
        std::string json_string;

        // 解包
        int n = Unpack(inbuffer, &json_string);
        if (n < 0)
        {
            LOG(LogLevel::DEBUG) << "no way!";
            return std::string();
        }
        if (n == 0)
        {
            LOG(LogLevel::INFO) << "incomplete packet, waiting for more data";
            return std::string();
        }

        LOG(LogLevel::DEBUG) << "json_string: " << json_string;

        // 反序列化
        Response resp;
        if (!resp.Deserialize(json_string))
        {
            LOG(LogLevel::ERROR) << "deserialize failed: " << json_string;
            continue;;
        }

        // 回调处理（打印结果）
        if (_handler_response)
        {
            LOG(LogLevel::DEBUG) << "calling response handler";
            _handler_response(resp);
        }
        else
        {
            LOG(LogLevel::WARNING) << "_handler_response is null!";
        }
    }

    }
    ~Protocol()
    {
    }

private:
    std::string _version;
    HandlerRequest_t _handler_request;
    HandlerResponse_t _handler_response;
};
