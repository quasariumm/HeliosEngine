#pragma once

#include "debugging/logger.hpp"

/*
 * Typedefs
 */

namespace Helios
{
using color8_t    = glm::u8vec4;
using color32_f_t = glm::vec4;
}


// Changing name of entity to scene object for clearer naming
namespace Helios
{
// ReSharper disable once CppInconsistentNaming
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

static constexpr float PI          = std::numbers::pi_v<float>;
static constexpr float INVPI       = std::numbers::inv_pi_v<float>;
static constexpr float INV2PI      = 1.f / (2.f * PI);
static constexpr float TWOPI       = 2.f * PI;
static constexpr float SQRT_PI_INV = std::numbers::inv_sqrtpi_v<float>;
static constexpr float LARGE_FLOAT = 1e34f;


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

inline std::string LoadFile( const std::filesystem::path& path )
{
	std::ifstream file;
	file.open(path);

	if (!file.is_open())
	{
		Helios::Log::Error(std::format("Failed to open file: {}", path.string()));
		return "";
	}

	return {
			std::istreambuf_iterator(file),
			std::istreambuf_iterator<char>()
	};
}

inline std::string EnginePath( bool stringSafe = false )
{
	std::string enginePath = std::filesystem::current_path().generic_string();
	if (!stringSafe)
		return enginePath;
	std::ranges::replace(enginePath, '\\', '/');
	return enginePath;
}


/**
 * @brief Packs two floats into one 32-bit int containing the two floats as half floats complying with IEEE 754
 * @note Complies with OpenGL standards: \n
 *			The first vector component specifies the 16 least-significant bits of the result; the second component specifies the 16 most-significant bits.
 */
inline std::uint32_t PackHalf2x16( const float a, const float b )
{
	return (uint32_t)((uint16_t)numeric::float16_t(b) << 16 | (uint16_t)numeric::float16_t(a));
}
