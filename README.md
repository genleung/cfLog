# cfLog
A tiny, flexible, portable and thread-safe C++ logger for developers

## Compile
You can compile the cfLog as a static link library or a dynamic link library, based on your preferences. The CMake tool is required to build the libraries using our CMakeLists.txt. The procedure is quit simple on Linux & Mac system  (also easy with CMake-GUI on Windows system):
- mkdir build
- cd build
- cmake -DCMAKE_BUILD_TYPE=Debug ..
- make
- sudo make install

That's all, and you can find the samples in 'build/bin' directory, and the libraries are in 'build/lib' directory.

## Usage
cfLog is easy to use. Basically, it could be used in two styles:
- cf::utils::LOG*() Singleto macro mode (单实例宏模式)
- C++ object cf::utils::Log Multi-instances class-object mode(多实例类对象模式)

## Samples

    using namespace cf::utils;
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
    
    Log log;
    log()<<"kaka";
    log(LogLevel::WARNING)<<"This is a warning";
    
    DLOGF("dummy format string");


## Documents
cfLog uses doxygen to generate the source document. It is easy with doxygen:
    doxygen Doxyfile
    
