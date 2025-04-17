#pragma once

#include <glad/glad.h>

#include "IconDefines.h"

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
inline std::string WStringToUTF8( const std::wstring& wstr )
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