#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include "Logger.hpp"
#include <fstream>
#include <unordered_map> //结构化数据

const std::string linesep = "\r\n";
const std::string spacesep = " ";
const std::string headersep = ": ";
const std::string g_http_version = "HTTP/1.1";
const std::string g_first_page = "index.html";
const std::string g_wwwroot = "wwwzhangling";
const std::string page_404 ="./wwwzhangling/404.html";
using namespace NS_LOG_MODULE;
class Util
{
public:
    // 1.空，没有完整行
    // 2.\r\n读取到了空行
    // 3.其他情况：分析到了下一行的具体内容
    static std::string ReadLine(std::string &str)
    {
        auto pos = str.find(linesep);
        if (pos == std::string::npos)
            return std::string(); // 没有读取到，返回一个空字符串
        std::string line = str.substr(0, pos);
        str.erase(0, line.size() + linesep.size());
        if (line.empty())
            return linesep;
        return line;
    }

    static std::string ReadFile(const std::string &filename)
    {
        // 读取文件
        // bug
        std::ifstream in(filename, std::ios::binary);
        if (!in.is_open())
        {
            return std::string();
        }
        // 读取文件内容
        std::string content;
        // in>>content;//将文件内容重定向到这里,按空格截取错误
        std::string line;
        while (std::getline(in, line))
            content += line;
        in.close();
        return content;
    }
};

// 1. 你怎么知道你读到的是一个完整的报文
class HttpRequest // 客户端视角
{
    // 什么功能？
private:
    bool ParseReqLine(std::string &httpstr)
    {
        std::string req_line = Util::ReadLine(httpstr);
        if (req_line.empty() || req_line == linesep)
            return false;
        std::stringstream ss(req_line);
        ss >> _method >> _uri >> _version; // 将信息拆除
        if (_uri == "/")
            _uri += g_first_page; // 将首页发送过去
        // 不要让它去跟目录下找，从www文件下去找
        _uri = g_wwwroot + _uri; // wwwzhangling/index.html:web根目录
        return true;
    }
    bool ParseHeaderkv(std::string &httpstr)
    {
        std::string header_line;
        do
        {
            header_line = Util::ReadLine(httpstr);
            // 再次分割插入到map中
            if (header_line.empty())
                return false;
            if (header_line != linesep)
            {
                auto pos = header_line.find(headersep);
                if (pos == std::string::npos)
                {
                    return false;
                } // 请求报头不是只有一行
                std::string key = header_line.substr(0, pos);                   // 自动截取
                std::string value = header_line.substr(pos + headersep.size()); // 自动截取
                // _header[key]=value;
                _header.insert(std::make_pair(key, value));
                //_header.insert({key,value});
                // 插入
            }
        } while (header_line != linesep);
        return true;
    }

    bool ParseText(std::string httpstr) // 默认读到一个完整的报文
    {
        // 1.http请求有没有正文(还要结合方法)
        if (_header.find("Content-Length") == _header.end())
        {
            // 没有属性
            _text = "";
        }
        else
        {
            // 2.正文多大，有没有正文
            int content_len = std::stoi(_header["Content-Length"]);
            _text = httpstr.substr(0, content_len);
            httpstr.erase(0, content_len);
        }
        // 正式提出一个属性
        return true;
    }

public:
    // 序列化--将结构化的字段序列化，http请求实际上就是一行字符串
    bool Deserialize(std::string &httpstr)
    {
        // std::cout<<httpstr;//看看解析前是什么样子
        // 1.解析请求行
        bool n = ParseReqLine(httpstr);
        (void)n;
        LOG(LogLevel::DEBUG) << "_method# " << _method;
        LOG(LogLevel::DEBUG) << "_uri# " << _uri;
        LOG(LogLevel::DEBUG) << "_version# " << _version;
        // 2.解析报头:文本处理
        // n = ParseHeaderkv(httpstr); // 将数据取出来并插入到报头中
        // for (auto &header : _header)
        // {
        //     std::cout << header.first << "##" << header.second << "\r\t\n";
        // }
        //std::cout << "start|" << httpstr << "|end";
        _blank_line = linesep;
        // 3.解析正文部分
        n = ParseText(httpstr);
        (void)n;
        return true;
    }
    std::string Uri() { return _uri; }
    std::string RequestContent()
    {
        return Util::ReadFile(_uri);
    }
    // 反序列化，不依赖任何库的序列化和反序列化协议
private:
    std::string _method;
    std::string _uri;     // 如果是裸的\默认把首页带上
    std::string _version; // 请求的版本
    std::unordered_map<std::string, std::string> _header;
    std::string _blank_line; // 空行
    std::string _text;       // 表示你的正文
};

