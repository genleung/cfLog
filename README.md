# cfarLog
A tiny, flexible and thread-safe C++ logger for developers

## Compile
You can compile the cfarlog as a static link library or a dynamic link library, based on your preferences. The CMake tool is required to build the libraries using our CMakeLists.txt. The procedure is quit simple on Linux system:
- mkdir build
- cd build
- cmake ..
- make

That's all, and you can find the samples in 'build/bin' directory, and the libraries are in 'build/lib' directory.

## Usage
cfarLog is easy to use. Basically, it could be used in two styles:
- cfar::LOG*() Macro style 
- C++ object cfar::Log style

## Samples

    using namespace cfar;
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
    
    cfar::Log log;
    log()<<"kaka";
    log(LogLevel::WARNING)<<"This is a warning";
    
    DLOGF("dummy format string");
