#pragma once

#include "Vector.h"

#ifdef USE_SIMD
#include <immintrin.h>
#endif

#pragma warning ( push )
#pragma warning ( disable: 4201 /* nameless struct / union */ )

namespace Engine
{

#pragma region Matrix structs

/*
 * Matrix structs
 */

template<typename T> requires numeric<T>
struct mat2t
{
	mat2t() : rows({1, 0}, {0, 1}) {}

	explicit mat2t(T s) : rows({s, 0}, {0, s}) {}

	mat2t(const vec2t<T>& r1, const vec2t<T>& r2) : rows(r1, r2) {}

	union
	{
		T cell[4];
		struct { vec2t<T> rows[2]; };
	};

	vec2t<T>& operator[](const int i)
	{
		if (i < 0 || i > 1)
			throw std::out_of_range("Can't index mat2t with index " + std::to_string(i) + ".");
		return rows[i];
	}
	const vec2t<T>& operator[](const int i) const
	{
		if (i < 0 || i > 1)
			throw std::out_of_range("Can't index mat2t with index " + std::to_string(i) + ".");
		return rows[i];
	}
};

typedef mat2t<float>	mat2;
typedef mat2t<float>	mat2f;
typedef mat2t<int>		mat2i;
typedef mat2t<unsigned> mat2u;
typedef mat2t<double>	mat2d;



template<typename T> requires numeric<T>
struct mat3t
{
	mat3t() : rows({1, 0, 0}, {0, 1, 0}, {0, 0, 1}) {}

	explicit mat3t(T s) : rows({s, 0, 0}, {0, s, 0}, {0, 0, s}) {}

	mat3t(const vec3t<T>& r1, const vec3t<T>& r2, const vec3t<T>& r3) : rows(r1, r2, r3) {}

	union
	{
		T cell[9];
		struct { vec3t<T> rows[3]; };
	};

	vec3t<T>& operator[](const int i)
	{
		if (i < 0 || i > 2)
			throw std::out_of_range("Can't index mat3t with index " + std::to_string(i) + ".");
		return rows[i];
	}
	const vec3t<T>& operator[](const int i) const
	{
		if (i < 0 || i > 2)
			throw std::out_of_range("Can't index mat3t with index " + std::to_string(i) + ".");
		return rows[i];
	}
};

typedef mat3t<float>	mat3;
typedef mat3t<float>	mat3f;
typedef mat3t<int>		mat3i;
typedef mat3t<unsigned> mat3u;
typedef mat3t<double>	mat3d;


template<typename T> requires numeric<T>
struct mat4t
{
	mat4t() : rows({1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}) {}

	explicit mat4t(T s) : rows({s, 0, 0, 0}, {0, s, 0, 0}, {0, 0, s, 0}, {0, 0, 0, s}) {}

	mat4t(const vec4t<T>& r1, const vec4t<T>& r2, const vec4t<T>& r3, const vec4t<T>& r4) : rows(r1, r2, r3, r4) {}

	union
	{
		T cell[16];
		struct { vec4t<T> rows[4]; };
	};

	vec4t<T>& operator[](const int i)
	{
		if (i < 0 || i > 3)
			throw std::out_of_range("Can't index mat4t with index " + std::to_string(i) + ".");
		return rows[i];
	}
	const vec4t<T>& operator[](const int i) const
	{
		if (i < 0 || i > 3)
			throw std::out_of_range("Can't index mat4t with index " + std::to_string(i) + ".");
		return rows[i];
	}
};

typedef mat4t<float>	mat4;
typedef mat4t<float>	mat4f;
typedef mat4t<int>		mat4i;
typedef mat4t<unsigned> mat4u;
typedef mat4t<double>	mat4d;


#ifdef USE_SIMD

/*
 * SIMD 4x4 matrix
 */

template<>
struct mat4t<float>
{
	mat4t() : rows({1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}) {}

	explicit mat4t(float s) : rows({s, 0, 0, 0}, {0, s, 0, 0}, {0, 0, s, 0}, {0, 0, 0, s}) {}

	mat4t(const vec4f& r1, const vec4f& r2, const vec4f& r3, const vec4f& r4) : rows(r1, r2, r3, r4) {}

	mat4t(const __m128& r1, const __m128& r2, const __m128& r3, const __m128& r4) : row14(r1), row24(r2), row34(r3), row44(r4) {}

#ifdef USE_AVX2
	mat4t(const __m256& r12, const __m256& r34) : rows12(r12), rows34(r34) {}
#endif

	union
	{
		float cell[16];
		struct { vec4f rows[4]; };
		struct { __m128 row14, row24, row34, row44; };
#ifdef USE_AVX2
		struct { __m256 rows12, rows34; };
#endif
	};

