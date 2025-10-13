#pragma once

#include <glad/glad.h>

#include "IconDefines.h"
#include "float16_t.hpp"

#ifdef ENGINE_BUILD_DLL
  #define ENGINE_API __declspec(dllexport)
#else
  #define ENGINE_API __declspec(dllimport)
#endif

namespace Engine
{
class Material;
class Light;
}

/*
 * Console logging tags
 */

static const std::wstring ANSI_OK = L"[\033[32m OK \033[0m] ";
static const std::wstring ANSI_DONE = L"[\033[32mDONE\033[0m] ";
static const std::wstring ANSI_ERR = L"[\033[31mERRO\033[0m] ";
static const std::wstring ANSI_WARN = L"[\033[33mWARN\033[0m] ";
static const std::wstring ANSI_INFO = L"[\033[36mINFO\033[0m] ";

/*
 * Typedefs
 */

namespace Engine
{
typedef glm::u8vec4		Color8;
typedef glm::vec4		Color32F;
}



/*
 * Static lists
 */

// TODO: Replace these with handlers to prevent issues with project dll loading

static std::unordered_set<std::string> loadedShaders;
static std::vector<Engine::Light*> lights;

/*
 * Defines
 */

static constexpr float PI 			= 	std::numbers::pi_v<float>;
static constexpr float INVPI 		= 	std::numbers::inv_pi_v<float>;
static constexpr float INV2PI		=	1.f / (2.f * PI);
static constexpr float TWOPI		=	2.f * PI;
static constexpr float SQRT_PI_INV	=	std::numbers::inv_sqrtpi_v<float>;
static constexpr float LARGE_FLOAT	=	1e34f;



/*
 * Macros
 */

#define DEG(x) ( (x) * 57.2957795131f )
#define RAD(x) ( (x) * 0.0174532925f )

#define CALL(func, ...) if (func) (func).operator()(__VA_ARGS__)

/**
 * Aligned new operator	\n
 * Usage example: ALIGNED_NEW(64) uint32_t[100];
 */
#define ALIGNED_NEW(alignment) new (std::align_val_t(alignment))
#define ALIGNED_LIST_DELETE(ptr, alignment) ::operator delete[](ptr, std::align_val_t(alignment))

// Because OpenGL likes their size types as signed
#define sizeofll(obj) (int64_t)sizeof(obj)

/**
 * @brief Converts a string to a wide string
 * @attention This is slow. If you work with string literals, please use TEXT
 * @param str The string (any std type you want) you want to convert
 */
#define STR_TO_WSTR(str) std::filesystem::path(str).wstring()

/**
 * @brief Converts a wide string to a string
 * @attention This is slow.
 * @param wstr The string (any std type you want) you want to convert
 */
#define WSTR_TO_STR(wstr) std::filesystem::path(wstr).string()

/**
 * @brief An alternative way to convert a string literal to the wstrings we use
 * @attention This only works for inline string literals. E.g. TEXT("Hello")
 * @param str The string literal
 */
#define TEXT(str) L##str

/**
 * @brief Checks if a point is in a 2D bounding box
 * @param cmp The point to compare
 * @param min The top left corner
 * @param max The bottom right corner
 */
#define CHECK_AABB2D(cmp, min, max) ((cmp).x >= (min).x && (cmp).x <= (max).x && (cmp).y >= (min).y && (cmp).y <= (max).y)

/**
 * @brief Checks if a point is in a 3D bounding box
 * @param cmp The point to compare
 * @param min The top left corner
 * @param max The bottom right corner
 */
#define CHECK_AABB3D(cmp, min, max) ((cmp).x >= (min).x && (cmp).x <= (max).x && (cmp).y >= (min).y && (cmp).y <= (max).y && (cmp).z >= (min).z && (cmp).z <= (max).z)



/*
 * Functions
 */

/**
 * @brief Converts a wide string to a UTF-8 encoded basic string
 * @param wstr The wide string to convert
 * @return The converted UTF-8 encoded string
 */
inline std::string WStringToUTF8( const std::wstring& wstr )
{
	const std::filesystem::path path( wstr );
	return {(const char*)path.u8string().c_str()};
}


inline std::wstring GLDebugTypeToString( const GLenum type )
{
	switch (type)
	{
	case 0x824c:
		return L"GL_DEBUG_TYPE_ERROR";
	case 0x824d:
		return L"GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR";
	case 0x824e:
		return L"GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR";
	case 0x824f:
		return L"GL_DEBUG_TYPE_PORTABILITY";
	case 0x8250:
		return L"GL_DEBUG_TYPE_PERFORMANCE";
	case 0x8251:
		return L"GL_DEBUG_TYPE_OTHER";
	default:
		return std::to_wstring(type);
	}
}


