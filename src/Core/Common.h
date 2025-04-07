#pragma once

#include <string>

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
