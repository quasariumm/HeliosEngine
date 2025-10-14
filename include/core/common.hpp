#pragma once

#include "debugging/logger.hpp"

/*
 * Console logging tags
 */

static const std::string ANSI_OK = "[\033[32m OK \033[0m] ";
static const std::string ANSI_DONE = "[\033[32mDONE\033[0m] ";
static const std::string ANSI_ERR = "[\033[31mERRO\033[0m] ";
static const std::string ANSI_WARN = "[\033[33mWARN\033[0m] ";
static const std::string ANSI_INFO = "[\033[36mINFO\033[0m] ";

/*
 * Typedefs
 */

namespace Engine
{
typedef glm::u8vec4		Color8;
typedef glm::vec4		Color32F;
}

// Changing name of entity to scene object for clearer naming
namespace Engine
{
using SceneObject = entt::entity;
}

/*
 * Static lists
 */

// TODO: Replace these with handlers to prevent issues with project dll loading

static std::unordered_set<std::string> loadedShaders;

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

inline std::string LoadFile(const std::filesystem::path& path)
{
	std::ifstream file;
	file.open(path);

	if (!file.is_open())
	{
		Engine::Log::Error(std::format("Failed to open file: {}", path.string()));
		return "";
	}

	return {
		std::istreambuf_iterator<char>(file),
		std::istreambuf_iterator<char>()
	};
}

// TODO(Patrick): Should prob be somewhere like window cuz its platform dependent
// inline std::string GLDebugTypeToString( const GLenum type )
// {
// 	switch (type)
// 	{
// 	case 0x824c:
// 		return "GL_DEBUG_TYPE_ERROR";
// 	case 0x824d:
// 		return "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR";
// 	case 0x824e:
// 		return "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR";
// 	case 0x824f:
// 		return "GL_DEBUG_TYPE_PORTABILITY";
// 	case 0x8250:
// 		return "GL_DEBUG_TYPE_PERFORMANCE";
// 	case 0x8251:
// 		return "GL_DEBUG_TYPE_OTHER";
// 	default:
// 		return std::to_string(type);
// 	}
// }
//
//
// inline std::string GLErrorToString( const uint32_t error )
// {
// 	switch (error)
// 	{
// 	// Errors
// 	case 0:
// 		return "GL_NO_ERROR";
// 	case 0x0500:
// 		return "GL_INVALID_ENUM";
// 	case 0x0501:
// 		return "GL_INVALID_VALUE";
// 	case 0x0502:
// 		return "GL_INVALID_OPERATION";
// 	case 0x0503:
// 		return "GL_STACK_OVERFLOW";
// 	case 0x0504:
// 		return "GL_STACK_UNDERFLOW";
// 	case 0x0505:
// 		return "GL_OUT_OF_MEMORY";
// 	case 0x0506:
// 		return "GL_INVALID_FRAMEBUFFER_OPERATION";
// 	case 0x0507:
// 		return "GL_CONTEXT_LOST";
// 	// Framebuffer status
// 	case 0x8cd5:
// 		return "GL_FRAMEBUFFER_COMPLETE";
// 	case 0x8219:
// 		return "GL_FRAMEBUFFER_UNDEFINED";
// 	case 0x8dc6:
// 		return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
// 	case 0x8cd7:
// 		return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
// 	case 0x8cd8:
// 		return "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
// 	case 0x8cd9:
// 		return "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
// 	default:
// 		return std::to_string(error);
// 	}
// }
//
//
// inline std::string GLSeverityToString( const GLenum sev )
// {
// 	switch (sev)
// 	{
// 	case 0x826b:
// 		return "GL_DEBUG_SEVERITY_NOTIFICATION";
// 	case 0x9146:
// 		return "GL_DEBUG_SEVERITY_HIGH";
// 	case 0x9147:
// 		return "GL_DEBUG_SEVERITY_MEDIUM";
// 	case 0x9148:
// 		return "GL_DEBUG_SEVERITY_LOW";
// 	default:
// 		return std::to_string(sev);
// 	}
// }

