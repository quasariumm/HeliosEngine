#pragma once
#include <string>
#include "Projects/ProjectHandler.h"
#include "Debugger/Debugger.h"

namespace Engine
{

std::string ENGINE_API Demangle(const char* name);

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
    {
        std::cout << "Trying to copy non existing file: " << a << std::endl;
        return false;
    }
    if (std::filesystem::exists(b))
        std::filesystem::remove(b);
    return std::filesystem::copy_file(a, b, std::filesystem::copy_options::overwrite_existing);
}

enum class DefaultFileType
{
    // Requires project name to be set
    MAINHEADER,
    // Requires project name to be set
    MAINSOURCE,
    // Requires project name to be set
    CMAKELISTS,
};

// Thnx Stack overflow person
// https://stackoverflow.com/questions/3418231/replace-part-of-a-string-with-another-string
    static void ReplaceAll(std::string &str, const std::string &from, const std::string &to) {
        if (from.empty())
            return;
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
        }
    }


static bool DefaultFile(DefaultFileType type, const std::filesystem::path& targetLocation)
{
    // Check if file already exists
    if (std::filesystem::exists(targetLocation)) {
        DebugLog(LogSeverity::SEVERE, L"Did not create default file. File already exists");
        return false;
    }

    std::filesystem::path defaultFileLocation = std::filesystem::current_path().append("assets/defaults/");

    switch (type)
    {
        case DefaultFileType::MAINHEADER:
            defaultFileLocation.append("Default_MainHeader.txt");
            break;
        case DefaultFileType::MAINSOURCE:
            defaultFileLocation.append("Default_MainSource.txt");
            break;
        case DefaultFileType::CMAKELISTS:
            defaultFileLocation.append("Default_CmakeLists.txt");
            break;
    }

    std::wstring fileBuffer;
    std::ifstream file(defaultFileLocation);

    std::string line;
    while (std::getline(file, line)) {
        ReplaceAll(line, "{ProjectName}", WStringToUTF8(ProjectHandler::ProjectName()));
        ReplaceAll(line, "{EnginePath}", WStringToUTF8(EnginePath(true)));

        fileBuffer.append(STR_TO_WSTR(line + "\n"));
    }

    file.close();

    std::ofstream resultFile(targetLocation);

    if (!resultFile.is_open())
    {
        DebugLog(LogSeverity::SEVERE, L"Did not create default file. Failed to create new file");
        return false;
    }

    resultFile << WStringToUTF8(fileBuffer);
    resultFile.close();

    DebugLog(LogSeverity::DONE, L"Created default file");
    return true;
}

}