class HttpResponse
{
private:
    std::string CodeToDesc(int code)
    {
        switch (_code) // 本质就是一张映射表
        {
        case 100:
            return "Continue";
            break;
        case 101:
            return "Switching Protocols";
            break;
        case 200:
            return "OK";
            break;
        case 201:
            return "Created";
            break;
        case 204:
            return "No Content";
            break;
        case 301:
            return "Moved Permanently";
            break;
        case 302:
            return "Found";
            break;
        case 304:
            return "Not Modified";
            break;
        case 400:
            return "Bad Request";
            break;
        case 401:
            return "Unauthorized";
            break;
        case 403:
            return "Forbidden";
            break;
        case 404:
            return "Not Found";
            break;
        case 405:
            return "Method Not Allowed";
            break;
        case 408:
            return "Request Timeout";
            break;
        case 500:
            return "Internal Server Error";
            break;
        case 502:
            return "Bad Gateway";
            break;
        case 503:
            return "Service Unavailable";
            break;
        case 504:
            return "Gateway Timeout";
            break;
        default:
            return "Unknown Status Code";
            break;
        }
    }

public:
    // 根据结构化请求转化成为结构化http_resp
    HttpResponse()
        : _version(g_http_version), _code(0), _blank_line(linesep)
    {
    }
    void SetCode(int code) // 设置状态码
    {
        _code = code;
        _code_desc = CodeToDesc(_code);
    }
    void SetBody(const std::string &content)
    {
        _text = content;
    }
    int BodySize()
    {
        return _text.size();
    }
    void AddHeader(const std::string &key,const std::string &value)
    {
        //动态添加报头，如果存在就覆盖，不存在就添加
        _header[key]=value;   
    }
    // bool Build(HttpRequest &req)
    // {
    //     std::string target_file = req.Uri();
    //     if (_text.empty())
    //     {
    //         // 请求的资源为空
    //         _code = 404;
    //         return false;
    //     }
    //     else
    //     {
    //         _code = 200;
    //         _code_desc = "OK";
    //         return true;
    //     }
    //     return true;
    // }
    std::string Serialize()
    {
        // 构建状态行
        std::string respstr = _version;
        respstr += spacesep;
        respstr += std::to_string(_code);
        respstr += spacesep;
        respstr += _code_desc;
        respstr += linesep;
        // 构建报头
        for (auto &header : _header) // 将报头添加进去
        {
            std::string line = header.first + headersep + header.second + linesep;
            respstr += line;
        }
        // 添加换行符
        respstr += _blank_line;
        // 添加正文
        respstr += _text;
        return respstr;
    }

private:
    std::string _version;
    int _code;
    std::string _code_desc;
    std::unordered_map<std::string, std::string> _header;
    std::string _blank_line;
    std::string _text;
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
        // 1. 保证报文的完整性--暂时忽略
        // 2. 反序列化
        HttpRequest http_req;
        http_req.Deserialize(req);
        // 3. 根据不同的请求方法和请求URI，给用户返回不同的报文--构建应答
        HttpResponse http_resp;
        std::string content= http_req.RequestContent();
        if(content.empty())
        {
            // //为空没找到404，正文不存在
            // http_resp.SetCode(200);
            // //404也是要对应内容页面的
            // //重新读取一下新的网页内容
            // http_resp.SetBody(Util::ReadFile(page_404));
            // http_resp.AddHeader("Content-Length",std::to_string(http_resp.BodySize()));
            http_resp.SetCode(302);
            //http_resp.AddHeader("Location","https://www.baidu.com/");//重定向，让浏览器二次请求
            http_resp.AddHeader("Location","/404.html");//只要访问失败了，都可以进行一个重定向将所有的错误都归结在一个页面里
        }
        else
        {
            http_resp.SetCode(200);//为应答设置状态码
            http_resp.AddHeader("Content-Length",std::to_string(content.size()));
            http_resp.SetBody(content);
        }
        //http_resp.Build(http_req);
        return http_resp.Serialize();//序列化
    }

    ~HttpProtocol()
    {
    }

private:
};