inline std::string CLErrorString( const cl_int result )
{
	switch (result)
	{
	case CL_DEVICE_NOT_FOUND:
		return "CL_DEVICE_NOT_FOUND";
	case CL_DEVICE_NOT_AVAILABLE:
		return "CL_DEVICE_NOT_AVAILABLE";
	case CL_COMPILER_NOT_AVAILABLE:
		return "CL_COMPILER_NOT_AVAILABLE";
	case CL_MEM_OBJECT_ALLOCATION_FAILURE:
		return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
	case CL_OUT_OF_RESOURCES:
		return "CL_OUT_OF_RESOURCES";
	case CL_OUT_OF_HOST_MEMORY:
		return "CL_OUT_OF_HOST_MEMORY";
	case CL_PROFILING_INFO_NOT_AVAILABLE:
		return "CL_PROFILING_INFO_NOT_AVAILABLE";
	case CL_MEM_COPY_OVERLAP:
		return "CL_MEM_COPY_OVERLAP";
	case CL_IMAGE_FORMAT_MISMATCH:
		return "CL_IMAGE_FORMAT_MISMATCH";
	case CL_IMAGE_FORMAT_NOT_SUPPORTED:
		return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
	case CL_BUILD_PROGRAM_FAILURE:
		return "CL_BUILD_PROGRAM_FAILURE";
	case CL_MAP_FAILURE:
		return "CL_MAP_FAILURE";
	case CL_MISALIGNED_SUB_BUFFER_OFFSET:
		return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
	case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:
		return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
	case CL_INVALID_VALUE:
		return "CL_INVALID_VALUE";
	case CL_INVALID_DEVICE_TYPE:
		return "CL_INVALID_DEVICE_TYPE";
	case CL_INVALID_PLATFORM:
		return "CL_INVALID_PLATFORM";
	case CL_INVALID_DEVICE:
		return "CL_INVALID_DEVICE";
	case CL_INVALID_CONTEXT:
		return "CL_INVALID_CONTEXT";
	case CL_INVALID_QUEUE_PROPERTIES:
		return "CL_INVALID_QUEUE_PROPERTIES";
	case CL_INVALID_COMMAND_QUEUE:
		return "CL_INVALID_COMMAND_QUEUE";
	case CL_INVALID_HOST_PTR:
		return "CL_INVALID_HOST_PTR";
	case CL_INVALID_MEM_OBJECT:
		return "CL_INVALID_MEM_OBJECT";
	case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
		return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
	case CL_INVALID_IMAGE_SIZE:
		return "CL_INVALID_IMAGE_SIZE";
	case CL_INVALID_SAMPLER:
		return "CL_INVALID_SAMPLER";
	case CL_INVALID_BINARY:
		return "CL_INVALID_BINARY";
	case CL_INVALID_BUILD_OPTIONS:
		return "CL_INVALID_BUILD_OPTIONS";
	case CL_INVALID_PROGRAM:
		return "CL_INVALID_PROGRAM";
	case CL_INVALID_PROGRAM_EXECUTABLE:
		return "CL_INVALID_PROGRAM_EXECUTABLE";
	case CL_INVALID_KERNEL_NAME:
		return "CL_INVALID_KERNEL_NAME";
	case CL_INVALID_KERNEL_DEFINITION:
		return "CL_INVALID_KERNEL_DEFINITION";
	case CL_INVALID_KERNEL:
		return "CL_INVALID_KERNEL";
	case CL_INVALID_ARG_INDEX:
		return "CL_INVALID_ARG_INDEX";
	case CL_INVALID_ARG_VALUE:
		return "CL_INVALID_ARG_VALUE";
	case CL_INVALID_ARG_SIZE:
		return "CL_INVALID_ARG_SIZE";
	case CL_INVALID_KERNEL_ARGS:
		return "CL_INVALID_KERNEL_ARGS";
	case CL_INVALID_WORK_DIMENSION:
		return "CL_INVALID_WORK_DIMENSION";
	case CL_INVALID_WORK_GROUP_SIZE:
		return "CL_INVALID_WORK_GROUP_SIZE";
	case CL_INVALID_WORK_ITEM_SIZE:
		return "CL_INVALID_WORK_ITEM_SIZE";
	case CL_INVALID_GLOBAL_OFFSET:
		return "CL_INVALID_GLOBAL_OFFSET";
	case CL_INVALID_EVENT_WAIT_LIST:
		return "CL_INVALID_EVENT_WAIT_LIST";
	case CL_INVALID_EVENT:
		return "CL_INVALID_EVENT";
	case CL_INVALID_OPERATION:
		return "CL_INVALID_OPERATION";
	case CL_INVALID_GL_OBJECT:
		return "CL_INVALID_GL_OBJECT";
	case CL_INVALID_BUFFER_SIZE:
		return "CL_INVALID_BUFFER_SIZE";
	case CL_INVALID_MIP_LEVEL:
		return "CL_INVALID_MIP_LEVEL";
	case CL_INVALID_GLOBAL_WORK_SIZE:
		return "CL_INVALID_GLOBAL_WORK_SIZE";
	case CL_INVALID_PROPERTY:
		return "CL_INVALID_PROPERTY";
	case CL_INVALID_IMAGE_DESCRIPTOR:
		return "CL_INVALID_IMAGE_DESCRIPTOR";
	case CL_INVALID_COMPILER_OPTIONS:
		return "CL_INVALID_COMPILER_OPTIONS";
	case CL_INVALID_LINKER_OPTIONS:
		return "CL_INVALID_LINKER_OPTIONS";
	case CL_INVALID_DEVICE_PARTITION_COUNT:
		return "CL_INVALID_DEVICE_PARTITION_COUNT";
	default:
		return "CL_INVALID_VALUE";
	}
}

inline std::string EnginePath(bool stringSafe = false)
{
    std::string enginePath = std::filesystem::current_path().generic_string();
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