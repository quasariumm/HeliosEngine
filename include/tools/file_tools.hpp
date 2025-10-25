#pragma once
#include <string>

// TODO(Quillan): Debugger
//#include "Debugger/Debugger.h"

namespace Engine
{
std::string Demangle( const char* name );


static bool IsToken( const std::wstring& line, const std::wstring& token )
{
	return line.substr(0, line.find(L" = ")) == token;
}


static std::wstring TokenValue( const std::wstring& line )
{
	std::wstring token = line;
	return token.erase(0, line.find(L" = ") + 3);
}


static constexpr auto SEP = ", ";


template <typename T>
static glm::vec<2, T> ParseVec2( std::string text )
{
	glm::vec<2, T> result;
	text.erase(text.begin(), text.begin() + 1); // Remove first bracket
	text.erase(text.end() - 1, text.end()); // Remove last bracket
	result.x = std::stof(text.substr(0, text.find(SEP))); // Get the first value
	text.erase(text.begin(), text.begin() + text.find(SEP) + 2ll); // Remove the first value
	result.y = std::stof(text.substr(0, text.find(SEP))); // Get the second value
	return result;
}


static glm::uvec2 ParseVec2u( std::string text )
{
	glm::uvec2 result;
	text.erase(text.begin(), text.begin() + 1); // Remove first bracket
	text.erase(text.end() - 1, text.end()); // Remove last bracket
	result.x = std::stoi(text.substr(0, text.find(SEP))); // Get the first value
	text.erase(text.begin(), text.begin() + text.find(SEP) + 2ll); // Remove the first value
	result.y = std::stoi(text.substr(0, text.find(SEP))); // Get the second value
	return result;
}


static glm::vec3 ParseVec3( std::string text )
{
	glm::vec3 result;
	text.erase(text.begin(), text.begin() + 1); // Remove first bracket
	text.erase(text.end() - 1, text.end()); // Remove last bracket
	result.x = std::stof(text.substr(0, text.find(SEP))); // Get the first value
	text.erase(text.begin(), text.begin() + text.find(SEP) + 2); // Remove the first value
	result.y = std::stof(text.substr(0, text.find(SEP))); // Get the second value
	text.erase(text.begin(), text.begin() + text.find(SEP) + 2); // Remove the second value
	result.z = std::stof(text.substr(0, text.find(SEP))); // Get the third value
	return result;
}


static bool ForceCopy( const std::filesystem::path& a, const std::filesystem::path& b )
{
	if (!std::filesystem::exists(a))
	{
		std::cout << "Trying to copy non existing file: " << a << std::endl;
		return false;
	}
	if (std::filesystem::exists(b))
		std::filesystem::remove(b);
	std::filesystem::path folderPath = b;
	std::filesystem::create_directories(folderPath.remove_filename());
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
	SOURCE_EMPTY_H,
	SOURCE_EMPTY_C,
	SOURCE_COMPONENT_H,
	SOURCE_COMPONENT_C,
	SOURCE_EDITOR_H,
	SOURCE_EDITOR_C,
};


// Thnx Stack overflow person
// https://stackoverflow.com/questions/3418231/replace-part-of-a-string-with-another-string
static void ReplaceAll( std::string& str, const std::string& from, const std::string& to )
{
	if (from.empty())
		return;
	size_t startPos = 0;
	while ((startPos = str.find(from, startPos)) != std::string::npos)
	{
		str.replace(startPos, from.length(), to);
		startPos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}


static bool DefaultFile( DefaultFileType type, const std::filesystem::path& targetLocation )
{
	// Check if file already exists
	if (std::filesystem::exists(targetLocation))
	{
		// DebugLog(LogSeverity::SEVERE, "Did not create default file. File already exists");
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
	case DefaultFileType::SOURCE_EMPTY_H:
		defaultFileLocation.append("Default_EmptyHeader.txt");
		break;
	case DefaultFileType::SOURCE_EMPTY_C:
		defaultFileLocation.append("Default_EmptySource.txt");
		break;
	case DefaultFileType::SOURCE_COMPONENT_H:
		defaultFileLocation.append("Default_ComponentHeader.txt");
		break;
	case DefaultFileType::SOURCE_COMPONENT_C:
		defaultFileLocation.append("Default_ComponentSource.txt");
		break;
	case DefaultFileType::SOURCE_EDITOR_H:
		defaultFileLocation.append("Default_EditorHeader.txt");
		break;
	case DefaultFileType::SOURCE_EDITOR_C:
		defaultFileLocation.append("Default_EditorSource.txt");
		break;
	}

	std::string   fileBuffer;
	std::ifstream file(defaultFileLocation);

	std::string line;
	while (std::getline(file, line))
	{
		ReplaceAll(line, "{ProjectName}", "ProjectName");
		// TODO(Quillan): Project name is not really a thing, how to replace?
		ReplaceAll(line, "{EnginePath}", EnginePath(true));
		std::filesystem::path targetName = targetLocation;
		ReplaceAll(line, "{FileName}", targetName.replace_extension().filename().generic_string());

		fileBuffer.append(line + "\n");
	}

	file.close();

	std::ofstream resultFile(targetLocation);

	if (!resultFile.is_open())
	{
		//DebugLog(LogSeverity::SEVERE, "Did not create default file. Failed to create new file");
		return false;
	}

	resultFile << fileBuffer;
	resultFile.close();

	//DebugLog(LogSeverity::DONE, "Created default file");
	return true;
}
}
