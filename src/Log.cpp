#include "Log.h"
#include <string>
#include <cstdarg>
#include <chrono>
#include <iomanip>

namespace cf {
    namespace utils {

        //---------------------------------------------------------------------------
        // 初始化静态数据成员
        //---------------------------------------------------------------------------
        /// 初始化为空指针
        std::shared_ptr<Log> Log::_ptr = nullptr;

        /// 静态互斥量.
        /// 互斥量主要用于保证多线程环境下切换输出文件、多个Log实例同时写log信息时的线程安全.
        std::mutex Log::_mutex;

        /// once_flag对象，用于std::call_once
        std::once_flag Log::_ponce;
        //---------------------------------------------------------------------------
        // 公共成员函数
        //---------------------------------------------------------------------------

        Log::Log() :_os(&std::cout) {
        }

        ///@param logFile [IN] 用以保存log信息的文件
        ///@param append [IN] 是否以追加模式保存
        Log::Log(std::string logFile, bool append) {
            setLogFile(logFile, append);
        }

        Log::~Log() {
            cleanupStream();
        }

        /// @param logFile [IN] 用以保存log信息的文件
        /// @param append [IN] 是否以追加模式保存
        void Log::setLogFile(std::string logFile, bool append) {
            if (logFile.empty()) {
                // 加锁，防止此时有log信息写入
                _mutex.lock();

                cleanupStream();
                _os = &std::cout;

                _mutex.unlock();

                return;
            }

            // 加锁，防止此时有log信息写入
            _mutex.lock();

            cleanupStream();
            if (append) {
                _ofs.open(logFile, std::ios::out | std::ios::app);
            }
            else {
                _ofs.open(logFile, std::ios::out);
            }

            if (_ofs.is_open()) {
                _os = &_ofs;
            }
            else {
                _os = &std::cout;
            }

            _mutex.unlock();
        }

        /// 设定Log阈值. 当log动作对应的log等级必须大于或等于Log阈值，log信息才会被记录下来.
        /// @param level [IN] Log阈值
        void Log::setLogLevel(LogLevel level) {
            _level = level;
        }

        void Log::enableLogPosition(bool enabled, bool fullpathEnabled) {
            _positionEnabled = enabled;
            _positionFullpathEnabled = fullpathEnabled;
        }

        void Log::enableLogTime(bool flag) {
            _timeEnabled = flag;
        }

        /// 创建LogStream对象.
        /// 首先把当前的log等级、log位置、log时间写入到LogStream对象中，再返回此对象
        /// @param curLevel [IN] 当前的log等级
        /// @param srcFile [IN] 当前进行log动作的文件
        /// @param srcLine [IN] 当前进行log动作的代码行
        LogStream Log::createLogStream(LogLevel curLevel, std::string srcFile, int srcLine) {
            const static char* levelStr[] = { "[I]", "[N]", "[W]", "[E]", "[F]" };
            std::stringstream ss;
            std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
            std::time_t now_c = std::chrono::system_clock::to_time_t(now - std::chrono::hours(24));

            ss << levelStr[(int)curLevel];

            if (_timeEnabled) {
                tm t;
#if defined(__WIN32) || defined(__WIN64)
                localtime_s(&t, &now_c);
#else
                localtime_r(&now_c, &t);
#endif
                ss << "[" << t.tm_year + 1900 << "-"
                    << std::setw(2) << std::setfill('0') << t.tm_mon + 1 << "-"
                    << std::setw(2) << std::setfill('0') << t.tm_mday + 1 << " "
                    << std::setw(2) << std::setfill('0') << t.tm_hour << ":"
                    << std::setw(2) << std::setfill('0') << t.tm_min << ":"
                    << std::setw(2) << std::setfill('0') << t.tm_sec << "]";
            }

            if (_positionEnabled && srcFile.length()) {
                if (_positionFullpathEnabled) {
                    // 获得srcFile的完整路径，以及log发生的所在行
                    ss << "[" << srcFile;
                    if (srcLine >= 0) ss << ":" << srcLine;
                    ss << "] ";
                }
                else {
                    // 仅获得srcFile路径中的最后文件名，以及log发生的所在行
                    ss << "[" << srcFile.substr(srcFile.find_last_of("/\\") + 1);
                    if (srcLine >= 0) ss << ":" << srcLine;
                    ss << "] ";
                }
            }
            else {
                ss << " ";
            }

            return LogStream(this, curLevel, ss.str());
        }

        //---------------------------------------------------------------------------
        // 私有成员函数
        //---------------------------------------------------------------------------

        /// 首先输出流内的数据，然后检查并关闭打开的文件.
        void Log::cleanupStream() {
            if (_os) _os->flush();
            if (_ofs.is_open()) {
                _ofs.flush();
                _ofs.close();
            }
        }

        /// 把LogStream中的log信息写入到目标文件.
        void Log::log(LogStream* ls) {
            if (ls == nullptr) return;

            // log输出受到_level限制。
            if (ls->curLevel < _level) return;

            // 写入log信息时上锁，基于两点考虑：
            // 1、防止线程间写入彼此干扰
            // 2、防止写入时setLogFile()被调用
            _mutex.lock();
            (*_os) << ls->str() << std::endl;
            _mutex.unlock();

            // 如果是fatal log，则终结进程
            if (ls->curLevel == LogLevel::FATAL) {
                fatal();
            }
        }

        /// @todo 需要继承类支持用户自定义退出机制
        void Log::fatal() {
            exit(-1);
        }

        /// 处理格式化可变参数的辅助函数
        std::string Log::formatString(char* format, ...) {
            char buf[512] = { 0 };

            va_list st;
            va_start(st, format);
            int nw = vsnprintf(buf, sizeof(buf), format, st);
            va_end(st);

            std::string str(buf);
            if (nw >= sizeof(buf)) {
                // buf太小，导致log信息被截断了
                // TODO: 增大buf的固定大小，或者其他机制。
                str += " ... ";
            }

            return str;
        }

        //---------------------------------------------------------------------------
        // 全局函数，为方便使用
        //---------------------------------------------------------------------------
        void setLogLevel(LogLevel level) {
            Log::instance()->setLogLevel(level);
        }

        void setLogFile(std::string file, bool append) {
            Log::instance()->setLogFile(file, append);
        }

        void enableLogPosition(bool filenameLogged, bool fullpathLogged) {
            Log::instance()->enableLogPosition(filenameLogged, fullpathLogged);
        }

        void enableLogTime(bool timeLogged) {
            Log::instance()->enableLogTime(timeLogged);
        }

    };
};
