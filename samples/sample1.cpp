#define TRACE_ENABLED
#define LOG_TAG "test"
#include "Log.h"
#include <thread>
#include <string>

using namespace cf::utils;
using namespace std;

void threadFunc(int id){
    setLogFile("threads.txt", true);
    LOGI("DD in thread")<<id;

    Log log;
    log.setLogFile("threads.txt", true);
    log()<<"thread " << id;
}

int main(){
    int a=123;
    char ch='a';
    float f=1.234;
    const char *str="世界你好！";
       
    setLogLevel(LogLevel::INFO);
    enableLogPosition(true, true);
    setLogFile("test.txt", true);

    LOGI("A")<<a<<":Hello!!!";
    setLogFile("log.txt", true);
    TRACEI("world!");
    LOGE("")<<"error!";
    setLogFile();
    LOGW("warn...");
    TRACEE("debug error");
    
    LOGI("hello int:%d, char:%c, float:%f, string:%s", a, ch, f, str);
    TRACEFF("hello int:%d, char:%c, float:%f, string:%s", a, ch, f, str);

    Log log;
    log()<<"kaka";
    log(LogLevel::INFO, "warningTag")<<"This is a warn";

    TRACEF("dummy format string");
    std::string str2="string test";
    TRACEF("where is the string:%s", str2.c_str());
   
   
    // 多线程测试
    std::thread t[3];
    for(int i=0; i<sizeof(t)/sizeof(t[0]); i++){
        t[i]=std::thread(threadFunc, i);
    }
    
    for(int i=0; i<sizeof(t)/sizeof(t[0]); i++){
        t[i].join();
    }
        

    return 0;
    
    

}
