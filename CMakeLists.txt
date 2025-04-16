cmake_minimum_required(VERSION 3.16)
project(Engine)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCE CONFIGURE_DEPENDS src/*.cpp src/*.h lib/glad/src/glad.c)
file(GLOB IMGUI CONFIGURE_DEPENDS
        extern/imgui/*.cpp
        extern/imgui/*.h
        extern/imgui/backends/imgui_impl_opengl3.h
        extern/imgui/backends/imgui_impl_opengl3.cpp
        extern/imgui/backends/imgui_impl_opengl3_loader.h
        extern/imgui/backends/imgui_impl_glfw.h
        extern/imgui/backends/imgui_impl_glfw.cpp
        )


include(CheckCXXCompilerFlag)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # For GCC and Clang
    check_cxx_compiler_flag("-msse2" COMPILER_SUPPORTS_SSE2)
    check_cxx_compiler_flag("-mavx" COMPILER_SUPPORTS_AVX)

    if(COMPILER_SUPPORTS_SSE2)
        add_compile_options(-msse2)
    endif()

    if(COMPILER_SUPPORTS_AVX)
        add_compile_options(-mavx)
    endif()
elseif(MSVC)
    # For MSVC
    add_compile_options(/arch:SSE2 /arch:AVX)
endif()


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
        pch
        src
        extern
        extern/imgui
        extern/nativefiledialog-extended/src/include
        lib
        lib/glfw/include
        lib/glad/include
)

target_precompile_headers(${CMAKE_PROJECT_NAME} PUBLIC
        "$<$<COMPILE_LANGUAGE:CXX>:PrecompCXX.h>"
        "$<$<COMPILE_LANGUAGE:C>:PrecompC.h>"
)

set(MSVC_LIBS
        glfw3.lib
)

set(UNIX_LIBS
        glfw3
)

add_subdirectory(extern/nativefiledialog-extended)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE nfd)

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
