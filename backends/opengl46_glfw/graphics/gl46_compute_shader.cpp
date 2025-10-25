#include "gl46_compute_shader.hpp"

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

	glDeleteProgram(m_programId);
	glDeleteShader(m_shaderID);
}


void GL46_ComputeShader::LoadFromFile(const std::string& filename, const bool spirV)
{
	if (m_initialised)
	{
		glDeleteProgram(m_programId);
		glDeleteShader(m_shaderID);
	}

	// Load the file from disk
	std::wifstream filestream;
	filestream.open(filename.c_str());

	if (filestream.is_open())
	{
		std::cout << ANSI_OK << "Compute shader file " << filename << " successfully loaded." << std::endl;
	}
	else
	{
		std::cout << ANSI_ERR << "Compute shader file " << filename << " failed to open." << std::endl;
		throw std::runtime_error("Failed to load compute shader");
	}

	// Load contents
	std::stringstream contentStream;
	contentStream << filestream.rdbuf();

	// Get the includes
	std::ostringstream includedStream;

	static bool supportsGLSLIncludes = glfwExtensionSupported("ARB_shading_language_include") == GLFW_TRUE;
	ManageIncludes(contentStream, includedStream, supportsGLSLIncludes);

	// Set contents to the current version
	std::string contentString = includedStream.str();
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
		if (supportsGLSLIncludes)
		{
			static const char* paths[] = {"/\00", "/Engine/\00", "/Project/\00"};
			glCompileShaderIncludeARB(m_shaderID, 1, paths, nullptr);
		}
		else
		{
			glCompileShader(m_shaderID);
		}
	}

	GLint success;
	glGetShaderiv(m_shaderID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[4096];
		glGetShaderInfoLog(m_shaderID, 4096, nullptr, infoLog);
		std::cerr << ANSI_ERR << "Shader compilation error: " << infoLog << std::endl;
	}

	// Create the shader program
	m_programId = glCreateProgram();
	glAttachShader(m_programId, m_shaderID);
	glLinkProgram(m_programId);

	glGetShaderiv(m_shaderID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[4096];
		glGetShaderInfoLog(m_shaderID, 4096, nullptr, infoLog);
		std::cerr << ANSI_ERR << "Shader linking error: " << infoLog << std::endl;
	}

	if (!m_initialised)
		m_initialised = true;

	// Close file
	filestream.close();
}


void GL46_ComputeShader::Dispatch(const glm::uvec3& threads) const
{
	if (!m_initialised)
		throw std::runtime_error("Compute shader cannot be dispatched when no shader is loaded!");

	glDispatchCompute(threads.x, threads.y, threads.z);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}


void GL46_ComputeShader::ManageIncludes(
	std::stringstream& contents, std::ostringstream& outStream,
	bool supportsGLSLIncludes, uint32_t currentFileIndex
)
{
	std::string line;
	uint32_t lineCount = 0;
	while (std::getline(contents, line))
	{
		lineCount++;
		if (line.starts_with("#include"))
		{
			size_t firstSlash = line.find_first_of(L'/');
			std::string path = line.substr(firstSlash, line.length() - firstSlash);
			std::string glslPath = path;
			// Remove trailing "
			glslPath.erase(glslPath.length() - 1);

			const bool alreadyLoaded = loadedShaders.contains(glslPath);
			if (alreadyLoaded)
			{
				if (supportsGLSLIncludes)
					outStream << line << "\n";
				continue;
			}

			std::wifstream included;
			if (path.starts_with("/Engine"))
			{
				path.replace(0, 7, "src/Shaders");
				// Remove trailing "
				path.erase(path.length() - 1);

				included = std::wifstream(path.c_str());

				if (!included.is_open())
					continue;

				std::stringstream contentStream;
				contentStream << included.rdbuf();

				if (supportsGLSLIncludes)
				{
					std::string contentString = contentStream.str();
					const char* content = contentString.c_str();

					glNamedStringARB(GL_SHADER_INCLUDE_ARB, -1, glslPath.c_str(), -1, content);
					outStream << line << "\n";
					ManageIncludes(contentStream, outStream, supportsGLSLIncludes, currentFileIndex);
				}
				else
				{
					const uint32_t fileIndex = ((alreadyLoaded)
						? std::distance(loadedShaders.begin(), loadedShaders.find(glslPath))
						: loadedShaders.size()) + 1ull;
					outStream << "#line 0 " << std::to_string(fileIndex) << "\n";
					ManageIncludes(contentStream, outStream, supportsGLSLIncludes, fileIndex);
					outStream << "#line " << std::to_string(lineCount + 1) << " " << std::to_string(currentFileIndex) << "\n";
				}

				included.close();
			}
			// TODO: Add the /Project/
			if (!alreadyLoaded)
				loadedShaders.emplace(glslPath);
		}
		else
		{
			outStream << line << "\n";
		}
	}
}

}
