#pragma once

#include "_VectorSwizzles.h"

// NOTE: Temporary
#define DEFINE_SWIZZLES

#pragma warning ( push )
#pragma warning ( disable: 4201 /* nameless struct / union */ )

namespace Engine
{

template<typename T>
concept numeric = std::is_arithmetic_v<T>;

#pragma region Vector structs

template<typename T> requires numeric<T>
struct vec3t;
template<typename T> requires numeric<T>
struct vec4t;

/*
 * Vector structs
 */

// Alignment is purely from a performance standpoint.
// I am aware that there might be gaps when using types smaller than 4 bytes
// - Patrick

template<typename T> requires numeric<T>
struct alignas(8) vec2t
{
	vec2t() = default;
	vec2t(const T x, const T y) : x(x), y(y) {}
	explicit vec2t(const T s) : x(s), y(s) {}

	template<typename U> requires numeric<U>
	explicit vec2t(const vec2t<U>& v) : x(v.x), y(v.y) {}

	template<typename U> requires numeric<U>
	explicit vec2t(const vec3t<U>& v) : x(v.x), y(v.y) {}

	template<typename U> requires numeric<U>
	explicit vec2t(const vec4t<U>& v) : x(v.x), y(v.y) {}

	union
	{
		struct { T x, y; };
        struct { T r, g; };
		T cell[2];
	};
	T& operator[](const unsigned i) { return cell[i]; }
	const T& operator[](const unsigned i) const { return cell[i]; }

#ifdef DEFINE_SWIZZLES
    SWIZZLES_XY(T)
#endif
};

typedef vec2t<float>	vec2;
typedef vec2t<float>	vec2f;
typedef vec2t<int>		vec2i;
typedef vec2t<unsigned> vec2u;
typedef vec2t<double>	vec2d;

template<typename T> requires numeric<T>
struct vec3t
{
	vec3t() = default;
	vec3t(const T x, const T y, const T z) : x(x), y(y), z(z) {}
	explicit vec3t(const T s) : x(s), y(s), z(s) {}

	template<typename U> requires numeric<U>
	explicit vec3t(const vec2t<U>& v) : x(v.x), y(v.y), z(0) {}

	template<typename U> requires numeric<U>
	explicit vec3t(const vec3t<U>& v) : x(v.x), y(v.y), z(v.z) {}

	template<typename U> requires numeric<U>
	explicit vec3t(const vec4t<U>& v) : x(v.x), y(v.y), z(v.z) {}

	union
	{
		struct { T x, y, z; };
		struct { T r, g, b; };
		struct { T u, v, w; };
		T cell[3];
	};
	T& operator[](const unsigned i) { return cell[i]; }
	const T& operator[](const unsigned i) const { return cell[i]; }

#ifdef DEFINE_SWIZZLES
    SWIZZLES_XY(T)
    SWIZZLES_XYZ(T)
#endif
};

typedef vec3t<float>	vec3;
typedef vec3t<float>	vec3f;
typedef vec3t<int>		vec3i;
typedef vec3t<unsigned> vec3u;
typedef vec3t<double>	vec3d;

template<typename T> requires numeric<T>
struct alignas(16) vec4t
{
	vec4t() = default;
	vec4t(const T x, const T y, const T z, const T w) : x(x), y(y), z(z), w(w) {}
	explicit vec4t(const T s) : x(s), y(s), z(s), w(s) {}

	template<typename U> requires numeric<U>
	explicit vec4t(const vec2t<U>& v) : x(v.x), y(v.y), z(v), w(0) {}

	template<typename U> requires numeric<U>
	explicit vec4t(const vec3t<U>& v) : x(v.x), y(v.y), z(v.z), w(0) {}

	template<typename U> requires numeric<U>
	explicit vec4t(const vec4t<U>& v) : x(v.x), y(v.y), z(v.z), w(v.w) {}

	union
	{
		struct { T x, y, z, w; };
		struct { T r, g, b, a; };
		T cell[4];
	};
	T& operator[](const unsigned i) { return cell[i]; }
	const T& operator[](const unsigned i) const { return cell[i]; }

#ifdef DEFINE_SWIZZLES
    SWIZZLES_XY(T)
    SWIZZLES_XYZ(T)
    SWIZZLES_XYZW(T)
#endif
};

// SIMD version for float type
#ifdef ENGINE_USE_SSE
template<>
struct alignas(16) vec4t<float>
{
	vec4t() = default;
	vec4t(const float x, const float y, const float z, const float w) : v4(_mm_setr_ps(x, y, z, w)) {}
	explicit vec4t(const float s) : v4(_mm_set1_ps(s)) {}

	template<typename U> requires numeric<U>
	explicit vec4t(const vec2t<U>& v) : v4(_mm_setr_ps(v.x, v.y, 0.f, 0.f)) {}