	vec4f& operator[](const int i)
	{
		if (i < 0 || i >= 3)
			throw std::out_of_range("Can't index mat4t with index " + std::to_string(i) + ".");
		return rows[i];
	}
	const vec4f& operator[](const int i) const
	{
		if (i < 0 || i >= 3)
			throw std::out_of_range("Can't index mat4t with index " + std::to_string(i) + ".");
		return rows[i];
	}
};

#endif

#pragma endregion

#pragma region Matrix operators

/*
 * Matrix operators
 */

/*
 * Binary operators
 */

FUNC2_TMPL bool operator==(const mat3t<T>& a, const mat3t<U>& b) noexcept { return a.rows[0] == b.rows[0] && a.rows[1] == b.rows[1] && a.rows[2] == b.rows[2]; }
FUNC2_TMPL bool operator==(const mat2t<T>& a, const mat2t<U>& b) noexcept { return a.rows[0] == b.rows[0] && a.rows[1] == b.rows[1]; }
FUNC2_TMPL bool operator==(const mat4t<T>& a, const mat4t<U>& b) noexcept { return a.rows[0] == b.rows[0] && a.rows[1] == b.rows[1] && a.rows[2] == b.rows[2] && a.rows[3] == b.rows[3]; }
FUNC2_TMPL bool operator==(const mat2t<T>& a, U b) noexcept { return a.rows[0] == b && a.rows[1] == b; }
FUNC2_TMPL bool operator==(const mat3t<T>& a, U b) noexcept { return a.rows[0] == b && a.rows[1] == b && a.rows[2] == b; }
FUNC2_TMPL bool operator==(const mat4t<T>& a, U b) noexcept { return a.rows[0] == b && a.rows[1] == b && a.rows[2] == b && a.rows[3] == b; }

FUNC2_TMPL bool operator<(const mat2t<T>& a, const mat2t<U>& b) noexcept { return a.rows[0] < b.rows[0] && a.rows[1] < b.rows[1]; }
FUNC2_TMPL bool operator<(const mat3t<T>& a, const mat3t<U>& b) noexcept { return a.rows[0] < b.rows[0] && a.rows[1] < b.rows[1] && a.rows[2] < b.rows[2]; }
FUNC2_TMPL bool operator<(const mat4t<T>& a, const mat4t<U>& b) noexcept { return a.rows[0] < b.rows[0] && a.rows[1] < b.rows[1] && a.rows[2] < b.rows[2] && a.rows[3] < b.rows[3]; }
FUNC2_TMPL bool operator<(const mat2t<T>& a, U b) noexcept { return a.rows[0] < b && a.rows[1] < b; }
FUNC2_TMPL bool operator<(const mat3t<T>& a, U b) noexcept { return a.rows[0] < b && a.rows[1] < b && a.rows[2] < b; }
FUNC2_TMPL bool operator<(const mat4t<T>& a, U b) noexcept { return a.rows[0] < b && a.rows[1] < b && a.rows[2] < b && a.rows[3] < b; }

FUNC2_TMPL bool operator<=(const mat2t<T>& a, const mat2t<U>& b) noexcept { return !(b < a); }
FUNC2_TMPL bool operator<=(const mat3t<T>& a, const mat3t<U>& b) noexcept { return !(b < a); }
FUNC2_TMPL bool operator<=(const mat4t<T>& a, const mat4t<U>& b) noexcept { return !(b < a); }
FUNC2_TMPL bool operator<=(const mat2t<T>& a, U b) noexcept { return !(b < a); }
FUNC2_TMPL bool operator<=(const mat3t<T>& a, U b) noexcept { return !(b < a); }
FUNC2_TMPL bool operator<=(const mat4t<T>& a, U b) noexcept { return !(b < a); }

FUNC2_TMPL bool operator>(const mat2t<T>& a, const mat2t<U>& b) noexcept { return b < a; }
FUNC2_TMPL bool operator>(const mat3t<T>& a, const mat3t<U>& b) noexcept { return b < a; }
FUNC2_TMPL bool operator>(const mat4t<T>& a, const mat4t<U>& b) noexcept { return b < a; }
FUNC2_TMPL bool operator>(const mat2t<T>& a, U b) noexcept { return b < a; }
FUNC2_TMPL bool operator>(const mat3t<T>& a, U b) noexcept { return b < a; }
FUNC2_TMPL bool operator>(const mat4t<T>& a, U b) noexcept { return b < a; }

FUNC2_TMPL bool operator>=(const mat2t<T>& a, const mat2t<U>& b) noexcept { return !(a < b); }
FUNC2_TMPL bool operator>=(const mat3t<T>& a, const mat3t<U>& b) noexcept { return !(a < b); }
FUNC2_TMPL bool operator>=(const mat4t<T>& a, const mat4t<U>& b) noexcept { return !(a < b); }
FUNC2_TMPL bool operator>=(const mat2t<T>& a, U b) noexcept { return !(a < b); }
FUNC2_TMPL bool operator>=(const mat3t<T>& a, U b) noexcept { return !(a < b); }
FUNC2_TMPL bool operator>=(const mat4t<T>& a, U b) noexcept { return !(a < b); }

FUNC2_TMPL std::strong_ordering operator<=>(const mat2t<T>& a, const mat2t<U>& b) noexcept { return std::lexicographical_compare_three_way(std::begin(a.cell), std::end(a.cell), std::begin(b.cell), std::end(b.cell)); }
FUNC2_TMPL std::strong_ordering operator<=>(const mat3t<T>& a, const mat3t<U>& b) noexcept { return std::lexicographical_compare_three_way(std::begin(a.cell), std::end(a.cell), std::begin(b.cell), std::end(b.cell)); }
FUNC2_TMPL std::strong_ordering operator<=>(const mat4t<T>& a, const mat4t<U>& b) noexcept { return std::lexicographical_compare_three_way(std::begin(a.cell), std::end(a.cell), std::begin(b.cell), std::end(b.cell)); }

/*
 * Arithmetic operators
 */

FUNC2_TMPL mat2t<T> operator+(const mat2t<T>& a, const mat2t<U>& b) noexcept { return mat2t<T>(a.rows[0] + b.rows[0], a.rows[1] + b.rows[1]); }
FUNC2_TMPL mat3t<T> operator+(const mat3t<T>& a, const mat3t<U>& b) noexcept { return mat3t<T>(a.rows[0] + b.rows[0], a.rows[1] + b.rows[1], a.rows[2] + b.rows[2]); }
FUNC2_TMPL mat4t<T> operator+(const mat4t<T>& a, const mat4t<U>& b) noexcept { return mat4t<T>(a.rows[0] + b.rows[0], a.rows[1] + b.rows[1], a.rows[2] + b.rows[2], a.rows[3] + b.rows[3]); }
FUNC2_TMPL mat2t<T> operator+(const mat2t<T>& a, const U b) noexcept { return mat2t<T>(a.rows[0] + b, a.rows[1] + b); }
FUNC2_TMPL mat3t<T> operator+(const mat3t<T>& a, const U b) noexcept { return mat3t<T>(a.rows[0] + b, a.rows[1] + b, a.rows[2] + b); }
FUNC2_TMPL mat4t<T> operator+(const mat4t<T>& a, const U b) noexcept { return mat4t<T>(a.rows[0] + b, a.rows[1] + b, a.rows[2] + b, a.rows[3] + b); }
FUNC2_TMPL void operator+=(mat2t<T>& a, const mat2t<U>& b) noexcept { a.rows[0] += b.rows[0]; a.rows[1] += b.rows[1]; }
FUNC2_TMPL void operator+=(mat3t<T>& a, const mat3t<U>& b) noexcept { a.rows[0] += b.rows[0]; a.rows[1] += b.rows[1]; a.rows[2] += b.rows[2]; }
FUNC2_TMPL void operator+=(mat4t<T>& a, const mat4t<U>& b) noexcept { a.rows[0] += b.rows[0]; a.rows[1] += b.rows[1]; a.rows[2] += b.rows[2]; a.rows[3] += b.rows[3]; }
FUNC2_TMPL void operator+=(mat2t<T>& a, const U b) noexcept { a.rows[0] += b; a.rows[1] += b; }
FUNC2_TMPL void operator+=(mat3t<T>& a, const U b) noexcept { a.rows[0] += b; a.rows[1] += b; a.rows[2] += b; }
FUNC2_TMPL void operator+=(mat4t<T>& a, const U b) noexcept { a.rows[0] += b; a.rows[1] += b; a.rows[2] += b; a.rows[3] += b; }

FUNC2_TMPL mat2t<T> operator-(const mat2t<T>& a, const mat2t<U>& b) noexcept { return mat2t<T>(a.rows[0] - b.rows[0], a.rows[1] - b.rows[1]); }
FUNC2_TMPL mat3t<T> operator-(const mat3t<T>& a, const mat3t<U>& b) noexcept { return mat3t<T>(a.rows[0] - b.rows[0], a.rows[1] - b.rows[1], a.rows[2] - b.rows[2]); }
FUNC2_TMPL mat4t<T> operator-(const mat4t<T>& a, const mat4t<U>& b) noexcept { return mat4t<T>(a.rows[0] - b.rows[0], a.rows[1] - b.rows[1], a.rows[2] - b.rows[2], a.rows[3] - b.rows[3]); }
FUNC2_TMPL mat2t<T> operator-(const mat2t<T>& a, const U b) noexcept { return mat2t<T>(a.rows[0] - b, a.rows[1] - b); }
FUNC2_TMPL mat3t<T> operator-(const mat3t<T>& a, const U b) noexcept { return mat3t<T>(a.rows[0] - b, a.rows[1] - b, a.rows[2] - b); }
FUNC2_TMPL mat4t<T> operator-(const mat4t<T>& a, const U b) noexcept { return mat4t<T>(a.rows[0] - b, a.rows[1] - b, a.rows[2] - b, a.rows[3] - b); }
FUNC2_TMPL void operator-=(mat2t<T>& a, const mat2t<U>& b) noexcept { a.rows[0] -= b.rows[0]; a.rows[1] -= b.rows[1]; }
FUNC2_TMPL void operator-=(mat3t<T>& a, const mat3t<U>& b) noexcept { a.rows[0] -= b.rows[0]; a.rows[1] -= b.rows[1]; a.rows[2] -= b.rows[2]; }
FUNC2_TMPL void operator-=(mat4t<T>& a, const mat4t<U>& b) noexcept { a.rows[0] -= b.rows[0]; a.rows[1] -= b.rows[1]; a.rows[2] -= b.rows[2]; a.rows[3] -= b.rows[3]; }
FUNC2_TMPL void operator-=(mat2t<T>& a, const U b) noexcept { a.rows[0] -= b; a.rows[1] -= b; }
FUNC2_TMPL void operator-=(mat3t<T>& a, const U b) noexcept { a.rows[0] -= b; a.rows[1] -= b; a.rows[2] -= b; }
FUNC2_TMPL void operator-=(mat4t<T>& a, const U b) noexcept { a.rows[0] -= b; a.rows[1] -= b; a.rows[2] -= b; a.rows[3] -= b; }

FUNC2_TMPL mat2t<T> operator*(const mat2t<T>& a, const mat2t<U>& b) noexcept
{
	mat2t<T> result;
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			T temp = a.cell[i * 2 + 0] * b.cell[j];
			for (int k = 1; k < 2; k++) {
				temp += a.cell[i * 2 + k] * b.cell[k * 2 + j];
			}
			result.cell[i * 2 + j] = temp;
		}
	}
	return result;
}
FUNC2_TMPL mat3t<T> operator*(const mat3t<T>& a, const mat3t<U>& b) noexcept
{
	mat3t<T> result;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			T temp = a.cell[i * 3 + 0] * b.cell[j];
			for (int k = 1; k < 3; k++) {
				temp += a.cell[i * 3 + k] * b.cell[k * 3 + j];
			}
			result.cell[i * 3 + j] = temp;
		}
	}
	return result;
}
FUNC2_TMPL mat4t<T> operator*(const mat4t<T>& a, const mat4t<U>& b) noexcept
{
	mat4t<T> result;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			T temp = a.cell[i * 4 + 0] * b.cell[j];
			for (int k = 1; k < 4; k++) {
				temp += a.cell[i * 4 + k] * b.cell[k * 4 + j];
			}
			result.cell[i * 4 + j] = temp;
		}
	}
	return result;
}
FUNC2_TMPL mat2t<T> operator*(const mat2t<T>& a, const U b) noexcept { return {a.rows[0] * b, a.rows[1] * b}; }
FUNC2_TMPL mat3t<T> operator*(const mat3t<T>& a, const U b) noexcept { return {a.rows[0] * b, a.rows[1] * b, a.rows[2] * b}; }
FUNC2_TMPL mat4t<T> operator*(const mat4t<T>& a, const U b) noexcept { return {a.rows[0] * b, a.rows[1] * b, a.rows[2] * b, a.rows[3] * b}; }
FUNC2_TMPL void operator*=(mat2t<T>& a, const mat2t<U>& b) noexcept
{
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			T temp = a.cell[i * 2 + 0] * b.cell[j];
			for (int k = 1; k < 2; k++) {
				temp += a.cell[i * 2 + k] * b.cell[k * 2 + j];
			}
			a.cell[i * 2 + j] = temp;
		}
	}
}
FUNC2_TMPL void operator*=(const mat3t<T>& a, const mat3t<U>& b) noexcept
{
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			T temp = a.cell[i * 3 + 0] * b.cell[j];
			for (int k = 1; k < 3; k++) {
				temp += a.cell[i * 3 + k] * b.cell[k * 3 + j];
			}
			a.cell[i * 3 + j] = temp;
		}
	}
}
FUNC2_TMPL void operator*=(const mat4t<T>& a, const mat4t<U>& b) noexcept
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			T temp = a.cell[i * 4 + 0] * b.cell[j];
			for (int k = 1; k < 4; k++) {
				temp += a.cell[i * 4 + k] * b.cell[k * 4 + j];
			}
			a.cell[i * 4 + j] = temp;
		}
	}
}
FUNC2_TMPL void operator*=(mat2t<T>& a, const U b) noexcept { a.rows[0] *= b.rows[0]; a.rows[1] *= b.rows[1]; }
FUNC2_TMPL void operator*=(mat3t<T>& a, const U b) noexcept { a.rows[0] *= b.rows[0]; a.rows[1] *= b.rows[1]; a.rows[2] *= b.rows[2]; }
FUNC2_TMPL void operator*=(mat4t<T>& a, const U b) noexcept { a.rows[0] *= b.rows[0]; a.rows[1] *= b.rows[1]; a.rows[2] *= b.rows[2]; a.rows[3] *= b.rows[3]; }


