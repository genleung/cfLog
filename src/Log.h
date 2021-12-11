/**
 * @file Log.h
 * @author Genleung Lan (genleung@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2021-07-31
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#pragma once
#include <iostream>
#include <memory>
#include <mutex>
#include <fstream>
#include "LogStream.h"

namespace cf
{
    namespace utils
    {

        /// 定义Log标签
        #ifndef LOG_TAG
        #define LOG_TAG ""
        #endif

        #ifdef __ANDROID__  // using Android log
            #include <android/log.h>
            #define LOG(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
            #define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
            #define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
            #define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
            #define LOGF(...) __android_log_print(ANDROID_LOG_FATAL, LOG_TAG, __VA_ARGS__)
            #ifdef TRACE_ENABLED
                #define TRACE(msg) LOGI(" %s(%d)::%s: %s", __FILE__, __LINE__, __FUNCTION__, msg)
                #define TRACEI(msg) LOGI(" %s(%d)::%s: %s", __FILE__, __LINE__, __FUNCTION__, msg)
                #define TRACEW(msg) LOGW(" %s(%d)::%s: %s", __FILE__, __LINE__, __FUNCTION__, msg)
                #define TRACEE(msg) LOGE(" %s(%d)::%s: %s", __FILE__, __LINE__, __FUNCTION__, msg)
                #define TRACEF(msg) LOGF(" %s(%d)::%s: %s", __FILE__, __LINE__, __FUNCTION__, msg)
                #define TRACEIF(format, ...) LOGI(" %s(%d)::%s: " # format, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
                #define TRACEWF(format, ...) LOGW(" %s(%d)::%s: " # format, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
                #define TRACEEF(format, ...) LOGE(" %s(%d)::%s: " # format, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
                #define TRACEFF(format, ...) LOGF(" %s(%d)::%s: " # format, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
            #else
                #define TRACE(msg) (static_cast<void>(0))
                #define TRACEI(msg) (static_cast<void>(0))
                #define TRACEW(msg) (static_cast<void>(0))
                #define TRACEE(msg) (static_cast<void>(0))
                #define TRACEF(msg) (static_cast<void>(0))
                #define TRACEIF(format, ...) (static_cast<void>(0))
                #define TRACEWF(format, ...) (static_cast<void>(0))
                #define TRACEEF(format, ...) (static_cast<void>(0))
                #define TRACEFF(format, ...) (static_cast<void>(0))
            #endif
        #else   // using standard C++ (Linux/Windows/MacOS)
            #define LOGL(level) Log::instance()->createLogStream(LogLevel::level, LOG_TAG)
            #define LOG(...) LOGL(INFO) << Log::formatString(__VA_ARGS__)
            #define LOGI(...) LOGL(INFO) << Log::formatString(__VA_ARGS__)
            #define LOGW(...) LOGL(WARN) << Log::formatString(__VA_ARGS__)
            #define LOGE(...) LOGL(ERROR) << Log::formatString(__VA_ARGS__)
            #define LOGF(...) LOGL(FATAL) << Log::formatString(__VA_ARGS__)
            #ifdef TRACE_ENABLED
                #define TRACEL(level) Log::instance()->createLogStream(LogLevel::level, LOG_TAG, __FILE__, __LINE__)
                #define TRACE(...) TRACEL(INFO) << Log::formatString(__VA_ARGS__)
                #define TRACEI(...) TRACEL(INFO) << Log::formatString(__VA_ARGS__)
                #define TRACEW(...) TRACEL(WARN) << Log::formatString(__VA_ARGS__)
                #define TRACEE(...) TRACEL(ERROR) << Log::formatString(__VA_ARGS__)
                #define TRACEF(...) TRACEL(FATAL) << Log::formatString(__VA_ARGS__)
                #define TRACEIF(...) TRACEI(__VA_ARGS__)
                #define TRACEWF(...) TRACEW(__VA_ARGS__)
                #define TRACEEF(...) TRACEE(__VA_ARGS__)
                #define TRACEFF(...) TRACEF(__VA_ARGS__)
            #else
                #define TRACE(msg) (static_cast<void>(0))
                #define TRACEI(msg) (static_cast<void>(0))
                #define TRACEW(msg) (static_cast<void>(0))
                #define TRACEE(msg) (static_cast<void>(0))
                #define TRACEF(msg) (static_cast<void>(0))
                #define TRACEIF(format, ...) (static_cast<void>(0))
                #define TRACEWF(format, ...) (static_cast<void>(0))
                #define TRACEEF(format, ...) (static_cast<void>(0))
                #define TRACEFF(format, ...) (static_cast<void>(0))
            #endif 
        #endif  

        /**
         * @brief log的等级.
         * 
         */
        enum class LogLevel : int
        {
            INFO = 0, ///< 信息
            NOTICE,   ///< 提示
            WARN,     ///< 警告
            ERROR,    ///< 一般错误
            FATAL     ///< 致命错误(将导致进程退出)
        };

        /**
         * @class cf::utils::Log
         * @details 一个小巧灵活、线程安全的Log工具;主要特性如下：
         * 1. 小巧灵活、快速、线程安全
         * 2. 支持5个Log等级: INFO, NOTICE, WARN, ERROR, FATAL
         * 3. 支持以LOG宏模式(单例模式)和 C++多实例模式调用
         * 4. 支持C++的<<流式操作
         * 5. 支持可变参数列表格式化输出
         * 6. 支持输出到标准输出、文件输出(可选择覆盖或追加)
         * 7. 支持在运行时切换输出文件
         * 8. 支持DEBUG模式和非DEBUG模式(通过检查TRACE_ENABLED宏是否定义来区分这两种模式);在非DEBUG模式下，DLOG*系列的宏不会产生额外代码
         * 9. 可控制是否显示文件名、行位置
         * 10. ...
         */
        class Log
        {
            friend class LogStream;

        public:
            /**
             * @brief 默认以LogLevel::INFO等级输出到std::cout.
             * 
             */
            Log();

            /**
             * @brief 按设定创建Log对象
             * 
             * @param[in] logFile 指定的log文件 
             * @param[in] append 是否以追加模式写入到log文件中 
             */
            Log(std::string logFile, bool append = false);

            /**
             * @brief 析构函数运行时清理缓冲、关闭文件
             */
            virtual ~Log();

            /**
             * @brief 获取全局唯一的Log对象的指针.
             * 
             * @return 对象共享指针
             */
            static std::shared_ptr<Log> &instance()
            {
                // 仅会进行一次new操作，基于线程安全考量
                std::call_once(_ponce, [&]()
                               { _ptr = std::make_shared<Log>(); });
                return _ptr;
            }

            /**
             * @brief 操作符()方式写入log信息.
             * 
             * @param[in] level 指定Log等级
             * @return 返回LogStream对象
             * @see  cf::utils::LogLevel
             * @sa cf::utils::LogStream
             */
            LogStream operator()(LogLevel level = LogLevel::INFO, std::string logTag=LOG_TAG)
            {
                _tag = logTag;
                return createLogStream(level, _tag, "", 0);
            }

            /**
             * @brief 创建LogStream对象.
             * @details 首先把当前的log等级、log位置、log时间写入到LogStream对象中，再返回此对象
             * 
             * @param[in] curLevel 当前的log等级
             * @param[in] tagString TAG字符串
             * @param[in] srcFile 当前的log文件
             * @param[in] srcLine 当前进行log动作的代码行
             * @return LogStream 创建好的LogStream流对象
             * @see cf::utils::LogStream
             */
            LogStream createLogStream(LogLevel curLevel, std::string tagString="", std::string srcFile = "", int srcLine = -1);

            /**
             * @brief 设定log文件及写入模式。若file为空，则改为默认的std::cout输出
             * 
             * @param[in] file 指定的log文件 
             * @param[in] append 是否以追加模式写入到log文件中 
             */
            void setLogFile(std::string file = "", bool append = false);

            /**
             * @brief 设置log的阈值等级.
             * 
             * @param[in] level Log阈值
             * @see cf::utils::LogLevel
             */
            void setLogLevel(LogLevel level);

            /**
             * @brief 是否允许记录进行log的文件名和行号.
             * 
             * @param[in] enabled 允许在log中记录进行log动作的文件名
             * @param[in] showFullpath 是否显示文件名的全路径
             */
            void enableLogPosition(bool enabled, bool showFullpath = false);

            /**
             * @brief 是否允许记录进行log的时间
             * 
             * @param[in] timeLogged true:记录log时间; false:不记录log时间 
             */
            void enableLogTime(bool timeLogged);

            /**
             * @brief 调用log等级为FATAL的Log::log()时遇到致命错误的处理例程
             * @attention 本类不进行任何操作，可由继承类重载并设定相应fatal动作(如abort())
             * @todo 需要继承类支持用户自定义退出机制(如abort())
             */
            virtual void fatal();

            /**
             * @brief 格式化字符串
             * 
             * @param[in] format 字符串描述符
             * @param[in] ... 可变长的参数列表
             * @return std::string 格式化好后的字符串
             */
            static std::string formatString(char *format, ...);

            /**
             * @brief 格式化字符串
             * 
             * @param[in] format 字符串描述符
             * @param[in] ... 可变长的参数列表
             * @return std::string 格式化好后的字符串
             */
            static std::string formatString(const char *format, ...);

        private:
            /**
             * @brief 禁止拷贝构造.
             * 
             */
            Log(const Log &) = delete;

            /**
             * @brief 禁止赋值构造.
             * 
             * @return Log& 
             */
            Log &operator=(const Log &) = delete;

            /**
             * @brief 实例化Log对象.
             * 
             */
            static void init();

            /**
             * @brief 首先输出流内的数据，然后检查并关闭打开的文件.
             * 
             */
            void cleanupStream();

            /**
             * @brief 把LogStream中的log信息写入到目标文件.
             * 
             * @param[in] ls LogStream流对象指针 
             */
            void log(LogStream *ls);

        private:
            std::string _tag = LOG_TAG;            ///< Log Tag
            LogLevel _level = LogLevel::INFO;      ///< Log阈值，当log动作对应的log等级必须大于或等于Log阈值，log信息才会被记录下来.
            bool _positionEnabled = true;          ///< 是否允许显示log位置.
            bool _positionFullpathEnabled = false; ///< 是否记录完整的文件名路径(此开关在_positionEanbled被启用的前提下有效)
            bool _timeEnabled = true;              ///< 是否允许显示log时间
            std::ostream *_os = nullptr;           ///< Log输出流.
            std::ofstream _ofs;                    ///< Log文件对象

        private:
            /**
             * @brief 唯一次初始化对象标记. 
             * @details 用于std::call_once.
             * 
             */
            static std::once_flag _ponce; 

            /**
             * @brief 单例模式下的唯一的Log实例指针.
             * @detials 若使用对象(多实例)模式，这个指针可以忽略之
             * 
             */
            static std::shared_ptr<Log> _ptr;

            /** 
             * @brief 确保log时线程安全的互斥量.
             * @details 保证多线程环境下切换输出文件、多个Log实例同时写同一个log文件的操作完整性（不被打断）
             */
            static std::mutex _mutex;
        };

        /**
         * @brief 设置log的阈值等级.
         * 
         * @param[in] level Log阈值
         * @attention 该全局函数用于单例模式Log
         * @see cf::utils::Log::setLogLevel()
         */
        void setLogLevel(LogLevel level);

        /**
         * @brief 设定log文件及写入模式。若file为空，则改为默认的std::cout输出
         * 
         * @param[in] file 指定的log文件 
         * @param[in] append 是否以追加模式写入到log文件中 
         * @attention 该全局函数用于单例模式Log
         * @see cf::utils::Log::setLogFile()
         */
        void setLogFile(std::string file = "", bool append = true);

        /**
         * @brief 是否允许记录进行log的文件名和行号.
         * 
         * @param[in] enabled 允许在log中记录进行log动作的文件名
         * @param[in] showFullpath 是否显示文件名的全路径
         * @attention 该全局函数用于单例模式Log
         * @see cf::utils::Log::enableLogPosition()
         */
        void enableLogPosition(bool enabled, bool showFullpath);

        /**
         * @brief 是否允许记录进行log的时间
         * 
         * @param timeLogged true:记录log时间; false:不记录log时间 
         * @attention 该全局函数用于单例模式Log
         * @see cf::utils::Log::enableLogTime()
         */
        void enableLogTime(bool timeLogged);
    };
};