	template<typename U> requires numeric<U>
	explicit vec4t(const vec3t<U>& v) : v4(_mm_setr_ps(v.x, v.y, v.z, 0.f)) {}

	template<typename U> requires numeric<U>
	explicit vec4t(const vec4t<U>& v) : v4(_mm_setr_ps(v.x, v.y, v.z, v.w)) {}

	explicit vec4t(const __m128 v) : v4(v) {};

	union
	{
		struct { float x, y, z, w; };
		struct { float r, g, b, a; };
		float cell[4];
		__m128 v4;
	};
	float& operator[](const unsigned i) { return cell[i]; }
	const float& operator[](const unsigned i) const { return cell[i]; }

#ifdef DEFINE_SWIZZLES
    SWIZZLES_XY(float)
    SWIZZLES_XYZ(float)
    SWIZZLES_XYZW(float)
#endif
};
#endif

typedef vec4t<float>	vec4;
typedef vec4t<float>	vec4f;
typedef vec4t<int>		vec4i;
typedef vec4t<unsigned> vec4u;
typedef vec4t<double>	vec4d;

#pragma endregion

#pragma region Vector operators

// Needed for comparison operators
template <typename T>
float Length(vec2t<T> v) { return std::sqrt(v.x * v.x + v.y * v.y); }
template <typename T>
float Length(vec3t<T> v) { return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z); }
template <typename T>
float Length(vec4t<T> v) { return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w); }


/*
 * Operators
 */

// Stands for OPERator TeMPLate
#define FUNC1_TMPL template<typename T>
#define FUNC2_TMPL template<typename T, typename U>

/*
 * Binary operators
 */

FUNC2_TMPL bool operator==(const vec2t<T>& a, const vec2t<U>& b) noexcept { return a.x == b.x && a.y == b.y; };
FUNC2_TMPL bool operator==(const vec3t<T>& a, const vec3t<U>& b) noexcept { return a.x == b.x && a.y == b.y && a.z == b.z; };
FUNC2_TMPL bool operator==(const vec4t<T>& a, const vec4t<U>& b) noexcept { return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w; };
FUNC2_TMPL bool operator==(const vec2t<T>& a, const U b) noexcept { return a.x == b && a.y == b; };
FUNC2_TMPL bool operator==(const vec3t<T>& a, const U b) noexcept { return a.x == b && a.y == b && a.z == b; };
FUNC2_TMPL bool operator==(const vec4t<T>& a, const U b) noexcept { return a.x == b && a.y == b && a.z == b && a.w == b; };
FUNC2_TMPL bool operator==(const U a, const vec2t<T>& b) noexcept { return a == b.x && a == b.y; };
FUNC2_TMPL bool operator==(const U a, const vec3t<T>& b) noexcept { return a == b.x && a == b.y && a == b.z; };
FUNC2_TMPL bool operator==(const U a, const vec4t<T>& b) noexcept { return a == b.x && a == b.y && a == b.z && a == b.w; };

FUNC2_TMPL bool operator>(const vec2t<T>& a, const vec2t<U>& b) noexcept { return a.x > b.x && a.y > b.y; };
FUNC2_TMPL bool operator>(const vec3t<T>& a, const vec3t<U>& b) noexcept { return a.x > b.x && a.y > b.y && a.z > b.z; };
FUNC2_TMPL bool operator>(const vec4t<T>& a, const vec4t<U>& b) noexcept { return a.x > b.x && a.y > b.y && a.z > b.z && a.w > b.w; };
FUNC2_TMPL bool operator>(const vec2t<T>& a, const U b) noexcept { return Length(a) > b; };
FUNC2_TMPL bool operator>(const vec3t<T>& a, const U b) noexcept { return Length(a) > b; };
FUNC2_TMPL bool operator>(const vec4t<T>& a, const U b) noexcept { return Length(a) > b; };
FUNC2_TMPL bool operator>(const U a, const vec2t<T>& b) noexcept { return a > Length(b); };
FUNC2_TMPL bool operator>(const U a, const vec3t<T>& b) noexcept { return a > Length(b); };
FUNC2_TMPL bool operator>(const U a, const vec4t<T>& b) noexcept { return a > Length(b); };

