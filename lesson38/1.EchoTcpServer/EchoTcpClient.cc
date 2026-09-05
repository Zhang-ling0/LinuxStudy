#include<iostream>
int main(){
    //1. 创建tcpsocket套接字

    //2.要不要进行bind?需要
    //要不要显示的bind本地IP和端口？不需要自己手动显示的进行bind,OS会自动bind的
    //OS帮助我们进行随机端口。防止端口号冲突
    //3. 应该干什么？bind端口号ip,发起建立连接的工作connect
    return 0;
}