inline std::wstring GLErrorToString( const uint32_t error )
{
	switch (error)
	{
	// Errors
	case 0:
		return L"GL_NO_ERROR";
	case 0x0500:
		return L"GL_INVALID_ENUM";
	case 0x0501:
		return L"GL_INVALID_VALUE";
	case 0x0502:
		return L"GL_INVALID_OPERATION";
	case 0x0503:
		return L"GL_STACK_OVERFLOW";
	case 0x0504:
		return L"GL_STACK_UNDERFLOW";
	case 0x0505:
		return L"GL_OUT_OF_MEMORY";
	case 0x0506:
		return L"GL_INVALID_FRAMEBUFFER_OPERATION";
	case 0x0507:
		return L"GL_CONTEXT_LOST";
	// Framebuffer status
	case 0x8cd5:
		return L"GL_FRAMEBUFFER_COMPLETE";
	case 0x8219:
		return L"GL_FRAMEBUFFER_UNDEFINED";
	case 0x8dc6:
		return L"GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
	case 0x8cd7:
		return L"GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
	case 0x8cd8:
		return L"GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
	case 0x8cd9:
		return L"GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
	default:
		return std::to_wstring(error);
	}
}


inline std::wstring GLSeverityToString( const GLenum sev )
{
	switch (sev)
	{
	case 0x826b:
		return L"GL_DEBUG_SEVERITY_NOTIFICATION";
	case 0x9146:
		return L"GL_DEBUG_SEVERITY_HIGH";
	case 0x9147:
		return L"GL_DEBUG_SEVERITY_MEDIUM";
	case 0x9148:
		return L"GL_DEBUG_SEVERITY_LOW";
	default:
		return std::to_wstring(sev);
	}
}

#if defined CL_VERSION_3_0
static constexpr const char* CL_VERSION_STR = "CL3.0";
#elif defined CL_VERSION_2_2
static constexpr const char* CL_VERSION_STR = "CL2.2";
#elif defined CL_VERSION_2_1
static constexpr const char* CL_VERSION_STR = "CL2.1";
#elif defined CL_VERSION_2_0
static constexpr const char* CL_VERSION_STR = "CL2.0";
#elif defined CL_VERSION_1_2
static constexpr const char* CL_VERSION_STR = "CL1.2";
#elif defined CL_VERSION_1_1
static constexpr const char* CL_VERSION_STR = "CL1.1";
#elif defined CL_VERSION_1_0
static constexpr const char* CL_VERSION_STR = "CL1.0";
#endif

