cmake_minimum_required(VERSION 3.16)
project(Engine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 17)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
		set(PLATFORM "x64")
	elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
		set(PLATFORM "arm64")
	else()
		set(PLATFORM "unknown")
	endif()
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(PLATFORM "x64")
	elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
		set(PLATFORM "x86")
	elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64")
		set(PLATFORM "arm64")
	else()
		set(PLATFORM "unknown")
	endif()
else()
	set(PLATFORM "unknown")
endif()

include(FetchContent)

# Determines which graphics API the engine uses. The following are supported
# Name				Alias
# OpenGL 4.6		GL46
# DirectX 12		DX12
# Vulkan			VK
set(HELIOS_GRAPHICS_API "GL46")


##############################################
##  	  Determine files to build  		##
##############################################
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS include/*.hpp)
file(GLOB_RECURSE SOURCE CONFIGURE_DEPENDS source/*.cpp)
file(GLOB_RECURSE BACKENDS CONFIGURE_DEPENDS backends/*.cpp backends/*.hpp)

file(GLOB_RECURSE GL46_BACKEND CONFIGURE_DEPENDS backends/opengl46_glfw/*.cpp backends/opengl46_glfw/*.hpp)
file(GLOB_RECURSE DX12_BACKEND CONFIGURE_DEPENDS src/Backends/DirectX12/*.cpp src/Backends/DirectX12/*.hpp)
file(GLOB_RECURSE VK_BACKEND CONFIGURE_DEPENDS src/Backends/Vulkan/*.cpp src/Backends/Vulkan/*.hpp)

##############################################
##				SSE/AVX flags				##
##############################################
include(CheckCXXCompilerFlag)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	# For GCC and Clang
	check_cxx_compiler_flag("-msse2" COMPILER_SUPPORTS_SSE2)
	check_cxx_compiler_flag("-mavx" COMPILER_SUPPORTS_AVX)
	check_cxx_compiler_flag("-mavx2" COMPILER_SUPPORTS_AVX2)

	if (COMPILER_SUPPORTS_SSE2)
		add_compile_options(-msse2)
	endif ()

	if (COMPILER_SUPPORTS_AVX)
		add_compile_options(-mavx)
	endif ()

	if (COMPILER_SUPPORTS_AVX2)
		add_compile_options(-mavx2)
		# Add FMA instructions. These are supported by all chips that have AVX2,
		# but require explicit options to enable them.
		add_compile_options(-mfma)
	endif ()

	# AVX512 flags will be added when we feel we can benefit from it

elseif (MSVC)
	# For MSVC
	add_compile_options(/arch:SSE2 /arch:AVX /arch:AVX2)
endif ()

##############################################
##  Leave only selected API's backend code	##
##############################################

if (HELIOS_GRAPHICS_API MATCHES "OpenGL 4.6|GL46")
	include(backends/opengl46_glfw/cmake/GL46_includes.cmake)
elseif(HELIOS_GRAPHICS_API MATCHES "DirectX 12|DX12")
	include(backends/cmake/DX12_includes.cmake)
elseif(HELIOS_GRAPHICS_API MATCHES "Vulkan|VK")
	include(backends/cmake/VK_includes.cmake)
endif()

###############################################
## Logging Engine and software library files ##
###############################################
list(LENGTH SOURCE NUM_SOURCE)
message(STATUS "CMake is including the following ${NUM_SOURCE} source files:")
foreach (file ${SOURCE})
	file(RELATIVE_PATH REL_PATH ${CMAKE_CURRENT_SOURCE_DIR} ${file})
	message(STATUS "  ${REL_PATH}")
endforeach ()

##############################################
##  Create lib/exe based on launch option	##
##############################################
add_library(Engine ${HEADERS} ${SOURCE} ${BACKENDS})

if (CMAKE_BUILD_TYPE MATCHES "Debug")
	set(TRACY_ENABLE ON)
endif ()

##############################################
##  	  API-specific configuration	  	##
##############################################

if (HELIOS_GRAPHICS_API MATCHES "OpenGL 4.6|GL46")
	include(backends/opengl46_glfw/cmake/GL46_config.cmake)
elseif(HELIOS_GRAPHICS_API MATCHES "DirectX 12|DX12")
	include(backends/cmake/DX12_config.cmake)
elseif(HELIOS_GRAPHICS_API MATCHES "Vulkan|VK")
	include(backends/cmake/VK_config.cmake)
endif()


##############################################
##  	Add external libraries's cmake		##
##############################################
add_subdirectory(extern/nativefiledialog-extended)
add_subdirectory(extern/Tracy)

##############################################
##  	  Compile defs and options  		##
##############################################

# Tracy defs
target_compile_definitions(Engine PRIVATE
		"$<$<CONFIG:Debug>:DEBUG=1>"
		"$<$<CONFIG:Release>:NDEBUG=1>"
		"$<$<CONFIG:Debug>:TRACY_ENABLE=1>"
		"$<$<CONFIG:Debug>:TRACY_CALLSTACK=10>"
)

# UTF8 compile flags
if (WIN32)
	if (MSVC)
		target_compile_options(Engine PRIVATE "/utf-8")
	elseif (NOT MINGW) # clang-cl
		target_compile_options(Engine PRIVATE "/source-charset=utf-8")
	endif ()
else() # MinGW and Linux
	target_compile_options(Engine PRIVATE "-finput-charset=UTF-8;-fexec-charset=UTF-8")
endif ()


##############################################
##  	    	FMOD Studio API	    		##
##############################################
set(FMOD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/fmod
		CACHE INTERNAL "")

target_include_directories(Engine PUBLIC
		${FMOD_DIR}/core/inc
		${FMOD_DIR}/studio/inc
)
if (WIN32 AND PLATFORM STREQUAL "x64")
	target_link_directories(Engine PUBLIC
			${FMOD_DIR}/core/lib/x64
			${FMOD_DIR}/studio/lib/x64
	)
	set (FMOD_DYN_LIBS_DEBUG
			"$<$<CONFIG:Debug>:${FMOD_DIR}/core/lib/x64/fmodL.dll>"
			"$<$<CONFIG:Debug>:${FMOD_DIR}/studio/lib/x64/fmodstudioL.dll>"
			CACHE INTERNAL ""
	)
	set (FMOD_DYN_LIBS_RELEASE
			"$<$<CONFIG:Release>:${FMOD_DIR}/core/lib/x64/fmod.dll>"
			"$<$<CONFIG:Release>:${FMOD_DIR}/studio/lib/x64/fmodstudio.dll>"
			CACHE INTERNAL ""
	)
elseif (WIN2 AND PLATFORM STREQUAL "arm64")
	target_link_directories(Engine PUBLIC
			${FMOD_DIR}/core/lib/arm64
			${FMOD_DIR}/studio/lib/arm64
	)
	set (FMOD_DYN_LIBS_DEBUG
			"$<$<CONFIG:Debug>:${FMOD_DIR}/core/lib/arm64/fmodL.dll>"
			"$<$<CONFIG:Debug>:${FMOD_DIR}/studio/lib/arm64/fmodstudioL.dll>"
			CACHE INTERNAL ""
	)
	set (FMOD_DYN_LIBS_RELEASE
			"$<$<CONFIG:Release>:${FMOD_DIR}/core/lib/arm64/fmod.dll>"
			"$<$<CONFIG:Release>:${FMOD_DIR}/studio/lib/arm64/fmodstudio.dll>"
			CACHE INTERNAL ""
	)
else()
	message(FATAL_ERROR "Linux libs for FMOD Studio not yet added. x86 is just not supported by us. Mac users will not get support for a LONG while.")
endif()
target_link_libraries(Engine PUBLIC
		winmm ole32 user32
		"$<$<CONFIG:Debug>:fmodL_vc>"
		"$<$<CONFIG:Release>:fmod_vc>"
		"$<$<CONFIG:Debug>:fmodstudioL_vc>"
		"$<$<CONFIG:Release>:fmodstudio_vc>"
)

##############################################
##		    Fetch external libraries		##
##############################################

## JOLT PHYSICS
set(ENABLE_ALL_WARNINGS OFF)
set(BUILD_SHARED_LIBS 	ON)
FetchContent_Declare(
		JoltPhysics
		GIT_REPOSITORY "https://github.com/jrouwe/JoltPhysics"
		GIT_TAG "v5.4.0"
		SOURCE_SUBDIR "Build"
)
FetchContent_MakeAvailable(JoltPhysics)

## FMT
set(FMT_INSTALL ON)
FetchContent_Declare(
		fmt
		GIT_REPOSITORY "https://github.com/fmtlib/fmt.git"
		GIT_TAG "12.1.0"
)
FetchContent_MakeAvailable(fmt)

## NLOHMANN JSON
FetchContent_Declare(
		json
		GIT_REPOSITORY "https://github.com/nlohmann/json.git"
		GIT_TAG "v3.12.0"
)
FetchContent_MakeAvailable(json)

## VISIT STRUCT
FetchContent_Declare(
		visit_struct
		GIT_REPOSITORY "https://github.com/cbeck88/visit_struct.git"
		GIT_TAG "1.2.0"
)
FetchContent_MakeAvailable(visit_struct)
add_library(visit_struct INTERFACE)
target_include_directories(visit_struct INTERFACE ${visit_struct_SOURCE_DIR}/include)

## IMGUI
FetchContent_Declare(
		imgui
		GIT_REPOSITORY "https://github.com/ocornut/imgui.git"
		GIT_TAG "v1.92.4-docking"
)
FetchContent_MakeAvailable(imgui)
add_library(imgui INTERFACE)
target_include_directories(imgui INTERFACE ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)

## IMGUIZMO
FetchContent_Declare(
		imguizmo
		GIT_REPOSITORY "https://github.com/CedricGuillemet/ImGuizmo.git"
		GIT_TAG "1.83"
)
FetchContent_MakeAvailable(imguizmo)
add_library(imguizmo INTERFACE)
target_include_directories(imguizmo INTERFACE ${imguizmo_SOURCE_DIR})


##############################################
##		      Copy external DLLs  			##
##############################################

function(copy_engine_dlls PROJ)
	# Copy the FMOD dlls
	add_custom_command(TARGET ${PROJ} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"$<$<CONFIG:Debug>:${FMOD_DYN_LIBS_DEBUG}>"
			"$<$<CONFIG:Release>:${FMOD_DYN_LIBS_RELEASE}>"
			"$<$<CONFIG:Debug>:${JoltPhysics_SOURCE_DIR}/../../joltphysics-build/Jolt.dll>"
			"$<$<CONFIG:Release>:${JoltPhysics_SOURCE_DIR}/../../joltphysics-build/Jolt.dll>"
			$<TARGET_FILE_DIR:${PROJ}>
			COMMAND_EXPAND_LISTS
			COMMENT "Copying FMOD DLLs"
	)
endfunction()

##############################################
## Include directories + precompiled header ##
##############################################
target_include_directories(Engine PUBLIC
		pch
		source
		include
		extern
		extern/nativefiledialog-extended/src/include
		extern/glm
		extern/fmt/include
		lib
		lib/glfw/include
		lib/glad/include
		lib/fmod/core/inc
		lib/fmod/studio/inc
		${JoltPhysics_SOURCE_DIR}/..
		${TRACY_PUBLIC_DIR}
)

target_precompile_headers(Engine PUBLIC
		"$<$<COMPILE_LANGUAGE:CXX>:precomp_cxx.hpp>"
		"$<$<COMPILE_LANGUAGE:C>:precomp_c.hpp>"
)


##############################################
##  		  Linking libraries  			##
##############################################
target_link_libraries(Engine PUBLIC
		nfd
		Tracy::TracyClient
		fmt::fmt-header-only
		Jolt
		nlohmann_json
		visit_struct
		imgui
		imguizmo
)