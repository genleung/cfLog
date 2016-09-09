#include "Log.h"
#include <string>
#include <cstdarg>

namespace art{
    //---------------------------------------------------------------------------
    // 初始化静态数据成员
    //---------------------------------------------------------------------------
    
    /// 必须初始化为PTHREAD_ONCE_INIT
    pthread_once_t Log::_ponce=PTHREAD_ONCE_INIT;

    /// 初始化为空指针
    std::shared_ptr<Log> Log::_ptr=nullptr;
    
    /// 全局互斥量. 
    /// 互斥量主要用于保证多线程环境下切换输出文件、多个Log实例同时写log信息时的线程安全.    
    std::mutex Log::_mutex;

    //---------------------------------------------------------------------------
    // 公共成员函数
    //---------------------------------------------------------------------------
    
    Log::Log():_os(&std::cout){
    }        
    
    /// @param logFile [IN] 用以保存log信息的文件
    /// @param append [IN] 是否以追加模式保存
    Log::Log(std::string logFile, bool append){
        setLogFile(logFile, append);
    }
    
    Log::~Log(){
        cleanupStream();
    }
  
   
    void Log::setLogFile(){
        // 加锁，防止此时有log信息写入
        _mutex.lock();

        cleanupStream();
        _os=&std::cout;
        
        _mutex.unlock();
    }
    
    /// @param logFile [IN] 用以保存log信息的文件
    /// @param append [IN] 是否以追加模式保存
    void Log::setLogFile(std::string logFile, bool append){
        if(logFile.empty()){
            _os=&std::cout;
            return;
        }

        // 加锁，防止此时有log信息写入
        _mutex.lock();

        cleanupStream();       
        if(append){
            _ofs.open(logFile, std::ios::out|std::ios::app);
        }else{
            _ofs.open(logFile, std::ios::out);
        }
        
        if(_ofs.is_open()){
            _os=&_ofs;
        }else{
            _os=&std::cout;
        }

        _mutex.unlock();
    }
    
    /// 设定Log阈值. 当log动作对应的log等级必须大于或等于Log阈值，log信息才会被记录下来.
    /// @param level [IN] Log阈值    
    void Log::setLogLevel(LogLevel level){
        _level=level;
    }
    
    void Log::enableLogPosition(bool flag){
        _positionEnabled=flag;
    }

    //---------------------------------------------------------------------------
    // 私有成员函数
    //---------------------------------------------------------------------------    
    
    /// 首先输出流内的数据，然后检查并关闭打开的文件.
    void Log::cleanupStream(){
        if(_os) _os->flush();
        if(_ofs.is_open()){
            _ofs.flush();
            _ofs.close();
        }
    } 

    /// 实例化Log对象.
    void Log::init(){
        _ptr=std::shared_ptr<Log>(new Log());
    }
    
    /// 把LogStream中的log信息写入到目标文件.
    void Log::log(LogStream* ls){       
        if(ls==nullptr) return;

        // log输出被抑制
        if(ls->curLevel<_level) return;
        
        // 写入log信息时上锁，基于两点考虑：
        // 1、防止线程间写入彼此干扰
        // 2、防止写入时setLogFile()被调用
        _mutex.lock(); 
        (*_os)<<ls->str()<<std::endl;
       
        // 如果是fatal log，则终结进程
        if(ls->curLevel==LogLevel::FATAL){
            terminateProcess();
        }
        _mutex.unlock();        
    }

    /// @todo 需要回调函数支持用户自定义退出机制
    void Log::terminateProcess(){  
        exit(-1);
    }


    //---------------------------------------------------------------------------
    // 全局函数，为方便使用
    //---------------------------------------------------------------------------
    void setLogLevel(LogLevel level){
        Log::instance()->setLogLevel(level);
    }

    void setLogFile(){
        Log::instance()->setLogFile();
    }

    void setLogFile(std::string file, bool append){
        Log::instance()->setLogFile(file, append);
    }
    
    void enableLogPosition(bool flag){
        Log::instance()->enableLogPosition(flag);
    } 
       
    /// 处理格式化可变参数的辅助函数
    std::string formatString(char *format, ...){
        char buf[256]={0};
        
        va_list st;
        va_start(st, format);
        int nw=vsnprintf(buf, sizeof(buf), format, st);
        va_end(st);
        
        std::string str(buf);
        if(nw>=sizeof(buf)){
            // buf太小，导致log信息被截断了
            // TODO: 增大buf的固定大小，或者其他机制。
            str+=" ... ";
        }
        
        return str;
    }

};
