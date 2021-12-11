#include "Log.h"
#include <string>
#include <cstdarg>
#include <chrono>
#include <iomanip>
#include <exception>

namespace cf
{
    namespace utils
    {

        std::shared_ptr<Log> Log::_ptr = nullptr;
        std::mutex Log::_mutex;
        std::once_flag Log::_ponce;

        Log::Log() : _os(&std::cout)
        {
        }

        Log::Log(std::string logFile, bool append)
        {
            setLogFile(logFile, append);
        }

        Log::~Log()
        {
            cleanupStream();
        }

        void Log::setLogFile(std::string logFile, bool append)
        {
            if (logFile.empty())
            {
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
            if (append)
            {
                _ofs.open(logFile, std::ios::out | std::ios::app);
            }
            else
            {
                _ofs.open(logFile, std::ios::out);
            }

            if (_ofs.is_open())
            {
                _os = &_ofs;
            }
            else
            {
                _os = &std::cout;
            }

            _mutex.unlock();
        }

        void Log::setLogLevel(LogLevel level)
        {
            _level = level;
        }

        void Log::enableLogPosition(bool enabled, bool fullpathEnabled)
        {
            _positionEnabled = enabled;
            _positionFullpathEnabled = fullpathEnabled;
        }

        void Log::enableLogTime(bool flag)
        {
            _timeEnabled = flag;
        }

        LogStream Log::createLogStream(LogLevel curLevel, std::string tagString, std::string srcFile, int srcLine)
        {
            const static char *levelStr[] = {"[I]", "[N]", "[W]", "[E]", "[F]"};
            std::stringstream ss;
            std::time_t now_c = std::time(0);

            ss << levelStr[(int)curLevel];
            if(tagString.length()){
                ss << "[" << tagString << "]";
            }

            if (_timeEnabled)
            {
                std::tm t;
#if defined(_WIN32) || defined(_WIN64)
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

            if (_positionEnabled && srcFile.length())
            {
                if (_positionFullpathEnabled)
                {
                    // 获得srcFile的完整路径，以及log发生的所在行
                    ss << "[" << srcFile;
                    if (srcLine >= 0)
                        ss << ":" << srcLine;
                    ss << "] ";
                }
                else
                {
                    // 仅获得srcFile路径中的最后文件名，以及log发生的所在行
                    ss << "[" << srcFile.substr(srcFile.find_last_of("/\\") + 1);
                    if (srcLine >= 0)
                        ss << ":" << srcLine;
                    ss << "] ";
                }
            }
            else
            {
                ss << " ";
            }

            return LogStream(this, curLevel, ss.str());
        }

        void Log::cleanupStream()
        {
            if (_os)
                _os->flush();
            if (_ofs.is_open())
            {
                _ofs.flush();
                _ofs.close();
            }
        }

        /// 把LogStream中的log信息写入到目标文件.
        void Log::log(LogStream *ls)
        {
            if (ls == nullptr)
                return;

            // log输出受到_level限制。
            if (ls->_curLevel < _level)
                return;

            // 写入log信息时上锁，基于两点考虑：
            // 1、防止线程间写入彼此干扰
            // 2、防止写入时setLogFile()被调用
            _mutex.lock();
            (*_os) << ls->str() << std::endl;
            _mutex.unlock();

            // 如果是fatal log，则终结进程
            if (ls->_curLevel == LogLevel::FATAL)
            {
                std::cerr<<"[F] Fatal error occured!"<<std::endl;
                fatal();
            }
        }

        void Log::fatal()
        {
            throw "Fatal error occured.";
        }

        std::string Log::formatString(char *format, ...)
        {
            char buf[512] = {0};

            va_list st;
            va_start(st, format);
            int nw = vsnprintf(buf, sizeof(buf), format, st);
            va_end(st);

            std::string str(buf);
            if (nw >= sizeof(buf))
            {
                // buf太小，导致log信息被截断了
                // TODO: 增大buf的固定大小，或者其他机制。
                str += " ... ";
            }

            return str;
        }

        std::string Log::formatString(const char *format, ...)
        {
            char buf[512] = {0};

            va_list st;
            va_start(st, format);
            int nw = vsnprintf(buf, sizeof(buf), format, st);
            va_end(st);

            std::string str(buf);
            if (nw >= sizeof(buf))
            {
                // buf太小，导致log信息被截断了
                // TODO: 增大buf的固定大小，或者其他机制。
                str += " ... ";
            }

            return str;
        }

        void setLogLevel(LogLevel level)
        {
            Log::instance()->setLogLevel(level);
        }

        void setLogFile(std::string file, bool append)
        {
            Log::instance()->setLogFile(file, append);
        }

        void enableLogPosition(bool filenameLogged, bool fullpathLogged)
        {
            Log::instance()->enableLogPosition(filenameLogged, fullpathLogged);
        }

        void enableLogTime(bool timeLogged)
        {
            Log::instance()->enableLogTime(timeLogged);
        }

    };
};