FUNC2_TMPL bool operator>=(const vec2t<T>& a, const vec2t<U>& b) noexcept { return operator>(a, b) && operator==(a, b); };
FUNC2_TMPL bool operator>=(const vec3t<T>& a, const vec3t<U>& b) noexcept { return operator>(a, b) && operator==(a, b); };
FUNC2_TMPL bool operator>=(const vec4t<T>& a, const vec4t<U>& b) noexcept { return operator>(a, b) && operator==(a, b); };
FUNC2_TMPL bool operator>=(const vec2t<T>& a, const U b) noexcept { return operator>(a, b) && operator==(a, b); };
FUNC2_TMPL bool operator>=(const vec3t<T>& a, const U b) noexcept { return operator>(a, b) && operator==(a, b); };
FUNC2_TMPL bool operator>=(const vec4t<T>& a, const U b) noexcept { return operator>(a, b) && operator==(a, b); };
FUNC2_TMPL bool operator>=(const U a, const vec2t<T>& b) noexcept { return operator>(a, b) && operator==(a, b); };
FUNC2_TMPL bool operator>=(const U a, const vec3t<T>& b) noexcept { return operator>(a, b) && operator==(a, b); };
FUNC2_TMPL bool operator>=(const U a, const vec4t<T>& b) noexcept { return operator>(a, b) && operator==(a, b); };

FUNC2_TMPL bool operator<(const vec2t<T>& a, const vec2t<U>& b) noexcept { return a.x < b.x && a.y < b.y; };
FUNC2_TMPL bool operator<(const vec3t<T>& a, const vec3t<U>& b) noexcept { return a.x < b.x && a.y < b.y && a.z < b.z; };
FUNC2_TMPL bool operator<(const vec4t<T>& a, const vec4t<U>& b) noexcept { return a.x < b.x && a.y < b.y && a.z < b.z && a.w < b.w; };
FUNC2_TMPL bool operator<(const vec2t<T>& a, const U b) noexcept { return Length(a) < b; };
FUNC2_TMPL bool operator<(const vec3t<T>& a, const U b) noexcept { return Length(a) < b; };
FUNC2_TMPL bool operator<(const vec4t<T>& a, const U b) noexcept { return Length(a) < b; };
FUNC2_TMPL bool operator<(const U a, const vec2t<T>& b) noexcept { return a < Length(b); };
FUNC2_TMPL bool operator<(const U a, const vec3t<T>& b) noexcept { return a < Length(b); };
FUNC2_TMPL bool operator<(const U a, const vec4t<T>& b) noexcept { return a < Length(b); };

FUNC2_TMPL bool operator<=(const vec2t<T>& a, const vec2t<U>& b) noexcept { return operator<(a, b) && operator==(a, b); };
FUNC2_TMPL bool operator<=(const vec3t<T>& a, const vec3t<U>& b) noexcept { return operator<(a, b) && operator==(a, b); };
FUNC2_TMPL bool operator<=(const vec4t<T>& a, const vec4t<U>& b) noexcept { return operator<(a, b) && operator==(a, b); };
FUNC2_TMPL bool operator<=(const vec2t<T>& a, const U b) noexcept { return operator<(a, b) && operator==(a, b); };
FUNC2_TMPL bool operator<=(const vec3t<T>& a, const U b) noexcept { return operator<(a, b) && operator==(a, b); };
FUNC2_TMPL bool operator<=(const vec4t<T>& a, const U b) noexcept { return operator<(a, b) && operator==(a, b); };
FUNC2_TMPL bool operator<=(const U a, const vec2t<T>& b) noexcept { return operator<(a, b) && operator==(a, b); };
FUNC2_TMPL bool operator<=(const U a, const vec3t<T>& b) noexcept { return operator<(a, b) && operator==(a, b); };
FUNC2_TMPL bool operator<=(const U a, const vec4t<T>& b) noexcept { return operator<(a, b) && operator==(a, b); };

FUNC2_TMPL std::strong_ordering operator<=>(const vec2t<T>& a, const vec2t<U>& b) noexcept { return a < b ? std::strong_ordering::less : a > b ? std::strong_ordering::greater : std::strong_ordering::equal; };
FUNC2_TMPL std::strong_ordering operator<=>(const vec3t<T>& a, const vec3t<U>& b) noexcept { return a < b ? std::strong_ordering::less : a > b ? std::strong_ordering::greater : std::strong_ordering::equal; };
FUNC2_TMPL std::strong_ordering operator<=>(const vec4t<T>& a, const vec4t<U>& b) noexcept { return a < b ? std::strong_ordering::less : a > b ? std::strong_ordering::greater : std::strong_ordering::equal; };

/*
 * Arithmetic operators
 */

FUNC2_TMPL vec2t<T> operator+(const vec2t<T>& a, const vec2t<U>& b) noexcept { return { a.x + b.x, a.y + b.y }; }
FUNC2_TMPL vec3t<T> operator+(const vec3t<T>& a, const vec3t<U>& b) noexcept { return { a.x + b.x, a.y + b.y, a.z + b.z }; }
FUNC2_TMPL vec4t<T> operator+(const vec4t<T>& a, const vec4t<U>& b) noexcept { return { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w }; }
FUNC2_TMPL vec2t<T> operator+(const vec2t<T>& a, const U b) noexcept { return { a.x + b, a.y + b }; }
FUNC2_TMPL vec3t<T> operator+(const vec3t<T>& a, const U b) noexcept { return { a.x + b, a.y + b, a.z + b }; }
FUNC2_TMPL vec4t<T> operator+(const vec4t<T>& a, const U b) noexcept { return { a.x + b, a.y + b, a.z + b, a.w + b }; }

