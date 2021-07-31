/**
 * @file LogStream.h
 * @author Genleung Lan(genleung@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2021-07-31
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#pragma once
#include <iostream>
#include <sstream>

namespace cf {
    namespace utils {
        enum class LogLevel;
        class Log;        
        /**
         * @class LogStream
         * @brief Log对象使用的字符串刘，用来接收"<<"操作符输入.
         * @warning 此类不允许被单独使用，仅能被Log类使用
         */
        class LogStream : public std::ostringstream {
            friend class Log;
        public:
            /**
             * @brief Destroy the Log Stream object
             * @details  实例销毁时，析构函数即输出log信息. 这种在析构函数执行时，一次性输出全部log信息的做法，依赖于Gcc的RVO支持
             */
            ~LogStream();

        private:               
            /**
             * @brief 构造函数
             * 
             * @param pLog [in] pLog Log对象指针
             * @param curLevel [in] curLevel 当前Log信息的Log等级
             * @param prefix [in] prefix 当前Log信息的前缀字符串，如等级、位置、时间等
             * @see LogLevel
             */
            LogStream(Log* pLog, LogLevel curLevel, std::string prefix);

            /**
             * @brief LogStream拷贝构造函数
             * @attention Gcc在RVO(返回值优化)被启用时，该拷贝构造函数不会被用上
             */
            LogStream(const LogStream& ls);

        protected:
            LogLevel _curLevel;  ///< 当前待记录的Log等级
            Log* _pLog;          ///< Log指针
            std::string _prefix; ///< Log前缀
        };

    };
};

