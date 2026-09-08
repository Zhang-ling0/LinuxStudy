#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <cstdio>
#include <unistd.h>
#include <limits.h>
#include <pwd.h>
class Command
{
private:
    void LoadCommand()
    {
        _whitelist.push_back("ls -a -l");
        _whitelist.push_back("ls -l");
        _whitelist.push_back("pwd");
        _whitelist.push_back("who");
        _whitelist.push_back("ps -al");
        _whitelist.push_back("netstat -naup");
    }
    bool IsSafe(const std::string &cmd)
    {
        // 1. 去掉末尾的换行符
        std::string clean_cmd = cmd;
        if (!clean_cmd.empty() && (clean_cmd.back() == '\n' || clean_cmd.back() == '\r'))
        {
            clean_cmd.pop_back();
        }
        if (!clean_cmd.empty() && clean_cmd.back() == '\r')
        {
            clean_cmd.pop_back();
        }

        // 2. 用清洗过的 clean_cmd 去匹配
        for (auto &elem : _whitelist)
        {
            if (elem == clean_cmd)
                return true;
        }
        return false;
    }

public:
    // rm *.c *.cc *.hpp
    // 1.设置黑名单
    // 2.设置白名单
    Command()
    {
        LoadCommand();
    }
    std::string Execute(const std::string &cmd)
    {
        if (!IsSafe(cmd))
        {
            return "bad cmd!";
        }
        FILE *fp = popen(cmd.c_str(), "r");
        if (fp == nullptr)
        {
            return "execute error!";
        }
        std::string result;
        char buffer[1024];
        while (fgets(buffer, sizeof(buffer), fp))
        {
            result += buffer;
        }
        pclose(fp);
        return result;
    }
    std::string GetCommandString()
    {
        char hostname[HOST_NAME_MAX + 1];
        char cwd[PATH_MAX];
        // 获取用户名（系统调用 getuid + getpwuid）
        uid_t uid = getuid();
        struct passwd *pw = getpwuid(uid);
        const std::string username = (pw != nullptr) ? pw->pw_name : "unknown";

        // 获取主机名（系统调用 gethostname）
        gethostname(hostname, sizeof(hostname));

        // 获取当前目录（系统调用 getcwd）
        getcwd(cwd, sizeof(cwd));

        // 打印提示符
        return "[" + username + "@" + hostname + ":" + cwd + "]# ";
    }

private:
    std::vector<std::string> _whitelist;
};