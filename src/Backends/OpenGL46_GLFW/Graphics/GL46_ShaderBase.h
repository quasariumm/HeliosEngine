#pragma once
#include <cstdint>
#include <string>

#include "Math/Matrix.h"


namespace Engine {

/**
 * @brief Shader class used as a base class for all shader types
 */
class GL46_ShaderBase
{

protected:

	GL46_ShaderBase() = default;
	~GL46_ShaderBase();

	/**
	 * @brief Sets the current program as the active one
	 */
	inline void Use() const;

	/**
	 * @brief Sets a boolean value in the active shader
	 * @note Don't forget to use the shader before setting values
	 * @param name The name of the uniform
	 * @param value The value you want to set
	 */
	inline void SetBool(const std::string& name, bool value) const;

	/**
	 * @brief Sets a boolean value in the active shader by index
	 * @note Don't forget to use the shader before setting values
	 * @param index The location index of the uniform
	 * @param value The value you want to set
	 */
	inline void SetBool(int index, bool value) const;

	/**
	 * @brief Sets an integer value in the active shader
	 * @note Don't forget to use the shader before setting values
	 * @param name The name of the uniform
	 * @param value The value you want to set
	 */
	inline void SetInt(const std::string& name, int value) const;

	/**
	 * @brief Sets an integer value in the active shader by index
	 * @note Don't forget to use the shader before setting values
	 * @param index The location index of the uniform
	 * @param value The value you want to set
	 */
	inline void SetInt(int index, int value) const;

	/**
	 * @brief Sets a floating point value in the active shader
	 * @note Don't forget to use the shader before setting values
	 * @param name The name of the uniform
	 * @param value The value you want to set
	 */
	inline void SetFloat(const std::string& name, float value) const;

	/**
	 * @brief Sets a floating point value in the active shader by index
	 * @note Don't forget to use the shader before setting values
	 * @param index The location index of the uniform
	 * @param value The value you want to set
	 */
	inline void SetFloat(int index, float value) const;

	/**
	 * @brief Sets a floating point 2D vector value in the active shader
	 * @note Don't forget to use the shader before setting values
	 * @param name The name of the uniform
	 * @param value The value you want to set
	 */
	inline void SetVec2(const std::string& name, const vec2f& value) const;

	/**
	 * @brief Sets a floating point 2D vector value in the active shader by index
	 * @note Don't forget to use the shader before setting values
	 * @param index The location index of the uniform
	 * @param value The value you want to set
	 */
	inline void SetVec2(int index, const vec2f& value) const;

	/**
	 * @brief Sets a floating point 3D vector value in the active shader
	 * @note Don't forget to use the shader before setting values
	 * @param name The name of the uniform
	 * @param value The value you want to set
	 */
	inline void SetVec3(const std::string& name, const vec3f& value) const;

	/**
	 * @brief Sets a floating point 3D vector value in the active shader by index
	 * @note Don't forget to use the shader before setting values
	 * @param index The location index of the uniform
	 * @param value The value you want to set
	 */
	inline void SetVec3(int index, const vec3f& value) const;

	/**
	 * @brief Sets a floating point 4D vector value in the active shader
	 * @note Don't forget to use the shader before setting values
	 * @param name The name of the uniform
	 * @param value The value you want to set
	 */
	inline void SetVec4(const std::string& name, const vec4f& value) const;

	/**
	 * @brief Sets a floating point 4D vector value in the active shader by index
	 * @note Don't forget to use the shader before setting values
	 * @param index The location index of the uniform
	 * @param value The value you want to set
	 */
	inline void SetVec4(int index, const vec4f& value) const;

	/**
	 * @brief Sets a 4x4 floating point matrix value in the active shader
	 * @note Don't forget to use the shader before setting values
	 * @param name The name of the uniform
	 * @param value The value you want to set
	 */
	inline void SetMat4(const std::string& name, const mat4f& value) const;

	/**
	 * @brief Sets a 4x4 floating point matrix value in the active shader by index
	 * @note Don't forget to use the shader before setting values
	 * @param index The location index of the uniform
	 * @param value The value you want to set
	 */
	inline void SetMat4(int index, const mat4f& value) const;


	uint32_t programID = 0;

};

} // Engine
