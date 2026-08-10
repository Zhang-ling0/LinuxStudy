#include "Shm.hpp"

int main()
{
    //不需要创建和删除内存级共享内存                
    Shm sharedmem;
    sharedmem.Get();
    sharedmem.Attach();
    sleep(2);
    sharedmem.PrintAttr();
    char * shm_start = (char *)sharedmem.Add(); // 获取共享内存的起始地址
    int size = sharedmem.size();
    int index = 0;
    while(true)
    {
        std::cout << "Please Enter@ ";
        //死循环，等待客户端写入数据
        // char ch;
        // std::cin >>ch;
        //*(shm_start + index) = ch;
        std::cin >> *shm_start;
        index++;
        index %= size;//防止你写越界了
    }
    sharedmem.Detach();
    return 0;
}