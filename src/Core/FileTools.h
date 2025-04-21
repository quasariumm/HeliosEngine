#pragma once
#include <string>

namespace Engine
{

std::string Demangle(const char* name);

static bool IsToken(const std::string& line, const std::string& token)
{
    return line.substr(0, line.find(" = ")) == token;
}

static std::string TokenValue(const std::string& line)
{
    std::string token = line;
    return token.erase(0, line.find(" = ") + 3);
}

static vec2 ParseVec2(std::string text)
{
    vec2 result = {};
    text.erase(text.begin(), text.begin() + 1); // Remove first bracket
    text.erase(text.end() - 1, text.end()); // Remove last bracket
    result.x = std::stof(text.substr(0, text.find(", "))); // Get the first value
    text.erase(text.begin(), text.begin() + text.find(", ") + 2); // Remove the first value
    result.y = std::stof(text.substr(0, text.find(", "))); // Get the second value
    return result;
}

static vec3 ParseVec3(std::string text)
{
    vec3 result = {};
    text.erase(text.begin(), text.begin() + 1); // Remove first bracket
    text.erase(text.end() - 1, text.end()); // Remove last bracket
    result.x = std::stof(text.substr(0, text.find(", "))); // Get the first value
    text.erase(text.begin(), text.begin() + text.find(", ") + 2); // Remove the first value
    result.y = std::stof(text.substr(0, text.find(", "))); // Get the second value
    text.erase(text.begin(), text.begin() + text.find(", ") + 2); // Remove the second value
    result.z = std::stof(text.substr(0, text.find(", "))); // Get the third value
    return result;
}

}