FUNC2_TMPL vec2t<T> operator*(const mat2t<T>& matrix, const vec2t<U>& vector)
{
	vec2t<T> result;
	for (int i = 0; i < 2; i++)
	{
		T temp = matrix[i][0] * vector[0];
		for (int k = 1; k < 2; k++)
			temp += matrix[i][k] * vector[k];
		result[i] = temp;
	}
	return result;
}
FUNC2_TMPL vec3t<T> operator*(const mat3t<T>& matrix, const vec3t<U>& vector)
{
	vec3t<T> result;
	for (int i = 0; i < 3; i++)
	{
		T temp = matrix[i][0] * vector[0];
		for (int k = 1; k < 3; k++)
			temp += matrix[i][k] * vector[k];
		result[i] = temp;
	}
	return result;
}
FUNC2_TMPL vec4t<T> operator*(const mat4t<T>& matrix, const vec4t<U>& vector)
{
	vec4t<T> result;
	for (int i = 0; i < 4; i++)
	{
		T temp = matrix[i][0] * vector[0];
		for (int k = 1; k < 4; k++)
			temp += matrix[i][k] * vector[k];
		result[i] = temp;
	}
	return result;
}


#ifdef USE_SIMD

#ifndef USE_AVX2
/*
 * SSE operators
 */