FUNC2_TMPL void operator+=(vec2t<T>& a, const vec2t<U>& b) noexcept { a.x += b.x; a.y += b.y; }
FUNC2_TMPL void operator+=(vec3t<T>& a, const vec3t<U>& b) noexcept { a.x += b.x; a.y += b.y; a.z += b.z; }
FUNC2_TMPL void operator+=(vec4t<T>& a, const vec4t<U>& b) noexcept { a.x += b.x, a.y += b.y; a.z += b.z; a.w += b.w; }
FUNC2_TMPL void operator+=(vec2t<T>& a, const U b) noexcept { a.x += b; a.y += b; }
FUNC2_TMPL void operator+=(vec3t<T>& a, const U b) noexcept { a.x += b; a.y += b; a.z += b; }
FUNC2_TMPL void operator+=(vec4t<T>& a, const U b) noexcept { a.x += b, a.y += b; a.z += b; a.w += b; }

FUNC2_TMPL vec2t<T> operator-(const vec2t<T>& a, const vec2t<U>& b) noexcept { return { a.x - b.x, a.y - b.y }; }
FUNC2_TMPL vec3t<T> operator-(const vec3t<T>& a, const vec3t<U>& b) noexcept { return { a.x - b.x, a.y - b.y, a.z - b.z }; }
FUNC2_TMPL vec4t<T> operator-(const vec4t<T>& a, const vec4t<U>& b) noexcept { return { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w }; }
FUNC2_TMPL vec2t<T> operator-(const vec2t<T>& a, const U b) noexcept { return { a.x - b, a.y - b }; }
FUNC2_TMPL vec3t<T> operator-(const vec3t<T>& a, const U b) noexcept { return { a.x - b, a.y - b, a.z - b }; }
FUNC2_TMPL vec4t<T> operator-(const vec4t<T>& a, const U b) noexcept { return { a.x - b, a.y - b, a.z - b, a.w - b }; }
FUNC1_TMPL vec2t<T> operator-(const vec2t<T>& a) noexcept { return { -a.x, -a.y }; }
FUNC1_TMPL vec3t<T> operator-(const vec3t<T>& a) noexcept { return { -a.x, -a.y, -a.z }; }
FUNC1_TMPL vec4t<T> operator-(const vec4t<T>& a) noexcept { return { -a.x, -a.y, -a.z, -a.w }; }

FUNC2_TMPL void operator-=(vec2t<T>& a, const vec2t<U>& b) noexcept { a.x -= b.x; a.y -= b.y; }
FUNC2_TMPL void operator-=(vec3t<T>& a, const vec3t<U>& b) noexcept { a.x -= b.x; a.y -= b.y; a.z -= b.z; }
FUNC2_TMPL void operator-=(vec4t<T>& a, const vec4t<U>& b) noexcept { a.x -= b.x, a.y -= b.y; a.z -= b.z; a.w -= b.w; }
FUNC2_TMPL void operator-=(vec2t<T>& a, const U b) noexcept { a.x -= b; a.y -= b; }
FUNC2_TMPL void operator-=(vec3t<T>& a, const U b) noexcept { a.x -= b; a.y -= b; a.z -= b; }
FUNC2_TMPL void operator-=(vec4t<T>& a, const U b) noexcept { a.x -= b, a.y -= b; a.z -= b; a.w -= b; }

FUNC2_TMPL vec2t<T> operator*(const vec2t<T>& a, const vec2t<U>& b) noexcept { return { a.x * b.x, a.y * b.y }; }
FUNC2_TMPL vec3t<T> operator*(const vec3t<T>& a, const vec3t<U>& b) noexcept { return { a.x * b.x, a.y * b.y, a.z * b.z }; }
FUNC2_TMPL vec4t<T> operator*(const vec4t<T>& a, const vec4t<U>& b) noexcept { return { a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w }; }
FUNC2_TMPL vec2t<T> operator*(const vec2t<T>& a, const U b) noexcept { return { a.x * b, a.y * b }; }
FUNC2_TMPL vec3t<T> operator*(const vec3t<T>& a, const U b) noexcept { return { a.x * b, a.y * b, a.z * b }; }
FUNC2_TMPL vec4t<T> operator*(const vec4t<T>& a, const U b) noexcept { return { a.x * b, a.y * b, a.z * b, a.w * b }; }
FUNC2_TMPL vec2t<T> operator*(const U a, const vec2t<T>& b) noexcept { return { b.x * a, b.y * a }; }
FUNC2_TMPL vec3t<T> operator*(const U a, const vec3t<T>& b) noexcept { return { b.x * a, b.y * a, a * b.z }; }
FUNC2_TMPL vec4t<T> operator*(const U a, const vec4t<T>& b) noexcept { return { b.x * a, b.y * a, a * b.z, a * b.w }; }

