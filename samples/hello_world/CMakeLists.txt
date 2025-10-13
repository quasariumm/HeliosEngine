cmake_minimum_required(VERSION 3.31)
project(HelloWorldSample)

set(CMAKE_CXX_STANDARD 20)

set(ENGINE_PATH ../../)

add_subdirectory(${ENGINE_PATH} engine)

add_executable(HelloWorldSample main.cpp)

copy_engine_dlls(HelloWorldSample)

target_link_libraries(HelloWorldSample PRIVATE Engine)
