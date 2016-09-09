# artLog
A tiny, flexible and thread-safe C++ logger for developers

## Usage
artLog is easy to use. Basically, it could be used in two styles:
- art::LOG*() Macro style 
- C++ object art::Log style

## Samples

    using namespace art;
    ...
    
    int a=123;
    char ch='a';
    float f=1.234;
    const char *str="hello world";
    
    setLogLevel(LogLevel::INFO);
    enableLogPosition(true);
    setLogFile("test.txt", true);
    LOG()<<a<<":Hello!!!";
    setLogFile("log.txt", true);
    DLOG("world!");
    LOGL(ERROR)<<"error!";
    setLogFile();
    LOGL(WARNING)<<"warning...";
    DLOGL(ERROR, "debug error");
    
    LOGF("hello int:%d, char:%c, float:%f, string:%s", a, ch, f, str);
    DLOGLF(ERROR, "hello int:%d, char:%c, float:%f, string:%s", a, ch, f, str);
    
    art::Log log;
    log()<<"kaka";
    log(LogLevel::WARNING)<<"This is a warning";
    
    DLOGF("dummy format string");
