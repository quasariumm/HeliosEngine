#pragma once

namespace Engine {

// ReSharper disable once CppInconsistentNaming
/**
 * @brief Shader class used as a base class for all shader types
 */
class GL46_ShaderBase
{

public:

	/**
	 * @brief Sets the current program as the active one
	 */
	void Use() const;

	/**
	 * @brief Sets a boolean value in the active shader
	 * @note Don't forget to use the shader before setting values
	 * @param name The name of the uniform
	 * @param value The value you want to set
	 */
	void SetBool(const std::string& name, bool value) const;

	/**
	 * @brief Sets a boolean value in the active shader by index
	 * @note Don't forget to use the shader before setting values
	 * @param index The location index of the uniform
	 * @param value The value you want to set
	 */
	void SetBool(int index, bool value) const;

	/**
	 * @brief Sets an integer value in the active shader
	 * @note Don't forget to use the shader before setting values
	 * @param name The name of the uniform
	 * @param value The value you want to set
	 */
	void SetInt(const std::string& name, int value) const;

	/**
	 * @brief Sets an integer value in the active shader by index
	 * @note Don't forget to use the shader before setting values
	 * @param index The location index of the uniform
	 * @param value The value you want to set
	 */
	void SetInt(int index, int value) const;

	/**
	 * @brief Sets an unsigned integer value in the active shader
	 * @note Don't forget to use the shader before setting values
	 * @param name The name of the uniform
	 * @param value The value you want to set
	 */
	void SetUInt(const std::string& name, uint32_t value) const;

	/**
	 * @brief Sets an unsigned integer value in the active shader by index
	 * @note Don't forget to use the shader before setting values
	 * @param index The location index of the uniform
	 * @param value The value you want to set
	 */
	void SetUInt(int index, uint32_t value) const;

	/**
	 * @brief Sets a floating point value in the active shader
	 * @note Don't forget to use the shader before setting values
	 * @param name The name of the uniform
	 * @param value The value you want to set
	 */
	void SetFloat(const std::string& name, float value) const;

	/**
	 * @brief Sets a floating point value in the active shader by index
	 * @note Don't forget to use the shader before setting values
	 * @param index The location index of the uniform
	 * @param value The value you want to set
	 */
	void SetFloat(int index, float value) const;

	/**
	 * @brief Sets a floating point 2D vector value in the active shader
	 * @note Don't forget to use the shader before setting values
	 * @param name The name of the uniform
	 * @param value The value you want to set
	 */
	void SetVec2(const std::string& name, const glm::vec2& value) const;

	/**
	 * @brief Sets a floating point 2D vector value in the active shader by index
	 * @note Don't forget to use the shader before setting values
	 * @param index The location index of the uniform
	 * @param value The value you want to set
	 */
	void SetVec2(int index, const glm::vec2& value) const;

	/**
	 * @brief Sets a floating point 3D vector value in the active shader
	 * @note Don't forget to use the shader before setting values
	 * @param name The name of the uniform
	 * @param value The value you want to set
	 */
	void SetVec3(const std::string& name, const glm::vec3& value) const;

	/**
	 * @brief Sets a floating point 3D vector value in the active shader by index
	 * @note Don't forget to use the shader before setting values
	 * @param index The location index of the uniform
	 * @param value The value you want to set
	 */
	void SetVec3(int index, const glm::vec3& value) const;

	/**
	 * @brief Sets a floating point 4D vector value in the active shader
	 * @note Don't forget to use the shader before setting values
	 * @param name The name of the uniform
	 * @param value The value you want to set
	 */
	void SetVec4(const std::string& name, const glm::vec4& value) const;

	/**
	 * @brief Sets a floating point 4D vector value in the active shader by index
	 * @note Don't forget to use the shader before setting values
	 * @param index The location index of the uniform
	 * @param value The value you want to set
	 */
	void SetVec4(int index, const glm::vec4& value) const;

	/**
	 * @brief Sets a 4x4 floating point matrix value in the active shader
	 * @note Don't forget to use the shader before setting values
	 * @param name The name of the uniform
	 * @param value The value you want to set
	 */
	void SetMat4(const std::string& name, const glm::mat4& value) const;

	/**
	 * @brief Sets a 4x4 floating point matrix value in the active shader by index
	 * @note Don't forget to use the shader before setting values
	 * @param index The location index of the uniform
	 * @param value The value you want to set
	 */
	void SetMat4(int index, const glm::mat4& value) const;

protected:

	GL46_ShaderBase() = default;
	~GL46_ShaderBase();

	uint32_t m_programId = 0;

};

} // Engine
