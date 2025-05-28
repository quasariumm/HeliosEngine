#pragma once
#include <string>

namespace Engine
{

std::string Demangle(const char* name);

static bool IsToken(const std::wstring& line, const std::wstring& token)
{
    return line.substr(0, line.find(L" = ")) == token;
}

static std::wstring TokenValue(const std::wstring& line)
{
    std::wstring token = line;
    return token.erase(0, line.find(L" = ") + 3);
}

static constexpr const wchar_t* sep = L", ";
static vec2f ParseVec2(std::wstring text)
{
    vec2f result;
    text.erase(text.begin(), text.begin() + 1); // Remove first bracket
    text.erase(text.end() - 1, text.end()); // Remove last bracket
    result.x = std::stof(text.substr(0, text.find(sep))); // Get the first value
    text.erase(text.begin(), text.begin() + text.find(sep) + 2ull); // Remove the first value
    result.y = std::stof(text.substr(0, text.find(sep))); // Get the second value
    return result;
}

static vec3 ParseVec3(std::wstring text)
{
    vec3 result;
    text.erase(text.begin(), text.begin() + 1); // Remove first bracket
    text.erase(text.end() - 1, text.end()); // Remove last bracket
    result.x = std::stof(text.substr(0, text.find(sep))); // Get the first value
    text.erase(text.begin(), text.begin() + text.find(sep) + 2); // Remove the first value
    result.y = std::stof(text.substr(0, text.find(sep))); // Get the second value
    text.erase(text.begin(), text.begin() + text.find(sep) + 2); // Remove the second value
    result.z = std::stof(text.substr(0, text.find(sep))); // Get the third value
    return result;
}

static bool ForceCopy(const std::filesystem::path& a, const std::filesystem::path& b)
{
    if (!std::filesystem::exists(a))
        return false;
    if (std::filesystem::exists(b))
        std::filesystem::remove(b);
    std::filesystem::copy(a, b, std::filesystem::copy_options::overwrite_existing);
    return true;
}

}


