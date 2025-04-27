#include "GL46_ComputeShader.h"

#include <glad/glad.h>


namespace Engine
{

/*
 * Class: GL46_ComputeShader => GL64_ShaderBase
 * Interface of ComputeShader for OpenGL 4.6
 */

GL46_ComputeShader::~GL46_ComputeShader()
{
	if (!m_initialised)
		return;

	glDeleteProgram(programID);
	glDeleteShader(m_shaderID);
}


void GL46_ComputeShader::LoadFromFile(const std::wstring& filename, const bool spirV)
{
	if (m_initialised)
	{
		glDeleteProgram(programID);
		glDeleteShader(m_shaderID);
	}

	// Load the file from disk
	std::wifstream filestream;
	filestream.open(filename.c_str());

	if (filestream.is_open())
	{
		std::wcout << ANSI_OK << "Compute shader file " << filename << " successfully loaded." << std::endl;
	}
	else
	{
		std::wcout << ANSI_ERR << "Compute shader file " << filename << " failed to open." << std::endl;
		throw std::runtime_error("Failed to load compute shader");
	}

	// Load contents
	std::wstringstream contentStream;
	contentStream << filestream.rdbuf();

	std::string contentString = WSTR_TO_STR(contentStream.str());
	const char* content = contentString.c_str();

	// Get the includes
	std::wistringstream contentSS;
	contentSS.str(contentStream.str());
	ManageIncludes(contentSS);

	// Create and compile shader
	m_shaderID = glCreateShader(GL_COMPUTE_SHADER);
	if (spirV)
	{
		glShaderBinary(1, &m_shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, content, sizeof(content));
		glSpecializeShader(m_shaderID, "main", 0, 0, 0);
	}
	else
	{
		glShaderSource(m_shaderID, 1, &content, nullptr);
		static const char* paths[] = { "/\00", "/Engine/\00", "/Project/\00" };
		glCompileShaderIncludeARB(m_shaderID, 1, paths, nullptr);
	}

	GLint success;
	glGetShaderiv(m_shaderID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[4096];
		glGetShaderInfoLog(m_shaderID, 4096, nullptr, infoLog);
		std::wcerr << ANSI_ERR << "Shader compilation error: " << infoLog << std::endl;
	}

	// Create the shader program
	programID = glCreateProgram();
	glAttachShader(programID, m_shaderID);
	glLinkProgram(programID);

	glGetShaderiv(m_shaderID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[4096];
		glGetShaderInfoLog(m_shaderID, 4096, nullptr, infoLog);
		std::wcerr << ANSI_ERR << "Shader linking error: " << infoLog << std::endl;
	}

	if (!m_initialised)
		m_initialised = true;

	// Close file
	filestream.close();
}


void GL46_ComputeShader::Dispatch(const vec3u& threads) const
{
	if (!m_initialised)
		throw std::runtime_error("Compute shader cannot be dispatched when no shader is loaded!");

	glDispatchCompute(threads.x, threads.y, threads.z);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}


uint32_t GL46_ComputeShader::ManageIncludes( std::wistringstream& contents )
{
	std::wstring line;
	uint32_t count = 0;
	while (std::getline(contents, line))
	{
		if (line.starts_with(L"#include"))
		{
			size_t firstSlash = line.find_first_of(L'/');
			std::wstring path = line.substr(firstSlash, line.length() - firstSlash);
			std::string glslPath = WSTR_TO_STR(path);
			// Remove trailing "
			glslPath.erase(glslPath.length() - 1);

			if (loadedShaders.contains(glslPath))
				continue;

			count++;

			std::wifstream included;
			if (path.starts_with(L"/Engine"))
			{
				path.replace(0, 7, L"src/Shaders");
				// Remove trailing "
				path.erase(path.length() - 1);

				included = {path.c_str()};

				if (!included.is_open())
					continue;

				std::wstringstream contentStream;
				contentStream << included.rdbuf();

				std::string contentString = WSTR_TO_STR(contentStream.str());
				const char* content = contentString.c_str();

				glNamedStringARB(GL_SHADER_INCLUDE_ARB, -1, glslPath.c_str(), -1, content);

				std::wistringstream contentSS;
				contentSS.str(contentStream.str());
				ManageIncludes(contentSS);

				included.close();
			}
			// TODO: Add the /Project/

			loadedShaders.emplace(glslPath);
		}
	}

	return count;
}

}
