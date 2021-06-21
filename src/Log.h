/// @file Log.h
/// @brief Log类的头文件
/// @date 2016-09-09
/// @author Genleung Lan
/// @version 1.0

#pragma once
#include <iostream>
#include <memory>
#include <mutex>
#include <fstream>
#include "LogStream.h"


namespace cf {
    namespace utils {

        /// 以LogLevel::INFO等级调用单例Log对象.
        #define LOG() Log::instance()->createLogStream(cf::utils::LogLevel::INFO)
        /// 以指定等级调用单例Log对象.
        #define LOGL(level) Log::instance()->createLogStream(cf::utils::LogLevel::level)
        /// 以LogLevel::INFO等级输出可变参数格式的log信息.
        #define LOGF(format, ...) Log::instance()->createLogStream(cf::utils::LogLevel::INFO) << Log::formatString((char*)format,##__VA_ARGS__)
        /// 以指定等级输出可变参数格式的log信息.
        #define LOGLF(level, format, ...) Log::instance()->createLogStream(cf::utils::LogLevel::level) << Log::formatString((char*)format,##__VA_ARGS__)


        ///
        /// 通过定义NDEBUG可以切换Log是否显示；若已经定义NDEBUG，则DLOG*系列宏并不输出log信息.
        /// Windows下 VisualStudio Debug模式未定义NDEBUG，Release模式则定义了NDEBUG; 
        /// Linux下可以通过-DNDEBUG参数传入到g++
        ///
        #ifdef NDEBUG
            /// Debug模式下，以LogLevel::INFO等级调用单例Log对象
            #define DLOG(msg) Log::instance()->createLogStream(cf::utils::LogLevel::INFO, __FILE__, __LINE__) << msg
            /// Debug模式下，以指定等级调用单例Log对象
            #define DLOGL(level, msg) Log::instance()->createLogStream(cf::utils::LogLevel::level, __FILE__, __LINE__ ) << msg
            /// Debug模式下，以LogLevel::INFO等级输出可变参数格式的log信息
            #define DLOGF(format, ...) Log::instance()->createLogStream(cf::utils::LogLevel::INFO, __FILE__, __LINE__) << Log::formatString( (char*)format, ##__VA_ARGS__ )
            /// Debug模式下，以指定等级输出可变参数格式的log信息
            #define DLOGLF(level, format, ...) Log::instance()->createLogStream(cf::utils::LogLevel::level, __FILE__, __LINE__ ) << Log::formatString((char*)format,##__VA_ARGS__)
        #else
            #define DLOG(msg) do{}while(0)
            #define DLOGL(level, msg) do{}while(0)
            #define DLOGF(format, ...) do{}while(0)
            #define DLOGLF(level, format, ...) do{}while(0)
        #endif


        /// log的等级.
        enum class LogLevel :int {
            INFO = 0, ///< 信息
            NOTICE, ///< 提示
            WARNING,///< 警告
            ERROR,  ///< 一般错误
            FATAL   ///< 致命错误(将导致进程退出)
        };

        /// @class Log
        /// @brief 一个小巧灵活、线程安全的Log工具.
        ///
        /// 主要特性如下：
        /// 1. 小巧灵活、快速、线程安全
        /// 2. 支持5个Log等级: INFO, NOTICE, WARNING, ERROR, FATAL
        /// 3. 支持以LOG宏模式(单例模式)和C++对象模式调用
        /// 4. 支持C++的<<流式操作
        /// 5. 支持可变参数列表格式化输出
        /// 6. 支持输出到标准输出、文件输出(可选择覆盖或追加)
        /// 7. 支持在运行时切换输出文件
        /// 8. 支持DEBUG模式和非DEBUG模式(通过检查NDEBUG宏是否定义来区分这两种模式);在非DEBUG模式下，DLOG*系列的宏不会产生额外代码
        /// 9. 可控制是否显示文件名、行位置
        /// 10. ...
        class Log {
            friend class LogStream;

        public:
            /// 默认以LogLevel::INFO等级输出到std::cout.
            Log();
            /// 输出到指定文件.
            Log(std::string logFile, bool append = false);
            /// 析构函数. 主要负责清理缓冲、关闭文件
            virtual ~Log();

            // ------------------------------------------------------
            // inline公共成员函数
            // ------------------------------------------------------

            /// 获取全局唯一的Log对象的指针.
            static std::shared_ptr<Log>& instance() {
                // 仅会进行一次new操作，基于线程安全考量
                std::call_once(_ponce, [&]() { _ptr = std::shared_ptr<Log>(new Log()); });
                return _ptr;
            }

            /// 操作符()方式写入log信息.
            /// @param level [IN] 指定Log等级
            /// @return 返回LogStream对象
            /// @see LogLevel，LogStream
            LogStream operator()(LogLevel level = LogLevel::INFO) {
                return createLogStream(level, "", 0);
            }

            /// 创建LogStream对象.
            /// 首先把当前的log等级、log位置、log时间写入到LogStream对象中，再返回此对象
            /// @param curLevel [IN] 当前的log等级
            /// @param srcFile [IN] 当前进行log动作的文件
            /// @param srcLine [IN] 当前进行log动作的代码行
            LogStream createLogStream(LogLevel curLevel, std::string srcFile="", int srcLine=-1);

            // ------------------------------------------------------
            // 一般公共成员函数
            // ------------------------------------------------------

            /// 往文件中写入log信息。若file为空，则改为默认的std::cout输出
            void setLogFile(std::string file = "", bool append = false);
            /// 设置log的阈值等级.
            void setLogLevel(LogLevel level);
            /// 是否允许记录进行log的文件名和行号.
            void enableLogPosition(bool enabled, bool fullpathEnabled = false);
            /// 是否允许记录进行log的时间
            void enableLogTime(bool flag);
            /// 遇到致命错误的处理例程；可以由继承类重载。
            virtual void fatal();
            /// 格式化字符串
            static std::string formatString(char* format, ...);
        private:
            /// 禁止拷贝构造.
            Log(const Log&);
            /// 禁止赋值.
            Log& operator=(const Log&);

            /// 实例化Log对象.
            static void init();
            /// 清理流缓冲，并关闭流文件.
            void cleanupStream();
            /// 把LogStream中的log信息写入到目标文件.
            void log(LogStream* ls);

        private:
            /// pthread唯一次初始化对象标记.
            static std::once_flag _ponce;
            /// 唯一的Log实例指针.
            static std::shared_ptr<Log> _ptr;
            /// Log阈值，当log动作对应的log等级必须大于或等于Log阈值，log信息才会被记录下来.
            LogLevel _level = LogLevel::INFO;
            /// 是否允许显示log位置.
            bool _positionEnabled = true;
            /// 是否记录完整的文件名路径(此开关在_positionEanbled被启用的前提下有效)
            bool _positionFullpathEnabled = false;
            /// 是否允许显示log时间
            bool _timeEnabled = true;
            /// Log输出流.
            std::ostream* _os = nullptr;
            /// Log输出文件.
            std::ofstream _ofs;
            /// 互斥量，用以确保log输出不被打断.
            static std::mutex _mutex;
        };


        //---------------------------------------------------------------------------
        // 全局函数，为方便使用
        //---------------------------------------------------------------------------

        void setLogLevel(LogLevel level);
        void setLogFile(std::string file = "", bool append = true);
        void enableLogPosition(bool showFilename, bool showFullpath);
        void enableLogTime(bool timeLogged);
    };
};