inline mat4f operator+(const mat4f& a, const mat4f& b) noexcept { return { _mm_add_ps(a.row14, b.row14), _mm_add_ps(a.row24, b.row24), _mm_add_ps(a.row34, b.row34), _mm_add_ps(a.row44, b.row44) }; }
inline void operator+=(mat4f& a, const mat4f& b) noexcept { a.row14 = _mm_add_ps(a.row14, b.row14); a.row24 = _mm_add_ps(a.row24, b.row24); a.row34 = _mm_add_ps(a.row34, b.row34); a.row44 = _mm_add_ps(a.row44, b.row44); }

inline mat4f operator-(const mat4f& a, const mat4f& b) noexcept { return { _mm_sub_ps(a.row14, b.row14), _mm_sub_ps(a.row24, b.row24), _mm_sub_ps(a.row34, b.row34), _mm_sub_ps(a.row44, b.row44) }; }
inline void operator-=(mat4f& a, const mat4f& b) noexcept { a.row14 = _mm_sub_ps(a.row14, b.row14); a.row24 = _mm_sub_ps(a.row24, b.row24); a.row34 = _mm_sub_ps(a.row34, b.row34); a.row44 = _mm_sub_ps(a.row44, b.row44); }

// inline mat4f operator*(const mat4f& a, const mat4f& b) noexcept { return { _mm_mul_ps(a.row14, b.row14), _mm_mul_ps(a.row24, b.row24), _mm_mul_ps(a.row34, b.row34), _mm_mul_ps(a.row44, b.row44) }; }
// inline void operator*=(mat4f& a, const mat4f& b) noexcept { a.row14 = _mm_mul_ps(a.row14, b.row14); a.row24 = _mm_mul_ps(a.row24, b.row24); a.row34 = _mm_mul_ps(a.row34, b.row34); a.row44 = _mm_mul_ps(a.row44, b.row44); }