FUNC2_TMPL void operator*=(vec2t<T>& a, const vec2t<U>& b) noexcept { a.x *= b.x; a.y *= b.y; }
FUNC2_TMPL void operator*=(vec3t<T>& a, const vec3t<U>& b) noexcept { a.x *= b.x; a.y *= b.y; a.z *= b.z; }
FUNC2_TMPL void operator*=(vec4t<T>& a, const vec4t<U>& b) noexcept { a.x *= b.x; a.y *= b.y; a.z *= b.z; a.w *= b.w; }
FUNC2_TMPL void operator*=(vec2t<T>& a, const U b) noexcept { a.x *= b; a.y *= b; }
FUNC2_TMPL void operator*=(vec3t<T>& a, const U b) noexcept { a.x *= b; a.y *= b; a.z *= b; }
FUNC2_TMPL void operator*=(vec4t<T>& a, const U b) noexcept { a.x *= b; a.y *= b; a.z *= b; a.w *= b; }

FUNC2_TMPL vec2t<T> operator/(const vec2t<T>& a, const vec2t<U>& b) noexcept { return { a.x / b.x, a.y / b.y }; }
FUNC2_TMPL vec3t<T> operator/(const vec3t<T>& a, const vec3t<U>& b) noexcept { return { a.x / b.x, a.y / b.y, a.z / b.z }; }
FUNC2_TMPL vec4t<T> operator/(const vec4t<T>& a, const vec4t<U>& b) noexcept { return { a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w }; }
FUNC2_TMPL vec2t<T> operator/(const vec2t<T>& a, const U b) noexcept { return { a.x / b, a.y / b }; }
FUNC2_TMPL vec3t<T> operator/(const vec3t<T>& a, const U b) noexcept { return { a.x / b, a.y / b, a.z / b }; }
FUNC2_TMPL vec4t<T> operator/(const vec4t<T>& a, const U b) noexcept { return { a.x / b, a.y / b, a.z / b, a.w / b }; }
FUNC2_TMPL vec2t<T> operator/(const U a, const vec2t<T>& b) noexcept { return { b.x / a, b.y / a }; }
FUNC2_TMPL vec3t<T> operator/(const U a, const vec3t<T>& b) noexcept { return { b.x / a, b.y / a, a / b.z }; }
FUNC2_TMPL vec4t<T> operator/(const U a, const vec4t<T>& b) noexcept { return { b.x / a, b.y / a, a / b.z, a / b.w }; }

FUNC2_TMPL void operator/=(vec2t<T>& a, const vec2t<U>& b) noexcept { a.x /= b.x; a.y /= b.y; }
FUNC2_TMPL void operator/=(vec3t<T>& a, const vec3t<U>& b) noexcept { a.x /= b.x; a.y /= b.y; a.z /= b.z; }
FUNC2_TMPL void operator/=(vec4t<T>& a, const vec4t<U>& b) noexcept { a.x /= b.x; a.y /= b.y; a.z /= b.z; a.w /= b.w; }
FUNC2_TMPL void operator/=(vec2t<T>& a, const U b) noexcept { a.x /= b; a.y /= b; }
FUNC2_TMPL void operator/=(vec3t<T>& a, const U b) noexcept { a.x /= b; a.y /= b; a.z /= b; }
FUNC2_TMPL void operator/=(vec4t<T>& a, const U b) noexcept { a.x /= b; a.y /= b; a.z /= b; a.w /= b; }

#ifdef ENGINE_USE_SSE

/*
 * SIMD Arithmetic operators
 */

inline vec4f operator+(const vec4f& a, const vec4f& b) noexcept { return vec4f(_mm_add_ps(a.v4, b.v4)); }
FUNC1_TMPL vec4f operator+(const vec4f& a, const T b) noexcept { return vec4f(_mm_add_ps(a.v4, _mm_set1_ps(b))); }

inline void operator+=(vec4f& a, const vec4f& b) noexcept { a.v4 = _mm_add_ps(a.v4, b.v4); }
FUNC1_TMPL void operator+=(vec4f& a, const T b) noexcept { a.v4 = _mm_add_ps(a.v4, _mm_set1_ps(b)); }

inline vec4f operator-(const vec4f& a, const vec4f& b) noexcept { return vec4f(_mm_sub_ps(a.v4, b.v4)); }
FUNC1_TMPL vec4f operator-(const vec4f& a, const T b) noexcept { return vec4f(_mm_sub_ps(a.v4, _mm_set1_ps(b))); }

