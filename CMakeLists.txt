cmake_minimum_required(VERSION 3.16)
project(Engine)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCE CONFIGURE_DEPENDS src/*.cpp src/*.h lib/glad/src/glad.c)
file(GLOB IMGUI CONFIGURE_DEPENDS extern/imgui/*.cpp extern/imgui/*.h)

add_executable(${CMAKE_PROJECT_NAME} ${IMGUI} ${SOURCE})

target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE
        "$<$<C_COMPILER_ID:MSVC>:/utf-8>"
        "$<$<NOT:$<C_COMPILER_ID:MSVC>>:-finput-charset=UTF-8;-fexec-charset=UTF-8>"
)
target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE
        "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>"
        "$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-finput-charset=UTF-8;-fexec-charset=UTF-8>"
)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
        src
        extern
        extern/imgui
        lib
        lib/glfw/include
        lib/glad/include
)

set(MSVC_LIBS
        glfw3.lib
)

set(UNIX_LIBS
        glfw3
)

if (MSVC)

    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        # 64-bit
        target_link_directories(${CMAKE_PROJECT_NAME} PUBLIC
                lib/glfw/lib-vc2010-64
        )
    else()
        # 32 bit
        target_link_directories(${CMAKE_PROJECT_NAME} PUBLIC
                lib/glfw/lib-vc2010-32
        )
    endif()

    target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC ${MSVC_LIBS})

else()

    target_link_directories(${CMAKE_PROJECT_NAME} PUBLIC lib/glfw/lib-mingw)

    target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC ${UNIX_LIBS})

endif()
