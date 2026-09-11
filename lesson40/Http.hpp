#pragma once
#include <iostream>
#include <string>
#include<unordered_map>//结构化数据

const std::string lineseo="\r\n";
class HttpRequest//客户端视角
{
    //什么功能？
public:
//序列化--将结构化的字段序列化，http请求实际上就是一行字符串
//反序列化，不依赖任何库的序列化和反序列化协议
private:
    std::string _method;
    std::string _uri;
    std::string _version;//请求的版本
    std::unordered_map<std::string,std::string> _hander;
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
    }

    ~HttpProtocol()
    {
    }

private:
};