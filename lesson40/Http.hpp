#pragma once
#include <iostream>
#include <string>
#include<sstream>
#include"Logger.hpp"
#include<unordered_map>//结构化数据

const std::string linesep="\r\n";
const std::string spacesep=" ";
const std::string headersep=": ";
using namespace NS_LOG_MODULE;
class Util
{
public:
    //1.空，没有完整行
    //2.\r\n读取到了空行
    //3.其他情况：分析到了下一行的具体内容
    static std::string ReadLine(std::string &str)
    {
        auto pos = str.find(linesep);
        if(pos==std::string::npos)
            return std::string();//没有读取到，返回一个空字符串
        std::string line=str.substr(0,pos);
        str.erase(0,line.size()+linesep.size());
        if(line.empty())
            return linesep;
        return line;
    }
};
//1. 你怎么知道你读到的是一个完整的报文
class HttpRequest//客户端视角
{
    //什么功能？
private:
    bool ParseReqLine(std::string &httpstr)
    {
        std::string req_line=Util::ReadLine(httpstr);
        if(req_line.empty()||req_line==linesep)
            return false;
        std::stringstream ss(req_line);
        ss>>_method>>_uri>>_version;//将信息拆除
        return true;

    }
    bool ParseHeaderkv(std::string &httpstr)
    {
        std::string header_line;
        do{
        header_line=Util::ReadLine(httpstr);
        //再次分割插入到map中
        if(header_line.empty())
            return false;
        if(header_line!=linesep)
        {
        auto pos=header_line.find(headersep);
        if(pos==std::string::npos)
        {
            return false;
        }//请求报头不是只有一行
            std::string key = header_line.substr(0,pos);//自动截取
            std::string value = header_line.substr(pos+headersep.size());//自动截取
            // _header[key]=value;
            _header.insert(std::make_pair(key,value));
            //_header.insert({key,value});
        //插入
    }
    }while(header_line!=linesep);
       return true; 
    }
        
    bool ParseText(std::string httpstr)//默认读到一个完整的报文
    {
        //1.http请求有没有正文(还要结合方法)
        if(_header.find("Content-Length")==_header.end())
        {
            //没有属性
            _text="";
        }else{
        //2.正文多大，有没有正文
            int content_len =std::stoi(_header["Content-Length"]);
            _text=httpstr.substr(0,content_len);
            httpstr.erase(0,content_len);
        }
        //正式提出一个属性
    }
public:
//序列化--将结构化的字段序列化，http请求实际上就是一行字符串
    bool Deserialize(std::string &httpstr)
    {
        //std::cout<<httpstr;//看看解析前是什么样子
        //1.解析请求行
        bool n=ParseReqLine(httpstr);
        (void)n;
        // LOG(LogLevel::DEBUG)<<"_method# "<<_method;
        // LOG(LogLevel::DEBUG)<<"_uri# "<<_uri;
        // LOG(LogLevel::DEBUG)<<"_version# "<<_version;
        //2.解析报头:文本处理
        n=ParseHeaderkv(httpstr);//将数据取出来并插入到报头中
        // for(auto &header:_header)
        // {
        //     std::cout<<header.first<<"##"<<header.second<<"\r\t\n";
        // }
        // std::cout<<"start|"<<httpstr<<"|end";
        _blank_line=linesep;
        //3.解析正文部分
        n= ParseText(httpstr);
        (void)n;
        return true;
    }
//反序列化，不依赖任何库的序列化和反序列化协议
private:
    std::string _method;
    std::string _uri;
    std::string _version;//请求的版本
    std::unordered_map<std::string,std::string> _header;
    std::string _blank_line;//空行
    std::string _text;//表示你的正文
};

class HttpResponse
{

};

class HttpProtocol
{
public:
    HttpProtocol()
    {
    }

    std::string HandlerHttpRequest(std::string &req)
    {
#ifdef DEBUG
        std::cout << "######################" << std::endl;
        std::cout << req << std::endl;

        // 1. body 先准备好
        std::string html = "<html><body>hhhhh</body></html>";

        // 2. 状态行
        std::string status_line = "HTTP/1.1 200 OK\r\n";

        // 3. 响应头部
        std::string headers;
        headers += "Content-Type: text/html; charset=utf-8\r\n";
        headers += "Content-Length: " + std::to_string(html.size()) + "\r\n";
        headers += "Connection: close\r\n";

        // 4. 空行（关键！告诉浏览器"头部结束了"）
        std::string blank_line = "\r\n";

        // 5. 拼接：状态行 + 头部 + 空行 + body
        return status_line + headers + blank_line + html;
    #endif
        //1. 保证报文的完整性--暂时忽略
        //2. 反序列化
        HttpRequest http_req;
        http_req.Deserialize(req);
        //3. 根据不同的请求方法和请求URI，给用户返回不同的报文
        return std::string();
    }

    ~HttpProtocol()
    {
    }

private:
};