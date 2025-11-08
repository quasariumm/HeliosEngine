cmake_minimum_required(VERSION 3.16)
project(Offline)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 17)

set(ASSIMP_BUILD_IFC_IMPORTER OFF CACHE BOOL "" FORCE)

# Add Assimp from extern folder
add_subdirectory(../extern/Assimp ${CMAKE_BINARY_DIR}/assimp)

# Collect source and header files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
        source/*.cpp
)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
        include/*.h
        include/*.hpp
)

# Create executable for the asset preparation tool
add_executable(AssetPrep ${SOURCES} ${HEADERS})

# Add include directories
target_include_directories(AssetPrep PRIVATE
        ../extern/Assimp/include/assimp
        ../extern/glm/glm
)

# Link Assimp (GLM is header-only, no linking needed)
target_link_libraries(AssetPrep PRIVATE assimp)

# Copy Assimp DLL to output directory
if(WIN32)
    add_custom_command(TARGET AssetPrep POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:assimp>
            $<TARGET_FILE_DIR:AssetPrep>
    )

    # Copy MinGW runtime DLLs
    if(MINGW)
        get_filename_component(MINGW_BIN_PATH ${CMAKE_CXX_COMPILER} DIRECTORY)
        add_custom_command(TARGET AssetPrep POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${MINGW_BIN_PATH}/libgcc_s_seh-1.dll"
                "${MINGW_BIN_PATH}/libstdc++-6.dll"
                "${MINGW_BIN_PATH}/libwinpthread-1.dll"
                $<TARGET_FILE_DIR:AssetPrep>
        )
    endif()
endif()

set_target_properties(AssetPrep PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)