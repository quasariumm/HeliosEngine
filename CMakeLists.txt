cmake_minimum_required(VERSION 3.16)
project(Engine)

set(CMAKE_CXX_STANDARD 20)

# Determines whether the engine loads in standalone:
# - OFF (Default):
# 		Normal way of loading the engine. Launches using a dynamic library and separate launcher executable
# - ON (Standalone):
# 		Launches it as an executable, which makes debugging easier, but does not allow project loading
# 		NOTE: Make sure to use a config that launches the executable Engine.exe
option( ENGINE_STANDALONE
		"Use standalone mode"
		OFF)

# Determines which graphics API the engine uses. The following are supported
# Name				Alias
# OpenGL 4.6		GL46
# DirectX 12		DX12
# Vulkan			VK
set(HELIOS_GRAPHICS_API "GL46")

# Adds the launcher to the project
# Used in Default launch option
add_subdirectory(launcher)


##############################################
##  	  Determine files to build  		##
##############################################
file(GLOB_RECURSE SOURCE CONFIGURE_DEPENDS src/*.cpp src/*.h)

file(GLOB_RECURSE GL46_BACKEND CONFIGURE_DEPENDS src/Backends/OpenGL46_GLFW/*.cpp src/Backends/OpenGL46_GLFW/*.h)
file(GLOB_RECURSE DX12_BACKEND CONFIGURE_DEPENDS src/Backends/DirectX12/*.cpp src/Backends/DirectX12/*.h)
file(GLOB_RECURSE VK_BACKEND CONFIGURE_DEPENDS src/Backends/Vulkan/*.cpp src/Backends/Vulkan/*.h)

file(GLOB LIBS CONFIGURE_DEPENDS
		extern/imgui/*.cpp
		extern/imgui/*.h
)


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
	include(cmake/GL46_includes.cmake)
elseif(HELIOS_GRAPHICS_API MATCHES "DirectX 12|DX12")
	include(cmake/DX12_includes.cmake)
elseif(HELIOS_GRAPHICS_API MATCHES "Vulkan|VK")
	include(cmake/VK_includes.cmake)
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

message(STATUS "CMake is including the following library files:")
foreach (file ${LIBS})
	file(RELATIVE_PATH REL_PATH ${CMAKE_CURRENT_SOURCE_DIR} ${file})
	message(STATUS "  ${REL_PATH}")
endforeach ()

##############################################
##  Create lib/exe based on launch option	##
##############################################
if (ENGINE_STANDALONE)
	add_executable(Engine ${LIBS} ${SOURCE})
	message("Launching engine in standalone mode... Creating executable")
else ()
	add_library(Engine SHARED ${LIBS} ${SOURCE})
	message("Launching engine in default mode... Creating library")
endif ()

target_compile_definitions(Engine PRIVATE ENGINE_BUILD_DLL)

if (CMAKE_BUILD_TYPE MATCHES "Debug")
	set(TRACY_ENABLE ON)
endif ()

##############################################
##  	  API-specific configuration	  	##
##############################################

if (HELIOS_GRAPHICS_API MATCHES "OpenGL 4.6|GL46")
	include(cmake/GL46_config.cmake)
elseif(HELIOS_GRAPHICS_API MATCHES "DirectX 12|DX12")
	include(cmake/DX12_config.cmake)
elseif(HELIOS_GRAPHICS_API MATCHES "Vulkan|VK")
	include(cmake/VK_config.cmake)
endif()


##############################################
##  	Add external libraries's cmake		##
##############################################
add_subdirectory(extern/nativefiledialog-extended)
add_subdirectory(extern/Tracy)

##############################################
##  		  Configure Assimp  			##
##############################################
set(BUILD_SHARED_LIBS 	OFF	)
set(ASSIMP_INSTALL 		OFF	)
# Optimise build times by turning off certain importers
# and turning off exporters, since we don't support it (yet?)
set(ASSIMP_NO_EXPORT 						ON	)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT	OFF	)
set(ASSIMP_BUILD_3DS_IMPORTER 				ON	)
set(ASSIMP_BUILD_ASE_IMPORTER 				ON	)
set(ASSIMP_BUILD_OBJ_IMPORTER				ON	)
set(ASSIMP_BUILD_PLY_IMPORTER				ON	)
set(ASSIMP_BUILD_FBX_IMPORTER				ON	)
set(ASSIMP_BUILD_RAW_IMPORTER				ON	)
set(ASSIMP_BUILD_GLTF_IMPORTER				ON	)
set(ASSIMP_BUILD_AMF_IMPORTER 				OFF	)
set(ASSIMP_BUILD_AC_IMPORTER 				OFF	)
set(ASSIMP_BUILD_ASSBIN_IMPORTER			OFF	) # What is this name ?!?!?! XD
set(ASSIMP_BUILD_B3D_IMPORTER				OFF	)
set(ASSIMP_BUILD_BVH_IMPORTER				OFF	)
set(ASSIMP_BUILD_COLLADA_IMPORTER			OFF	) # This is already legacy, so why support it
set(ASSIMP_BUILD_DXF_IMPORTER				OFF	)
set(ASSIMP_BUILD_CSM_IMPORTER				OFF	)
set(ASSIMP_BUILD_HMP_IMPORTER				OFF	)
set(ASSIMP_BUILD_IRRMESH_IMPORTER			OFF	)
set(ASSIMP_BUILD_IQM_IMPORTER				OFF	)
set(ASSIMP_BUILD_IRR_IMPORTER				OFF	)
set(ASSIMP_BUILD_LWO_IMPORTER				OFF	)
set(ASSIMP_BUILD_LWS_IMPORTER				OFF	)
set(ASSIMP_BUILD_MD2_IMPORTER				OFF	)
set(ASSIMP_BUILD_MD3_IMPORTER				OFF	)
set(ASSIMP_BUILD_MD5_IMPORTER				OFF	)
set(ASSIMP_BUILD_MDC_IMPORTER				OFF	)
set(ASSIMP_BUILD_MDL_IMPORTER				OFF	)
set(ASSIMP_BUILD_NFF_IMPORTER				OFF	)
set(ASSIMP_BUILD_NDO_IMPORTER				OFF	)
set(ASSIMP_BUILD_OFF_IMPORTER				OFF	)
set(ASSIMP_BUILD_OGRE_IMPORTER				OFF	)
set(ASSIMP_BUILD_OPENGEX_IMPORTER			OFF	)
set(ASSIMP_BUILD_MS3D_IMPORTER				OFF	)
set(ASSIMP_BUILD_COB_IMPORTER				OFF	)
set(ASSIMP_BUILD_BLEND_IMPORTER				OFF	)
set(ASSIMP_BUILD_IFC_IMPORTER				OFF	)
set(ASSIMP_BUILD_XGL_IMPORTER				OFF	)
set(ASSIMP_BUILD_Q3D_IMPORTER				OFF	)
set(ASSIMP_BUILD_Q3BSP_IMPORTER				OFF	)
set(ASSIMP_BUILD_SIB_IMPORTER				OFF	)
set(ASSIMP_BUILD_SMD_IMPORTER				OFF	)
set(ASSIMP_BUILD_STL_IMPORTER				OFF	)
set(ASSIMP_BUILD_TERRAGEN_IMPORTER			OFF	)
set(ASSIMP_BUILD_3D_IMPORTER				OFF	)
set(ASSIMP_BUILD_X_IMPORTER					OFF	)
set(ASSIMP_BUILD_X3D_IMPORTER				OFF	)
set(ASSIMP_BUILD_3MF_IMPORTER				OFF	)
set(ASSIMP_BUILD_MDD_IMPORTER				OFF	)
set(ASSIMP_BUILD_TESTS 						OFF )

add_subdirectory(extern/Assimp)
set(CMAKE_CXX_STANDARD 20) # Because Assimp changes it back to C++17


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
target_compile_options(Engine PRIVATE
		"$<$<C_COMPILER_ID:MSVC>:/utf-8>"
		"$<$<NOT:$<C_COMPILER_ID:MSVC>>:-finput-charset=UTF-8;-fexec-charset=UTF-8>"
)
target_compile_options(Engine PRIVATE
		"$<$<CXX_COMPILER_ID:MSVC>:/utf-8>"
		"$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-finput-charset=UTF-8;-fexec-charset=UTF-8>"
)


##############################################
## Include directories + precompiled header ##
##############################################
target_include_directories(Engine PUBLIC
		pch
		src
		extern
		extern/imgui
		extern/nativefiledialog-extended/src/include
		extern/Assimp/include
		extern/glm
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


##############################################
##  		  Linking libraries  			##
##############################################
target_link_libraries(Engine PUBLIC nfd Tracy::TracyClient assimp::assimp)