#else

/*
 * AVX2 operators
 */

inline mat4f operator+(const mat4f& a, const mat4f& b) noexcept { return { _mm256_add_ps(a.rows12, b.rows12), _mm256_add_ps(a.rows34, b.rows34) }; }
inline void operator+=(mat4f& a, const mat4f& b) noexcept { a.rows12 = _mm256_add_ps(a.rows12, b.rows12); a.rows34 = _mm256_add_ps(a.rows34, b.rows34); }

inline mat4f operator-(const mat4f& a, const mat4f& b) noexcept { return { _mm256_sub_ps(a.rows12, b.rows12), _mm256_sub_ps(a.rows34, b.rows34) }; }
inline void operator-=(mat4f& a, const mat4f& b) noexcept { a.rows12 = _mm256_sub_ps(a.rows12, b.rows12); a.rows34 = _mm256_sub_ps(a.rows34, b.rows34); }

// inline mat4f operator*(const mat4f& a, const mat4f& b) noexcept { return { _mm256_mul_ps(a.rows12, b.rows12), _mm256_mul_ps(a.rows34, b.rows34) }; }
// inline void operator*=(mat4f& a, const mat4f& b) noexcept { a.rows12 = _mm256_mul_ps(a.rows12, b.rows12); a.rows34 = _mm256_mul_ps(a.rows34, b.rows34); }

