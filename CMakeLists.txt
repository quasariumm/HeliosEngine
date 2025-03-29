cmake_minimum_required(VERSION 3.16)
project(Engine)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCE CONFIGURE_DEPENDS src/*.cpp src/*.h)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCE})

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
        src
        extern
        extern/imgui
        extern/imgui/examples/libs/glfw/include
)

target_link_directories(${CMAKE_PROJECT_NAME} PUBLIC
    extern/imgui/examples/libs/glfw
)

#target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC lib-vc-2010-64/glfw3.lib)