inline std::wstring CLErrorString( const cl_int result )
{
	switch (result)
	{
	case CL_DEVICE_NOT_FOUND:
		return L"CL_DEVICE_NOT_FOUND";
	case CL_DEVICE_NOT_AVAILABLE:
		return L"CL_DEVICE_NOT_AVAILABLE";
	case CL_COMPILER_NOT_AVAILABLE:
		return L"CL_COMPILER_NOT_AVAILABLE";
	case CL_MEM_OBJECT_ALLOCATION_FAILURE:
		return L"CL_MEM_OBJECT_ALLOCATION_FAILURE";
	case CL_OUT_OF_RESOURCES:
		return L"CL_OUT_OF_RESOURCES";
	case CL_OUT_OF_HOST_MEMORY:
		return L"CL_OUT_OF_HOST_MEMORY";
	case CL_PROFILING_INFO_NOT_AVAILABLE:
		return L"CL_PROFILING_INFO_NOT_AVAILABLE";
	case CL_MEM_COPY_OVERLAP:
		return L"CL_MEM_COPY_OVERLAP";
	case CL_IMAGE_FORMAT_MISMATCH:
		return L"CL_IMAGE_FORMAT_MISMATCH";
	case CL_IMAGE_FORMAT_NOT_SUPPORTED:
		return L"CL_IMAGE_FORMAT_NOT_SUPPORTED";
	case CL_BUILD_PROGRAM_FAILURE:
		return L"CL_BUILD_PROGRAM_FAILURE";
	case CL_MAP_FAILURE:
		return L"CL_MAP_FAILURE";
	case CL_MISALIGNED_SUB_BUFFER_OFFSET:
		return L"CL_MISALIGNED_SUB_BUFFER_OFFSET";
	case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:
		return L"CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
	case CL_INVALID_VALUE:
		return L"CL_INVALID_VALUE";
	case CL_INVALID_DEVICE_TYPE:
		return L"CL_INVALID_DEVICE_TYPE";
	case CL_INVALID_PLATFORM:
		return L"CL_INVALID_PLATFORM";
	case CL_INVALID_DEVICE:
		return L"CL_INVALID_DEVICE";
	case CL_INVALID_CONTEXT:
		return L"CL_INVALID_CONTEXT";
	case CL_INVALID_QUEUE_PROPERTIES:
		return L"CL_INVALID_QUEUE_PROPERTIES";
	case CL_INVALID_COMMAND_QUEUE:
		return L"CL_INVALID_COMMAND_QUEUE";
	case CL_INVALID_HOST_PTR:
		return L"CL_INVALID_HOST_PTR";
	case CL_INVALID_MEM_OBJECT:
		return L"CL_INVALID_MEM_OBJECT";
	case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
		return L"CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
	case CL_INVALID_IMAGE_SIZE:
		return L"CL_INVALID_IMAGE_SIZE";
	case CL_INVALID_SAMPLER:
		return L"CL_INVALID_SAMPLER";
	case CL_INVALID_BINARY:
		return L"CL_INVALID_BINARY";
	case CL_INVALID_BUILD_OPTIONS:
		return L"CL_INVALID_BUILD_OPTIONS";
	case CL_INVALID_PROGRAM:
		return L"CL_INVALID_PROGRAM";
	case CL_INVALID_PROGRAM_EXECUTABLE:
		return L"CL_INVALID_PROGRAM_EXECUTABLE";
	case CL_INVALID_KERNEL_NAME:
		return L"CL_INVALID_KERNEL_NAME";
	case CL_INVALID_KERNEL_DEFINITION:
		return L"CL_INVALID_KERNEL_DEFINITION";
	case CL_INVALID_KERNEL:
		return L"CL_INVALID_KERNEL";
	case CL_INVALID_ARG_INDEX:
		return L"CL_INVALID_ARG_INDEX";
	case CL_INVALID_ARG_VALUE:
		return L"CL_INVALID_ARG_VALUE";
	case CL_INVALID_ARG_SIZE:
		return L"CL_INVALID_ARG_SIZE";
	case CL_INVALID_KERNEL_ARGS:
		return L"CL_INVALID_KERNEL_ARGS";
	case CL_INVALID_WORK_DIMENSION:
		return L"CL_INVALID_WORK_DIMENSION";
	case CL_INVALID_WORK_GROUP_SIZE:
		return L"CL_INVALID_WORK_GROUP_SIZE";
	case CL_INVALID_WORK_ITEM_SIZE:
		return L"CL_INVALID_WORK_ITEM_SIZE";
	case CL_INVALID_GLOBAL_OFFSET:
		return L"CL_INVALID_GLOBAL_OFFSET";
	case CL_INVALID_EVENT_WAIT_LIST:
		return L"CL_INVALID_EVENT_WAIT_LIST";
	case CL_INVALID_EVENT:
		return L"CL_INVALID_EVENT";
	case CL_INVALID_OPERATION:
		return L"CL_INVALID_OPERATION";
	case CL_INVALID_GL_OBJECT:
		return L"CL_INVALID_GL_OBJECT";
	case CL_INVALID_BUFFER_SIZE:
		return L"CL_INVALID_BUFFER_SIZE";
	case CL_INVALID_MIP_LEVEL:
		return L"CL_INVALID_MIP_LEVEL";
	case CL_INVALID_GLOBAL_WORK_SIZE:
		return L"CL_INVALID_GLOBAL_WORK_SIZE";
	case CL_INVALID_PROPERTY:
		return L"CL_INVALID_PROPERTY";
	case CL_INVALID_IMAGE_DESCRIPTOR:
		return L"CL_INVALID_IMAGE_DESCRIPTOR";
	case CL_INVALID_COMPILER_OPTIONS:
		return L"CL_INVALID_COMPILER_OPTIONS";
	case CL_INVALID_LINKER_OPTIONS:
		return L"CL_INVALID_LINKER_OPTIONS";
	case CL_INVALID_DEVICE_PARTITION_COUNT:
		return L"CL_INVALID_DEVICE_PARTITION_COUNT";
	default:
		return L"CL_INVALID_VALUE";
	}
}

inline std::wstring EnginePath(bool stringSafe = false)
{
    std::wstring enginePath = std::filesystem::current_path().wstring();
    if (!stringSafe) return enginePath;
    std::ranges::replace(enginePath, '\\', '/');
    return enginePath;
}

/**
 * @brief Packs two floats into one 32-bit int containing the two floats as half floats complying with IEEE 754
 * @note Complies with OpenGL standards: \n
 *			The first vector component specifies the 16 least-significant bits of the result; the second component specifies the 16 most-significant bits.
 */
inline std::uint32_t PackHalf2x16(const float a, const float b)
{
	return (uint32_t)( (uint16_t)numeric::float16_t(b) << 16 | (uint16_t)numeric::float16_t(a) );
}