inline void operator-=(vec4f& a, const vec4f& b) noexcept { a.v4 = _mm_sub_ps(a.v4, b.v4); }
FUNC1_TMPL void operator-=(vec4f& a, const T b) noexcept { a.v4 = _mm_sub_ps(a.v4, _mm_set1_ps(b)); }

inline vec4f operator*(const vec4f& a, const vec4f& b) noexcept { return vec4f(_mm_mul_ps(a.v4, b.v4)); }
FUNC1_TMPL vec4f operator*(const vec4f& a, const T b) noexcept { return vec4f(_mm_mul_ps(a.v4, _mm_set1_ps(b))); }
FUNC1_TMPL vec4f operator*(const T a, const vec4f& b) noexcept { return vec4f(_mm_mul_ps(b.v4, _mm_set1_ps(a))); }

inline void operator*=(vec4f& a, const vec4f& b) noexcept { a.v4 = _mm_mul_ps(a.v4, b.v4); }
FUNC1_TMPL void operator*=(vec4f& a, const T b) noexcept { a.v4 = _mm_mul_ps(a.v4, _mm_set1_ps(b)); }

inline vec4f operator/(const vec4f& a, const vec4f& b) noexcept { return vec4f(_mm_div_ps(a.v4, b.v4)); }
FUNC1_TMPL vec4f operator/(const vec4f& a, const T b) noexcept { return vec4f(_mm_div_ps(a.v4, _mm_set1_ps(b))); }
FUNC1_TMPL vec4f operator/(const T a, const vec4f& b) noexcept { return vec4f(_mm_div_ps(b.v4, _mm_set1_ps(a))); }

inline void operator/=(vec4f& a, const vec4f& b) noexcept { a.v4 = _mm_div_ps(a.v4, b.v4); }
FUNC1_TMPL void operator/=(vec4f& a, const T b) noexcept { a.v4 = _mm_div_ps(a.v4, _mm_set1_ps(b)); }

#endif

/*
 * Stream operators
 */

FUNC1_TMPL std::ostream& operator<<(std::ostream& os, const vec2t<T>& v)  noexcept { return os << "[" << v.x << ", " << v.y << "]"; }
FUNC1_TMPL std::ostream& operator<<(std::ostream& os, const vec2t<T>&& v) noexcept { return os << "[" << v.x << ", " << v.y << "]"; }
FUNC1_TMPL std::ostream& operator<<(std::ostream& os, const vec3t<T>& v)  noexcept { return os << "[" << v.x << ", " << v.y << ", " << v.z << "]"; }
FUNC1_TMPL std::ostream& operator<<(std::ostream& os, const vec3t<T>&& v) noexcept { return os << "[" << v.x << ", " << v.y << ", " << v.z << "]"; }
FUNC1_TMPL std::ostream& operator<<(std::ostream& os, const vec4t<T>& v)  noexcept { return os << "[" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << "]"; }
FUNC1_TMPL std::ostream& operator<<(std::ostream& os, const vec4t<T>&& v) noexcept { return os << "[" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << "]"; }

#pragma endregion

#pragma region Vector functions

/*
 * Functions
 */

FUNC2_TMPL vec2t<T> Min(const vec2t<T>& a, const vec2t<U>& b) noexcept { return { a.x < b.x ? a.x : b.x, a.y < b.y ? a.y : b.y }; }
FUNC2_TMPL vec3t<T> Min(const vec3t<T>& a, const vec3t<U>& b) noexcept { return { a.x < b.x ? a.x : b.x, a.y < b.y ? a.y : b.y, a.z < b.z ? a.z : b.z }; }
FUNC2_TMPL vec4t<T> Min(const vec4t<T>& a, const vec4t<U>& b) noexcept { return { a.x < b.x ? a.x : b.x, a.y < b.y ? a.y : b.y, a.z < b.z ? a.z : b.z, a.w < b.w ? a.w : b.w }; }

FUNC2_TMPL vec2t<T> Max(const vec2t<T>& a, const vec2t<U>& b) noexcept { return { a.x > b.x ? a.x : b.x, a.y > b.y ? a.y : b.y }; }
FUNC2_TMPL vec3t<T> Max(const vec3t<T>& a, const vec3t<U>& b) noexcept { return { a.x > b.x ? a.x : b.x, a.y > b.y ? a.y : b.y, a.z > b.z ? a.z : b.z }; }
FUNC2_TMPL vec4t<T> Max(const vec4t<T>& a, const vec4t<U>& b) noexcept { return { a.x > b.x ? a.x : b.x, a.y > b.y ? a.y : b.y, a.z > b.z ? a.z : b.z, a.w > b.w ? a.w : b.w }; }

