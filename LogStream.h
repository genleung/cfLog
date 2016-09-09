/// @file LogStream.h
/// @brief LogStream类的头文件
/// @date 2016-09-09
/// @author Genleung Lan
/// @version 1.0

#pragma once
#include <iostream>
#include <sstream>

namespace art{
    /// 前向声明    
    enum class LogLevel;
    class Log;

    /// @class LogStream
    /// @brief Log对象使用的字符串流,用来接收'<<'操作符输入.
    /// @warning 此类不应该被单独使用，仅能被Log类使用
    class LogStream : public std::ostringstream{
        friend class Log;
    public:
	    ~LogStream();    
    private:
        /// 构造函数. 
        /// @param pLog [IN] Log对象指针
        /// @param curLevel [IN] 当前log信息的Log等级
        /// @param prefix [IN] 当前log信息的前缀字串，如等级、位置、时间等
        /// @see LogLevel
	    LogStream(Log* pLog, LogLevel curLevel, std::string prefix);

        /// LogStream的拷贝构造函数. 
        LogStream(const LogStream& ls);
    protected:
        /// 当前待记录的log信息的级别
        LogLevel curLevel;
        /// Log对象指针
        Log* pLog;
        /// log前缀
        std::string prefix;
    };



};
