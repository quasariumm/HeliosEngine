cmake_minimum_required(VERSION 3.16)
project(Engine)

set(CMAKE_CXX_STANDARD 20)

# Set the way the engine launches:
# "Default"
# Normal way of loading the engine
# "Standalone"
# Launches it as an executable which makes debugging easier, but does not allow project loading
# Make sure to use a config that launches the executable

set(ENGINE_LAUNCH_MODE "Standalone")

add_subdirectory(launcher)

file(GLOB_RECURSE SOURCE CONFIGURE_DEPENDS src/*.cpp src/*.h lib/glad/src/glad.c)
file(GLOB LIBS CONFIGURE_DEPENDS
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
    check_cxx_compiler_flag("-mavx2" COMPILER_SUPPORTS_AVX2)

    if(COMPILER_SUPPORTS_SSE2)
        add_compile_options(-msse2)
    endif()

    if(COMPILER_SUPPORTS_AVX)
        add_compile_options(-mavx)
    endif()

    if(COMPILER_SUPPORTS_AVX2)
        add_compile_options(-mavx2)
    endif()
elseif(MSVC)
    # For MSVC
    add_compile_options(/arch:SSE2 /arch:AVX)
endif()

# Create library or executable based on launch option
if(ENGINE_LAUNCH_MODE MATCHES "Standalone")
    add_executable(Engine ${LIBS} ${SOURCE})
    message("Launching engine in standalone mode... Creating executable")
else ()
    add_library(Engine SHARED ${LIBS} ${SOURCE})
    message("Launching engine in default mode... Creating library")
endif ()

target_compile_definitions(Engine PRIVATE ENGINE_BUILD_DLL)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(TRACY_ENABLE     ON)
endif()

add_subdirectory(extern/nativefiledialog-extended)
add_subdirectory(extern/Tracy)


target_compile_definitions(Engine PRIVATE
        "$<$<CONFIG:Debug>:DEBUG=1>"
        "$<$<CONFIG:Release>:NDEBUG=1>"
        "$<$<CONFIG:Debug>:TRACY_ENABLE=1>"
        "$<$<CONFIG:Debug>:TRACY_CALLSTACK=10>"
)


target_compile_options(Engine PRIVATE
        "$<$<C_COMPILER_ID:MSVC>:/utf-8>"
        "$<$<NOT:$<C_COMPILER_ID:MSVC>>:-finput-charset=UTF-8;-fexec-charset=UTF-8>"
)
target_compile_options(Engine PRIVATE
        "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>"
        "$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-finput-charset=UTF-8;-fexec-charset=UTF-8>"
)

target_include_directories(Engine PUBLIC
        pch
        src
        extern
        extern/imgui
        extern/nativefiledialog-extended/src/include
        extern/Assimp/include
        lib
        lib/glfw/include
        lib/glad/include
        include
        ${TRACY_PUBLIC_DIR}
)

target_precompile_headers(Engine PUBLIC
        "$<$<COMPILE_LANGUAGE:CXX>:PrecompCXX.h>"
        "$<$<COMPILE_LANGUAGE:C>:PrecompC.h>"
)

set(MSVC_LIBS
        glfw3.lib
        libassimp-5d.dll
)

set(UNIX_LIBS
        glfw3
        assimpd
        zlibstaticd
)


target_link_libraries(Engine PUBLIC nfd Tracy::TracyClient)

if (MSVC)

    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        # 64-bit
        target_link_directories(Engine PUBLIC
                lib/glfw/lib-vc2010-64
                "$<$<CONFIG:Debug>:${CMAKE_CURRENT_SOURCE_DIR}/extern/Assimp/cmake-build-debug/bin>"
                "$<$<CONFIG:Release>:${CMAKE_CURRENT_SOURCE_DIR}/extern/Assimp/cmake-build-release/bin>"
        )
    else()
        # 32 bit
        target_link_directories(Engine PUBLIC
                lib/glfw/lib-vc2010-32
                "$<$<CONFIG:Debug>:${CMAKE_CURRENT_SOURCE_DIR}/extern/Assimp/cmake-build-debug/bin>"
                "$<$<CONFIG:Release>:${CMAKE_CURRENT_SOURCE_DIR}/extern/Assimp/cmake-build-release/bin>"
        )
    endif()

    target_link_libraries(Engine PUBLIC ${MSVC_LIBS})

else()

    target_link_directories(Engine PUBLIC
            lib/glfw/lib-mingw
            "$<$<CONFIG:Debug>:${CMAKE_CURRENT_SOURCE_DIR}/extern/Assimp/cmake-build-debug/lib>"
            "$<$<CONFIG:Debug>:${CMAKE_CURRENT_SOURCE_DIR}/extern/Assimp/cmake-build-debug/contrib/zlib>"
            "$<$<CONFIG:Release>:${CMAKE_CURRENT_SOURCE_DIR}/extern/Assimp/cmake-build-release/lib>"
            "$<$<CONFIG:Release>:${CMAKE_CURRENT_SOURCE_DIR}/extern/Assimp/cmake-build-release/contrib/zlib>"
    )

    target_link_libraries(Engine PUBLIC ${UNIX_LIBS})

endif()

list(LENGTH SOURCE NUM_SOURCE)
message(STATUS "CMake is including the following ${NUM_SOURCE} source files:")
foreach(file ${SOURCE})
    message(STATUS "  ${file}")
endforeach()

message(STATUS "CMake is including the following library files:")
foreach(file ${LIBS})
    message(STATUS "  ${file}")
endforeach()