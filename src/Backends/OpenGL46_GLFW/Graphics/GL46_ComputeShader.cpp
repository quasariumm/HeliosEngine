#include "GL46_ComputeShader.h"

#include <glad/glad.h>

#include "Backends/OpenGL46_GLFW/Core/GL46_Window.h"
#include "GLFW/glfw3.h"


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

	// Get the includes
	std::wostringstream includedStream;

	auto* window = static_cast<GL46_Window*>(glfwGetWindowUserPointer(glfwGetCurrentContext()));
	ManageIncludes(contentStream, includedStream, window->GetVendor() == "NVIDIA Corporation");

	// Set contents to the current version
	std::string contentString = WSTR_TO_STR(includedStream.str());
	const char* content = contentString.c_str();

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


void GL46_ComputeShader::ManageIncludes(
	std::wstringstream& contents, std::wostringstream& outStream,
	bool supportsGLSLIncludes, uint32_t currentFileIndex
)
{
	std::wstring line;
	uint32_t lineCount = 0;
	while (std::getline(contents, line))
	{
		lineCount++;
		if (line.starts_with(L"#include"))
		{
			size_t firstSlash = line.find_first_of(L'/');
			std::wstring path = line.substr(firstSlash, line.length() - firstSlash);
			std::string glslPath = WSTR_TO_STR(path);
			// Remove trailing "
			glslPath.erase(glslPath.length() - 1);

			const bool alreadyLoaded = loadedShaders.contains(glslPath);
			if (alreadyLoaded && supportsGLSLIncludes)
			{
				outStream << line << L"\n";
			}

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

				if (supportsGLSLIncludes)
				{
					std::string contentString = WSTR_TO_STR(contentStream.str());
					const char* content = contentString.c_str();

					glNamedStringARB(GL_SHADER_INCLUDE_ARB, -1, glslPath.c_str(), -1, content);
					outStream << line << L"\n";
					ManageIncludes(contentStream, outStream, supportsGLSLIncludes, currentFileIndex);
				}
				else
				{
					const uint32_t fileIndex = ((alreadyLoaded)
						? std::distance(loadedShaders.begin(), loadedShaders.find(glslPath))
						: loadedShaders.size()) + 1ull;
					outStream << L"#line 0 " << std::to_wstring(fileIndex) << L"\n";
					ManageIncludes(contentStream, outStream, supportsGLSLIncludes, fileIndex);
					outStream << L"#line " << std::to_wstring(lineCount + 1) << L" " << std::to_wstring(currentFileIndex) << L"\n";
				}

				included.close();
			}
			// TODO: Add the /Project/
			if (!alreadyLoaded)
				loadedShaders.emplace(glslPath);
		}
		else
		{
			outStream << line << L"\n";
		}
	}
}

}
