#include "GL46_ComputeShader.h"

#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>

#include "Core/Common.h"


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


void GL46_ComputeShader::LoadFromFile(const std::wstring& filename)
{
	if (m_initialised)
	{
		glDeleteProgram(programID);
		glDeleteShader(m_shaderID);
	}

	// Load the file from disk
	std::ifstream filestream;
	filestream.open(filename.c_str());

	if (filestream.is_open())
	{
		std::wcout << ANSI_OK << "Vertex shader file " << filename << " successfully loaded." << std::endl;
	}
	else
	{
		std::wcout << ANSI_ERR << "Vertex shader file " << filename << " failed to open." << std::endl;
		throw std::runtime_error("Failed to load vertex shader");
	}

	// Load contents
	std::stringstream contentStream;
	contentStream << filestream.rdbuf();

	const char* contentString = contentStream.str().c_str();

	// Create and compile shader
	m_shaderID = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(m_shaderID, 1, &contentString, nullptr);
	glCompileShader(m_shaderID);

	GLint success;
	glGetShaderiv(m_shaderID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(m_shaderID, 512, nullptr, infoLog);
		std::wcerr << ANSI_ERR << "Shader compilation error: " << infoLog << std::endl;
	}

	// Create the shader program
	programID = glCreateProgram();
	glAttachShader(programID, m_shaderID);
	glLinkProgram(programID);

	glGetShaderiv(m_shaderID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(m_shaderID, 512, nullptr, infoLog);
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

	glUseProgram(programID);
	glDispatchCompute(threads.x, threads.y, threads.z);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

}
