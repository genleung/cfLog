include_directories(${PROJECT_SOURCE_DIR}/src)

# -std=c++11, -std=c++14 are all OK
add_definitions(-std=c++14)

set(APP_SRC sample1.cpp)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
add_executable(sample1_static ${APP_SRC})
add_executable(sample1_dynamic ${APP_SRC})
target_link_libraries(sample1_static libcflog_static)
target_link_libraries(sample1_dynamic libcflog_dynamic)

#install(TARGETS sample1_static sample1_dynamic DESTINATION bin)