FUNC1_TMPL vec2t<T> Floor(const vec2t<T>& v) noexcept { return { std::floor(v.x), std::floor(v.y) }; }
FUNC1_TMPL vec3t<T> Floor(const vec3t<T>& v) noexcept { return { std::floor(v.x), std::floor(v.y), std::floor(v.z) }; }
FUNC1_TMPL vec4t<T> Floor(const vec4t<T>& v) noexcept { return { std::floor(v.x), std::floor(v.y), std::floor(v.z), std::floor(v.w) }; }

FUNC1_TMPL vec2t<T> Ceil(const vec2t<T>& v) noexcept { return { std::ceil(v.x), std::ceil(v.y) }; }
FUNC1_TMPL vec3t<T> Ceil(const vec3t<T>& v) noexcept { return { std::ceil(v.x), std::ceil(v.y), std::ceil(v.z) }; }
FUNC1_TMPL vec4t<T> Ceil(const vec4t<T>& v) noexcept { return { std::ceil(v.x), std::ceil(v.y), std::ceil(v.z), std::ceil(v.w) }; }

FUNC1_TMPL vec2t<T> Frac(const vec2t<T>& v) noexcept { return v - Floor(v); }
FUNC1_TMPL vec3t<T> Frac(const vec3t<T>& v) noexcept { return v - Floor(v); }
FUNC1_TMPL vec4t<T> Frac(const vec4t<T>& v) noexcept { return v - Floor(v); }

FUNC1_TMPL vec2t<T> Abs(const vec2t<T>& v) noexcept { return { std::abs(v.x), std::abs(v.y) }; }
FUNC1_TMPL vec3t<T> Abs(const vec3t<T>& v) noexcept { return { std::abs(v.x), std::abs(v.y), std::abs(v.z) }; }
FUNC1_TMPL vec4t<T> Abs(const vec4t<T>& v) noexcept { return { std::abs(v.x), std::abs(v.y), std::abs(v.z), std::abs(v.w) }; }

FUNC2_TMPL vec2t<T> Clamp(const vec2t<T>& v, const vec2t<U>& min, const vec2t<U>& max) noexcept { return { std::clamp(v.x, min.x, max.x), std::clamp(v.y, min.y, max.y) }; }
FUNC2_TMPL vec2t<T> Clamp(const vec2t<T>& v, const U min, const U max) noexcept { return { std::clamp(v.x, min, max), std::clamp(v.y, min, max) }; }
FUNC2_TMPL vec3t<T> Clamp(const vec3t<T>& v, const vec3t<U>& min, const vec3t<U>& max) noexcept { return { std::clamp(v.x, min.x, max.x), std::clamp(v.y, min.y, max.y), std::clamp(v.z, min.z, max.z) }; }
FUNC2_TMPL vec3t<T> Clamp(const vec3t<T>& v, const U min, const U max) noexcept { return { std::clamp(v.x, min, max), std::clamp(v.y, min, max), std::clamp(v.z, min, max) }; }
FUNC2_TMPL vec4t<T> Clamp(const vec4t<T>& v, const vec4t<U>& min, const vec4t<U>& max) noexcept { return { std::clamp(v.x, min.x, max.x), std::clamp(v.y, min.y, max.y), std::clamp(v.z, min.z, max.z), std::clamp(v.w, min.w) }; }
FUNC2_TMPL vec4t<T> Clamp(const vec4t<T>& v, const U min, const U max) noexcept { return { std::clamp(v.x, min, max), std::clamp(v.y, min, max), std::clamp(v.z, min, max), std::clamp(v.w, min, max) }; }

FUNC2_TMPL vec2t<T> Mod(const vec2t<T>& a, const vec2t<U>& b) noexcept { return { std::fmod(a.x, b.x), std::fmod(a.y, b.y) }; }
FUNC2_TMPL vec2t<T> Mod(const vec2t<T>& a, const U b) noexcept { return { std::fmod(a.x, b), std::fmod(a.y, b) }; }
FUNC2_TMPL vec3t<T> Mod(const vec3t<T>& a, const vec3t<U>& b) noexcept { return { std::fmod(a.x, b.x), std::fmod(a.y, b.y), std::fmod(a.z, b.z) }; }
FUNC2_TMPL vec3t<T> Mod(const vec3t<T>& a, const U b) noexcept { return { std::fmod(a.x, b), std::fmod(a.y, b), std::fmod(a.z, b) }; }
FUNC2_TMPL vec4t<T> Mod(const vec4t<T>& a, const vec4t<U>& b) noexcept { return { std::fmod(a.x, b.x), std::fmod(a.y, b.y), std::fmod(a.z, b.z), std::fmod(a.w, b.w) }; }
FUNC2_TMPL vec4t<T> Mod(const vec4t<T>& a, const U b) noexcept { return { std::fmod(a.x, b), std::fmod(a.y, b), std::fmod(a.z, b), std::fmod(a.w, b) }; }