#endif

#endif

/*
 * Stream operators
 */

FUNC1_TMPL std::ostream& operator<<(std::ostream& os, const mat2t<T>& m) { return os << "[" << m.row1 << "\n " << m.row2 << "]"; }
FUNC1_TMPL std::ostream& operator<<(std::ostream& os, const mat2t<T>&& m) { return os << "[" << m.row1 << "\n " << m.row2 << "]"; }
FUNC1_TMPL std::ostream& operator<<(std::ostream& os, const mat3t<T>& m) { return os << "[" << m.row1 << "\n " << m.row2 << "\n " << m.row3 << "]"; }
FUNC1_TMPL std::ostream& operator<<(std::ostream& os, const mat3t<T>&& m) { return os << "[" << m.row1 << "\n " << m.row2 << "\n " << m.row3 << "]"; }
FUNC1_TMPL std::ostream& operator<<(std::ostream& os, const mat4t<T>& m) { return os << "[" << m.row1 << "\n " << m.row2 << "\n " << m.row3 << "\n " << m.row4 << "]"; }
FUNC1_TMPL std::ostream& operator<<(std::ostream& os, const mat4t<T>&& m) { return os << "[" << m.row1 << "\n " << m.row2 << "\n " << m.row3 << "\n " << m.row4 << "]"; }

#pragma endregion

#pragma region Matrix functions

/*
 * Functions
 */

// Pass as value is for a reason
FUNC1_TMPL T Determinant(mat2t<T> mat)
{
	// https://www.geeksforgeeks.org/cpp-program-for-determinant-of-a-matrix/
	T det = 1.0;
	for (int i = 0; i < 2; i++)
	{
		int pivot = i;
		for (int j = i + 1; j < 2; j++)
			if (std::abs(mat[j][i]) > std::abs(mat[pivot][i]))
				pivot = j;

		if (pivot != i)
		{
			std::swap(mat[i], mat[pivot]);
			det *= -1;
		}
		if (mat[i][i] == 0) return 0;
		det *= mat[i][i];
		for (int j = i + 1; j < 2; j++)
		{
			float factor = mat[j][i] / mat[i][i];
			for (int k = i + 1; k < 2; k++)
			{
				mat[j][k] -= factor * mat[i][k];
			}
		}
	}
	return det;
}
// Pass as value is for a reason
FUNC1_TMPL T Determinant(mat3t<T> mat)
{
	// https://www.geeksforgeeks.org/cpp-program-for-determinant-of-a-matrix/
	T det = 1.0;
	for (int i = 0; i < 3; i++)
	{
		int pivot = i;
		for (int j = i + 1; j < 3; j++)
			if (std::abs(mat[j][i]) > std::abs(mat[pivot][i]))
				pivot = j;
		if (pivot != i)
		{
			std::swap(mat[i], mat[pivot]);
			det *= -1;
		}
		if (mat[i][i] == 0) return 0;
		det *= mat[i][i];
		for (int j = i + 1; j < 3; j++)
		{
			float factor = mat[j][i] / mat[i][i];
			for (int k = i + 1; k < 3; k++)
			{
				mat[j][k] -= factor * mat[i][k];
			}
		}
	}
	return det;
}
// Pass as value is for a reason
FUNC1_TMPL T Determinant(mat4t<T> mat)
{
	// https://www.geeksforgeeks.org/cpp-program-for-determinant-of-a-matrix/
	T det = 1.0;
	for (int i = 0; i < 4; i++)
	{
		int pivot = i;
		for (int j = i + 1; j < 4; j++)
			if (std::abs(mat[j][i]) > std::abs(mat[pivot][i]))
				pivot = j;

		if (pivot != i)
		{
			std::swap(mat[i], mat[pivot]);
			det *= -1;
		}
		if (mat[i][i] == 0) return 0;
		det *= mat[i][i];
		for (int j = i + 1; j < 4; j++)
		{
			float factor = mat[j][i] / mat[i][i];
			for (int k = i + 1; k < 4; k++)
			{
				mat[j][k] -= factor * mat[i][k];
			}
		}
	}
	return det;
}


FUNC1_TMPL mat2t<T> Transpose(const mat2t<T>& mat)
{
	mat2t<T> solution;
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			solution[i][j] = mat[j][i];
	return solution;
}
FUNC1_TMPL mat3t<T> Transpose(const mat3t<T>& mat)
{
	mat3t<T> solution;
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			solution[i][j] = mat[j][i];
	return solution;
}
FUNC1_TMPL mat4t<T> Transpose(const mat4t<T>& mat)
{
	mat4t<T> solution;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			solution[i][j] = mat[j][i];
	return solution;
}


