#include "Shm.hpp"

//非临界区代码
printf("Server is running...\n");
printf("Server is running...\n");
printf("Server is running...\n");
printf("Server is running...\n");
printf("Server is running...\n");
printf("Server is running...\n");
int main() {
  Shm sharedmem;
  sharedmem.Create();
  sharedmem.Attach();
  sleep(2); 
  sharedmem.PrintAttr();
  char *shm_start = (char *)sharedmem.Add(); // 获取共享内存的起始地址
  int size = sharedmem.size();
  while (true) {

  //临界区代码
  for (int i = 0; i < size; ++i) 
  {
    // 看到共享内存中的所有东西，本质就是读取共享内存的内容
    std::cout << shm_start[i];
  }
  std::cout << std::endl;
}
sharedmem.Detach();
sharedmem.Delete();
return 0;
}