FUNC1_TMPL vec2t<T> Exp(const vec2t<T>& v) noexcept { return { std::exp(v.x), std::exp(v.y) }; }
FUNC1_TMPL vec3t<T> Exp(const vec3t<T>& v) noexcept { return { std::exp(v.x), std::exp(v.y), std::exp(v.z) }; }
FUNC1_TMPL vec4t<T> Exp(const vec4t<T>& v) noexcept { return { std::exp(v.x), std::exp(v.y), std::exp(v.z), std::exp(v.w) }; }

FUNC1_TMPL int DominantAxis(const vec2t<T>& v) noexcept {
	return (std::abs(v.x) > std::abs(v.y)) ? 0 : 1;
};
FUNC1_TMPL int DominantAxis(const vec3t<T>& v) noexcept {
	return (std::abs(v.x) > std::abs(v.y) && std::abs(v.x) > std::abs(v.z)) ? 0 :
		   (std::abs(v.y) > std::abs(v.z) ? 1 : 2);
};
FUNC1_TMPL int DominantAxis(const vec4t<T>& v) noexcept {
	T ax = std::abs(v.x), ay = std::abs(v.y), az = std::abs(v.z), aw = std::abs(v.w);
	if(ax >= ay && ax >= az && ax >= aw) return 0;
	if(ay >= ax && ay >= az && ay >= aw) return 1;
	if(az >= ax && az >= ay && az >= aw) return 2;
	return 3;
};

FUNC2_TMPL float Dot(const vec2t<T>& a, const vec2t<U>& b) noexcept { return a.x * b.x + a.y * b.y; }
FUNC2_TMPL float Dot(const vec3t<T>& a, const vec3t<U>& b) noexcept { return a.x * b.x + a.y * b.y + a.z; }
FUNC2_TMPL float Dot(const vec4t<T>& a, const vec4t<U>& b) noexcept { return a.x * b.x + a.y * b.y + a.z + a.w; }

FUNC2_TMPL vec3t<T> Cross(const vec3t<T>& a, const vec3t<U>& b) noexcept { return { a.y * b.z - b.y * a.z, a.z * b.x - b.z * a.x, a.x * b.y - b.x * a.y }; }

FUNC1_TMPL float SqrLength(const vec2t<T>& v) noexcept { return Dot<T, T>(v, v); }
FUNC1_TMPL float SqrLength(const vec3t<T>& v) noexcept { return Dot<T, T>(v, v); }
FUNC1_TMPL float SqrLength(const vec4t<T>& v) noexcept { return Dot<T, T>(v, v); }

FUNC2_TMPL float Distance(const vec2t<T>& a, const vec2t<T>& b) noexcept { return Length(b - a); }
FUNC2_TMPL float Distance(const vec3t<T>& a, const vec3t<T>& b) noexcept { return Length(b - a); }
FUNC2_TMPL float Distance(const vec4t<T>& a, const vec4t<T>& b) noexcept { return Length(b - a); }

FUNC1_TMPL vec2t<T> Normalise(const vec2t<T>& v) noexcept { return v / Length(v); }
FUNC1_TMPL vec3t<T> Normalise(const vec3t<T>& v) noexcept { return v / Length(v); }
FUNC1_TMPL vec4t<T> Normalise(const vec4t<T>& v) noexcept { return v / Length(v); }
// Just for you American English people
FUNC1_TMPL vec2t<T> Normalize(const vec2t<T>& v) noexcept { return v / Length(v); }
FUNC1_TMPL vec3t<T> Normalize(const vec3t<T>& v) noexcept { return v / Length(v); }
FUNC1_TMPL vec4t<T> Normalize(const vec4t<T>& v) noexcept { return v / Length(v); }

FUNC2_TMPL float Angle(const vec2t<T>& a, const vec2t<T>& b) noexcept { return acos(Dot(a, b) / (Length(a) * Length(b))); }
FUNC2_TMPL float Angle(const vec3t<T>& a, const vec3t<T>& b) noexcept { return acos(Dot(a, b) / (Length(a) * Length(b))); }
FUNC2_TMPL float Angle(const vec4t<T>& a, const vec4t<T>& b) noexcept { return acos(Dot(a, b) / (Length(a) * Length(b))); }

FUNC2_TMPL vec2t<T> ProjectOnto(const vec2t<T>& a, const vec2t<T>& b) noexcept { return Normalise(b) * Dot(a, b); }
FUNC2_TMPL vec2t<T> ProjectOnto(const vec3t<T>& a, const vec3t<T>& b) noexcept { return Normalise(b) * Dot(a, b); }
FUNC2_TMPL vec2t<T> ProjectOnto(const vec4t<T>& a, const vec4t<T>& b) noexcept { return Normalise(b) * Dot(a, b); }

#pragma endregion

}

#pragma warning ( pop )