FUNC1_TMPL mat3t<T> Cofactor(const mat3t<T>& mat)
{
	mat3t<T> solution;
	mat2t<T> subMatrix; /* 1x1 matrix */

	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
		{
			int p = 0;
			for(int x = 0; x < 2; x++)
			{
				if(x == i) continue;
				int q = 0;
				for(int y = 0; y < 2; y++)
				{
					if(y == j) continue;
					subMatrix[p][q] = mat[x][y];
					q++;
				}
				p++;
			}
			solution[i][j] = pow(-1, i + j) * Determinant(subMatrix);
		}

	return solution;
}
FUNC1_TMPL mat4t<T> Cofactor(const mat4t<T>& mat)
{
	mat4t<T> solution;
	mat3t<T> subMatrix; /* 1x1 matrix */

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
		{
			int p = 0;
			for(int x = 0; x < 3; x++)
			{
				if(x == i) continue;
				int q = 0;
				for(int y = 0; y < 3; y++)
				{
					if(y == j) continue;
					subMatrix[p][q] = mat[x][y];
					q++;
				}
				p++;
			}
			solution[i][j] = pow(-1, i + j) * Determinant(subMatrix);
		}

	return solution;
}


FUNC1_TMPL mat3t<T> Invert(const mat3t<T>& mat)
{
	const float det = Determinant(mat);

	if (det == 0.f)
		return mat3t<T>();

	return Transpose(Cofactor(mat)) * (1.f / det);
}
FUNC1_TMPL mat4t<T> Invert(const mat4t<T>& mat)
{
	const float det = Determinant(mat);

	if (det == 0.f)
		return mat4t<T>();

	return Transpose(Cofactor(mat)) * (1.f / det);
}

#pragma endregion

#pragma region Matrix generators

// Inspired from https://github.com/jbikker/tmpl8rt_2025/blob/main/template/tmpl8math.h
FUNC1_TMPL mat3t<T> Translate( const T x, const T y ) { mat4t<T> result; result.cell[2] = x; result.cell[5] = y; return result; }
FUNC1_TMPL mat3t<T> Translate( const vec2t<T>& point ) { mat4t<T> result; result.cell[2] = point.x; result.cell[5] = point.y; return result; }
FUNC1_TMPL mat4t<T> Translate( const T x, const T y, const T z ) { mat4t<T> result; result.cell[3] = x; result.cell[7] = y; result.cell[11] = z; return result; }
FUNC1_TMPL mat4t<T> Translate( const vec3t<T>& point ) { mat4t<T> result; result.cell[3] = point.x; result.cell[7] = point.y; result.cell[11] = point.z; return result; }

FUNC1_TMPL mat2t<T> Scale( const T s ) { mat2t<T> result; result.cell[0] = s; return result; }
FUNC1_TMPL mat2t<T> Scale( const vec2t<T>& s ) { mat2t<T> result; result.cell[0] = s.x, result.cell[3] = s.y; return result; }
FUNC1_TMPL mat3t<T> Scale( const T s ) { mat3t<T> result; result.cell[0] = result.cell[4] = s; return result; }
FUNC1_TMPL mat3t<T> Scale( const vec2t<T>& s ) { mat3t<T> result; result.cell[0] = s.x, result.cell[4] = s.y; return result; }
FUNC1_TMPL mat3t<T> Scale( const vec3t<T>& s ) { mat3t<T> result; result.cell[0] = s.x, result.cell[5] = s.y, result.cell[8] = s.z; return result; }
FUNC1_TMPL mat4t<T> Scale( const T s ) { mat4t<T> result; result.cell[0] = result.cell[5] = result.cell[10] = s; return result; }
FUNC1_TMPL mat4t<T> Scale( const vec3t<T>& s ) { mat4t<T> result; result.cell[0] = s.x, result.cell[5] = s.y, result.cell[10] = s.z; return result; }
FUNC1_TMPL mat4t<T> Scale( const vec4t<T>& s ) { mat4t<T> result; result.cell[0] = s.x, result.cell[5] = s.y, result.cell[10] = s.z, result.cell[15] = s.w; return result; }

