#include "gl46_shader_base.hpp"

namespace Helios
{

GL46_ShaderBase::~GL46_ShaderBase()
{
	glDeleteProgram(m_programId);
}


void GL46_ShaderBase::Use() const
{
	glUseProgram(m_programId);
}


void GL46_ShaderBase::SetBool(const std::string& name, const bool value) const
{
	glUniform1i(glGetUniformLocation(m_programId, name.c_str()), static_cast<int>(value));
}


void GL46_ShaderBase::SetBool(const int index, const bool value) const
{
	glUniform1i(index, static_cast<int>(value));
}


void GL46_ShaderBase::SetInt(const std::string& name, const int value) const
{
	glUniform1i(glGetUniformLocation(m_programId, name.c_str()), value);
}


void GL46_ShaderBase::SetInt(const int index, const int value) const
{
	glUniform1i(index, value);
}


void GL46_ShaderBase::SetUInt( const std::string& name, const uint32_t value ) const
{
	glUniform1ui(glGetUniformLocation(m_programId, name.c_str()), value);
}


void GL46_ShaderBase::SetUInt( const int index, const uint32_t value ) const
{
	glUniform1ui(index, value);
}


void GL46_ShaderBase::SetFloat(const std::string& name, const float value) const
{
	glUniform1f(glGetUniformLocation(m_programId, name.c_str()), value);
}


void GL46_ShaderBase::SetFloat(const int index, const float value) const
{
	glUniform1f(index, value);
}


void GL46_ShaderBase::SetVec2(const std::string& name, const glm::vec2& value) const
{
	glUniform2fv(glGetUniformLocation(m_programId, name.c_str()), 1, glm::value_ptr(value));
}


void GL46_ShaderBase::SetVec2(int index, const glm::vec2& value) const
{
	glUniform2fv(index, 1, glm::value_ptr(value));
}


void GL46_ShaderBase::SetVec3(const std::string& name, const glm::vec3& value) const
{
	glUniform3fv(glGetUniformLocation(m_programId, name.c_str()), 1, glm::value_ptr(value));
}


void GL46_ShaderBase::SetVec3(const int index, const glm::vec3& value) const
{
	glUniform3fv(index, 1, glm::value_ptr(value));
}


void GL46_ShaderBase::SetVec4(const std::string& name, const glm::vec4& value) const
{
	glUniform4fv(glGetUniformLocation(m_programId, name.c_str()), 1, glm::value_ptr(value));
}


void GL46_ShaderBase::SetVec4(const int index, const glm::vec4& value) const
{
	glUniform4fv(index, 1, glm::value_ptr(value));
}


void GL46_ShaderBase::SetMat4(const std::string& name, const glm::mat4& value) const
{
	glUniformMatrix4fv(glGetUniformLocation(m_programId, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}


void GL46_ShaderBase::SetMat4(const int index, const glm::mat4& value) const
{
	glUniformMatrix4fv(index, 1, GL_FALSE, glm::value_ptr(value));
}

} // Engine