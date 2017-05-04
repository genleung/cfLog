#include "LogStream.h"
#include "Log.h"
namespace cfar{

    /// 该构造函数会把log前缀信息写入到串流中.
    LogStream::LogStream(Log* p, LogLevel l, std::string pre)
    : pLog(p),curLevel(l), prefix(pre){
        (*this)<<prefix;
    }

    /// Gcc在RVO(返回值优化)被启用时，该拷贝构造函数不会被用上
    LogStream::LogStream(const LogStream& ls)
    : pLog(ls.pLog),curLevel(ls.curLevel),prefix(ls.prefix){
        (*this)<<prefix;
    }    
    
    /// 当实例要销毁时，析构函数即输出log信息.
    /// 这种在析构函数执行时，一次性输出全部log信息的做法，依赖于Gcc的RVO支持
    LogStream::~LogStream(){
        if(pLog){
            pLog->log(this);
        }
    }
};
