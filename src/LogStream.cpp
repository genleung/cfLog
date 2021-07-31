#include "LogStream.h"
#include "Log.h"

namespace cf {
    namespace utils {
        LogStream::LogStream(Log* p, LogLevel l, std::string pre)
            : _pLog(p), _curLevel(l), _prefix(pre) {
            (*this) << _prefix;
        }

        LogStream::LogStream(const LogStream& ls)
            : _pLog(ls._pLog), _curLevel(ls._curLevel), _prefix(ls._prefix) {
            (*this) << _prefix;
        }

        LogStream::~LogStream() {
            if (_pLog) {
                _pLog->log(this);
            }
        }
    };
};
