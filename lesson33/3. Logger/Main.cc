#include "Logger.hpp"
using namespace NS_LOG_MODULE;

int main(){
    std::cout <<GetCurrentTime()<<std::endl;
    sleep(1);
    std::cout <<GetCurrentTime()<<std::endl;
    sleep(1);
    std::cout <<GetCurrentTime()<<std::endl;
    sleep(1);
    std::cout <<GetCurrentTime()<<std::endl;
    sleep(1);
}