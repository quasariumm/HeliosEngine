#ifndef PCH_CXX_H_
#define PCH_CXX_H_

// Include common STL libraries
#include <array>
#include <cfloat>
#include <chrono>
#include <climits>
#include <cmath>
#include <complex>
#include <cstdio>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <source_location>
#include <sstream>
#include <string>
#include <thread>
#include <typeinfo>
#include <unordered_map>
#include <utility>
#include <vector>

// C++ 20 features
#include <concepts>
#include <compare>
#include <numbers>
#include <type_traits>

// Engine libraries
#include <Math/Vector.h>
#include <Math/Matrix.h>
#include <Math/Random.h>
#include <Core/Common.h>
#include <Core/Timer.h>

// External libraries
#define IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_USE_WCHAR32
#include <imgui.h>

// SIMD
// Defines by Jacco Bikker
// SSE/AVX/AVX2/NEON support.
// SSE4.2 availability: Since Nehalem (2008)
// AVX1 availability: Since Sandy Bridge (2011)
// AVX2 availability: Since Haswell (2013)
#ifndef ENGINE_NO_SIMD
#if defined __x86_64__ || defined _M_X64 || defined __wasm_simd128__ || defined __wasm_relaxed_simd__
#if !defined __SSE4_2__  && !defined _MSC_VER
#pragma warning( "SSE4.2 not enabled in compilation." )
#else
#define ENGINE_USESSE
#ifndef __SSE4_2__
#define __SSE4_2__		// msvc doesn't set the SSE flag
#endif
#endif
#if !defined __AVX__
#pragma warning( "AVX not enabled in compilation." )
#define ENGINE_NO_SIMD
#else
#define ENGINE_USE_AVX
#define ENGINE_USE_SSE
#endif
#if !defined __AVX2__ || (!defined __FMA__ && !defined _MSC_VER)
#pragma warning( "AVX2 and FMA not enabled in compilation." )
#define ENGINE_NO_SIMD
#else
#define ENGINE_USE_AVX2
#define ENGINE_USE_AVX
#define ENGINE_USE_SSE
#endif
#include "immintrin.h"	// for __m128 and __m256
#elif defined __aarch64__ || defined _M_ARM64
#if !defined __NEON__
#pragma warning( "NEON not enabled in compilation." )
#define ENGINE_NO_SIMD
#else
#define ENGINE_USE_NEON
#include "arm_neon.h"
#endif
#endif
#endif // ENGINE_NO_SIMD

#endif // PCH_CXX_H_