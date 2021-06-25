cmake_minimum_required(VERSION 3.10)

# -std=c++11, -std=c++14 are all OK
add_definitions(-std=c++14)

set(LIB_SRC Log.cpp LogStream.cpp)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
add_library(libcflog_static ${LIB_SRC})
add_library(libcflog_dynamic SHARED ${LIB_SRC})
set_target_properties(libcflog_static PROPERTIES OUTPUT_NAME "cflog")
set_target_properties(libcflog_dynamic PROPERTIES OUTPUT_NAME "cflog")

install(TARGETS libcflog_static libcflog_dynamic DESTINATION lib)
install(FILES "Log.h" "LogStream.h" DESTINATION include/cf)
