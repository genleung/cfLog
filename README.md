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
    const char *str="世界你好！";
       
    setLogLevel(LogLevel::INFO);
    enableLogPosition(true, true);
    setLogFile("test.txt", true);

    LOGI("A")<<a<<":Hello!!!";
    setLogFile("log.txt", true);
    TRACEI("world!");
    LOGE("")<<"error!";
    setLogFile();
    LOGW("warn...");
    TRACEE("debug error");
    
    LOGI("hello int:%d, char:%c, float:%f, string:%s", a, ch, f, str);
    TRACEFF("hello int:%d, char:%c, float:%f, string:%s", a, ch, f, str);

    Log log;
    log()<<"kaka";
    log(LogLevel::INFO, "warningTag")<<"This is a warn";

    TRACEF("dummy format string");
    std::string str2="string test";
    TRACEF("where is the string:%s", str2.c_str());


## Documents
cfLog uses doxygen to generate the source document. It is easy with doxygen:
    doxygen Doxyfile
    
