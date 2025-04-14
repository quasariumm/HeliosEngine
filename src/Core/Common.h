#pragma once

#include <string>

static const std::wstring ANSI_OK = L"[\033[32m OK \033[0m] ";
static const std::wstring ANSI_DONE = L"[\033[32mDONE\033[0m] ";
static const std::wstring ANSI_ERR = L"[\033[31mERRO\033[0m] ";
static const std::wstring ANSI_WARN = L"[\033[33mWARN\033[0m] ";
static const std::wstring ANSI_INFO = L"[\033[36mINFO\033[0m] ";

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

#define DEG(x) { (x) * 57.2957795131f }
#define RAD(x) { (x) * 0.0174532925f }

#define CALL(func, ...) if (func) (func).operator()(__VA_ARGS__)

// TODO: Find alternative to std::wcsrtombs (deprecated)
/**
 * @brief Converts a wide string to a UTF-8 encoded basic string
 * @param wstr The wide string to convert
 * @return The converted UTF-8 encided string
 */
inline std::string WStringToUTF8(const std::wstring& wstr)
{
	std::string result;
	std::mbstate_t state = std::mbstate_t();
	const wchar_t* src = wstr.c_str();
	const size_t len = 1 + std::wcsrtombs(nullptr, &src, wstr.length(), &state);
	if (len == 0ull) return "";
	char buffer[len];
	std::wcsrtombs(buffer, &src, len, &state);
	result.assign(buffer);
	return result;
}
