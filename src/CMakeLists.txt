cmake_minimum_required(VERSION 2.8)

# -std=c++11, -std=c++14 are all OK
add_definitions(-std=c++11)

set(LIB_SRC Log.cpp LogStream.cpp)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
add_library(libartlog_static ${LIB_SRC})
add_library(libartlog_dynamic SHARED ${LIB_SRC})
set_target_properties(libartlog_static PROPERTIES OUTPUT_NAME "artlog")
set_target_properties(libartlog_dynamic PROPERTIES OUTPUT_NAME "artlog")
