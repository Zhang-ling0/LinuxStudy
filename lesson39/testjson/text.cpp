#include <iostream>
#include <jsoncpp/json/json.h>
int main()
{
    // 内部引号用 \" 转义
    std::string json_string = "{\"age\":18,\"name\":\"张三\",\"sex\":\"男\"}";
    std::cout << json_string << std::endl;
    Json::Reader reader;
    Json::Value root;
    bool parseok = reader.parse(json_string, root); // 解析成Value
    if (!parseok)
    {
        exit(1);
    }
    std::string name = root["name"].asString(); // key提取
    int age = root["age"].asInt();
    std::string sex = root["sex"].asString();
    std::cout << name << std::endl;
    std::cout << age << std::endl;
    std::cout << sex << std::endl;
        // Json::Value sub_root1;
        // sub_root1["name"] ="张三";//key:value
        // sub_root1["sex"] ="男";//key:value
        // sub_root1["age"] =18;//key:value
        // Json::Value sub_root2;
        // sub_root2["math"] =88;
        // sub_root2["chinese"] =88;
        // Json::Value one;
        // one["who"] =sub_root1;
        // one["score"] =sub_root2;//可以利用json进行复杂结构的构建
        // Json::Value root;
        // for(int i=0;i<10;i++)
        // {
        //     root.append(one);//可以得到json数组，每个数组都可以来一个json子串
        //     //写一个Json服务器，将数据库里每个人的成绩全部都拿出来，序列化形成字符串，将字符串返回给浏览器
        //     //浏览器拿到Json串，将Json串以网页的形式输出，形成排名
        // }
        // //可以将源代码以json串编成一个请求，可以向服务器发送json请求
        // Json::StreamWriterBuilder wbuilder;
        // std::unique_ptr<Json::StreamWriter> swriter(wbuilder.newStreamWriter());//创建Json对象
        // std::stringstream ss;
        // swriter->write(root,&ss);//需要一个流
        // std::cout <<ss.str()<<std::endl;

        // v1
        // 不推荐
        //  Json::Value root;
        //  root["name"] ="张三";//key:value
        //  root["sex"] ="男";//key:value
        //  root["age"] =18;//key:value
        //  std::string s=root.toStyledString();
        //  std::cout<<s<<std::endl;

        // v2
        // Json::StyledWriter writer;//定义一个写对象--为了避免'\n'等字符串占用网络资源，我们可以有
        //  Json::FastWriter writer;
        //  std::string s = writer.write(root);
        //  std::cout << s <<std::endl;
        return 0;
}