FUNC1_TMPL mat3t<T> Rotate(const vec3t<T>& eulerAngles)
{
	mat3t<T> result;
	const T cosX = (eulerAngles.x == 0) ? 1 : std::cos(eulerAngles.x);
	const T sinX = (eulerAngles.x == 0) ? 0 : std::sin(eulerAngles.x);
	const T cosY = (eulerAngles.y == 0) ? 1 : std::cos(eulerAngles.y);
	const T sinY = (eulerAngles.y == 0) ? 0 : std::sin(eulerAngles.y);
	const T cosZ = (eulerAngles.z == 0) ? 1 : std::cos(eulerAngles.z);
	const T sinZ = (eulerAngles.z == 0) ? 0 : std::sin(eulerAngles.z);
	result.cell[0] = cosX * cosY; result.cell[1] = cosX * sinY * sinZ - sinX * cosZ; result.cell[2] = cosX * sinY * sinZ + sinX * cosZ;
	result.cell[3] = sinX * cosY; result.cell[4] = sinX * sinY * sinZ + cosX * cosZ; result.cell[5] = sinX * sinY * sinZ - cosX * cosZ;
	result.cell[6] = -sinY;       result.cell[7] = cosY * sinZ;                      result.cell[8] = cosX * cosY;
	return result;
}

FUNC1_TMPL mat4t<T> RotateX( const T radians ) { mat4t<T> result; result.cell[5] = std::cos( radians ); result.cell[6] = -std::sin( radians ); result.cell[9] =  std::sin( radians ); result.cell[10] = std::cos( radians ); return result; };
FUNC1_TMPL mat4t<T> RotateY( const T radians ) { mat4t<T> result; result.cell[0] = std::cos( radians ); result.cell[2] =  std::sin( radians ); result.cell[8] = -std::sin( radians ); result.cell[10] = std::cos( radians ); return result; };
FUNC1_TMPL mat4t<T> RotateZ( const T radians ) { mat4t<T> result; result.cell[0] = std::cos( radians ); result.cell[1] = -std::sin( radians ); result.cell[4] =  std::sin( radians ); result.cell[5]  = std::cos( radians ); return result; };
FUNC1_TMPL mat4t<T> Rotate( const vec3t<T>& xyz, const T a ) { return Rotate( xyz.x, xyz.y, xyz.z, a ); }
FUNC1_TMPL mat4t<T> Rotate( const T x, const T y, const T z, const T a )
{
	const float c = std::cos( a ), l_c = 1 - c, s = std::sin( a );
	// row major
	mat4t<T> result;
	result.cell[0] = x * x + (1 - x * x) * c, result[1] = x * y * l_c + z * s,	   result.cell[2] = x * z * l_c - y * s,	  result.cell[3] = 0;
	result.cell[4] = x * y * l_c - z * s,	  result[5] = y * y + (1 - y * y) * c, result.cell[6] = y * z * l_c + x * s,	  result.cell[7] = 0;
	result.cell[8] = x * z * l_c + y * s,	  result[9] = y * z * l_c - x * s,	   result.cell[10] = z * z + (1 - z * z) * c, result.cell[11] = 0;
	result.cell[12] =						  result.cell[13] =					   result.cell[14] = 0,						  result.cell[15] = 1;
	return result;
}


/**
 * @see https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/gluLookAt.xml for reference
 */
FUNC1_TMPL mat4t<T> LookAt(const vec3t<T>& eye, const vec3t<T>& center, const vec3t<T>& up)
{
	mat4t<T> result;
	vec3t<T> f = Normalise(center - eye);
	vec3t<T> s = Cross(f, Normalise(up));
	vec3t<T> u = Cross(Normalise(s), f);

	result.cell[0] =  s.x; result.cell[1] =  s.y; result.cell[2]  =  s.z;
	result.cell[4] =  u.x; result.cell[5] =  u.y; result.cell[6]  =  u.z;
	result.cell[8] = -f.x; result.cell[9] = -f.y; result.cell[10] = -f.z;

	return Translate(result, -eye);
}

/**
 * @see https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/gluPerspective.xml for reference
 */
FUNC1_TMPL mat4t<T> Perspective(T fovy, T aspect, T zNear, T zFar)
{
	mat4t<T> matrix;

	T f = std::cos(0.5 * fovy) / std::sin(0.5 * fovy);

	matrix.cell[0]  = f / (aspect);
	matrix.cell[5]  = f;
	matrix.cell[10] = (zFar + zNear) / (zNear - zFar);
	matrix.cell[11] = (2 * zFar * zNear) / (zNear - zFar);
	matrix.cell[14] = -1;

	return matrix;
}

/**
 * @see https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glOrtho.xml for reference
 */
FUNC1_TMPL mat4t<T> Orthographic(T left, T right, T bottom, T top, T zNear = -1, T zFar = 1)
{
	mat4t<T> matrix;
	matrix.cell[0]  = 2 / (right - left);
	matrix.cell[5]  = 2 / (top - bottom);
	matrix.cell[10] = -2 / (zFar - zNear);
	matrix.cell[15] = 1;

	matrix.cell[3]  = -(right + left) / (right - left);
	matrix.cell[7]  = -(top + bottom) / (top - bottom);
	matrix.cell[11] = -(zFar + zNear) / (zFar - zNear);

	return matrix;
}

#pragma endregion

}

#pragma warning ( pop )
